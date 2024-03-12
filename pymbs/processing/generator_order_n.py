from pymbs.processing.generator import *
from pymbs.processing.loads.constraint import Constraint

from pymbs.common.functions import skew, transpose, solve, der, scalar, scalar_if_possible,\
                                    block_matrix, block_vector, outer, vector_if_possible
from pymbs.common.state import State

from pymbs.symbolics import zeros, Matrix, jacobian, eye, Graph, VarKind

import pymbs.processing.sensors

from pymbs.processing import Body, FlexibleBody
import pymbs.symbolics as sym


BETA_STAR = 'int_beta_star_%s'
RR = 'int_RR_%s_%s'
A_STAR = 'int_a_star_%s'
J = 'int_J_%s'
J_HAT = 'int_J_hat_%s'
D = 'int_D_%s'
D_GAMMA = 'int_D_Gamma_%s'
D_HAT = 'int_d_hat_%s'
D_GAMMA_HAT = 'int_d_gamma_hat_%s'
E = 'int_e_%s'
RHO = 'int_rho_%s'
QDD = 'int_qdd_%s'
A = 'int_a_%s'
WF_CONSTR = 'int_w_f_%s'
WT_CONSTR = 'int_w_t_%s'
GAMMA = "int_GAMMA_%s"
GAMMA_SOL = "int_gamma_sol"
W = "int_W"
M_C_T = 'int_MC_T_%s'
M_C = 'int_MC_%s'
C_R_T = 'int_C_R_T_%s'
C_T_T = 'int_C_T_T_%s'
C_R = 'int_C_R_%s'
C_T = 'int_C_T_%s'
Ja = 'int_J_a_%s'
Jq = 'int_J_q_%s'
Me_HAT = 'int_Me_hat_%s'
Mc_HAT = 'int_Mc_hat_%s'
_l = 'int_l%s'
Da = 'int_D_a_%s'
hg = 'int_h_g_%s'
hgt = 'int_h_g_t_%s'
hgr = 'int_h_g_r_%s'
D_q_HAT = 'int_D_q_hat_%s'
dq = 'int_D_q_%s'
hgt = 'int_h_gt_%s'
hgr = 'int_h_gr_%s'
hge = 'int_h_ge_%s'
h_omegat = 'int_h_omega_t_%s'
h_omegar = 'int_h_omega_r_%s'
h_omegae = 'int_h_omega_e_%s'
omega_q = 'int_omega_q_%s'
he = 'int_h_e_%s'
hde = 'int_h_de_%s'
qdd_FLEX = 'int_qdd_flex_%s'
inert = 'int_inertia_%s'

FAST_ACCESS = True  # Use Fast Access, i.e. instead of e^T*b, directly access b(e_index)


import numpy as np


class Generator_OrderN(Generator):
    '''
    Generator That Calculates the Equations of Motion in OrderN Form
    '''

    def __init__(self, name, graph, inertial, g, sensors = None,
                 loops = None, diff = 0, kinematicsOnly = False):
        '''
        Constructor
        '''
        # Call Constructor of Super Class
        Generator.__init__(self, name, graph, inertial, g, sensors,
                           loops, diff, kinematicsOnly)


    def getSystemEqns(self):
        '''
        Calculate System Equations
        '''

        # Abbreviate Graph
        graph = self.graph

        # Haben wir constraints?
        self.hasConstraints = (len(self.state.mu) > 0)
        self.hasKinematicLoops = (self.hasConstraints or (len(self.loops)>0))

        # Dimension der Zwangskraefte (Constraints & Loops)
        self.n_gamma = len(self.state.mu) + len(self.w)

        # Look for loops and let them apply their constraints
        self.loopConstraints = []
        dummy = State()
        for loop in self.loops:
            c = loop.applyConstraintLoads()
            if isinstance(c, list):
                for el in c:
                    el.initialize(graph, dummy)
                self.loopConstraints.extend( c )
            else:
                c.initialize(graph, dummy)
                self.loopConstraints.append( c )
        # save gamma (mu for loops)
        self.state.gamma = dummy.mu
        self.state.Lambda = self.state.mu + self.state.gamma

        # Calculate Body State
        self.start1stForwardRecursion(graph)

        # Calculate Sensors (before accelerations)
        for sens in self.sensors:
            if (not sens.delayCalc):
                if (isinstance(sens, pymbs.processing.sensors.EnergySensor)):
                    sens.gravity = -self.inertial._alpha_C
                sens.calc(graph)

        if (self.kinematicsOnly):
            self.handleLoops(graph)
            self.defineStates()
        else:
            # Calculate Load Values
            self.getLoadValues(graph, self.fels + self.loopConstraints)

            # Obtain Equations
            self.startBackwardRecursion(graph, self.joints)
            self.start2ndForwardRecursion(graph, self.joints, self.fels, self.loops)

        # Calculate Sensors (after accelerations)
        for sens in self.sensors:
            if (sens.delayCalc):
                if (isinstance(sens, pymbs.processing.sensors.EnergySensor)):
                    sens.gravity = -self.inertial._alpha_C
                sens.calc(graph)

        # Temporary Workaround
        for mu in self.state.mu:
            graph.addEquation(mu, 0)

        # return all equations
        return True


    def start1stForwardRecursion(self, graph):
        '''
        Calculates All Positions w.r.t. Inertial Frame
        Calculates All Rotations w.r.t. Body Frame
        '''

        assert isinstance(graph, Graph)

        for body in self.bodies:
            # Is it the Inertial Frame itself?
            bName = body.name

            if (self.hasConstraints):
                body._A_M = dict()
                body._O_M = dict()
                body._O_M_tilde = dict()

            if (body.joint is None):
                # Set Position and Angular Velocity to Zero, Rotational Matrix = Identity
                body.I_r = graph.addEquation(I_p%bName, sym.zeros((3,)), shape=(3,))
                body.I_v = graph.addEquation(I_v%bName, sym.zeros((3,)), shape=(3,))

                if (not self.kinematicsOnly):
                    body.I_a = graph.addEquation(I_a%bName, sym.zeros((3,)), shape=(3,))

                body.I_R = graph.addEquation(T1%bName, sym.eye((3,3)), shape=(3,3))
                body._R_ih = body.I_R
                body.K_om = graph.addEquation(omega%bName, sym.zeros((3,)), shape=(3,))
                body.I_om = sym.zeros((3,))
                body._K_om_tilde = graph.addEquation(omega_tilde%bName, zeros((3,3)), shape=(3,3))
                body._a = Matrix([0,0,0,-self.g[0], -self.g[1], -self.g[2]])
                if (self.hasKinematicLoops):
                    body._gamma = zeros( (1, 1,self.n_gamma) )
                    body._a_gamma = zeros( (6,self.n_gamma) )

                if (not self.kinematicsOnly):
                    body.K_al = graph.addEquation(omegad%bName, sym.zeros((3,)), shape=(3,))
                    body.I_al = sym.zeros((3,))
                    body._K_al_tilde = graph.addEquation(omegad_tilde%bName, sym.zeros((3,3)))
                    body._beta_star = graph.addEquation(BETA_STAR%bName, zeros((3,3)))

                # Initial Values
                body._omegad_C = graph.addEquation(omegad_C%bName, sym.zeros((3,)), shape=(3,))
                body._beta_C = graph.addEquation(beta_C%bName, zeros((3,3)), shape=(3,3))
                body._alpha_C = graph.addEquation(alpha_C%bName, -self.g, shape=(3,))

            elif isinstance(body,Body):
                # Get Reference to Parent Body
                joint = body.joint
                jName = joint.name
                cs = joint.coordSys
                parentBody = cs.parentBody
                pName = parentBody.name

                # Joint Properties
                T_cs = cs.I_R
                T_j = graph.addEquation(T1%jName, joint.R)
                body.I_R = graph.addEquation(T1%bName, T_cs*T_j, shape=(3,3))
                body._R = graph.addEquation(T2%(pName,bName), transpose(cs.R*T_j))

                p_z_p_b = graph.addEquation(p_z%(pName,bName), cs.p+joint.Psi*joint.q, shape=(3,))
                body._p_z = p_z_p_b
                body._p_z_tilde = graph.addEquation(p_z_tilde%(pName,bName), skew(p_z_p_b), shape=(3,3))

                # Mass, cg and Inertia
                body._l = graph.addEquation(l%bName, body.cg, shape=(3,))
                body._l_tilde = graph.addEquation(l_tilde%bName, skew(body._l), shape=(3,3))
                body.mass = graph.addEquation(m%bName, body.mass, shape=())
                body.inertia = graph.addEquation(I%bName, body.inertia - body.mass*body._l_tilde*body._l_tilde, shape=(3,3))

                # Position
                body.I_r = graph.addEquation(I_p%bName, parentBody.I_r + parentBody.I_R*p_z_p_b, shape=(3,))
                body.I_l = graph.addEquation(I_l%bName, body.I_r + body.I_R*body._l)

                # Omega (Angular Velocity)
                body.K_om = graph.addEquation(omega%bName, body._R*parentBody.K_om + joint.qd*joint.Phi, shape=(3,))    # (14)
                body.I_om = graph.addEquation("_I_"+omega%bName, body.I_R*body.K_om) # compatibility
                body._K_om_tilde = graph.addEquation(omega_tilde%bName, skew(body.K_om), shape=(3,3))

                # Velocity
                body.I_v = graph.addEquation(I_v%bName, parentBody.I_v + parentBody.I_R*(parentBody._K_om_tilde*p_z_p_b + joint.qd*joint.Psi), shape=(3,))
                body.I_ldot = graph.addEquation(I_ldot%bName, body.I_v + body.I_R*body._K_om_tilde*body._l, shape=(3,))

                if (not self.kinematicsOnly):
                    # Accelerations
                    body.I_a = graph.addVariable(I_a%bName, shape=(3,))
                    body.K_al = graph.addVariable(omegad%bName, shape=(3,))
                    body.I_al = graph.addEquation("_I_"+omegad%bName, body.I_R*body.K_al) # compatibility
                    body._K_al_tilde = graph.addEquation(omegad_tilde%bName, skew(body.K_al))

                    # These are needed for the OrderN algorithm
                    body._beta_star = graph.addEquation(BETA_STAR%bName, body._K_om_tilde*body._K_om_tilde)             # (16)
                    R_tmp = graph.addEquation(RR%(bName,pName)+"_1", -body._R*body._p_z_tilde)
                    body._RR = graph.addEquation(RR%(bName,pName), block_matrix([[body._R, zeros((3,3))], [R_tmp, body._R]]) )       # (22 a)
                    a_tmp1 = graph.addEquation(A_STAR%bName+"_1", joint.qd*body._K_om_tilde*joint.Phi, shape=(3,))
                    a_tmp2 = graph.addEquation(A_STAR%bName+"_2", body._R*parentBody._beta_star*body._p_z+2*joint.qd*body._K_om_tilde*joint.Psi)
                    body._a_star = graph.addEquation(A_STAR%bName, block_vector([ a_tmp1, a_tmp2 ]))                                 # (22 b)

                    # Constraints
                    if (self.hasConstraints):
                        # body._R transforms from parent to body frame
                        # all equations are written w.r.t. body frame
                        body._omegad_C = graph.addEquation(omegad_C%bName, body._R*parentBody._omegad_C + body._K_om_tilde*joint.Phi*joint.qd, shape=(3,)) # (19, Recursive Paper)
                        omegad_C_tilde_b = graph.addEquation(omegad_C_tilde%bName, skew(body._omegad_C), shape=(3,3))
                        body._beta_C = graph.addEquation(beta_C%bName, body._beta_star + omegad_C_tilde_b, shape=(3,3)) # (20, Recursive Paper)
                        body._alpha_C = graph.addEquation(alpha_C%bName, body._R*parentBody._alpha_C + parentBody._beta_C*p_z_p_b + 2*body._K_om_tilde*joint.Psi*joint.qd, shape=(3,)) # (21, Recursive Paper)

                        # we will need Phi and Psi written w.r.t. the inertial frame
                        joint._I_Psi = graph.addEquation('int_%s_I_Psi'%joint.name, T_cs*joint.Psi)
                        joint._I_Phi = graph.addEquation('int_%s_I_Phi'%joint.name, T_cs*joint.Phi)
                        joint._I_Phi_tilde = graph.addEquation('int_%s_I_Phi_tilde'%joint.name, skew(joint._I_Phi))

            elif isinstance(body,FlexibleBody):

                # Get Reference to Parent Body
                joint = body.joint
                jName = joint.name
                cs = joint.coordSys
                parentBody = cs.parentBody
                pName = parentBody.name

                # Joint Properties
                T_cs = cs.I_R
                T_j = graph.addEquation(T1%jName, joint.R)
                body.I_R = graph.addEquation(T1%bName, T_cs*T_j, shape=(3,3))
                body._R = graph.addEquation(T2%(pName,bName), transpose(cs.R*T_j))

                p_z_p_b = graph.addEquation(p_z%(pName,bName), cs.p+joint.Psi*joint.q, shape=(3,))
                body._p_z = p_z_p_b
                body._p_z_tilde = graph.addEquation(p_z_tilde%(pName,bName), skew(p_z_p_b), shape=(3,3))

                # Mass, cg and Inertia
                body._l = graph.addEquation(l%bName, body.cg, shape=(3,))
                body._l_tilde = graph.addEquation(l_tilde%bName, skew(body._l), shape=(3,3))
                body.mass = graph.addEquation(m%bName, body.mass, shape=())
                body.inertia = graph.addEquation(I%bName, body.inertia - body.mass*body._l_tilde*body._l_tilde, shape=(3,3))

                # Position
                body.I_r = graph.addEquation(I_p%bName, parentBody.I_r + parentBody.I_R*p_z_p_b, shape=(3,))
                body.I_l = graph.addEquation(I_l%bName, body.I_r + body.I_R*body._l)

                # Omega (Angular Velocity)
                body.K_om = graph.addEquation(omega%bName, body._R*parentBody.K_om + joint.qd*joint.Phi, shape=(3,))    # (14)
                body.I_om = graph.addEquation("_I_"+omega%bName, body.I_R*body.K_om) # compatibility
                body._K_om_tilde = graph.addEquation(omega_tilde%bName, skew(body.K_om), shape=(3,3))
                # eq.(6.248) Wallrap "Dynamik flexibler Mehrkörpersysteme"
                omegaq = sym.zeros((6,))
                omegaq[0] = body.K_om[0,0]*body.K_om[0,0]
                omegaq[1] = body.K_om[1,0]*body.K_om[1,0]
                omegaq[2] = body.K_om[2,0]*body.K_om[2,0]
                omegaq[3] = body.K_om[0,0]*body.K_om[1,0]
                omegaq[4] = body.K_om[1,0]*body.K_om[2,0]
                omegaq[5] = body.K_om[0,0]*body.K_om[2,0]

                #K_om_q = Symbolics.Matrix = ([[body.K_om[0,0]*body.K_om[0,0]],[body.K_om[1,0]*body.K_om[1,0]],[body.K_om[2,0]*body.K_om[2,0]],[body.K_om[0,0]*body.K_om[1,0]],[body.K_om[1,0]*body.K_om[2,0]],[body.K_om[0,0]*body.K_om[2,0]]])
                body._K_om_q = graph.addEquation(omega_q%bName,omegaq)

                body.I_v = graph.addEquation(I_v%bName, parentBody.I_v + parentBody.I_R*(parentBody._K_om_tilde*p_z_p_b + joint.qd*joint.Psi), shape=(3,))
                body.I_ldot = graph.addEquation(I_ldot%bName, body.I_v + body.I_R*body._K_om_tilde*body._l, shape=(3,))

                if (not self.kinematicsOnly):
                    # Accelerations
                    body.I_a = graph.addVariable(I_a%bName, shape=(3,))
                    body.K_al = graph.addVariable(omegad%bName, shape=(3,))
                    body.I_al = graph.addEquation("_I_"+omegad%bName, body.I_R*body.K_al) # compatibility
                    body._K_al_tilde = graph.addEquation(omegad_tilde%bName, skew(body.K_al))

                    # These are needed for the OrderN algorithm
                    body._beta_star = graph.addEquation(BETA_STAR%bName, body._K_om_tilde*body._K_om_tilde)             # (16)
                    R_tmp = graph.addEquation(RR%(bName,pName)+"_1", -body._R*body._p_z_tilde)
                    body._RR = graph.addEquation(RR%(bName,pName), block_matrix([[body._R, zeros((3,3))], [R_tmp, body._R]]) )       # (22 a)
                    a_tmp1 = graph.addEquation(A_STAR%bName+"_1", joint.qd*body._K_om_tilde*joint.Phi, shape=(3,))
                    a_tmp2 = graph.addEquation(A_STAR%bName+"_2", body._R*parentBody._beta_star*body._p_z+2*joint.qd*body._K_om_tilde*joint.Psi)
                    body._a_star = graph.addEquation(A_STAR%bName, block_vector([ a_tmp1, a_tmp2 ]))                                 # (22 b)

                    # Constraints
                    if (self.hasConstraints):
                        # body._R transforms from parent to body frame
                        # all equations are written w.r.t. body frame
                        body._omegad_C = graph.addEquation(omegad_C%bName, body._R*parentBody._omegad_C + body._K_om_tilde*joint.Phi*joint.qd, shape=(3,)) # (19, Recursive Paper)
                        omegad_C_tilde_b = graph.addEquation(omegad_C_tilde%bName, skew(body._omegad_C), shape=(3,3))
                        body._beta_C = graph.addEquation(beta_C%bName, body._beta_star + omegad_C_tilde_b, shape=(3,3)) # (20, Recursive Paper)
                        body._alpha_C = graph.addEquation(alpha_C%bName, body._R*parentBody._alpha_C + parentBody._beta_C*p_z_p_b + 2*body._K_om_tilde*joint.Psi*joint.qd, shape=(3,)) # (21, Recursive Paper)

                        # we will need Phi and Psi written w.r.t. the inertial frame
                        joint._I_Psi = graph.addEquation('int_%s_I_Psi'%joint.name, T_cs*joint.Psi)
                        joint._I_Phi = graph.addEquation('int_%s_I_Phi'%joint.name, T_cs*joint.Phi)
                        joint._I_Phi_tilde = graph.addEquation('int_%s_I_Phi_tilde'%joint.name, skew(joint._I_Phi))

                for node in body.sid.node_List:
                    # get PHI and PSI matrices from SID-object and save them as CSymbol type
                    # the matrices refer to bodyframe
                    PHI_flex = node.phi.phimatrix.M0
                    PSI_flex = node.psi.psimatrix.M0
                    node.PHI_loc = graph.addEquation(PHI_flex_node_loc%(bName,node.node_number), PHI_flex, shape=(3,body.sid.modal.refmod.nelastq))

                    # PHI and PSI matrices become attribut (directly) of node class (SID-File)
                    node.PSI = graph.addEquation(PSI_flex_node%(bName,node.node_number), PSI_flex, shape=(3,body.sid.modal.refmod.nelastq))
                    # PHi matrice refer to Inertial-frame
                    node.PHI = graph.addEquation(PHI_flex_node%(bName,node.node_number),body.I_R*node.PHI_loc,shape=(3,body.sid.modal.refmod.nelastq))

                    if node.node_number == body.sid.nNodes:
                        '''
                         body._V matrix is V matrix in Christian´s Lyx File
                         contains the deflection of the flexible body
                         first three rows contain rotation, last three rows contain displacement
                        '''
                        V_node_joint = graph.addEquation(V_NODE%(bName,node.node_number),block_matrix([[node.PSI], [node.PHI_loc]]))
                        body._V = graph.addEquation(V%(bName,node.node_number),V_node_joint)



                    '''
                    There are two possibilities to calculate the displacement (rotation as well) of node i according to deflection of body k.
                    In SID-File, every node has four matrices (origin, phi, psi and AP).
                    each column of Phi is an Eigenvector which belongs to the eigenmode respectively, written down column by column in origin matrix
                    ------> origin matrix is a taylor expansion, contains flexible coordinates already
                    Psi <------> AP matrix (see Phi <------> origin)

                    first possibility (bodyframe):
                    u = phi * q, theta(vector) = psi * q

                    second possibility (bodyframe):
                    u = origin, theta(matrix) = AP

                    We use the second possibility, because it is easier to add the second step in the taylor-expansion,
                    modelling a second translation (2.d.o.f.)
                    '''

                    # translational and rotational displacement of node i (body-frame)
                    node.u_trans = graph.addEquation(u_trans_node%(bName,node.node_number),node.PHI*body.q_vec,shape=(3,))
                    node.theta_rot = graph.addEquation(theta_rot_node%(bName,node.node_number),node.PSI*body.q_vec,shape=(3,))


                    '''
                    origin Matrix, taylor expansion
                    '''

                    # position of node i (Taylor: M0-zero Order, M1-first order)
                    # M0-matrix
                    # M1-list, each element is a matrix
                    origin_M0 = node.origin.originmatrix.M0
                    origin_M1 = node.origin.originmatrix.M1


                    if (node.origin.originmatrix.nrow != 3):
                        raise NotImplementedError('ORIGIN matrix must have 3 rows')

                    if (node.origin.originmatrix.ncol != 1):
                        raise NotImplementedError('ORIGIN matrix must have 1 column')


                    origin_m0_loc = graph.addEquation(origin_M0_loc%(bName,node.node_number), vector_if_possible(origin_M0))


                    list_index = 0
                    # constant Taylor-term
                    Taylor_ORIGIN_const_loc = graph.addEquation(Taylor_origin_const_loc%(bName,node.node_number),origin_m0_loc,shape=(3,))
                    # preparation taylor-expansion
                    Taylor_ORIGIN_loc = graph.addEquation(Taylor_origin_loc%(bName,node.node_number),Taylor_ORIGIN_const_loc,shape=(3,))
                    list_index = 1

                    # first order matrices are saved in a list
                    # get M1 matrix and add it to taylor-expansion
                    for matrix_M1 in origin_M1:

                        Taylor_ORIGIN_m1_activ = graph.addEquation(Taylor_origin_m1_activ%(bName,list_index,node.node_number),vector_if_possible(matrix_M1))
                        Taylor_ORIGIN_loc = Taylor_ORIGIN_loc+Taylor_ORIGIN_m1_activ*body.q[list_index-1]
                        list_index = list_index+1

                    # local origin taylor-expansion becomes attribut of node (bodyframe)
                    node.origin_loc = Taylor_ORIGIN_loc
                    node.frame.p = node.origin_loc


                    '''
                    AP Matrix, taylor-expansion (skew-symetric)
                    '''

                    # orientation of nodeframe to bodyframe
                    # M0-matrix
                    # M1-list, each element is a matrix
                    AP_M0 = node.ap.apmatrix.M0
                    AP_M1 = node.ap.apmatrix.M1

                    if (node.ap.apmatrix.nrow != 3):
                        raise NotImplementedError('AP matrix must have 3 rows')

                    if (node.ap.apmatrix.ncol != 3):
                        raise NotImplementedError('AP matrix must have 3 column')


                    AP_m0_loc = graph.addEquation(AP_M0_loc%(bName,node.node_number), AP_M0, shape=(3,3))


                    list_index = 0
                    # constant Taylor-term
                    Taylor_ap_const_loc = graph.addEquation(Taylor_AP_const_loc%(bName,node.node_number),AP_m0_loc,shape=(3,3))
                    # preparation taylor-expansion
                    Taylor_ap_loc = graph.addEquation(Taylor_AP_loc%(bName,node.node_number),Taylor_ap_const_loc,shape=(3,3))
                    list_index = 1

                    # first order matrices are saved in a list
                    # get M1 matrix and add it to taylor-expansion
                    for matrix_M1 in AP_M1:

                        Taylor_ap_m1_activ = graph.addEquation(Taylor_AP_m1_activ%(bName,list_index,node.node_number),matrix_M1,shape=(3,3))
                        Taylor_ap_loc = Taylor_ap_loc+Taylor_ap_m1_activ*body.q[list_index-1]
                        list_index = list_index+1

                    # AP taylor-expansion becomes attribut of node
                    node.AP_loc = Taylor_ap_loc

                    # origin and AP in Inertialframe
                    #node.origin = graph.addEquation(Origin%(bName,node.node_number),body.I_R*node.origin_loc,shape=(3,1))
                    #node.AP = graph.addEquation(ap%(bName,node.node_number),body.I_R*node.AP_loc)


                    # rotaion nodeframe-Inertialframe

                    node.frame.R = node.AP_loc

            else:
                raise NotImplementedError("Unknown Type of Body!")


            # Update Child Coordinate Systems
            for childCS in body.children:
                csName = childCS.name
                # Do the Update
                childCS.R = graph.addEquation(T2%(bName,csName), childCS.R, shape=(3,3))
                childCS.I_R = graph.addEquation(T1%('%s_%s'%(bName,csName)), body.I_R*childCS.R, shape=(3,3))

                childCS._p = graph.addEquation(p%(bName,csName), childCS.p, shape=(3,))
                childCS._p_tilde = graph.addEquation(p_tilde%(bName,csName), skew(childCS._p), shape=(3,3) )

                childCS.I_r = graph.addEquation(I_p%('%s_%s'%(bName,csName)), body.I_r + body.I_R*childCS.p, shape=(3,))
                childCS.I_v = graph.addEquation(I_v%('%s_%s'%(bName,csName)), body.I_v + body.I_R*(body._K_om_tilde*childCS.p), shape=(3,))

                childCS.K_om = body.K_om
                childCS.I_om = body.I_om        # compatibility

                if (not self.kinematicsOnly):
                    childCS.I_a = graph.addEquation(I_a%('%s_%s'%(bName,csName)), body.I_a + body.I_R*(body._K_al_tilde + body._K_om_tilde**2)*childCS._p, shape=(3,))
                    childCS.K_al = body.K_al    # omegad_BodyName here, since it is equal to omegad_csName anyway
                    childCS.I_al = body.I_al    # compatibility



    def getLoadValues(self, graph, fels):
        '''
        Write All Equations needed for loads
        '''

        idx = 0

        if (self.hasKinematicLoops):
            Lambda = Matrix(self.state.Lambda)
            n = len(self.state.Lambda)

        for load in fels:
            lName = load.name
            load.calc(graph, self.state)

            if (not isinstance(load, Constraint)):  # Normal Load
                # Force
                if (load.force is None):
                    load._F = sym.zeros((3,))
                else:
                    Wf = graph.addEquation(WF%lName, load.Wf)
                    load._F = graph.addEquation(F%lName, Wf*load.force, shape=(3,))
                # Torque
                if (load.torque is None):
                    load._L = sym.zeros((3,))
                else:
                    Wt = graph.addEquation(WT%lName, load.Wt)
                    load._L = graph.addEquation(L%lName, Wt*load.torque, shape=(3,))
            else:

                # self.state.mu dictates in which column which information is stored
                # Force
                if (load.force is None):
                    load._Wf = sym.zeros((3,n))
                else:
                    Wf = load.Wf*jacobian(load.force,Lambda)
                    load._Wf = graph.addEquation(WF%lName, Wf)
                # Torque
                if (load.torque is None):
                    load._Wt = sym.zeros((3,n))
                else:
                    Wt = load.Wt*jacobian(load.torque,Lambda)
                    load._Wt = graph.addEquation(WT%lName, Wt)



    def startBackwardRecursion(self, graph, joints):
        '''
        Find Expressions for Cut Forces
        '''

        ## 2nd Backward Recursion
        joints.reverse()
        for joint in joints:

            # Abbreviations
            body = joint.body
            bName = body.name

            if (isinstance(body,Body)):

                # External and Constraint Force/Torque Vector
                f_ext = 0
                Wf_constraint = 0
                l_ext = 0
                Wt_constraint = 0
                for child in body.children:
                    for load in child.loads:
                        # Load must have this Coordinate System (child) as a parent or a child!
                        assert child in (load.parent, load.child), "Internal Error: load %s must have this Coordinate System (child) as a parent or a child!"%(load.name)

                        # Calculate Rotation Matrix
                        if ((load.ref is None) or (load.ref == child)): # ref = parent
                            if (load.parent == child):
                                T = 1
                            else:
                                # child <- world <- parent
                                T = transpose(child.I_R)*load.parent.I_R
                        else:   # ref = irgendetwas
                            # child (child/parent) <- I <- ref
                            T = transpose(child.I_R)*load.ref.I_R


                        if (not isinstance(load, Constraint)):  # We are dealing with a normal load
                            f_local = T*load._F
                            l_local = child._p_tilde*f_local + T*load._L
                            if (child == load.parent):
                                f_ext += f_local
                                l_ext += l_local
                            else: # (child == load.child)
                                f_ext -= f_local
                                l_ext -= l_local

                        else:   # We have a constraint load
                            Wf_local = T*load._Wf
                            Wt_local = child._p_tilde*Wf_local + T*load._Wt
                            if (child == load.parent):
                                Wf_constraint += Wf_local
                                Wt_constraint += Wt_local
                            else: # (child == load.child)
                                Wf_constraint -= Wf_local
                                Wt_constraint -= Wt_local

                if (f_ext == 0): f_ext = sym.zeros((3,))
                if (l_ext == 0): l_ext = sym.zeros((3,))
                body._F_ext = graph.addEquation(F_EXT%bName, f_ext)
                body._L_ext = graph.addEquation(L_EXT%bName, l_ext)
                if (self.hasKinematicLoops):
                    if (Wf_constraint == 0): Wf_constraint = sym.zeros((3,len(self.state.Lambda)))
                    if (Wt_constraint == 0): Wt_constraint = sym.zeros((3,len(self.state.Lambda)))
                    body._WF_constraint = graph.addEquation(WF_CONSTR%bName, Wf_constraint)
                    body._WT_constraint = graph.addEquation(WT_CONSTR%bName, Wt_constraint)
                body._Q = 0     # Joint load, could be used for implementing a better jointLoad later

                J_1 = graph.addEquation(J%bName+"_1", body.mass*body._l_tilde)
                J_2 = graph.addEquation(J%bName+"_2", -J_1)
                J_3 = graph.addEquation(J%bName+"_3", body.mass*eye((3,3)))
                body._J = graph.addEquation(J%bName, block_matrix([[body.inertia, J_1], [J_2, J_3]]))        # (21 a)
    #            D_1 = graph.addEquation(D%bName+"_1", body._L_ext + body._l_tilde*body._F_ext - body._K_om_tilde*body.inertia*body.K_om)
                # Warum  + body._K_om_tilde*body.inertia*body.K_om und nicht -
                D_1 = graph.addEquation(D%bName+"_1", body._L_ext + body._K_om_tilde*body.inertia*body.K_om)
                D_2 = graph.addEquation(D%bName+"_2", body._F_ext - body.mass*body._beta_star*body.cg)
                body._d = graph.addEquation(D%bName, block_vector([D_1, D_2]))                               # (21 b)
                tmp_e = block_vector([joint.Phi, joint.Psi])
                body._e = graph.addEquation(E%bName, tmp_e)                   # (22 c)
                body._e_index = self.getIndex(tmp_e)
                if (self.hasKinematicLoops):
                    #D_GAMMA_1 = graph.addEquation(D_GAMMA%bName+"_1", body._l_tilde*body._WF_constraint + body._WT_constraint)
                    #body._d_gamma = graph.addEquation(D_GAMMA%bName, blockMatrix([[D_GAMMA_1], [body._WF_constraint]]))     # (47)
                    # Umrechnung Kraft<->Moment ist schon in _WT_constraint enthalten
                    body._d_gamma = graph.addEquation(D_GAMMA%bName, block_matrix([[body._WT_constraint], [body._WF_constraint]]))     # (47)

                # Calculate J_hat and d_hat
                J_hat = body._J
                d_hat = body._d
                if (self.hasKinematicLoops):
                    d_gamma_hat = body._d_gamma

                # Go through all children
                for child in body.children:
                    for childJoint in child.joints:
                        c = childJoint.body

                        if (FAST_ACCESS):
                            J_tmp = graph.addEquation('int_RT_J_%s'%c.name, transpose(c._RR)*c._J_hat)
                            J_e = c._J_hat_red
                            J_hat = J_hat - (1/c._rho)*(J_tmp[:,c._e_index]*transpose(J_tmp[:,c._e_index])) + J_tmp*c._RR    # (31)
                            d_hat = d_hat - transpose(c._RR)*((1/c._rho)*J_e*(c._d_hat[c._e_index] + c._Q - transpose(J_e)*c._a_star) + c._J_hat*c._a_star - c._d_hat)  # (32)
                            if (self.hasKinematicLoops):
                                d_gamma_hat = d_gamma_hat - transpose(c._RR)*((1/c._rho)*J_e*c._d_gamma_hat[c._e_index,:] - c._d_gamma_hat)  # (49)
                        else:
                            J_hat = J_hat - (1/c._rho)*(transpose(c._RR)*c._J_hat*outer(c._e,c._e)*c._J_hat*c._RR) + transpose(c._RR)*c._J_hat*c._RR    # (31)
                            d_hat = d_hat - transpose(c._RR)*((1/c._rho)*c._J_hat*c._e*(transpose(c._e)*c._d_hat + c._Q - transpose(c._e)*c._J_hat*c._a_star) + c._J_hat*c._a_star - c._d_hat)  # (32)
                            if (self.hasKinematicLoops):
                                d_gamma_hat = d_gamma_hat - transpose(c._RR)*((1/c._rho)*c._J_hat*outer(c._e,c._e)*c._d_gamma_hat - c._d_gamma_hat)  # (49)


                # Save equation
                body._J_hat = graph.addEquation(J_HAT%bName, J_hat)
                body._J_hat_red = graph.addEquation('int_J_hat_%s_red'%bName, body._J_hat[:,body._e_index])
                body._d_hat = graph.addEquation(D_HAT%bName, d_hat)

                '''
                 check if there is a flexible parent body
                 yes, calculate the Ja and Jq matrices
                '''
                cs = joint.coordSys
                parentBody = cs.parentBody

                if isinstance(parentBody,FlexibleBody):

                    J_a = transpose(body._RR)*(body._J_hat-body._J_hat_red*(1/(transpose(body._e)*body._J_hat*body._e))*transpose(body._e)*body._J_hat)*body._RR
                    body._J_a = graph.addEquation(Ja%(bName), J_a)

                    J_q = transpose(parentBody._V)*body._J_a*parentBody._V
                    body._J_q = graph.addEquation(Jq%(bName),J_q)
#-------fragen ob das richtig ist-----------------------------------------------------------
                    D_a =transpose(body._RR)*(body._J_hat_red*(1/(transpose(body._e)*body._J_hat*body._e))*(transpose(body._e)*block_vector([body._L_ext,body._F_ext])-transpose(body._e)*body._J_hat*body._a_star-transpose(body._e)*body._d_hat)+body._J_hat*body._a_star+body._d_hat)
                    body._D_a = graph.addEquation(Da%bName,D_a)



                if (self.hasKinematicLoops):
                    body._d_gamma_hat = graph.addEquation(D_GAMMA_HAT%bName, d_gamma_hat)

                if (FAST_ACCESS):
                    body._rho = graph.addEquation(RHO%bName, body._J_hat[body._e_index, body._e_index])    # between (25) and (26)
                else:
                    body._rho = graph.addEquation(RHO%bName, scalar(transpose(body._e)*body._J_hat*body._e))    # between (25) and (26)

            elif (isinstance(body,FlexibleBody)):
                # External and Constraint Force/Torque Vector
                f_ext = 0
                Wf_constraint = 0
                l_ext = 0
                Wt_constraint = 0
                h_d_e = 0
                for child in body.children:
                    for load in child.loads:
                        # Load must have this Coordinate System (child) as a parent or a child!
                        assert child in (load.parent, load.child), "Internal Error: load %s must have this Coordinate System (child) as a parent or a child!"%(load.name)

                        # Calculate Rotation Matrix
                        if ((load.ref is None) or (load.ref == child)): # ref = parent
                            if (load.parent == child):
                                T = 1
                            else:
                                # child <- world <- parent
                                T = transpose(child.I_R)*load.parent.I_R
                        else:   # ref = irgendetwas
                            # child (child/parent) <- I <- ref
                            T = transpose(child.I_R)*load.ref.I_R

                        if (not isinstance(load, Constraint)):  # We are dealing with a normal load
                            Phi_SID = None
                            Psi_SID = None
                            # get the PHI matrix from SID, respectively
                            for node in body.sid.node_List:
                                if node.frame.name == child.name:
                                    Phi_SID = node.PHI_loc
                                    Psi_SID = node.PSI

                            if Phi_SID == None or Psi_SID == None:
                                raise ValueError('loads must be attached to node i')
                            # surface force, see Wallrap eq.(6.276)
                            f_local = T*load._F
                            l_local = child._p_tilde*f_local + skew(Phi_SID*body.q_vec)*f_local + T*load._L
                            h_de_local = transpose(Phi_SID)*f_local + transpose(Psi_SID)*T*load._L
                            if (child == load.parent):
                                f_ext += f_local
                                l_ext += l_local
                                h_d_e += h_de_local
                            else: # (child == load.child)
                                f_ext -= f_local
                                l_ext -= l_local
                                h_d_e -= h_de_local
                        else:   # We have a constraint load
                            Wf_local = T*load._Wf
                            Wt_local = child._p_tilde*Wf_local + T*load._Wt
                            if (child == load.parent):
                                Wf_constraint += Wf_local
                                Wt_constraint += Wt_local
                            else: # (child == load.child)
                                Wf_constraint -= Wf_local
                                Wt_constraint -= Wt_local

                if (f_ext == 0): f_ext = sym.zeros((3,))
                if (l_ext == 0): l_ext = sym.zeros((3,))
                if (h_d_e == 0): h_de = sym.zeros((body.sid.nModes,))
                body._F_ext = graph.addEquation(F_EXT%bName, f_ext)
                body._L_ext = graph.addEquation(L_EXT%bName, l_ext)
                h_de = graph.addEquation(hde%bName,h_d_e)
                if (self.hasKinematicLoops):
                    if (Wf_constraint == 0): Wf_constraint = sym.zeros((3,len(self.state.Lambda)))
                    if (Wt_constraint == 0): Wt_constraint = sym.zeros((3,len(self.state.Lambda)))
                    body._WF_constraint = graph.addEquation(WF_CONSTR%bName, Wf_constraint)
                    body._WT_constraint = graph.addEquation(WT_CONSTR%bName, Wt_constraint)
                body._Q = 0     # Joint load, could be used for implementing a better jointLoad later

                # body mass from SID-File
                body.mass = body.sid.modal.refmod.mass
                '''
                 SID-matrices
                 M0-matrix
                 M1-list, each element is a matrix
                '''
                Me_M0 = body.sid.modal.me.mematrix.M0
                Me_M1 = body.sid.modal.me.mematrix.M1

                mDCM_M0 = body.sid.modal.mdcm.mdcmmatrix.M0
                mDCM_M1 = body.sid.modal.mdcm.mdcmmatrix.M1

                I_M0 = body.sid.modal.j.Jmatrix.M0
                I_M1 = body.sid.modal.j.Jmatrix.M1

                Ct_M0 = body.sid.modal.ct.ctmatrix.M0
                Ct_M1 = body.sid.modal.ct.ctmatrix.M1

                Cr_M0 = body.sid.modal.cr.crmatrix.M0
                Cr_M1 = body.sid.modal.cr.crmatrix.M1

                Ke_M0 = body.sid.modal.ke.kematrix.M0
                Ke_M1 = body.sid.modal.ke.kematrix.M1

                De_M0 = body.sid.modal.de.dematrix.M0
                De_M1 = body.sid.modal.de.dematrix.M1

                Ge_M0 = body.sid.modal.ge.gematrix.M0
                Ge_M1 = body.sid.modal.ge.gematrix.M1

                Gr_M0 = body.sid.modal.gr.grmatrix.M0
                Gr_M1 = body.sid.modal.gr.grmatrix.M1

                Oe_M0 = body.sid.modal.oe.oematrix.M0
                Oe_M1 = body.sid.modal.oe.oematrix.M1

                kSigma_M0 = body.sid.modal.ksigma.ksigmamatrix.M0
                kSigma_M1 = body.sid.modal.ksigma.ksigmamatrix.M1

                # Me matrix (SID-File) is equivalent to Mq matrix in Christian´s Lyx-File
                # # mass matrix  see Wallrap eq.(6.235)
                Me = self.get_SID_matrix('Me',bName,Me_M0,Me_M1,body)
                Me_hat = Me

                # secondary diagonal of mass matrix (shape=(3,1))
                mDCM = self.get_SID_matrix('mDCM',bName,mDCM_M0,mDCM_M1,body)
                body._l = graph.addEquation(_l%bName,mDCM/body.mass)

                # Inertia (shape=(6,1)) I_ab, ab = 11,22,33,12,13,23
                I = self.get_SID_matrix('I',bName,I_M0,I_M1,body)

                # coupling translation and deflection (shape=(nq,3))
                CT = self.get_SID_matrix('Ct',bName,Ct_M0,Ct_M1,body)
                # coupling rotation and deflection (shape=(nq,3))
                CR = self.get_SID_matrix('Cr',bName,Cr_M0,Cr_M1,body)
                # stiffness matrix, zero order only ----> no pretension, no geometrical stiffness
                KE = self.get_SID_matrix('Ke',bName,Ke_M0,Ke_M1,body)
                # damping matrix
                DE = self.get_SID_matrix('De',bName,De_M0,De_M1,body)
                # coriolis force
                GE = self.get_SID_matrix('Ge',bName,Ge_M0,Ge_M1,body)
                # coriolis force
                GR = self.get_SID_matrix('Gr',bName,Gr_M0,Gr_M1,body)
                # centrifugal force
                OE = self.get_SID_matrix('Oe',bName,Oe_M0,Oe_M1,body)
                # bias
                KSIGMA = self.get_SID_matrix('kSigma',bName,kSigma_M0,kSigma_M1,body)

                # Inertia of flexible body (symetric inertia tensor)
                Y = sym.Matrix([[I[0,0], I[3,0], I[4,0]],
                                 [I[3,0], I[1,0], I[5,0]],
                                 [I[4,0], I[5,0], I[2,0]]])


                body.inertia = graph.addEquation(inert%bName, Y)
                Cr = graph.addEquation(C_R%bName,CR)
                Ct = graph.addEquation(C_T%bName,CT)
                Cr_T = graph.addEquation(C_R_T%bName,transpose(Cr))
                Ct_T = graph.addEquation(C_T_T%bName,transpose(Ct))
                # M_c matrix (SID-File) is equivalent to Mc matrix in Christian´s Lyx-File
                #M_c_T = graph.addEquation(M_C_T%(bName),blockMatrix([[Cr_T],[Ct_T]]))
                M_c = graph.addEquation(M_C%(bName),block_matrix([[Cr_T],[Ct_T]]))
                Mc_hat = M_c
#-----------------Gravitationsvektor fehlt---------------------------------
                # weight, see Wallrap "Dynamik flexibler Mehrkörpersysteme" page 300 eq. (6.253)
                # h_g = graph.addEquation(hg%bName, blockMatrix([[tmp_weight_1],[tmp_weight_2],[Ct]]))
                h_gt = graph.addEquation(hgt%bName,body.mass*eye((3,3)))
                h_gr = graph.addEquation(hgr%bName,body.mass*skew(body._l))
                h_ge = graph.addEquation(hge%bName,Ct*self.g)
                # force of inertia
                h_omega_t = graph.addEquation(h_omegat%bName, -body.mass*body._K_om_tilde*body.I_v-2*body._K_om_tilde*transpose(Ct)*body.qd_vec-body.mass*body._beta_star*body._l)
                # summation of Gr (with flexible velocity and omega), see Wallrap eq.(6.251)
                sum_Gr = self.sum_Matrix(GR,body)
                h_omega_r = graph.addEquation(h_omegar%bName,-body.mass*skew(body._l)*body._K_om_tilde*body.I_v-sum_Gr-body._K_om_tilde*body.inertia*body.K_om)
                # summation of Ge (with flexible velocity and omega), see Wallrap eq.(6.251)
                sum_Ge = self.sum_Matrix(GE,body)
                h_omega_e = graph.addEquation(h_omegae%bName,-Ct*body._K_om_tilde*body.I_v-sum_Ge-OE*body._K_om_q)
                #internal force, no consideration of pretension and geometrical stiffness
                #----->-Ke*q-De*qd instead of -ke0-Ke0*q-Ke*q-KeN*q-De*qd see Wallrap eq.(6.270)
                h_e = graph.addEquation(he%bName, -KE*body.q_vec-DE*body.qd_vec)

#---h_g fehlt-----dq muss noch richtig berechnet werden, Gravitation fehlt--------------------------------
                d_q = h_omega_e + h_de + h_e + h_ge
                body._d_q = graph.addEquation(dq%bName,d_q)
                d_q_hat = body._d_q

                # mass matrix
                J_1 = graph.addEquation(J%bName+"_1", skew(mDCM),shape=(3,3))
                J_2 = graph.addEquation(J%bName+"_2", -J_1)
                J_3 = graph.addEquation(J%bName+"_3", body.mass*eye((3,3)))

                body._J = graph.addEquation(J%bName, block_matrix([[body.inertia, J_1], [J_2, J_3]]))      # (21 a)
#------h_g fehlt----- stimmt hier '-' , es stand vorher '+' Gravitation fehlt
                D_1 = graph.addEquation(D%bName+"_1", body._L_ext)
                D_2 = graph.addEquation(D%bName+"_2", body._F_ext)
                body._d = graph.addEquation(D%bName, block_vector([D_1, D_2]) + block_vector([h_omega_r, h_omega_t]))                             # (21 b)
                # joint.Phi, joint.Psi is no part of PHI and PSI from SID-File!!
                tmp_e = block_vector([joint.Phi, joint.Psi])
                body._e = graph.addEquation(E%bName, tmp_e)                   # (22 c)
                body._e_index = self.getIndex(tmp_e)
                if (self.hasKinematicLoops):
                    #D_GAMMA_1 = graph.addEquation(D_GAMMA%bName+"_1", body._l_tilde*body._WF_constraint + body._WT_constraint)
                    #body._d_gamma = graph.addEquation(D_GAMMA%bName, blockMatrix([[D_GAMMA_1], [body._WF_constraint]]))     # (47)
                    # Umrechnung Kraft<->Moment ist schon in _WT_constraint enthalten
                    body._d_gamma = graph.addEquation(D_GAMMA%bName, block_matrix([[body._WT_constraint], [body._WF_constraint]]))     # (47)

                # Calculate J_hat and d_hat
                J_hat = body._J
                d_hat = body._d
                if (self.hasKinematicLoops):
                    d_gamma_hat = body._d_gamma

                # Go through all children
                child_Body = None
                for child in body.children:
                    for childJoint in child.joints:
                        child_Body = childJoint.body
                        Me_hat = Me_hat + child_Body._J_q
                        Mc_hat = Mc_hat + child_Body._J_a*body._V

                if not(child_Body == None):
                    M_e_hat = Me_hat
                    M_c_hat = Mc_hat
                else:
                    M_e_hat = Me
                    M_c_hat = M_c
                # body_M_e_hat and body_M_c_hat: submatrices of flexible mass matrix
                # see Christian´s Lyx File / see Wallrap SID-Paper (Standardization of
                # Flexible Body Modeling in Multibody
                # System Codes, Part I: Definition of Standard Input Data) page 16
                body._M_e_hat = graph.addEquation(Me_HAT%(bName),M_e_hat)
                body._M_c_hat = graph.addEquation(Mc_HAT%(bName),M_c_hat)

                child_Body = None
                for child in body.children:
                    for childJoint in child.joints:
                        child_Body = childJoint.body
                        J_hat = J_hat + child_Body._J_a
                        d_hat = d_hat + child_Body._D_a
                        d_q_hat = d_q_hat + transpose(body._V)*child_Body._D_a

                if not(child_Body == None):
                    J_hat = J_hat - body._M_c_hat*(1/body._M_e_hat)*transpose(body._M_c_hat)
                    d_hat = d_hat - body._M_c_hat*(1/body._M_e_hat)*d_q_hat

                else:
                    J_hat = body._J - body._M_c_hat*(1/body._M_e_hat)*transpose(body._M_c_hat)
                    d_hat = body._d - body._M_c_hat*(1/body._M_e_hat)*d_q_hat

                # Save equation
                body._J_hat = graph.addEquation(J_HAT%bName, J_hat)
                body._J_hat_red = graph.addEquation('int_J_hat_%s_red'%bName, body._J_hat[:,body._e_index])
                body._d_hat = graph.addEquation(D_HAT%bName, d_hat)
                body._d_q_hat = graph.addEquation(D_q_HAT%bName, d_q_hat)

                cs = joint.coordSys
                parentBody = cs.parentBody

                '''
                 check if there is a flexible parent body
                 yes, calculate Ja and Jq matrices
                 see Christian´s Lyx File
                '''

                if isinstance(parentBody,FlexibleBody):

                    J_a = transpose(body._RR)*(body._J_hat-body._J_hat_red*(1/(transpose(body._e)*body._J_hat*body._e))*transpose(body._e)*body._J_hat)*body._RR
                    body._J_a = graph.addEquation(Ja%(bName), J_a)

                    J_q = transpose(parentBody._V)*body._J_a*parentBody._V
                    body._J_q = graph.addEquation(Jq%(bName),J_q)
#---------fragen ob das richtig ist-----------------------------------------------------------
                    D_a =transpose(body._RR)*(body._J_hat_red*(1/(transpose(body._e)*body._J_hat*body._e))*(transpose(body._e)*block_vector([body._L_ext,body._F_ext])-transpose(body._e)*body._J_hat*body._a_star-transpose(body._e)*body._d_hat)+body._J_hat*body._a_star+body._d_hat)
                    body._D_a = graph.addEquation(Da%bName,D_a)

                if (self.hasKinematicLoops):
                    body._d_gamma_hat = graph.addEquation(D_GAMMA_HAT%bName, d_gamma_hat)

                if (FAST_ACCESS):
                    body._rho = graph.addEquation(RHO%bName, body._J_hat[body._e_index, body._e_index])    # between (25) and (26)
                else:
                    body._rho = graph.addEquation(RHO%bName, scalar(transpose(body._e)*body._J_hat*body._e))    # between (25) and (26)

            else:
                raise NotImplementedError("Unknown Type of Body!")

    def start2ndForwardRecursion(self, graph, joints, fels, loops):

        # Forward Recursion
        if (self.hasKinematicLoops):
            Gamma = Matrix( (len(self.state.qdd), len(self.state.Lambda)) )
            j_qdd = [0]*len(self.state.qdd)

        joints.reverse()
        qdd = [0]*len(joints)
        for i in range(len(joints)):
            j = joints[i]
            jName = j.name
            b = j.body                  # Body
            p = j.coordSys.parentBody   # ParentBody

            if (self.hasKinematicLoops):
                qdd_i = graph.addVariable(str(j.qdd) + "_star")
                j_qdd[i] = j.qdd
            else:
                qdd_i = j.qdd
#----------nach Q fragen---------------------------------------------------------------------------------------------------------------------------------------------------------
            if (FAST_ACCESS):
                if isinstance(p,FlexibleBody):
                    qdd[i] = graph.addEquation(qdd_i, (1/b._rho)*(b._d_hat[b._e_index] + b._Q - transpose(b._J_hat_red)*(b._a_star + b._RR*p._a + b._RR*p._V*p.qdd_flex) ) )   # (26)
                else:
                    qdd[i] = graph.addEquation(qdd_i, (1/b._rho)*(b._d_hat[b._e_index] + b._Q - transpose(b._J_hat_red)*(b._a_star + b._RR*p._a) ) )   # (26)
            else:
                if isinstance(p,FlexibleBody):
                    qdd[i] = graph.addEquation(qdd_i, (1/b._rho)*(transpose(b._e)*b._d_hat + b._Q - transpose(b._e)*b._J_hat*(b._a_star + b._RR*p._a + b._RR*p._V*p.qdd_flex) ) )   # (26)
                else:
                    qdd[i] = graph.addEquation(qdd_i, (1/b._rho)*(transpose(b._e)*b._d_hat + b._Q - transpose(b._e)*b._J_hat*(b._a_star + b._RR*p._a) ) )   # (26)

            b._a = graph.addEquation(A%b.name, b._RR*p._a + b._a_star + b._e*qdd[i]) # (22)
            graph.addEquation(b.K_al, b._a[0:3])
            graph.addEquation(b.I_a, b.I_R*b._a[3:6])

            if isinstance(b,FlexibleBody):
                # calculate the flexible accelerations
                # im Lyx File stand es falsch, M_c_hat transponiert statt M_c_hat
                b.qdd_flex = graph.addEquation(qdd_FLEX%b.name,(1/b._M_e_hat)*(b._d_q_hat - transpose(b._M_c_hat) * b._a))

            if (self.hasKinematicLoops):
                # Equation (50) in the paper is wrong, since a_h depends on former gammas, hence this part has to be included
                #gamma = graph.addEquation(GAMMA%b.name, (1/b._rho)*(transpose(b._e)*b._d_gamma_hat)) # (50)
                if (FAST_ACCESS):
                    b._gamma = graph.addEquation(GAMMA%b.name, (1/b._rho)*(b._d_gamma_hat[b._e_index,:] - transpose(b._J_hat_red)*b._RR*p._a_gamma), shape=(1, 1,self.n_gamma)) # (50, modified)
                else:
                    b._gamma = graph.addEquation(GAMMA%b.name, (1/b._rho)*(transpose(b._e)*b._d_gamma_hat - transpose(b._e)*b._J_hat*b._RR*p._a_gamma), shape=(1, 1,self.n_gamma)) # (50, modified)

                b._a_gamma = graph.addEquation('int_a_gamma_%s'%b.name, b._RR*p._a_gamma + b._e*b._gamma) # (22, modified)
                Gamma[i,:] = b._gamma


        if (self.hasKinematicLoops):
            # Constraints and loops together are not supported
            if (self.hasConstraints and (len(self.loops)>0)):
                raise NotImplementedError('Solving Kinematic Loops via Constraints and Loops is currently not supported!')

            # Constraints
            if (self.hasConstraints):

                # J und Jdqd, as in the recursive paper
                J, Jdqd = self.accConstraint(graph, len(qdd))

            # Loops
            if (len(self.loops) > 0):
                # Man war ich hier blind!
                # Also: Wir haben qdd = J*udd + d (mit J und d aus self.handleLoops)
                # Das kann man umschreiben in
                #   qdd = J*HR*qdd + d mit HR = du/dq (Selektieren der Werte u aus q)
                #   wdd = HL*qdd = HL*(J*HR*qdd + d) mit HL = dw/dq (Selektieren der Werte v aus q)
                # und damit
                # 0 = HL*(J*HR-I)*qdd + HL*d
                #     <--------->       <-->
                #         J             Jdqd

                # Calculate HL and HR
                n = len(self.q)
                HR = jacobian(Matrix(self.u), self.q)
                HL = jacobian(Matrix(self.w), self.q)

                # Temporarily set self.kinematicsOnly = True, so that no acceleration data is written
                tmp = self.kinematicsOnly
                self.kinematicsOnly = True
                J, d = self.handleLoops(graph)
                self.kinematicsOnly = tmp

                # Transform J and d
                J = graph.addEquation( 'int_J', HL*(J*HR-eye(n)) )
                Jdqd = graph.addEquation( 'int_Jdqd', HL*d )

            # From here on it is like in the OrderN Paper
            # qdd_star schreiben
            qdd_star = graph.addEquation('int_qdd_star', Matrix(qdd), shape=self.reduce_shape( (len(qdd),) ) )

            # W schreiben
            Gamma_sym = graph.addEquation('int_Gamma', Gamma)
            # Calculate Constraint Forces
            gamma_sol = graph.addEquation( GAMMA_SOL, solve(J*Gamma_sym, -(J*qdd_star+Jdqd)) )

            # Adjust Accelerations
            qdd = graph.addEquation('int_qdd', qdd_star + Gamma_sym*gamma_sol)

            # in case of loops, extract only the independent qdd
            if (len(self.w) > 0):
                # save all joint-accelerations
                graph.addEquation(j_qdd, qdd)

                # Take only the "interesting" accelerations
                m_j_qdd = Matrix(j_qdd)
                qdd = jacobian(self.qdd, m_j_qdd)*m_j_qdd
        else:
            # add the flexible accelerations to qdd list
            for joint in joints:
                flex_body = joint.body

                if isinstance(flex_body,FlexibleBody):

                    for i in range(len(flex_body.qdd_flex)):
                        flex_qdd_i = flex_body.qdd[i]
                        pre_qdd = graph.addEquation(flex_qdd_i,flex_body.qdd_flex[i])
                        qdd.append(pre_qdd)


            qdd = Matrix(qdd)

        # Define State variables
        q = graph.addVariable(name='q', shape=self.reduce_shape( self.q.shape() ), initialValue=scalar_if_possible(self.q0) )
        qd = graph.addVariable(name='qd', shape=self.reduce_shape( self.qd.shape() ), initialValue=scalar_if_possible(self.qd0) )
        graph.addEquation(self.state.q, q)
        graph.addEquation(self.state.qd, qd)
        qdd = graph.addEquation('qdd', scalar_if_possible(qdd), shape=self.reduce_shape( (len(qdd),) ) )

        graph.addEquation(der(q), qd)
        graph.addEquation(der(qd), qdd)



    def accConstraint(self, graph, n):
        '''
        Obtain G and Gdqd of constraints on acceleration level,
        such that G*qdd+Gdqd=0
        '''

        # Generate empty matrices
        G = sym.zeros( (len(self.state.mu), n) )
        Gdqd = sym.zeros( (len(self.state.mu),) )
        row = 0

        # Go through all constraints
        for c in self.constraints:

            # Take transLock and rotLock together
            lock = [c.rotLock[0], c.rotLock[1], c.rotLock[2], \
                    c.transLock[0], c.transLock[1], c.transLock[2] ]

            # Relative Rotation between child and parent
            # parent <- I <- child
            R_rel = transpose(c.parent.I_R)*c.child.I_R

            # G*qdd + Gdqd = 0 (constraints on acceleration level w.r.t. to c.parent frame)
            # get first (parent) cs (w.r.t. c.parent frame)
            Gp, Gpdqd = self.getG(graph, n, c.parent, c.parent)

            # get second (child) cs (w.r.t. c.parent frame)
            Gc, Gcdqd = self.getG(graph, n, c.child, c.parent)

            # take the difference and pick elements
            for i in range(len(lock)):
                if (lock[i]):
                    for j in range(n):
                        G[row,j] = Gp[i,j] - Gc[i,j]
                    Gdqd[row] = Gpdqd[i] - Gcdqd[i]
                    row = row + 1

            # add them to the graph
            G = graph.addEquation('int_J', G)
            Gdqd = graph.addEquation('int_Jdqd', Gdqd)

        return (G, Gdqd)



    def getG(self, graph, n, cs, csref):
        '''
        Return G for a specific coordinate system
        '''

        csName = cs.name
        body = cs.parentBody
        bName = body.name
        bcsName = bName + "_" + csName
        joint = body.joint
        if (joint is None):
            return 0, 0

        G = sym.Matrix( (6,n) )
        Gdqd = sym.Matrix( (6,) )

        parentBody = joint.coordSys.parentBody
        pName = parentBody.name

        if (cs == csref):
            R_rel = cs.R
        else:
            R_rel = transpose(csref.I_R)*cs.I_R*cs.R     # to csref from cs

        R = cs.R*body._R    # to cs from parentBody
        p_z_p_b = graph.addEquation(p_z%(pName,bcsName), joint.coordSys.p+joint.Psi*joint.q+transpose(body._R)*cs.p, shape=(3,))    # w.r.t. parentBody
        # both _alpha_c and _omegad_C are w.r.t. csref
        _alpha_C = graph.addEquation(alpha_C%bcsName, R_rel*( body._R*(parentBody._alpha_C + parentBody._beta_C*p_z_p_b) + 2*body._K_om_tilde*joint.Psi*joint.qd ), shape=(3,)) # (21, Recursive Paper)
        _omegad_C = graph.addEquation(omegad_C%bcsName, R_rel*body._omegad_C, shape=(3,)) # (19, Recursive Paper)

        # w.r.t. csref
        O_M_c_b = graph.addEquation(O_M%(bName,csName), R_rel*joint.Phi)
        A_M_c_b = graph.addEquation(A_M%(bName,csName), R_rel*(joint.Psi + skew(joint.Phi)*cs.p))

        Gdqd[0:3] = _omegad_C
        Gdqd[3:6] = _alpha_C
        G[0:3, body.index] = O_M_c_b
        G[3:6, body.index] = A_M_c_b

        # Go into direction of world
        R_rel = transpose(csref.I_R)
        temp = parentBody;
        while (temp.joint is not None):
            tName = temp.name;
            O_M_c_t = graph.addEquation(O_M%(bcsName,tName), R_rel*temp.joint._I_Phi)
            tmp = R_rel*(temp.joint._I_Psi + temp.joint._I_Phi_tilde*(cs.I_r - temp.I_r))
            A_M_c_t = graph.addEquation(A_M%(bcsName,tName), tmp, shape=(3,)) # (23b)

            if (temp.index >= 0):
                G[0:3, temp.index] = O_M_c_t
                G[3:6, temp.index] = A_M_c_t

            # GoTo Next Body
            temp = temp.joint.coordSys.parentBody

        return G, Gdqd



    def getIndex(self, e):
        '''
        return index i, where e[i] != 0 (e[i] == 1)
        '''

        # get shape
        shape = e.shape()
        assert shape==(6,), 'Unexpected shape of e: %s'%shape

        # search for index
        index = None
        for i in range( shape[0] ):
            if (e[i] != 0):
                assert e[i] == 1, '%s th element of e is nonzero (%s) but not 1'%(str(i), str(e[i]))
                assert index is None, 'There are two nonzero elements in i (%s,%s)'%(str(index), str(i))
                index = i

        assert index is not None, 'Vector e does not have a non zero element!'

        # return index
        return index

    def defineStates(self):
        '''
        Generate equations for q, qd and qdd if self.kinematicsOnly
        '''

        assert self.kinematicsOnly == True

        graph = self.graph

        # Define State variables
        q = graph.addVariable(name='q', shape=self.reduce_shape( self.q.shape() ), initialValue=scalar_if_possible(self.q0) )
        qd = graph.addVariable(name='qd', shape=self.reduce_shape( self.qd.shape() ), initialValue=scalar_if_possible(self.qd0) )
        qdd = graph.addVariable(name='qdd', shape=self.reduce_shape( self.qdd.shape() ),varKind=VarKind.Input)

        # Make a relation between vector q and its elements
        graph.addEquation(self.state.q, q)
        graph.addEquation(self.state.qd, qd)
        graph.addEquation(self.state.qdd, qdd)

        # Define q and qd as states
        graph.addEquation(der(q), qd)
        graph.addEquation(der(qd), qdd)

    def get_SID_matrix(self,arg,bname,M0,M1,body):
        '''
        create matrices from SID-File
        '''
        assert isinstance(arg,str),'arg must be a string'
        assert isinstance(bname,str),'body name (bname) must be a string'
        assert isinstance(M0,np.matrix),'M0 must be a numpy matrix'
        assert isinstance(M1,list),'M1 must be a list'
        size=M0.shape

        if arg == 'mDCM':
            m0_loc = self.graph.addEquation(M0_loc%(arg,bname), vector_if_possible(M0))


        else:
            m0_loc = self.graph.addEquation(M0_loc%(arg,bname), M0,shape=(size[0],size[1]))


        list_index = 0
        # constant Taylor-term
        const_loc = self.graph.addEquation(Taylor_const_loc%(arg,bname),m0_loc)
        # preparation taylor-expansion
        Taylor_loc = self.graph.addEquation(Taylor_loc_const%(arg,bname),const_loc)
        list_index = 1


        for matrix_M1 in M1:

            if arg == 'mDCM':
                 Taylor_activ_M1 = self.graph.addEquation(Taylor_m1_activ%(arg,list_index,bname),vector_if_possible(matrix_M1))
            else:
                Taylor_activ_M1 = self.graph.addEquation(Taylor_m1_activ%(arg,list_index,bname),matrix_M1,shape=(size[0],size[1]))

            Taylor_loc = Taylor_loc+Taylor_activ_M1*body.q[list_index-1]
            list_index = list_index+1

        # local origin taylor-expansion becomes attribut of node (bodyframe)
        return Taylor_loc

    def sum_Matrix(self,matrix,body):
        '''
         see Wallrap "Dynamik flexibler Mehrkörpersysteme" page 299 eq.6.251
         to get the Coriolis force (part of inertia force)
        '''

        nq = body.sid.nModes
        sum_matrix = matrix[:,0:3]*body.qd[0]*body.K_om
        i = 0
        for i in range(nq-1):
            sum_matrix = sum_matrix + matrix[:,(i+1)*3:(i+1)*3+3]*body.qd[i+1]*body.K_om

        return sum_matrix


