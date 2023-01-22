from pymbs.common.mbselement import MbsElement
from pymbs.common.state import State

from pymbs.common.functions import transpose, element, norm

from pymbs.processing.sensors.sensor import Sensor
from pymbs.processing.loops.loop import Loop
from pymbs.processing.loops.exp_joint import ExpJoint
from pymbs.processing.loads.constraint import Constraint
from pymbs.processing import Body

import pymbs.symbolics as symbolics
from pymbs.symbolics import jacobian


omega = 'int_omega_%s'
omega_tilde = 'int_omega_tilde_%s'
PHI = 'int_PHI_%s'
PHI_LOC = 'int_PHI_loc_%s'
PSI = 'int_PSI_%s'
PSI_LOC = 'int_PSI_loc_%s'
omegad_C = 'int_omegad_C_%s'
omegad_C_tilde = 'int_omegad_C_tilde_%s'
alpha_C = 'int_alpha_C_%s'
beta_C = 'int_beta_C_%s'
p = 'int_p_%s_%s'
p_tilde = 'int_p_tilde_%s_%s'
p_LOC = 'int_p_loc_%s_%s'
p_z = 'int_p_z_%s_%s'
p_z_tilde = 'int_p_z_tilde_%s_%s'
O_M = 'int_O_M_%s_%s'
O_M_tilde = 'int_O_M_tilde_%s_%s'
A_M = 'int_A_M_%s_%s'
m = 'int_m_%s'
I = 'int_I_%s'
I_LOC = 'int_I_loc_%s'
l = 'int_l_%s'
l2 = 'int_l_%s_%s'
l_LOC = 'int_l_loc_%s'
l_tilde = 'int_l_tilde_%s'
l2_tilde = 'int_l_tilde_%s_%s'
G_C = 'int_G_C_%s'
G_M = 'int_G_M_%s_%s'
F = 'int_F_%s'
F_C = 'int_F_C_%s'
F_M = 'int_F_M_%s_%s'
L = 'int_L_%s'
L_C = 'int_L_C_%s'
L_M = 'int_L_M_%s_%s'
T1 = 'int_T_%s'
T2 = 'int_T_loc_%s_%s'
F = 'int_F_%s'
F_EXT = 'int_F_ext_%s'
L = 'int_L_%s'
L_EXT = 'int_L_ext_%s'
C = 'int_C_%i'
M = 'int_M_%i_%i'
TRANSPOSE = '.T'
WF = 'int_WF_%s'
WT = 'int_WT_%s'
I_p = 'int_I_p_%s'
I_v = 'int_I_v_%s'
I_a = 'int_I_a_%s'
omegad = 'int_omegad_%s'
omegad_tilde = 'int_omegad_tilde_%s'
I_l = 'int_I_l_%s'
I_ldot = 'int_I_ldot_%s'
I_lddot = 'int_I_lddot_%s'

der_q = 'der_q'
der_qd = 'der_qd'
Phi = 'Phi_'

BWU = "int_Bwu_%s"
BPRIME = "int_b_prime_%s"

PHI_flex_node = 'int_%s_flex_phi_%i'
PHI_flex_node_loc = 'int_%s_flex_phi_%i_loc'
PSI_flex_node = 'int_%s_flex_psi_%i'

u_trans_node = 'int_%s_u_trans_node_%i'
theta_rot_node = 'int_%s_theta_rot_node_%i'

origin_M0_loc = 'int_%s_origin_M0_node_%i_loc'
origin_M0 = 'int_%s_origin_M0_node_%i'

AP_M0_loc = 'int_%s_AP_M0_node_%i_loc'
AP_M0 = 'int_%s_AP_M0_node_%i'

Taylor_origin_const_loc = 'int_%s_taylor_origin_node_%i_const_loc'
Taylor_origin_loc = 'int_%s_taylor_origin_node_%i_loc'
Taylor_origin_m1_activ = 'int_%s_Taylor_origin_m1_order_%i_node_%i'

Taylor_AP_const_loc = 'int_%s_taylor_AP_node_%i_const_loc'
Taylor_AP_loc = 'int_%s_taylor_AP_node_%i_loc'
Taylor_AP_m1_activ = 'int_%s_Taylor_AP_m1_order_%i_node_%i'


Origin = '%s_origin_node_%i'
ap = '%s_AP_node_%i'

T_I_node_cs = '%s_node_%i_I_R'

M0_loc = 'int_%s_M0_%s_loc'
Taylor_const_loc = 'int_%s_Taylor_M0_%s_const_loc'
Taylor_loc_const = 'int_%s_Taylor_%s_loc'
Taylor_m1_activ = 'int_%s_Taylor_M1_order_%i_%s'
V_NODE = 'int_V_%s_node%i'
V = 'int_V_%s_node_%i'



class Generator(MbsElement):
    '''
    Super Class For All Generators
    '''


    def __init__(self, name, graph, inertial, g, sensors = None,
                loops = None, diff = 0, kinematicsOnly = False):
        '''
        Constructor
        name: Name of the Generator
        inertial: Inertial Body
        '''

        if (sensors is None): sensors = []
        if (loops is None): loops = []

        # Type Conversion
        if (isinstance(g, list)): g = symbolics.Matrix(g)
        if (isinstance(diff, int)): diff = [diff]

        # Assertions
        # Todo: py3 error
        #assert( isinstance(inertial, Body.Body) )
        assert isinstance(g, symbolics.Basic) and (g.shape() in ((3,),(3,1)))
        assert ( isinstance(sensors, list) )
        for s in sensors:
            assert( isinstance(s, Sensor) )
        assert( isinstance(loops, list) )
        for l in loops:
            assert( isinstance(l, Loop) )
        assert( isinstance(diff, list) )
        for d in diff:
            assert( d in [0,1,2] )

        # Call Constructor of Super Class
        MbsElement.__init__(self, name)

        # Save Parameters
        self.graph = graph
        self.inertial = inertial
        self.g = g
        self.sensors = sensors
        self.loops = loops
        self.diff = diff
        self.kinematicsOnly = kinematicsOnly

        # Get a List of All Joints and Bodies
        self.joints, self.bodies, self.fels = self.getJointsBodiesLels(inertial)

        # Check if there are some joints
        if (len(self.joints) < 1):
            raise ValueError('There are no joints connected to the inertial system')

        # Are there any bodies
        if (len(self.bodies) < 2):
            raise ValueError('There are no bodies connected to the inertial system')

        # Get a Vector of All Generalised Positions (q) and Velocities (qd)
        q = []
        q0 = []
        qd = []
        qd0 = []
        qdd = []
        for joint in self.joints:
            q.append(joint.q)
            q0.append(joint.q0)
            qd.append(joint.qd)
            qd0.append(joint.qd0)
            qdd.append(joint.qdd)

        for body in self.bodies:
            # Todo: py3 error
            '''
            if isinstance(body, (Body.FlexibleBody)):
                q.extend(body.q)
                q0.extend(body.q0)
                qd.extend(body.qd)
                qd0.extend(body.qd0)
                qdd.extend(body.qdd)
            '''

        # Save Matrices
        self.q = symbolics.Matrix(q)
        self.q0 = symbolics.Matrix(q0)
        self.qd = symbolics.Matrix(qd)
        self.qd0 = symbolics.Matrix(qd0)
        self.qdd = symbolics.Matrix(qdd)

        # Set Up State
        self.state = State(q, qd, qdd, None)

        # Gather (in-)dependent coordinates
        u = []
        w = []
        for loop in loops:
            u += loop.u     # independent coordinates
            w += loop.v     # dependent coordinates

        # obtain u and v
        self.u = list()
        self.u += q         # take all joint coordinates and remove all dependent ones, while preserving their order
        for dep in w:
            self.u.remove(dep)

        self.w = w

        # update indepence list
        for joint in self.joints:
            if (joint.q in w):
                joint.IsIndependent = False

        # handle constraints and extract state.mu
        self.constraints = [lel for lel in self.fels if isinstance(lel, Constraint)]
        for lel in self.constraints:
            lel.initialize(self.graph, self.state)



    def reduce_shape(self, shape):
        '''
        Return smallest shape possible
        '''
        if (len(shape)==0):
            return shape
        if (len(shape)==1):
            if (shape[0] == 1):
                return tuple()
            else:
                return shape
        if (shape[1] == 1):
            if (shape[0] == 1):
                return tuple()
            else:
                return (shape[0],)
        return shape


    def getJointsBodiesLels(self, body, joints = None, bodies = None, lels = None):
        '''
        Traverse Tree and Collect All Joints, Bodies and Loadelements
        '''

        if (joints is None):
            joints = []

        if  bodies is None:
            bodies = []

        if lels is None:
            lels = []

        children = []
        children += body.children

        # call child body
        body.index = None
        bodies.append(body)

        # are there any force elements
        if hasattr(body, 'loads'):
            for lel in body.loads:
                if (lel not in lels): lels.append(lel)

        i = 0
        # Try every child
        while (i<len(children)):
            childCS = children[i]
            # if there is at least one joint ...
            for joint in childCS.joints:
                # add it to the list
                if ((norm(joint.Psi) > 0) or (norm(joint.Phi) > 0)):
                    joints.append(joint)

                # call child body
                body = joint.body
                body.index = len(bodies)-1
                assert body not in bodies, 'Loop with %s'%body
                bodies.append(body)
                children.extend(body.children)

            # are there any force elements
            for lel in childCS.loads:
                if (lel not in lels): lels.append(lel)

            i = i+1

        return joints, bodies, lels


    def addExpression(self, graph, symbolName, exp, category = None):
        '''
        Add Expression to List of Equations
        '''

        # Make symbol a Symbol and symbolName a string
        if (isinstance(symbolName, str)):
            symbol = sym(symbolName)
        else:
            symbol = symbolName
            symbolName = str(symbolName)

        graph += [Expression(symbolName, symbol, exp, category)]


    def handleConstraints(self, graph):
        '''
        Deals with all Constraints
        '''

        assert( graph is not None )

        # Constraints
        if (len(self.state.mu) > 0):
            phi = []
            for load in self.fels:
                if (isinstance(load, Constraint)):
                    for i in range(len(self.diff)):
                        phi += load.getConstraint(graph, self.diff[i])

            p = graph.addEquation(Phi, phi)

            graph.addEquation(None,p)


    def handleLoops(self, graph):
        '''
        return J and d_prime, such that
        pdd = J*qdd + d_prime

        p: Vector of All Generalised Positions
        q: Vector of independent Positions of all Loops
        w: Vector of dependent Positions of all Loops

        loop.u: Vector of independent Positions of the Loop
        loop.v: Vector of dependent Positions of the Loops
        '''

        # are there any loops
        assert( graph is not None )
        if (len(self.loops) == 0 ):
            return

        # Local Variables
        p = self.state.q    # all variables (p = positions)
        q = self.u          # all independen variables (q = minimal coordinates)
        wd = list()
        wdd = list()

        # Put Loops in the right order
        def SortingLoops(q, L):

            if (len(L) == 0):
                return None

            L0 = []
            L1 = []
            for loop in L:
                for loopu in loop.u:
                    if (isinstance(loopu, (int,float)) or (loopu in q)):
                        if loop not in L0:
                            L0.append(loop)
                    else:
                        if loop not in L1:
                            L1.append(loop)
            for loop in L1:
                if loop in L0:
                    L0.remove(loop)

            w1 = []
            for qelement in q:
                w1.append(qelement)
            for loop in L0:
                for loopw in loop.v:
                    w1.append(loopw)

            l = SortingLoops(w1, L1)
            if (l is None):
                return L0
            else:
                tmp = L0
                tmp.extend(l)
                return tmp

        # putting all Loops in a List
        L=[]
        for loop in self.loops:
            L.append(loop)

        Lsorted = SortingLoops(q, L)

        # Solving Loops in the right order
        def eqnLoops(p, qe, L, graph):
            '''
            padd = J*qedd + d until pdd = J*qdd + d
            '''

            # Stop Criterion
            if (len(L) == 0):
                assert( len(p) == len(qe) )
                p_mat = symbolics.Matrix(p)
                qe_mat = symbolics.Matrix(qe)
                J = jacobian(p_mat,qe_mat)
                d = symbolics.Matrix( p_mat.shape() )
                return J, d

            # Extract Loop
            loop = L[0]
            L = L[1:]

            # get 'local' (in-)dependent coordinates
            u_loc = symbolics.Matrix(loop.u)
            w_loc = symbolics.Matrix(loop.v)

            qa = qe + loop.v
            qa_mat = symbolics.Matrix(qa)
            qe_mat = symbolics.Matrix(qe)


            # Distribution of u and v w.r.t. u_loc and w_loc
            J_uloc_qe = jacobian(u_loc, qe_mat)
            J_qa_wloc = jacobian(qa_mat, w_loc)

            # get Expression for local v, Bvu, b_prime
            (w_exp, Bwu_loc, b_prime_loc) = loop.calc(graph)

			#if (Bwu_loc != Symbolics.Matrix):

            # export Bvu and b_prime to the code
            tmp_bwu = graph.addEquation(BWU%loop.name, Bwu_loc)
            tmp_bprime = graph.addEquation(BPRIME%loop.name, b_prime_loc)

            # for subsequent calculations, discard actual elements in bwu and
            # replace them by references to name_Bwu and name_bprime respectively
            Bwu_loc = self.strip(Bwu_loc,tmp_bwu, BWU%loop.name,graph)
            b_prime_loc = self.strip(b_prime_loc,tmp_bprime, BPRIME%loop.name,graph)

            # distribute local Bwu and b_prime to the global ones
            Bpq = J_qa_wloc*Bwu_loc*J_uloc_qe
            dsorted = J_qa_wloc*b_prime_loc

            # Write Down Equations for Dependent Variables
            for i in range(len(loop.v)):
                graph.addEquation(loop.v[i], w_exp[i])
                # This Line is a complicated expression for obtaining a vector full of zeros except for the i-th position which is one
                wd_pick = symbolics.Matrix((1,len(loop.v)))
                wd_pick[0,i] = 1
                graph.addEquation(loop.vd[i], (wd_pick*Bwu_loc*symbolics.Matrix(loop.ud))[0])
                tmp = (wd_pick*(Bwu_loc*symbolics.Matrix(loop.udd)+b_prime_loc))
                graph.addEquation(loop.vdd[i], tmp[0])

            # Assemble J
            Jpq = jacobian(qa_mat, qe_mat)
            Jsorted = Jpq
            Jsorted += Bpq

            # Call eqnLoops again
            [J1, d1] = eqnLoops(p, qa, L, graph)

            J = J1*Jsorted
            d = J1*dsorted + d1

            return (J, d)

        # Assertion
        for loop in L:
            if (isinstance(loop, ExpJoint) and not self.kinematicsOnly):
                print("**************************************************************************************************")
                print("Warning: ExpJoint is currently only supported for 'world.genEquations(..., kinematicsOnly=True)' !")
                print("-- As a result your system might not behave as expected --")
                print("**************************************************************************************************")

        [J, d_prime] = eqnLoops(p, q, Lsorted, graph)

        #graph += [Expression('J', sym('J'), J, category=VarKind.Sensor)]
        #graph += [Expression('d_prime', sym('d_prime'), d_prime, category=VarKind.Sensor)]

        # remove v from the state ( Vector of All Generalised Positions (q) and Velocities (qd) )
        for loop in Lsorted:
            wd += loop.vd
            wdd += loop.vdd
        for item in self.w: self.state.q.remove(item)
        for item in wd: self.state.qd.remove(item)
        oldq = self.q
        self.q = symbolics.Matrix(self.state.q)
        oldqd = self.qd
        self.qd = symbolics.Matrix(self.state.qd)

        for item in wdd: self.state.qdd.remove(item)
        self.qdd = symbolics.Matrix(self.state.qdd)

        self.q0 = jacobian(self.q,oldq)*self.q0
        self.qd0 = jacobian(self.qd,oldqd)*self.qd0

        return (J, d_prime)




    def strip(self, mat, symbol, symbolname, graph):
        '''
        Replaces all Elements within var by references to name
        using the element method
        '''

        # ToDo: convert var_name into a symbol if a string has been passed

		# Test if mat has shape of Matrix
        assert( isinstance(mat, symbolics.Matrix) )
        assert( isinstance(symbol, symbolics.Symbol) )

        # get Dimension
        shape = mat.shape()
        assert( len(shape) > 0 )
        new_var = symbolics.Matrix(shape)

        if len(shape) == 1:
            for i in range(shape[0]):
                    new_var[i]=graph.addEquation('%s_%s'%(symbolname,i),symbol[i])

        else:
            for i in range(shape[0]):
                for j in range(shape[1]):

                    new_var[i,j]=graph.addEquation('%s_%s_%s'%(symbolname,i,j),symbol[i,j])

        #graph.addEquation(new_var,symbol)

        # create new vector/matrix
#        for m in range(shape[0]):
#            if (len(shape)==1):
#                new_var[m] = mat[m]
#            else:
#                for n in range(shape[1]):
#                    new_var[m,n] = mat[m,n]

        # return
        return new_var