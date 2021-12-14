# -*- coding: utf-8 -*-
'''
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert,
                     Jens Frenkel, Sebastian Voigt
'''

'''
Created on 01.07.2009

@author: Christian Schubert
'''

from PyMbs.Processing.Generator import *
from PyMbs.Processing.LoadElements.Constraint import Constraint

from PyMbs.Symbolics import Graph, SymmetricMatrix

from PyMbs.Common.Functions import skew, transpose, solve, der, scalar_if_possible, scalar, vector_if_possible

from PyMbs.Symbolics import zeros, Matrix

from PyMbs.Processing.Body import Body
from PyMbs.Processing.Body import FlexibleBody

import PyMbs.Processing.Sensors.EnergySensor

import numpy as np


class Generator_Recursive(Generator):
    '''
    Generator That Calculates the Equations of Motion in Recursive Form
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

        # Create Empty List of Equations
        graph = self.graph

        # Calculate Body State
        self.start1stForwardRecursion(graph)

        # Calculate Sensors (before accelerations)
        for sens in self.sensors:
            if (not sens.delayCalc):
                if (isinstance(sens, PyMbs.Processing.Sensors.EnergySensor)):
                    sens.gravity = -self.inertial._alpha_C;
                sens.calc(graph)

        # Calculate Load Values
        self.getLoadValues(graph, self.fels)

        # Obtain Equations
        self.startBackwardRecursion(graph, self.joints)
        self.start2ndForwardRecursion(graph, self.joints, self.fels, self.loops)

        # Calculate Sensors (after accelerations)
        for sens in self.sensors:
            if (sens.delayCalc):
                if (isinstance(sens, PyMbs.Processing.Sensors.EnergySensor)):
                    sens.gravity = -self.inertial._alpha_C;
                sens.calc(graph)

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

            # Make placeholders
            body._A_M = dict()
            body._O_M = dict()
            body._O_M_tilde = dict()
            body._F_M = dict()
            body._L_M = dict()

            if (body.joint is None):
                # Set Position and Angular Velocity to Zero, Rotational Matrix = Identity
                body.I_r = graph.addEquation(I_p%bName, Symbolics.zeros((3,)), shape=(3,))
                body.I_v = graph.addEquation(I_v%bName, Symbolics.zeros((3,)), shape=(3,))

                if (not self.kinematicsOnly):
                    body.I_a = graph.addEquation(I_a%bName, Symbolics.zeros((3,)), shape=(3,))

                body.I_R = graph.addEquation(T1%bName, Symbolics.eye((3,3)), shape=(3,3))
                # K_om is written w.r.t. the Inertial Frame!!!
                body.I_om = graph.addEquation(omega%bName, Symbolics.zeros((3,)), shape=(3,))
                body._I_om_tilde = graph.addEquation(omega_tilde%bName, zeros((3,3)), shape=(3,3))

                if (not self.kinematicsOnly):
                    body.I_al = graph.addEquation(omegad%bName, Symbolics.zeros((3,)), shape=(3,))
                    body._I_al_tilde = graph.addEquation(omegad_tilde%bName, zeros((3,3)), shape=(3,3))

                # Initial Values
                body._omegad_C = graph.addEquation(omegad_C%bName, Symbolics.zeros((3,)), shape=(3,))
                body._beta_C = graph.addEquation(beta_C%bName, zeros((3,3)), shape=(3,3))
                body._alpha_C = graph.addEquation(alpha_C%bName, -self.g, shape=(3,))
            elif isinstance(body, Body):
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
                Phi_loc_b = graph.addEquation(PHI_LOC%bName, joint.Phi, shape=(3,))
                body._Phi = graph.addEquation(PHI%bName, T_cs*Phi_loc_b, shape=(3,))
                Psi_loc_b = graph.addEquation(PSI_LOC%bName, joint.Psi, shape=(3,))
                body._Psi = graph.addEquation(PSI%bName, T_cs*Psi_loc_b, shape=(3,))

                p_z_p_b = graph.addEquation(p_z%(pName,bName), cs._p+body._Psi*joint.q, shape=(3,))
                body._p_z_tilde = graph.addEquation(p_z_tilde%(pName,bName), skew(p_z_p_b), shape=(3,3))

                # Mass and Inertia
                body.mass = graph.addEquation(m%bName, body.mass, shape=())
                I_loc_b = graph.addEquation(I_LOC%bName, body.inertia, shape=(3,3))
                # update body.inertia (reference to this variable)
                body.inertia = graph.addEquation(I%bName, body.I_R * I_loc_b * transpose(body.I_R), shape=(3,3))

                # Position
                body.I_r = graph.addEquation(I_p%bName, parentBody.I_r + p_z_p_b, shape=(3,))

                # Omega (Angular Velocity)
                # K_om is written w.r.t. the Inertial Frame!!!
                body.I_om = graph.addEquation(omega%bName, parentBody.I_om + joint.qd*body._Phi, shape=(3,)) # (18)
                body._I_om_tilde = graph.addEquation(omega_tilde%bName, skew(body.I_om), shape=(3,3))

                # Velocity
                body.I_v = graph.addEquation(I_v%bName, parentBody.I_v + parentBody._I_om_tilde*p_z_p_b + joint.qd*body._Psi, shape=(3,))

                # These are needed for the recursive algorithm

                body._omegad_C = graph.addEquation(omegad_C%bName, parentBody._omegad_C + body._I_om_tilde*body._Phi*joint.qd, shape=(3,)) # (19)
                omegad_C_tilde_b = graph.addEquation(omegad_C_tilde%bName, skew(body._omegad_C), shape=(3,3))
                body._beta_C = graph.addEquation(beta_C%bName, body._I_om_tilde*body._I_om_tilde + omegad_C_tilde_b, shape=(3,3)) # (20)
                body._alpha_C = graph.addEquation(alpha_C%bName, parentBody._alpha_C + parentBody._beta_C*p_z_p_b + 2*body._I_om_tilde*body._Psi*joint.qd, shape=(3,)) # (21)

                O_M_b_b = graph.addEquation(O_M%(bName,bName), body._Phi, shape=(3,)) # (22a)
                body._O_M[body] = O_M_b_b
                body._O_M_tilde[body] = graph.addEquation(O_M_tilde%(bName,bName), skew(body._Phi), shape=(3,3)) # (22b)
                A_M_b_b = graph.addEquation(A_M%(bName,bName), body._Psi, shape=(3,)) # (23a)
                body._A_M[body] = A_M_b_b

                # Translational and Angular Acceleration
                alpha = A_M_b_b*joint.qdd
                omegadot = O_M_b_b*joint.qdd

                # Go into direction of world
                temp = parentBody;
                while (temp.joint is not None):
                    tName = temp.name;
                    O_M_p_t = parentBody._O_M[temp]
                    O_M_b_t = graph.addEquation(O_M%(bName,tName), O_M_p_t, shape=(3,)) # (22c)
                    body._O_M[temp] = O_M_b_t
                    #body._O_M_tilde[temp] = graph.addEquation(O_M_tilde%(bName,tName), skew(O_M_b_t), shape=(3,3)) # (22d)
                    body._O_M_tilde[temp] = graph.addEquation(O_M_tilde%(bName,tName), parentBody._O_M_tilde[temp], shape=(3,3)) # (22d)
                    O_M_tilde_p_t = parentBody._O_M_tilde[temp]
                    A_M_p_t = parentBody._A_M[temp]
                    A_M_b_t = graph.addEquation(A_M%(bName,tName), A_M_p_t + O_M_tilde_p_t*p_z_p_b, shape=(3,)) # (23b)
                    body._A_M[temp] = A_M_b_t

                    alpha += A_M_b_t*temp.joint.qdd
                    omegadot += O_M_b_t*temp.joint.qdd

                    # GoTo Next Body
                    temp = temp.joint.coordSys.parentBody

                alpha += body._alpha_C
                omegadot += body._omegad_C

                # Add these Expressions to the List of Equations
                body.I_al = graph.addEquation(omegad%bName, omegadot, shape=(3,))
                body._I_al_tilde = graph.addEquation(omegad_tilde%bName, skew(body.I_al), shape=(3,3))
                body.I_a = graph.addEquation(I_a%bName, alpha - self.inertial._alpha_C, shape=(3,) ) # self.inertial._alpha_C = -g, see below eq (10), p.189

                # Update cg
                l_LOC_b = graph.addEquation(l_LOC%bName, body.cg, shape=(3,))
                body._l = graph.addEquation(l%bName, body.I_R * l_LOC_b, shape=(3,))
                body._l_tilde = graph.addEquation(l_tilde%bName, skew(body._l), shape=(3,3))

                body.I_l = graph.addEquation(I_l%bName, body.I_r + body._l, shape=(3,))
                body.I_ldot = graph.addEquation(I_ldot%bName, body.I_v + body._I_om_tilde*body._l, shape=(3,))

                body._I_l_ddot = graph.addEquation(I_lddot%bName, body.I_a + (body._I_al_tilde + body._I_om_tilde**2)*body._l, shape=(3,))

            elif (isinstance(body, FlexibleBody)):

                joint = body.joint
                jName = joint.name
                cs = joint.coordSys
                parentBody = cs.parentBody
                pName = parentBody.name


                # Joint Properties
                T_cs = cs.I_R
                T_j = graph.addEquation(T1%jName, joint.R)
                body.I_R = graph.addEquation(T1%bName, T_cs*T_j, shape=(3,3))
                Phi_loc_b = graph.addEquation(PHI_LOC%bName, joint.Phi, shape=(3,))
                # axis of rotation (Inertial frame)
                body._Phi = graph.addEquation(PHI%bName, T_cs*Phi_loc_b, shape=(3,))
                Psi_loc_b = graph.addEquation(PSI_LOC%bName, joint.Psi, shape=(3,))
                # axis of translation (Inertial-frame)
                body._Psi = graph.addEquation(PSI%bName, T_cs*Psi_loc_b, shape=(3,))

                # vector from parentbody to actual body
                p_z_p_b = graph.addEquation(p_z%(pName,bName), cs._p+body._Psi*joint.q, shape=(3,))
                body._p_z_tilde = graph.addEquation(p_z_tilde%(pName,bName), skew(p_z_p_b), shape=(3,3))

                # Position body (bodyframe)
                body.I_r = graph.addEquation(I_p%bName, parentBody.I_r + p_z_p_b, shape=(3,))

                # Omega (Angular Velocity)
                # K_om is written w.r.t. the Inertial Frame!!!
                body.I_om = graph.addEquation(omega%bName, parentBody.I_om + joint.qd*body._Phi, shape=(3,)) # (18)
                body._I_om_tilde = graph.addEquation(omega_tilde%bName, skew(body.I_om), shape=(3,3))

                 # Velocity
                body.I_v = graph.addEquation(I_v%bName, parentBody.I_v + parentBody._I_om_tilde*p_z_p_b + joint.qd*body._Psi, shape=(3,))


                # These are needed for the recursive algorithm

                body._omegad_C = graph.addEquation(omegad_C%bName, parentBody._omegad_C + body._I_om_tilde*body._Phi*joint.qd, shape=(3,)) # (19)
                omegad_C_tilde_b = graph.addEquation(omegad_C_tilde%bName, skew(body._omegad_C), shape=(3,3))
                body._beta_C = graph.addEquation(beta_C%bName, body._I_om_tilde*body._I_om_tilde + omegad_C_tilde_b, shape=(3,3)) # (20)
                body._alpha_C = graph.addEquation(alpha_C%bName, parentBody._alpha_C + parentBody._beta_C*p_z_p_b + 2*body._I_om_tilde*body._Psi*joint.qd, shape=(3,)) # (21)

                O_M_b_b = graph.addEquation(O_M%(bName,bName), body._Phi, shape=(3,)) # (22a)
                body._O_M[body] = O_M_b_b
                body._O_M_tilde[body] = graph.addEquation(O_M_tilde%(bName,bName), skew(body._Phi), shape=(3,3)) # (22b)
                A_M_b_b = graph.addEquation(A_M%(bName,bName), body._Psi, shape=(3,)) # (23a)
                body._A_M[body] = A_M_b_b

                # Translational and Angular Acceleration
                alpha = A_M_b_b*joint.qdd
                omegadot = O_M_b_b*joint.qdd

                # Go into direction of world
                temp = parentBody;
                while (temp.joint is not None):
                    tName = temp.name;
                    O_M_p_t = parentBody._O_M[temp]
                    O_M_b_t = graph.addEquation(O_M%(bName,tName), O_M_p_t, shape=(3,)) # (22c)
                    body._O_M[temp] = O_M_b_t
                    #body._O_M_tilde[temp] = graph.addEquation(O_M_tilde%(bName,tName), skew(O_M_b_t), shape=(3,3)) # (22d)
                    body._O_M_tilde[temp] = graph.addEquation(O_M_tilde%(bName,tName), parentBody._O_M_tilde[temp], shape=(3,3)) # (22d)
                    O_M_tilde_p_t = parentBody._O_M_tilde[temp]
                    A_M_p_t = parentBody._A_M[temp]
                    A_M_b_t = graph.addEquation(A_M%(bName,tName), A_M_p_t + O_M_tilde_p_t*p_z_p_b, shape=(3,)) # (23b)
                    body._A_M[temp] = A_M_b_t

                    alpha += A_M_b_t*temp.joint.qdd
                    omegadot += O_M_b_t*temp.joint.qdd

                    # GoTo Next Body
                    temp = temp.joint.coordSys.parentBody

                alpha += body._alpha_C
                omegadot += body._omegad_C

                # Add these Expressions to the List of Equations
                body.I_al = graph.addEquation(omegad%bName, omegadot, shape=(3,))
                body._I_al_tilde = graph.addEquation(omegad_tilde%bName, skew(body.I_al), shape=(3,3))
                body.I_a = graph.addEquation(I_a%bName, alpha - self.inertial._alpha_C, shape=(3,) )

                # Update cg
                l_LOC_b = graph.addEquation(l_LOC%bName, body.cg, shape=(3,))
                body._l = graph.addEquation(l%bName, body.I_R * l_LOC_b, shape=(3,))
                body._l_tilde = graph.addEquation(l_tilde%bName, skew(body._l), shape=(3,3))

                body.I_l = graph.addEquation(I_l%bName, body.I_r + body._l, shape=(3,))
                body.I_ldot = graph.addEquation(I_ldot%bName, body.I_v + body._I_om_tilde*body._l, shape=(3,))

                body._I_l_ddot = graph.addEquation(I_lddot%bName, body.I_a + (body._I_al_tilde + body._I_om_tilde**2)*body._l, shape=(3,))


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

                    if (node.phi.phimatrix.nrow != 3) or (node.psi.psimatrix.nrow != 3):
                        raise NotImplementedError('PHI and PSI matrices must have 3 rows')

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

##            if (not isinstance(body,FlexibleBody)):

            for childCS in body.children:
                csName = childCS.name
                # Do the Update
                T2_b_cs = graph.addEquation(T2%(bName,csName), childCS.R, shape=(3,3))
                childCS.I_R = graph.addEquation(T1%('%s_%s'%(bName,csName)), body.I_R*T2_b_cs, shape=(3,3))

                p_loc_b_cs = graph.addEquation(p_LOC%(bName,csName), childCS.p, shape=(3,))
                childCS._p = graph.addEquation(p%(bName,csName), body.I_R*p_loc_b_cs, shape=(3,))
                childCS._p_tilde = graph.addEquation(p_tilde%(bName,csName), skew(childCS._p), shape=(3,3) )
                childCS.I_r = graph.addEquation(I_p%('%s_%s'%(bName,csName)), body.I_r + childCS._p, shape=(3,))
                childCS.I_v = graph.addEquation(I_v%('%s_%s'%(bName,csName)), body.I_v + body._I_om_tilde*childCS._p, shape=(3,))

                if (not self.kinematicsOnly):
                    childCS.I_a = graph.addEquation(I_a%('%s_%s'%(bName,csName)), body.I_a + (body._I_al_tilde + body._I_om_tilde**2)*childCS._p, shape=(3,))

                # K_om is written w.r.t. the Inertial Frame!!!
                childCS.I_om = body.I_om

                if (not self.kinematicsOnly):
                    childCS.I_al = body.I_al    # omegad_BodyName here, since it is equal to omegad_csName anyway

            else:
                for childCS in body.children:
                    csName = childCS.name



    def getLoadValues(self, graph, fels):
        '''
        Write All Equations needed for loads
        '''

        for load in fels:
            lName = load.name
            load.calc(graph, self.state)

            if (load.ref is None):
                T_cs = 1
            else:
                T_cs = load.ref.I_R

            # Force
            if (load.force is None):
                load._F = Symbolics.zeros((3,))
            else:
                Wf = graph.addEquation(WF%lName, load.Wf)
                load._F = graph.addEquation(F%lName, T_cs*Wf*load.force, shape=(3,))
            # Torque
            if (load.torque is None):
                load._L = Symbolics.zeros((3,))
            else:
                Wt = graph.addEquation(WT%lName, load.Wt)
                load._L = graph.addEquation(L%lName, T_cs*Wt*load.torque, shape=(3,))


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

            # External Force/Torque Vector
            f_ext = 0
            l_ext = 0
            for child in body.children:
                for load in child.loads:
                    # Load must have this Coordinate System (child) as a parent or a child!
                    assert (child in (load.parent, load.child))
                    f_local = load._F
                    l_local = load._L
                    l_local = child._p_tilde*f_local + l_local
                    if (child == load.parent):
                        f_ext += f_local
                        l_ext += l_local
                    else: # (child == load.child)
                        f_ext -= f_local
                        l_ext -= l_local

            if (f_ext == 0): f_ext = Symbolics.zeros((3,))
            if (l_ext == 0): l_ext = Symbolics.zeros((3,))
            body._F_ext = graph.addEquation(F_EXT%bName, f_ext, shape=(3,))
            body._L_ext = graph.addEquation(L_EXT%bName, l_ext, shape=(3,))

            # Reactive Loads
            body._G_C = graph.addEquation(G_C%bName, body.mass*(body._alpha_C+body._beta_C*body._l), shape=(3,)) # (31) - modified
            # Equation 24, 26
            F_b = 0
            L_b = 0
            # Equation 32
            F_C_b = 0
            for child in body.children:
                for tmpJoint in child.joints:
                    cBody = tmpJoint.body;
                    F_C_b += cBody._F_C
                    F_b += cBody._F
                    p_z_tilde_b_c = cBody._p_z_tilde
                    L_b += cBody._L + (p_z_tilde_b_c-body._l_tilde)*cBody._F

            F_C_b += body._G_C - body._F_ext # (32) - modified
            body._F_C = graph.addEquation(F_C%bName, F_C_b, shape=(3,))

            F_b += body.mass*(body._I_l_ddot+self.inertial._alpha_C)-body._F_ext   # (24), self.inertial._alpha_C = -g, see p.189 below (10)
            body._F = graph.addEquation(F%bName, F_b, shape=(3,))
            L_b += body._l_tilde*body._F + \
                   body.inertia*body.I_al + \
                   body._I_om_tilde*body.inertia*body.I_om - \
                   body._L_ext   # (26)
            body._L = graph.addEquation(L%bName, L_b, shape=(3,))

            # Save result in joint
            joint.F = body._F
            joint.L = body._L

            # Equation 33
            L_C_b = 0
            for child in body.children:
                for tmpJoint in child.joints:
                    cBody = tmpJoint.body;
                    p_z_tilde_b_c = cBody._p_z_tilde
                    L_C_b += (cBody._L_C + p_z_tilde_b_c*cBody._F_C)

            L_C_b += body._l_tilde * body._G_C - body._L_ext + body.inertia * body._omegad_C + body._I_om_tilde*body.inertia*body.I_om # (33)
            body._L_C = graph.addEquation(L_C%bName, L_C_b, shape=(3,))

            temp = body;
            while (temp.joint is not None):
                tName = temp.name;

                A_M_b_t = body._A_M[temp]
                O_M_tilde_b_t = body._O_M_tilde[temp]
                G_M_b_t = graph.addEquation(G_M%(bName,tName), body.mass *( A_M_b_t+O_M_tilde_b_t*body._l), shape=(3,)) # (34)
                ## Equation 35
                F_M_bName_tName = 0
                for child in body.children:
                    for tmpJoint in child.joints:
                        cBody = tmpJoint.body
                        F_M_c_t = cBody._F_M[temp]
                        F_M_bName_tName += F_M_c_t

                F_M_bName_tName += G_M_b_t # (35)
                body._F_M[temp] = graph.addEquation(F_M%(bName,tName), F_M_bName_tName, shape=(3,))


                ## Equation 36
                L_M_bName_tName = 0
                for child in body.children:
                    for tmpJoint in child.joints:
                        cBody = tmpJoint.body
                        L_M_c_t = cBody._L_M[temp]
                        F_M_c_t = cBody._F_M[temp]
                        L_M_bName_tName += ( L_M_c_t + cBody._p_z_tilde*F_M_c_t )

                O_M_b_t = body._O_M[temp]
                L_M_bName_tName += body._l_tilde*G_M_b_t + body.inertia*O_M_b_t # (36)
                body._L_M[temp] = graph.addEquation(L_M%(bName,tName), L_M_bName_tName, shape=(3,))

                # GoTo Next Body
                temp = temp.joint.coordSys.parentBody;


    def start2ndForwardRecursion(self, graph, joints, fels, loops):
        # 3rd Forward Recursion
        n = len(joints)
        M_red = Matrix((n,n))
        C_red = Matrix((n,))
        for joint in joints:

            # Abbreviations
            body = joint.body
            i = body.index

            # C-Vector
            C_red[i] = graph.addEquation(C%i, transpose(body._Psi)*body._F_C + transpose(body._Phi)*body._L_C, shape=()) # (37)

            # M
            temp = body;
            while (temp.joint is not None):
                j = temp.index;
                F_M_b_t = body._F_M[temp]
                L_M_b_t = body._L_M[temp]
                M_red[i,j] = graph.addEquation(M%(i,j), transpose(body._Psi)*F_M_b_t + transpose(body._Phi)*L_M_b_t, shape=()) # (37)
                if (i != j):
                    M_red[j,i] = M_red[i,j]
                # GoTo Next Body
                temp = temp.joint.coordSys.parentBody;

        # Loops
        if (len(self.loops) > 0):
            (J, b_prime) = self.handleLoops(graph)

            # Reduce Mass Matrix again
            JT = transpose(J)
            M_red2 = JT*M_red*J
            C_red2 = JT*(M_red*b_prime + C_red)

            #M_red2 = SymmetricMatrix(M_red2)
            M_ = graph.addEquation('M_', scalar_if_possible(M_red2))
            C_ = graph.addEquation('C_', scalar_if_possible(C_red2))

        # No Loops
        else:
            #M_red = SymmetricMatrix(M_red)
            M_ = graph.addEquation('M_', scalar_if_possible(M_red))
            C_ = graph.addEquation('C_', scalar_if_possible(C_red))


        # Define State variables
        q = graph.addVariable(name='q', shape=self.reduce_shape( self.q.shape() ), initialValue=scalar_if_possible(self.q0) )
        qd = graph.addVariable(name='qd', shape=self.reduce_shape( self.qd.shape() ), initialValue=scalar_if_possible(self.qd0) )
        if (self.kinematicsOnly):
            qdd = graph.addVariable(name='qdd', shape=self.reduce_shape( self.qdd.shape() ),varKind=VarKind.Input)
        else:
            qdd = graph.addVariable(name='qdd', shape=self.reduce_shape( self.qdd.shape() ) )
        graph.addEquation(self.state.q, q)
        graph.addEquation(self.state.qd, qd)
        graph.addEquation(self.state.qdd, qdd)

        if (self.kinematicsOnly):
            pass
#            der_qd = graph.addEquation(qdd, zeros(qdd.shape()), varKind = VarKind.Der_State)
        else:
            if (len(self.state.q) > 1):
                der_qd = graph.addEquation(qdd, solve(M_, -C_), varKind = VarKind.Der_State)
                #der_qd = graph.addEquation(C_, -M_*qdd, varKind = VarKind.Der_State)
            else:
                der_qd = graph.addEquation(qdd, -C_/M_, varKind = VarKind.Der_State)

        graph.addEquation(der(q), qd)
        graph.addEquation(der(qd), qdd)

        self.handleConstraints(graph)

