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
Created on 14.07.2011

@author: Jens Scheffler
'''

from PyMbs.Processing.Loops.Loop import Loop, eps
from PyMbs.Common.Functions import sqrt

from PyMbs.Processing.Joint import Joint
from PyMbs.Processing.Frame import Frame


from PyMbs.Symbolics import Matrix, eye, atan, atan2, acos, asin,sin, cos
from scipy import pi


alpha_Zyl = 'HP_%s_AL_Zyl'
beta_Zyl = 'HP_%s_BE_Zyl'
L_Zyl = 'HP_%s_L_Zyl'



class Hexapod(Loop):
    '''
    Explicit Treatment of a Hexapod Mechanism
    '''

    '''
    Sketch:

       -D--=Arbeitsplattform=---4--C  Anlenkpunkt
                                    \
        6 DOF                        3   Zylinderstange
                                      \
                                       B    Translationsgelenk
                                       \\\
                                        \2\   Zylindergehäuse
                                         \\\
       ----=Basisplattform=--------1-------A    Anlenkpunkt
                ||||||
              Grundkörper

            Befestigung des HP ( Verschiebung / Verdrehung gegenüber Grundkörper)
    '''
    def __init__(self, name, csCa, csCb):

        '''
        Constructor
        @param setup: Four Bar Linkage has two setups: -1, 1
        '''

        # Assertions
        assert ( isinstance(csCa, Frame) )
        assert ( isinstance(csCb, Frame) )

        # Check parents
        if (csCa.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCa.name))
        if (csCb.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCb.name))

        # Build complete ThreeBarLinkage


        csC3 = (csCb)
        csC4 = csCa
        jB = csC3.parentBody.joint


        if (jB.coordSys.parentBody.joint == None):



            csC3 = csCa
            csC4 = csCb

            jA1 = jB.coordSys.parentBody.joint
            jA2 = jA1.coordSys.parentBody.joint

            jB = csC3.parentBody.joint

            jD1 = csC4.parentBody.joint
            jD2 = jD1.coordSys.parentBody.joint
            jD3 = jD2.coordSys.parentBody.joint
            jD4 = jD3.coordSys.parentBody.joint
            jD5 = jD4.coordSys.parentBody.joint
            jD6 = jD5.coordSys.parentBody.joint

        else:

            jA1 = jB.coordSys.parentBody.joint
            jA2 = jA1.coordSys.parentBody.joint

            jD1 = csC4.parentBody.joint
            jD2 = jD1.coordSys.parentBody.joint
            jD3 = jD2.coordSys.parentBody.joint
            jD4 = jD3.coordSys.parentBody.joint
            jD5 = jD4.coordSys.parentBody.joint
            jD6 = jD5.coordSys.parentBody.joint



        '''
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

        assert( jA.Phi == jC.Phi )
        assert( jA.Psi.norm() == 0 )
        assert( jC.Psi.norm() == 0 )
        assert( jCB.Phi.norm() == 0 )
        assert( jCB.Psi.norm() == 1 )
        assert( not (jCB.Psi == jC.Phi ))

        # Are All Coordinate Systems Aligned like Their parentBody?
        assert( jA.coordSys.R == eye(3) )
        assert( jC.coordSys.R == eye(3) )


        # Check that bodies between joints are the same
        assert( jA.coordSys.parentBody == jC.coordSys.parentBody )
        assert( jA.body == csB2.parentBody )
        assert( jC.body == jCB.coordSys.parentBody )
        assert( jCB.body == csB3a.parentBody )

        assert( jCB.coordSys.p.norm() == 0 )
        assert( csB3a.p.norm() == 0)
        '''

        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.jA1 = jA1
        self.jA2 = jA2
        self.jB = jB

        self.jD1 = jD1
        self.jD2 = jD2
        self.jD3 = jD3
        self.jD4 = jD4
        self.jD5 = jD5
        self.jD6 = jD6

        self.csC3 = csC3
        self.csC4 = csC4

        # Independent Coordinates

        self.u = [jD6.q,jD5.q,jD4.q,jD3.q,jD2.q,jD1.q]
        self.ud = [jD6.qd,jD5.qd,jD4.qd,jD3.qd,jD2.qd,jD1.qd]
        self.udd = [jD6.qdd,jD5.qdd,jD4.qdd,jD3.qdd,jD2.qdd,jD1.qdd]

        # Dependent Coordinates
        self.v = [jA1.q,jA2.q,jB.q]
        self.vd = [jA1.qd,jA2.qd,jB.qd]
        self.vdd = [jA1.qdd,jA2.qdd,jB.qdd]

    def calc(self, graph):
        print("berechne",self.name)
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


        # Generalised Coordinates

        q1 = self.jD6.q         # u[0]    # angle between z-axes
        q1d = self.jD6.qd
        q2 = self.jD5.q         # u[1]    # angle between y-axes
        q2d = self.jD5.qd
        q3 = self.jD4.q         # u[2]    # angle between x-axes
        q3d = self.jD4.qd

        q4 = self.jD3.q         # u[3]    # distance z
        q4d = self.jD3.qd
        q5 = self.jD2.q         # u[4]    # distance y
        q5d = self.jD2.qd
        q6 = self.jD1.q         # u[5]    # distance x
        q6d = self.jD1.qd


        v0 = self.jA1.q         # v[0]    # angle between y-axes
        v0d = self.jA1.qd
        v1 = self.jA2.q         # v[1]    # angle between z-axes
        v1d = self.jA2.qd
        v2 = self.jB.q          # v[2]    # distance L
        v2d = self.jB.qd



        print(self.jA2.coordSys.R)

        # Length of bars and angle between x-axes

        # Verschiebung der Arbveitsplatte im Raum
        r_0_BP_0_AP=Matrix([q1,q2,q3])  # x,y,z

        # Verdrehung der Arbveitsplatte im Raum

        T_x=Matrix([    [       1,      0 ,       0],
                        [       0, cos(q4),-sin(q4)],
                        [       0, sin(q4), cos(q4)]     ])

        T_y=Matrix([    [ cos(q5),       0, sin(q5)],
                        [       0,       1,       0],
                        [-sin(q5),       0, cos(q5)]     ])

        T_z=Matrix([    [ cos(q6),-sin(q6),       0],
                        [ sin(q6), cos(q6),       0],
                        [       0,       0,       1]     ])


        # In relativen Koordinaten zum Bezugskörper

        r_0_BP_P_BP=Matrix([self.jA2.coordSys.p[0],self.jA2.coordSys.p[1],self.jA2.coordSys.p[2]]) # In Bezug zur Basisplattform

        r_0_AP_P_AP=T_x*T_y*T_z*Matrix([self.csC4.p[0],self.csC4.p[1],self.csC4.p[2]]) # In Bezug zur Arbeitsplattform


        l=r_0_BP_0_AP+r_0_AP_P_AP-r_0_BP_P_BP



        l_Zyl = graph.addEquation(L_Zyl%self.name,(sqrt(l[0]**2+l[1]**2+l[2]**2)))



        Alpha_Zyl=graph.addEquation(alpha_Zyl%self.name, atan2(l[1],l[0]) )


        Beta_Zyl=graph.addEquation(beta_Zyl%self.name, acos(l[2]/l_Zyl) )




        print('r_AP=%s'%(self.csC4.p))
        print('r_BP=%s'%(self.jA2.coordSys.p))

        # TODO:

        v0 = Beta_Zyl
        v1 = Alpha_Zyl
        v2 = l_Zyl


        Bvu = Matrix([
        [                  (cos(v0)*cos(v1))/v2, (cos(v0)*sin(v1))/v2, -sin(v0)/v2, -(self.csC4.p[1]*cos(q4)*cos(q6)*sin(v0) - self.csC4.p[2]*cos(q5)*sin(q4)*sin(v0) + self.csC4.p[0]*cos(q4)*sin(q6)*sin(v0) + self.csC4.p[2]*cos(q4)*cos(q5)*cos(v0)*sin(v1) + self.csC4.p[1]*cos(q6)*cos(v0)*sin(q4)*sin(v1) + self.csC4.p[0]*cos(q6)*sin(q4)*sin(q5)*sin(v0) + self.csC4.p[0]*cos(v0)*sin(q4)*sin(q6)*sin(v1) - self.csC4.p[1]*sin(q4)*sin(q5)*sin(q6)*sin(v0) + self.csC4.p[1]*cos(q4)*cos(v0)*sin(q5)*sin(q6)*sin(v1) - self.csC4.p[0]*cos(q4)*cos(q6)*cos(v0)*sin(q5)*sin(v1))/v2, (self.csC4.p[2]*cos(q5)*cos(v0)*cos(v1) + self.csC4.p[2]*cos(q4)*sin(q5)*sin(v0) + self.csC4.p[0]*cos(q4)*cos(q5)*cos(q6)*sin(v0) - self.csC4.p[0]*cos(q6)*cos(v0)*cos(v1)*sin(q5) - self.csC4.p[1]*cos(q4)*cos(q5)*sin(q6)*sin(v0) + self.csC4.p[1]*cos(v0)*cos(v1)*sin(q5)*sin(q6) + self.csC4.p[2]*cos(v0)*sin(q4)*sin(q5)*sin(v1) - self.csC4.p[1]*cos(q5)*cos(v0)*sin(q4)*sin(q6)*sin(v1) + self.csC4.p[0]*cos(q5)*cos(q6)*cos(v0)*sin(q4)*sin(v1))/v2, -(self.csC4.p[0]*cos(q6)*sin(q4)*sin(v0) - self.csC4.p[1]*sin(q4)*sin(q6)*sin(v0) - self.csC4.p[0]*cos(q4)*cos(q6)*cos(v0)*sin(v1) + self.csC4.p[0]*cos(q5)*cos(v0)*cos(v1)*sin(q6) + self.csC4.p[1]*cos(q4)*cos(q6)*sin(q5)*sin(v0) + self.csC4.p[1]*cos(q4)*cos(v0)*sin(q6)*sin(v1) + self.csC4.p[0]*cos(q4)*sin(q5)*sin(q6)*sin(v0) + self.csC4.p[1]*cos(q5)*cos(q6)*cos(v0)*cos(v1) + self.csC4.p[1]*cos(q6)*cos(v0)*sin(q4)*sin(q5)*sin(v1) + self.csC4.p[0]*cos(v0)*sin(q4)*sin(q5)*sin(q6)*sin(v1))/v2],
        [                 -sin(v1)/(v2*sin(v0)), cos(v1)/(v2*sin(v0)),           0,                                                                                                                                                                                                                                        -(cos(v1)*(self.csC4.p[2]*cos(q4)*cos(q5) + self.csC4.p[1]*cos(q6)*sin(q4) + self.csC4.p[0]*sin(q4)*sin(q6) - self.csC4.p[0]*cos(q4)*cos(q6)*sin(q5) + self.csC4.p[1]*cos(q4)*sin(q5)*sin(q6)))/(v2*sin(v0)),                                                                                                                                                                                (cos(v1)*(self.csC4.p[2]*sin(q4)*sin(q5) + self.csC4.p[0]*cos(q5)*cos(q6)*sin(q4) - self.csC4.p[1]*cos(q5)*sin(q4)*sin(q6)) - sin(v1)*(self.csC4.p[2]*cos(q5) - self.csC4.p[0]*cos(q6)*sin(q5) + self.csC4.p[1]*sin(q5)*sin(q6)))/(v2*sin(v0)),                                                                                                                                                                                                                (sin(v1)*(self.csC4.p[1]*cos(q5)*cos(q6) + self.csC4.p[0]*cos(q5)*sin(q6)) - cos(v1)*(self.csC4.p[1]*cos(q4)*sin(q6) - self.csC4.p[0]*cos(q4)*cos(q6) + self.csC4.p[1]*cos(q6)*sin(q4)*sin(q5) + self.csC4.p[0]*sin(q4)*sin(q5)*sin(q6)))/(v2*sin(v0))],
        [ (sin(v0)*(cos(2*v1) + 1))/(2*cos(v1)),      sin(v0)*sin(v1),     cos(v0),       self.csC4.p[1]*cos(q4)*cos(q6)*cos(v0) - self.csC4.p[2]*cos(q5)*cos(v0)*sin(q4) + self.csC4.p[0]*cos(q4)*cos(v0)*sin(q6) + self.csC4.p[0]*cos(q6)*cos(v0)*sin(q4)*sin(q5) - self.csC4.p[2]*cos(q4)*cos(q5)*sin(v0)*sin(v1) - self.csC4.p[1]*cos(v0)*sin(q4)*sin(q5)*sin(q6) - self.csC4.p[1]*cos(q6)*sin(q4)*sin(v0)*sin(v1) - self.csC4.p[0]*sin(q4)*sin(q6)*sin(v0)*sin(v1) + self.csC4.p[0]*cos(q4)*cos(q6)*sin(q5)*sin(v0)*sin(v1) - self.csC4.p[1]*cos(q4)*sin(q5)*sin(q6)*sin(v0)*sin(v1),      self.csC4.p[2]*cos(q5)*cos(v1)*sin(v0) - self.csC4.p[2]*cos(q4)*cos(v0)*sin(q5) + self.csC4.p[1]*cos(q4)*cos(q5)*cos(v0)*sin(q6) - self.csC4.p[0]*cos(q6)*cos(v1)*sin(q5)*sin(v0) + self.csC4.p[1]*cos(v1)*sin(q5)*sin(q6)*sin(v0) + self.csC4.p[2]*sin(q4)*sin(q5)*sin(v0)*sin(v1) - self.csC4.p[0]*cos(q4)*cos(q5)*cos(q6)*cos(v0) + self.csC4.p[0]*cos(q5)*cos(q6)*sin(q4)*sin(v0)*sin(v1) - self.csC4.p[1]*cos(q5)*sin(q4)*sin(q6)*sin(v0)*sin(v1),       self.csC4.p[0]*cos(q6)*cos(v0)*sin(q4) - self.csC4.p[1]*cos(v0)*sin(q4)*sin(q6) + self.csC4.p[1]*cos(q4)*cos(q6)*cos(v0)*sin(q5) - self.csC4.p[1]*cos(q5)*cos(q6)*cos(v1)*sin(v0) + self.csC4.p[0]*cos(q4)*cos(v0)*sin(q5)*sin(q6) + self.csC4.p[0]*cos(q4)*cos(q6)*sin(v0)*sin(v1) - self.csC4.p[0]*cos(q5)*cos(v1)*sin(q6)*sin(v0) - self.csC4.p[1]*cos(q4)*sin(q6)*sin(v0)*sin(v1) - self.csC4.p[1]*cos(q6)*sin(q4)*sin(q5)*sin(v0)*sin(v1) - self.csC4.p[0]*sin(q4)*sin(q5)*sin(q6)*sin(v0)*sin(v1)]
        ])

        b_prime=Matrix([

         (v1d**2*v2*cos(v0)*sin(v0) - 2*v0d*v2d + self.csC4.p[2]*q4d**2*cos(q4)*cos(q5)*sin(v0) + self.csC4.p[2]*q5d**2*cos(q4)*cos(q5)*sin(v0) - self.csC4.p[2]*q5d**2*cos(v0)*cos(v1)*sin(q5) + self.csC4.p[1]*q4d**2*cos(q6)*sin(q4)*sin(v0) + self.csC4.p[1]*q6d**2*cos(q6)*sin(q4)*sin(v0) + self.csC4.p[0]*q4d**2*sin(q4)*sin(q6)*sin(v0) + self.csC4.p[0]*q6d**2*sin(q4)*sin(q6)*sin(v0) - 2*self.csC4.p[0]*q4d*q6d*cos(q4)*cos(q6)*sin(v0) + 2*self.csC4.p[1]*q4d*q6d*cos(q4)*sin(q6)*sin(v0) - 2*self.csC4.p[2]*q4d*q5d*sin(q4)*sin(q5)*sin(v0) - self.csC4.p[0]*q5d**2*cos(q5)*cos(q6)*cos(v0)*cos(v1) - self.csC4.p[0]*q6d**2*cos(q5)*cos(q6)*cos(v0)*cos(v1) - self.csC4.p[1]*q4d**2*cos(q4)*cos(q6)*cos(v0)*sin(v1) + self.csC4.p[1]*q5d**2*cos(q5)*cos(v0)*cos(v1)*sin(q6) - self.csC4.p[1]*q6d**2*cos(q4)*cos(q6)*cos(v0)*sin(v1) + self.csC4.p[1]*q6d**2*cos(q5)*cos(v0)*cos(v1)*sin(q6) - self.csC4.p[0]*q4d**2*cos(q4)*cos(q6)*sin(q5)*sin(v0) - self.csC4.p[0]*q5d**2*cos(q4)*cos(q6)*sin(q5)*sin(v0) - self.csC4.p[0]*q6d**2*cos(q4)*cos(q6)*sin(q5)*sin(v0) + self.csC4.p[2]*q4d**2*cos(q5)*cos(v0)*sin(q4)*sin(v1) - self.csC4.p[0]*q4d**2*cos(q4)*cos(v0)*sin(q6)*sin(v1) + self.csC4.p[2]*q5d**2*cos(q5)*cos(v0)*sin(q4)*sin(v1) - self.csC4.p[0]*q6d**2*cos(q4)*cos(v0)*sin(q6)*sin(v1) + self.csC4.p[1]*q4d**2*cos(q4)*sin(q5)*sin(q6)*sin(v0) + self.csC4.p[1]*q5d**2*cos(q4)*sin(q5)*sin(q6)*sin(v0) + self.csC4.p[1]*q6d**2*cos(q4)*sin(q5)*sin(q6)*sin(v0) + 2*self.csC4.p[1]*q4d*q5d*cos(q5)*sin(q4)*sin(q6)*sin(v0) + 2*self.csC4.p[1]*q4d*q6d*cos(q6)*sin(q4)*sin(q5)*sin(v0) + 2*self.csC4.p[1]*q4d*q6d*cos(v0)*sin(q4)*sin(q6)*sin(v1) + 2*self.csC4.p[0]*q4d*q6d*sin(q4)*sin(q5)*sin(q6)*sin(v0) - self.csC4.p[0]*q4d**2*cos(q6)*cos(v0)*sin(q4)*sin(q5)*sin(v1) - self.csC4.p[0]*q5d**2*cos(q6)*cos(v0)*sin(q4)*sin(q5)*sin(v1) - self.csC4.p[0]*q6d**2*cos(q6)*cos(v0)*sin(q4)*sin(q5)*sin(v1) + self.csC4.p[1]*q4d**2*cos(v0)*sin(q4)*sin(q5)*sin(q6)*sin(v1) + self.csC4.p[1]*q5d**2*cos(v0)*sin(q4)*sin(q5)*sin(q6)*sin(v1) + self.csC4.p[1]*q6d**2*cos(v0)*sin(q4)*sin(q5)*sin(q6)*sin(v1) - 2*self.csC4.p[1]*q5d*q6d*cos(q4)*cos(q5)*cos(q6)*sin(v0) + 2*self.csC4.p[1]*q5d*q6d*cos(q6)*cos(v0)*cos(v1)*sin(q5) - 2*self.csC4.p[0]*q4d*q5d*cos(q5)*cos(q6)*sin(q4)*sin(v0) - 2*self.csC4.p[0]*q5d*q6d*cos(q4)*cos(q5)*sin(q6)*sin(v0) + 2*self.csC4.p[2]*q4d*q5d*cos(q4)*cos(v0)*sin(q5)*sin(v1) - 2*self.csC4.p[0]*q4d*q6d*cos(q6)*cos(v0)*sin(q4)*sin(v1) + 2*self.csC4.p[0]*q5d*q6d*cos(v0)*cos(v1)*sin(q5)*sin(q6) + 2*self.csC4.p[0]*q4d*q5d*cos(q4)*cos(q5)*cos(q6)*cos(v0)*sin(v1) - 2*self.csC4.p[1]*q4d*q5d*cos(q4)*cos(q5)*cos(v0)*sin(q6)*sin(v1) - 2*self.csC4.p[1]*q4d*q6d*cos(q4)*cos(q6)*cos(v0)*sin(q5)*sin(v1) - 2*self.csC4.p[1]*q5d*q6d*cos(q5)*cos(q6)*cos(v0)*sin(q4)*sin(v1) - 2*self.csC4.p[0]*q4d*q6d*cos(q4)*cos(v0)*sin(q5)*sin(q6)*sin(v1) - 2*self.csC4.p[0]*q5d*q6d*cos(q5)*cos(v0)*sin(q4)*sin(q6)*sin(v1))/v2,
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      -(2*v1d*v2d*sin(v0) - self.csC4.p[2]*q5d**2*sin(q5)*sin(v1) + 2*v0d*v1d*v2*cos(v0) - self.csC4.p[2]*q4d**2*cos(q5)*cos(v1)*sin(q4) + self.csC4.p[0]*q4d**2*cos(q4)*cos(v1)*sin(q6) - self.csC4.p[2]*q5d**2*cos(q5)*cos(v1)*sin(q4) - self.csC4.p[0]*q5d**2*cos(q5)*cos(q6)*sin(v1) + self.csC4.p[0]*q6d**2*cos(q4)*cos(v1)*sin(q6) - self.csC4.p[0]*q6d**2*cos(q5)*cos(q6)*sin(v1) + self.csC4.p[1]*q5d**2*cos(q5)*sin(q6)*sin(v1) + self.csC4.p[1]*q6d**2*cos(q5)*sin(q6)*sin(v1) + self.csC4.p[1]*q4d**2*cos(q4)*cos(q6)*cos(v1) + self.csC4.p[1]*q6d**2*cos(q4)*cos(q6)*cos(v1) - 2*self.csC4.p[2]*q4d*q5d*cos(q4)*cos(v1)*sin(q5) + 2*self.csC4.p[0]*q4d*q6d*cos(q6)*cos(v1)*sin(q4) - 2*self.csC4.p[1]*q4d*q6d*cos(v1)*sin(q4)*sin(q6) + 2*self.csC4.p[1]*q5d*q6d*cos(q6)*sin(q5)*sin(v1) + 2*self.csC4.p[0]*q5d*q6d*sin(q5)*sin(q6)*sin(v1) + self.csC4.p[0]*q4d**2*cos(q6)*cos(v1)*sin(q4)*sin(q5) + self.csC4.p[0]*q5d**2*cos(q6)*cos(v1)*sin(q4)*sin(q5) + self.csC4.p[0]*q6d**2*cos(q6)*cos(v1)*sin(q4)*sin(q5) - self.csC4.p[1]*q4d**2*cos(v1)*sin(q4)*sin(q5)*sin(q6) - self.csC4.p[1]*q5d**2*cos(v1)*sin(q4)*sin(q5)*sin(q6) - self.csC4.p[1]*q6d**2*cos(v1)*sin(q4)*sin(q5)*sin(q6) - 2*self.csC4.p[0]*q4d*q5d*cos(q4)*cos(q5)*cos(q6)*cos(v1) + 2*self.csC4.p[1]*q4d*q5d*cos(q4)*cos(q5)*cos(v1)*sin(q6) + 2*self.csC4.p[1]*q4d*q6d*cos(q4)*cos(q6)*cos(v1)*sin(q5) + 2*self.csC4.p[1]*q5d*q6d*cos(q5)*cos(q6)*cos(v1)*sin(q4) + 2*self.csC4.p[0]*q4d*q6d*cos(q4)*cos(v1)*sin(q5)*sin(q6) + 2*self.csC4.p[0]*q5d*q6d*cos(q5)*cos(v1)*sin(q4)*sin(q6))/(v2*sin(v0)),
         v0d**2*v2 + v1d**2*v2 - v1d**2*v2*cos(v0)**2 - self.csC4.p[1]*q4d**2*cos(q6)*cos(v0)*sin(q4) - self.csC4.p[1]*q6d**2*cos(q6)*cos(v0)*sin(q4) - self.csC4.p[0]*q4d**2*cos(v0)*sin(q4)*sin(q6) - self.csC4.p[0]*q6d**2*cos(v0)*sin(q4)*sin(q6) - self.csC4.p[2]*q5d**2*cos(v1)*sin(q5)*sin(v0) - self.csC4.p[2]*q4d**2*cos(q4)*cos(q5)*cos(v0) - self.csC4.p[2]*q5d**2*cos(q4)*cos(q5)*cos(v0) + 2*self.csC4.p[0]*q4d*q6d*cos(q4)*cos(q6)*cos(v0) - 2*self.csC4.p[1]*q4d*q6d*cos(q4)*cos(v0)*sin(q6) + 2*self.csC4.p[2]*q4d*q5d*cos(v0)*sin(q4)*sin(q5) + self.csC4.p[0]*q4d**2*cos(q4)*cos(q6)*cos(v0)*sin(q5) + self.csC4.p[0]*q5d**2*cos(q4)*cos(q6)*cos(v0)*sin(q5) + self.csC4.p[0]*q6d**2*cos(q4)*cos(q6)*cos(v0)*sin(q5) - self.csC4.p[0]*q5d**2*cos(q5)*cos(q6)*cos(v1)*sin(v0) - self.csC4.p[0]*q6d**2*cos(q5)*cos(q6)*cos(v1)*sin(v0) - self.csC4.p[1]*q4d**2*cos(q4)*cos(v0)*sin(q5)*sin(q6) - self.csC4.p[1]*q5d**2*cos(q4)*cos(v0)*sin(q5)*sin(q6) - self.csC4.p[1]*q6d**2*cos(q4)*cos(v0)*sin(q5)*sin(q6) - self.csC4.p[1]*q4d**2*cos(q4)*cos(q6)*sin(v0)*sin(v1) + self.csC4.p[1]*q5d**2*cos(q5)*cos(v1)*sin(q6)*sin(v0) - self.csC4.p[1]*q6d**2*cos(q4)*cos(q6)*sin(v0)*sin(v1) + self.csC4.p[1]*q6d**2*cos(q5)*cos(v1)*sin(q6)*sin(v0) + self.csC4.p[2]*q4d**2*cos(q5)*sin(q4)*sin(v0)*sin(v1) - self.csC4.p[0]*q4d**2*cos(q4)*sin(q6)*sin(v0)*sin(v1) + self.csC4.p[2]*q5d**2*cos(q5)*sin(q4)*sin(v0)*sin(v1) - self.csC4.p[0]*q6d**2*cos(q4)*sin(q6)*sin(v0)*sin(v1) - 2*self.csC4.p[0]*q4d*q6d*cos(v0)*sin(q4)*sin(q5)*sin(q6) + 2*self.csC4.p[2]*q4d*q5d*cos(q4)*sin(q5)*sin(v0)*sin(v1) - 2*self.csC4.p[0]*q4d*q6d*cos(q6)*sin(q4)*sin(v0)*sin(v1) + 2*self.csC4.p[0]*q5d*q6d*cos(v1)*sin(q5)*sin(q6)*sin(v0) + 2*self.csC4.p[1]*q4d*q6d*sin(q4)*sin(q6)*sin(v0)*sin(v1) + 2*self.csC4.p[1]*q5d*q6d*cos(q4)*cos(q5)*cos(q6)*cos(v0) - self.csC4.p[0]*q4d**2*cos(q6)*sin(q4)*sin(q5)*sin(v0)*sin(v1) - self.csC4.p[0]*q5d**2*cos(q6)*sin(q4)*sin(q5)*sin(v0)*sin(v1) - self.csC4.p[0]*q6d**2*cos(q6)*sin(q4)*sin(q5)*sin(v0)*sin(v1) + 2*self.csC4.p[0]*q4d*q5d*cos(q5)*cos(q6)*cos(v0)*sin(q4) + 2*self.csC4.p[0]*q5d*q6d*cos(q4)*cos(q5)*cos(v0)*sin(q6) + self.csC4.p[1]*q4d**2*sin(q4)*sin(q5)*sin(q6)*sin(v0)*sin(v1) + self.csC4.p[1]*q5d**2*sin(q4)*sin(q5)*sin(q6)*sin(v0)*sin(v1) + self.csC4.p[1]*q6d**2*sin(q4)*sin(q5)*sin(q6)*sin(v0)*sin(v1) - 2*self.csC4.p[1]*q4d*q5d*cos(q5)*cos(v0)*sin(q4)*sin(q6) - 2*self.csC4.p[1]*q4d*q6d*cos(q6)*cos(v0)*sin(q4)*sin(q5) + 2*self.csC4.p[1]*q5d*q6d*cos(q6)*cos(v1)*sin(q5)*sin(v0) + 2*self.csC4.p[0]*q4d*q5d*cos(q4)*cos(q5)*cos(q6)*sin(v0)*sin(v1) - 2*self.csC4.p[1]*q4d*q5d*cos(q4)*cos(q5)*sin(q6)*sin(v0)*sin(v1) - 2*self.csC4.p[1]*q4d*q6d*cos(q4)*cos(q6)*sin(q5)*sin(v0)*sin(v1) - 2*self.csC4.p[1]*q5d*q6d*cos(q5)*cos(q6)*sin(q4)*sin(v0)*sin(v1) - 2*self.csC4.p[0]*q4d*q6d*cos(q4)*sin(q5)*sin(q6)*sin(v0)*sin(v1) - 2*self.csC4.p[0]*q5d*q6d*cos(q5)*sin(q4)*sin(q6)*sin(v0)*sin(v1)])

        '''
        Bvu = Matrix([  [1,0,0,0,0,0],
                        [0,1,0,0,0,0],
                        [0,0,1,0,0,0]   ])

        b_prime = Matrix([0,0,0])

        '''
        return ([v0,v1,v2],Bvu,b_prime)


