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
Created on 13.07.2009

@author: Christian Schubert
'''

from PyMbs.Processing.Loops.Loop import Loop
from PyMbs.Common.Functions import sqrt
from PyMbs.Processing.Joint import Joint
from PyMbs.Processing.Frame import Frame

from PyMbs.Processing.LoadElements import Constraint

from scipy import pi

from PyMbs.Symbolics import Matrix, eye, cos, sin, atan, atan2, acos, zeros, transpose

AL = 'FB_%s_AL'
BE = 'FB_%s_BE'
GA = 'FB_%s_GA'
DE = 'FB_%s_DE'

L1 = 'FB_%s_L1'
L2 = 'FB_%s_L2'
L3 = 'FB_%s_L3'
L4 = 'FB_%s_L4'

PHI = 'FB_%s_PHI'
PSI = 'FB_%s_PSI'
THETA = 'FB_%s_THETA'

A = 'FB_%s_A'
B = 'FB_%s_B'
C = 'FB_%s_C'

D = 'FB_%s_D'
E = 'FB_%s_E'
F = 'FB_%s_F'

from PyMbs.Symbolics import Graph

class FourBar(Loop):
    '''
    Explicit Treatment of a Four Bar Linkage Mechanism
    '''

    '''
    Sketch:
       B--3--C
      /       \
     2        4
    /          \
   A-----1------D
    '''
    def __init__(self, name, csCa, csCb, posture):
        '''
        Constructor
        @param setup: Four Bar Linkage has two setups: -1, 1
        '''

        # Assertions
        assert ( isinstance(csCa, Frame) )
        assert ( isinstance(csCb, Frame) )
        assert ( isinstance(posture, int) )
        assert ( (posture == 1) or (posture == -1 ))

        # Check parents
        if (csCa.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCa.name))
        if (csCb.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCb.name))

        # Build complete FourBarLinkage
        jB = csCa.parentBody.joint
        jD = csCb.parentBody.joint
        if (jB.coordSys.parentBody.joint == None):
            jB = csCb.parentBody.joint
            jD = csCa.parentBody.joint
            jA = jB.coordSys.parentBody.joint
            csC3 = csCb
            csC4 = csCa
        else:
            jA = jB.coordSys.parentBody.joint
            csC3 = csCa
            csC4 = csCb


        # Do the Joints have the same axis of Rotation
        if (jA.Phi == Matrix([1,0,0])):
            self.sign = 1
            self.pick = Matrix([[0,1,0],
                                [0,0,1]])
        elif (jA.Phi == Matrix([0,1,0])):
            self.sign = -1
            self.pick = Matrix([[1,0,0],
                                [0,0,1]])
        elif (jA.Phi == Matrix([0,0,1])):
            self.sign = 1
            self.pick = Matrix([[1,0,0],
                                [0,1,0]])
        else:
            raise ValueError('Loop "%s": Axis of Rotation must be either x,y or z!'%name)

        assert( jA.Phi == jB.Phi ), 'jA.Phi(%s): %s, jB.Phi(%s): %s'%(jA.name,jA.Phi,jB.name,jB.Phi)
        assert( jA.Phi == jD.Phi ), 'jA.Phi(%s): %s, jD.Phi(%s): %s'%(jA.name,jA.Phi,jD.name,jD.Phi)
        assert( jA.Psi.norm() == 0 )
        assert( jB.Psi.norm() == 0 )
        assert( jD.Psi.norm() == 0 )

        # Are All Coordinate Systems aligned like their parentBody?
        assert( (jA.coordSys.R - eye(3)) == zeros(3) )
        assert( (jB.coordSys.R - eye(3)) == zeros(3) )
        assert( (jD.coordSys.R - eye(3)) == zeros(3) )

        # Check that bodies between joints are the same
        assert( jA.coordSys.parentBody == jD.coordSys.parentBody )
        assert( jA.body == jB.coordSys.parentBody )
        assert( jB.body == csC3.parentBody )
        assert( jD.body == csC4.parentBody )

        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.jA = jA
        self.jB = jB
        self.jD = jD
        self.csC3 = csC3
        self.csC4 = csC4
        self.posture = posture

        # Independent Coordinates
        self.u = [jA.q]
        self.ud = [jA.qd]
        self.udd = [jA.qdd]

        # Dependent Coordinates
        self.v = [jB.q, jD.q]
        self.vd = [jB.qd, jD.qd]
        self.vdd = [jB.qdd, jD.qdd]

    def calc(self, graph):
        '''
        Returns precalculated v(u), Bvu and b_prime, s.t.
        q = [u,v]', where
            u: independent coordinates
            v: dependent coordinates
        Starting from the Constraint Equation: Phi(q) = 0,
        One Obtains by Differentiation:
        (d(Phi)/du)*u_dot + (d(Phi)/dv)*v_dot = 0
        Ju*u_dot + Jv+v_dot = 0
        Thus, v_dot = -(inv(Jv)*Ju)*u_dot = Bvu*u_dot, with Jv = d(Phi)/dv and Ju = d(Phi)/du
        Differentiating once more, yields
        Ju*u_ddot + Jv*v_ddot + [Ju_dot, Jv_dot]*[u_dot,v_dot]' = 0
        Ju*u_ddot + Jv*v_ddot + J_dot*q_dot = 0
        Using this relations, one may obtain an expression for v_ddot
        v_ddot = -(inv(Jv)*Ju)*u_ddot - inv(Jv)*J_dot*q_dot
               = Bvu*u_ddot + b_prime, with b_prime = -inv(Jv)*J_dot*q_dot

        Finally one can transform the Equation of Motion
        M*q_ddot + h = f + W'*mu
        M*(J*u_ddot + b) + h = f + W'*mu    with J = [1, Bvu']' and b = [0,b_prime']'
        (J'*M*J)*u_ddot + J'*M*b + J'*h = J'*f, since J'*W' = 0
        M_star*u_ddot + h_star = f_star
        M_star = (J'*M*J)
        h_star = J'*M*b + J'*h
        f_star = J'*f
        '''

        assert isinstance(graph, Graph)

        # Abbrevations
        s = self.sign

        # Generalised Coordinates
        q1 = self.jA.q     # u[0]    # angle between x-axes
        q1d = self.jA.qd
        q2 = self.jB.q     # v[0]    # angle between x-axes
        q2d = self.jB.qd
        q3 = self.jD.q     # v[1]    # angle between x-axes
        q3d = self.jD.qd

        # Length of bars and angle between x-axis and bar
        l1_vec = self.jD.coordSys.p - self.jA.coordSys.p
        l1_vec2 = self.pick*l1_vec
        l1 = graph.addEquation(L1%self.name, sqrt((transpose(l1_vec)*l1_vec)))
        alpha = graph.addEquation(AL%self.name, s*atan2(l1_vec2[1],l1_vec2[0]))

        l2_vec = self.jB.coordSys.p
        l2_vec2 = self.pick*l2_vec
        l2 = graph.addEquation(L2%self.name, sqrt((transpose(l2_vec)*l2_vec)))
        beta = graph.addEquation(BE%self.name, s*atan2(l2_vec2[1],l2_vec2[0]))

        l3_vec = self.csC3.p
        l3_vec2 = self.pick*l3_vec
        l3 = graph.addEquation(L3%self.name, sqrt((transpose(l3_vec)*l3_vec)))
        gamma = graph.addEquation(GA%self.name, s*atan2(l3_vec2[1],l3_vec2[0]))

        l4_vec = self.csC4.p
        l4_vec2 = self.pick*l4_vec
        l4 = graph.addEquation(L4%self.name, sqrt((transpose(l4_vec)*l4_vec)))
        delta = graph.addEquation(DE%self.name, s*atan2(l4_vec2[1],l4_vec2[0]))

        # angle between bars
        phi_prime = graph.addEquation(PHI%self.name, q1 + beta - alpha)

        # A = P1, B = P2, C = P3
        #P1 = graph.addEquation(A%self.name, 2*l4*(l1-l2*cos(phi_prime)))
        #P2 = graph.addEquation(B%self.name, -2*l2*l4*sin(phi_prime))
        #P3 = graph.addEquation(C%self.name, l1**2+l2**2-l3**2+l4**2-2*l1*l2*cos(phi_prime))

        # D = P1, E = P2, F = P3
        P4 = graph.addEquation(D%self.name, 2*l3*(l2-l1*cos(-phi_prime)))
        P5 = graph.addEquation(E%self.name, -2*l1*l3*sin(-phi_prime))
        P6 = graph.addEquation(F%self.name, l2**2+l1**2-l4**2+l3**2-2*l2*l1*cos(-phi_prime))

        # Calculate v
        theta_prime = graph.addEquation(THETA%self.name, 2*atan((P5-self.posture*sqrt(P4**2+P5**2-P6**2))/(P4-P6)))
        psi_prime = graph.addEquation(PSI%self.name, ((l2*sin(phi_prime)+l3*sin(phi_prime+theta_prime))/abs(l2*sin(phi_prime)+l3*sin(phi_prime+theta_prime)))*acos((l2*cos(phi_prime)+l3*cos(phi_prime+theta_prime)-l1)/l4))

        v1 = (psi_prime + alpha - delta)
        v0 = (theta_prime + beta - gamma)

        Bvu = Matrix( [[-l2*sin(phi_prime-psi_prime)/(l3*sin(phi_prime+theta_prime-psi_prime))-1], [(l2*sin(theta_prime))/(l4*sin(phi_prime+theta_prime-psi_prime))]] )

        b_prime = Matrix( [-(q1d**2*l2*cos(phi_prime-psi_prime)+l3*cos(phi_prime+theta_prime-psi_prime)*(q1d+q2d)**2-l4*q3d**2)/(l3*sin(phi_prime+theta_prime-psi_prime)) , -(q1d**2*l2*cos(theta_prime)+l3*(q1d+q2d)**2-l4*q3d**2*cos(phi_prime+theta_prime-psi_prime))/(l4*sin(phi_prime+theta_prime-psi_prime)) ] )

        return ([v0,v1],Bvu,b_prime)



    def applyConstraintLoads(self):
        '''
        apply Constraint Forces at the end of the cut
        '''

        # locking all directions perpendicular to axis of rotation
        transLock = [0,0,0]
        for i in [0,1,2]:
            if (self.jA.Phi[i] == 0):
                transLock[i] = 1

        # apply Constraint
        c = Constraint(name='Constraint_%s'%self.name, parent=self.csC3, child=self.csC4, \
                       transLock=transLock, rotLock=[0,0,0], active=False)

        # return load object
        return c

