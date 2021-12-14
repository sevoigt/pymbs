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

from PyMbs.Processing.Loops.Loop import Loop, eps
from PyMbs.Common.Functions import sqrt, transpose

from PyMbs.Processing.Joint import Joint
from PyMbs.Processing.Frame import Frame

from scipy import pi

from PyMbs.Symbolics import Matrix, eye, cos, sin, atan, atan2, acos, zeros

AL = 'FB_%s_AL'
BE = 'FB_%s_BE'
GA = 'FB_%s_GA'

L1 = 'FB_%s_L1'
L2 = 'FB_%s_L2'
L3 = 'FB_%s_L3'

PHI = 'FB_%s_PHI'
PSI = 'F_%s_PSI'



class ThreeBarTrans(Loop):
    '''
    Explicit Treatment of a Three Bar Linkage Mechanism with Slider
    '''

    '''
    Sketch:
          B
        /   \
       /    3a
      2      \
     /        3b
    /          \
   A-----1-----C
    '''
    def __init__(self, name, csBa, csBb):

        '''
        Constructor
        @param setup: Four Bar Linkage has two setups: -1, 1
        '''

        # Assertions
        assert ( isinstance(csBa, Frame) )
        assert ( isinstance(csBb, Frame) )

        # Check parents
        if (csBa.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csBa.name))
        if (csBb.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csBb.name))

        # Build complete ThreeBarLinkage
        jA = csBa.parentBody.joint
        jCB = csBb.parentBody.joint
        if (jCB.coordSys.parentBody.joint == None):
            jA = csBb.parentBody.joint
            jCB = csBa.parentBody.joint
            jC = jCB.coordSys.parentBody.joint
            csB2 = csBb
            csB3a = csBa
        else:
            jC = jCB.coordSys.parentBody.joint
            csB2 = csBa
            csB3a = csBb



        # Do the Joints have the same axis of Rotation and lie in the same plane
        jB = jA.coordSys.p+csB2.p
        if (jA.Phi == Matrix([1,0,0])):
            delta = jB[0] - jC.coordSys.p[0];
            if ( abs(delta) < eps ):
                self.sign = 1
                self.pick = Matrix([[0,1,0],
                                    [0,0,1]])
            else:
                raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for x. There is a difference of %f'%(name,delta))
        elif (jA.Phi == Matrix([0,1,0])):
            delta = jB[1] - jC.coordSys.p[1]
            if ( abs(delta) < eps ):
                self.sign = -1
                self.pick = Matrix([[1,0,0],
                                    [0,0,1]])
            else:
                raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for y. There is a difference of %f'%(name,delta))
        elif (jA.Phi == Matrix([0,0,1])):
            delta = jB[2] - jC.coordSys.p[2]
            if (abs(delta) < eps):
                self.sign = 1
                self.pick = Matrix([[1,0,0],
                                    [0,1,0]])
            else:
                raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for z. There is a difference of %f'%(name,delta))
        else:
            raise ValueError('Loop "%s": Axis of Rotation must be either x,y or z!'%name)

        assert jA.Phi == jC.Phi, "jA.Phi and jC.Phi were supposed to be equal but %s != %s"%(jA.Phi,jC.Phi)
        assert jA.Psi.norm() == 0, "The norm of jA.Psi should be zero but is %s"%(jA.Psi.norm())
        assert jC.Psi.norm() == 0, "The norm of jC.Psi should be zero but is %s"%(jC.Psi.norm())
        assert jCB.Phi.norm() == 0, "The norm of jCB.Phi should be zero but is %s"%(jCB.Phi.norm())
        assert jCB.Psi.norm() == 1, "The norm of jCB.Psi should be one but is %s"%(jCB.Psi.norm())
        assert not (jCB.Psi == jC.Phi ), "jCB.Psi and jC.Phi should be not equal but %s=%s"%(jCB.Psi,jC.Phi)

        # Are All Coordinate Systems Aligned like Their parentBody?
        assert( (jA.coordSys.R - eye(3)) == zeros(3) )
        assert( (jC.coordSys.R - eye(3)) == zeros(3) )


        # Check that bodies between joints are the same
        assert( jA.coordSys.parentBody == jC.coordSys.parentBody )
        assert( jA.body == csB2.parentBody )
        assert( jC.body == jCB.coordSys.parentBody )
        assert( jCB.body == csB3a.parentBody )

        assert( jCB.coordSys.p.norm() == 0 )
        assert( csB3a.p.norm() == 0), csB3a.p.norm()


        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.jA = jA
        self.jC = jC
        self.jCB = jCB
        self.csB2 = csB2
        self.csB3a = csB3a


        # Independent Coordinates
        self.u = [jA.q]
        self.ud = [jA.qd]
        self.udd = [jA.qdd]

        # Dependent Coordinates
        self.v = [jCB.q, jC.q]
        self.vd = [jCB.qd, jC.qd]
        self.vdd = [jCB.qdd, jC.qdd]

    def calc(self, graph):
        '''
        Returns precalculated v(u), Bvu and b_prime, s.t.
        q = [u,v]', where
            u: independent coordinates
            v: dependent coordinates
        Starting from the Constraint Equation: Phi(q) = 0,
        One Obtains by Differentiation:
        (d(Phi)/du)*u_dot + (d(Phi)/dv)*v_dot = 0
        Ju*u_dot + Jv*v_dot = 0
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

        assert( graph is not None )

        # Abbrevations
        s = self.sign

        # Generalised Coordinates
        q1 = self.jA.q     # u[0]    # angle between x-axes
        q1d = self.jA.qd
        q2 = self.jCB.q     # v[0]    # distance
        q2d = self.jCB.qd
        q3 = self.jC.q     # v[1]    # angle between x-axes
        q3d = self.jC.qd

        # Length of bars and angle between x-axes
        l1_vec = self.jC.coordSys.p - self.jA.coordSys.p
        l1_vec2 = self.pick*l1_vec
        #l1 = graph.addEquation(L1%self.name, sqrt((transpose(l1_vec2)*l1_vec2)[0]))
        l1 = graph.addEquation(L1%self.name, sqrt((transpose(l1_vec2)*l1_vec2)))
        t = atan2(l1_vec2[1],l1_vec2[0])
        alpha = graph.addEquation(AL%self.name, s*t)

        l2_vec = self.csB2.p
        l2_vec2 = self.pick*l2_vec
        #l2 = graph.addEquation(L2%self.name, sqrt((transpose(l2_vec2)*l2_vec2)[0]))
        l2 = graph.addEquation(L2%self.name, sqrt((transpose(l2_vec2)*l2_vec2)))
        beta = graph.addEquation(BE%self.name, s*atan2(l2_vec2[1],l2_vec2[0]))


        l3_vec2 = self.pick*self.jCB.Psi
        gamma = graph.addEquation(GA%self.name, s*atan2(l3_vec2[1],l3_vec2[0]))



        # angle between bars
        phi_prime = graph.addEquation(PHI%self.name, q1 + beta - alpha)


        # Calculate v
        l3 = graph.addEquation(L3%self.name, sqrt(l1**2+l2**2-2*l1*l2*cos(phi_prime)))
        psi_prime = graph.addEquation(PSI%self.name, pi-sin(phi_prime)/abs(sin(phi_prime))*acos((l1**2+l3**2-l2**2)/(2*l1*l3)))


        # TODO: qsi_prime, psi
        v1 = psi_prime + alpha - gamma
        v0 = (l3)

        Bvu = Matrix( [[-l2*sin(phi_prime-psi_prime)], [l2*cos(phi_prime-psi_prime)/l3]] )

        b_prime = Matrix( [-q1d**2*l2*cos(phi_prime-psi_prime)+l3*q3d**2, -(q1d**2*l2*sin(phi_prime-psi_prime)+2*q2d*q3d)/l3] )

        return ([v0,v1],Bvu,b_prime)


