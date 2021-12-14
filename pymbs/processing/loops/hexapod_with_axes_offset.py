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

@author: Jens Scheffler
'''

from PyMbs.Processing.Loops.Loop import Loop, eps
from PyMbs.Common.Functions import sqrt, transpose

from PyMbs.Processing.Joint import Joint
from PyMbs.Processing.Frame import Frame

from scipy import pi,cross

from PyMbs.Symbolics import Matrix, eye, cos, sin, atan, atan2, acos, zeros, asin, jacobian,inv


theta_1 = 'HP_%s_theta1'
theta_2 = 'HP_%s_theta2'
theta_4 = 'HP_%s_theta4'
d_4 = 'HP_%s_d4'
theta_7 = 'HP_%s_theta7'


class Hexapod_m_AV(Loop):
    '''
    Explicit Beschreibung einer Parallelkinematik mit Achsversatz
    '''

    '''
    Sketch:

       -F--=Arbeitsplattform=---6--E  Anlenkpunkt (wird aufgeschnitten)
                                   |
         6 DOF                     5
                                   |
                                   D
                                    \
         -----------                 4   Zylinderstange
         |                            \
         |     Loop     ^              C    Translations- und Rotationsgelenk
         |              |              \ \
         ---------------|               \3\   Zylindergehäuse
                                         \\\
                                           B
                                           |
                                           2
                                           |
       ----=Basisplattform=--------1-------A    Anlenkpunkt
                ||||||
              Grundkörper

            Befestigung des HP ( Verschiebung / Verdrehung gegenüber Grundkörper)
    '''
    def __init__(self, name, cs_E_Arbeitsplattform, cs_E_Gelenk):

        '''
        Constructor
        @param setup: Four Bar Linkage has two setups: -1, 1
        '''


        # Assertions
        assert ( isinstance(cs_E_Arbeitsplattform, Frame) )
        assert ( isinstance(cs_E_Gelenk, Frame) )

        # Check parents
        if (cs_E_Arbeitsplattform.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCa.name))
        if (cs_E_Gelenk.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCb.name))

        # E = Auftrenngelenk der kinematischen Schleife

        csE5 = cs_E_Gelenk
        csE6 = cs_E_Arbeitsplattform

        jD = csE5.parentBody.joint
        jC1 = jD.coordSys.parentBody.joint
        jC2 = jC1.coordSys.parentBody.joint
        jB = jC2.coordSys.parentBody.joint
        jA = jB.coordSys.parentBody.joint

        # Freiheitsgrade der Arbeitsplattform

        jF1 = csE6.parentBody.joint
        jF2 = jF1.coordSys.parentBody.joint
        jF3 = jF2.coordSys.parentBody.joint
        jF4 = jF3.coordSys.parentBody.joint
        jF5 = jF4.coordSys.parentBody.joint
        jF6 = jF5.coordSys.parentBody.joint





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
        print(self.name)
        # Save Parameters
        self.jA = jA
        self.jB = jB
        self.jC1 = jC1
        self.jC2 = jC2
        self.jD = jD

        self.jF1 = jF1
        self.jF2 = jF2
        self.jF3 = jF3
        self.jF4 = jF4
        self.jF5 = jF5
        self.jF6 = jF6

        self.csE5 = csE5
        self.csE6 = csE6

        # Independent Coordinates

        self.u = [jF6.q,jF5.q,jF4.q,jF3.q,jF2.q,jF1.q]
        self.ud = [jF6.qd,jF5.qd,jF4.qd,jF3.qd,jF2.qd,jF1.qd]
        self.udd = [jF6.qdd,jF5.qdd,jF4.qdd,jF3.qdd,jF2.qdd,jF1.qdd]

        # Dependent Coordinates
        self.v = [jA.q,jB.q,jC1.q,jC2.q,jD.q]
        self.vd = [jA.qd,jB.qd,jC1.qd,jC2.qd,jD.qd]
        self.vdd = [jA.qdd,jB.qdd,jC1.qdd,jC2.qdd,jD.qdd]

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

        q1 = self.jF6.q         # u[0]    # distance x
        q1d = self.jF6.qd
        q2 = self.jF5.q         # u[1]    # distance y
        q2d = self.jF5.qd
        q3 = self.jF4.q         # u[2]    # distance z
        q3d = self.jF4.qd

        q4 = self.jF3.q         # u[3]    # angle between x-axes
        q4d = self.jF3.qd
        q5 = self.jF2.q         # u[4]    # angle between y-axes
        q5d = self.jF2.qd
        q6 = self.jF1.q         # u[5]    # angle between z-axes
        q6d = self.jF1.qd



        v0 = self.jA.q         # v[0]    # angle 1
        v0d = self.jA.qd
        v1 = self.jB.q         # v[1]    # angle 2
        v1d = self.jB.qd
        v2 = self.jC1.q        # v[2]    # angle 3
        v2d = self.jC1.qd
        v3 = self.jC2.q        # v[3]    # distance 3
        v3d = self.jC2.qd
        v4 = self.jD.q         # v[4]    # angle 4
        v4d = self.jD.qd

        '''

        print self.jA.name
        #print self.jA1.coordSys.p
        print self.jB.name
        #print self.jA2.coordSys.p
        print self.jC1.name
        #print self.jB.coordSys.p
        print self.jC2.name
        print self.jD.name


        print self.jF1.name
        #print self.jD1.coordSys.p
        print self.jF2.name
        #print self.jD2.coordSys.p
        print self.jF3.name
        #print self.jD3.coordSys.p
        print self.jF4.name
        #print self.jD4.coordSys.p
        print self.jF5.name
        #print self.jD5.coordSys.p
        print self.jF6.name
        #print self.jD6.coordSys.p

        '''


        # Verschiebung der Arbeitsplattform im Raum
        r_0_BP_0_AP=Matrix([q1,q2,q3])  # x,y,z

        # Rotationsmatrizen

        T_x=Matrix([    [       1,      0 ,       0],
                        [       0, cos(q4),-sin(q4)],
                        [       0, sin(q4), cos(q4)]     ])

        T_y=Matrix([    [ cos(q5),       0, sin(q5)],
                        [       0,       1,       0],
                        [-sin(q5),       0, cos(q5)]     ])

        T_z=Matrix([    [ cos(q6),-sin(q6),       0],
                        [ sin(q6), cos(q6),       0],
                        [       0,       0,       1]     ])




        R = T_x*T_y*T_z

        # Anlenkpunkte auf der Basis- und Arbeitsplattform

        BP_x=self.jA.coordSys.p[0]
        BP_y=self.jA.coordSys.p[1]
        BP_z=self.jA.coordSys.p[2]

        AP_x=self.csE6.p[0]
        AP_y=self.csE6.p[1]
        AP_z=self.csE6.p[2]

        print('BP_x = %s'%self.jA.coordSys.p[0])
        print('BP_y = %s'%self.jA.coordSys.p[1])
        print('BP_z = %s'%self.jA.coordSys.p[2])

        print('AP_x = %s'%self.csE6.p[0])
        print('AP_y = %s'%self.csE6.p[1])
        print('AP_z = %s'%self.csE6.p[2])


        r_0_BP_P_BP=Matrix([BP_x,BP_y,BP_z])

        r_0_AP_P_AP= R*Matrix([AP_x,AP_y,AP_z])


        l=r_0_BP_0_AP+r_0_AP_P_AP-r_0_BP_P_BP


        # Variablen des Gleichungssystems

        theta1=graph.addVariable(theta_1%self.name,initialValue=1.5)
        theta2=graph.addVariable(theta_2%self.name,initialValue=0)
        theta4=graph.addVariable(theta_4%self.name,initialValue=0)
        d4=graph.addVariable(d_4%self.name,initialValue=0.8)
        theta7=graph.addVariable(theta_7%self.name,initialValue=0)



        l_AV=self.csE5.p[2]-self.jD.coordSys.p[2]

        #l_AV=sqrt(delta_p[0]**2+delta_p[1]**2+delta_p[2]**2)
        #l_AV=0.075

        #theta0 = atan2(self.jA.coordSys.R[2,1],self.jA.coordSys.R[2,2])
        phi_x = atan(self.jA.coordSys.R[2,1]/self.jA.coordSys.R[2,2])

        phi_y = atan(-self.jA.coordSys.R[2,0]*sin(phi_x)/self.jA.coordSys.R[2,1])

        phi_z = atan(self.jA.coordSys.R[1,0]/self.jA.coordSys.R[0,0])


        print('T_init=%s'%self.jA.coordSys.R)
        print('phi_x=%s'%phi_x)
        print('phi_y=%s'%phi_y)
        print('phi_z=%s'%phi_z)



        # Beschreibung der kinematischen Schleife mit Hilfe der Vorwärtskinematik


        T_vor =Matrix([
        [                                                                                                                                      sin(theta4)*(sin(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) - cos(phi_y)*cos(theta2)*sin(phi_z)) - cos(theta4)*(sin(phi_y)*sin(theta1) - cos(phi_y)*cos(phi_z)*cos(theta1)),                                                                                                                                                                                                                           sin(theta7)*(cos(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) + cos(phi_y)*sin(phi_z)*sin(theta2)) + cos(theta7)*(cos(theta4)*(sin(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) - cos(phi_y)*cos(theta2)*sin(phi_z)) + sin(theta4)*(sin(phi_y)*sin(theta1) - cos(phi_y)*cos(phi_z)*cos(theta1))),                                                                                                                                                                                                                           cos(theta7)*(cos(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) + cos(phi_y)*sin(phi_z)*sin(theta2)) - sin(theta7)*(cos(theta4)*(sin(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) - cos(phi_y)*cos(theta2)*sin(phi_z)) + sin(theta4)*(sin(phi_y)*sin(theta1) - cos(phi_y)*cos(phi_z)*cos(theta1))),                                                                                                                                                                                                                                                                                                                                                                d4*(cos(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) + cos(phi_y)*sin(phi_z)*sin(theta2)) + l_AV*cos(theta1)*sin(phi_y) + l_AV*cos(theta7)*(cos(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) + cos(phi_y)*sin(phi_z)*sin(theta2)) - l_AV*sin(theta7)*(cos(theta4)*(sin(theta2)*(cos(theta1)*sin(phi_y) + cos(phi_y)*cos(phi_z)*sin(theta1)) - cos(phi_y)*cos(theta2)*sin(phi_z)) + sin(theta4)*(sin(phi_y)*sin(theta1) - cos(phi_y)*cos(phi_z)*cos(theta1))) + l_AV*cos(phi_y)*cos(phi_z)*sin(theta1)],
        [ cos(theta4)*(cos(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) + cos(phi_y)*sin(phi_x)*sin(theta1)) + sin(theta4)*(sin(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) + cos(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z))), sin(theta7)*(cos(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) - sin(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z))) - cos(theta7)*(sin(theta4)*(cos(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) + cos(phi_y)*sin(phi_x)*sin(theta1)) - cos(theta4)*(sin(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) + cos(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z)))), sin(theta7)*(sin(theta4)*(cos(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) + cos(phi_y)*sin(phi_x)*sin(theta1)) - cos(theta4)*(sin(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) + cos(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z)))) + cos(theta7)*(cos(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) - sin(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z))), d4*(cos(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) - sin(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z))) + l_AV*sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) + l_AV*sin(theta7)*(sin(theta4)*(cos(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) + cos(phi_y)*sin(phi_x)*sin(theta1)) - cos(theta4)*(sin(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) + cos(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z)))) + l_AV*cos(theta7)*(cos(theta2)*(sin(theta1)*(cos(phi_x)*sin(phi_z) + cos(phi_z)*sin(phi_x)*sin(phi_y)) - cos(phi_y)*cos(theta1)*sin(phi_x)) - sin(theta2)*(cos(phi_x)*cos(phi_z) - sin(phi_x)*sin(phi_y)*sin(phi_z))) - l_AV*cos(phi_y)*cos(theta1)*sin(phi_x)],
        [ cos(theta4)*(cos(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) - cos(phi_x)*cos(phi_y)*sin(theta1)) + sin(theta4)*(sin(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) + cos(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z))), sin(theta7)*(cos(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) - sin(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z))) - cos(theta7)*(sin(theta4)*(cos(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) - cos(phi_x)*cos(phi_y)*sin(theta1)) - cos(theta4)*(sin(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) + cos(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z)))), sin(theta7)*(sin(theta4)*(cos(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) - cos(phi_x)*cos(phi_y)*sin(theta1)) - cos(theta4)*(sin(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) + cos(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z)))) + cos(theta7)*(cos(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) - sin(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z))), d4*(cos(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) - sin(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z))) + l_AV*sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + l_AV*sin(theta7)*(sin(theta4)*(cos(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) - cos(phi_x)*cos(phi_y)*sin(theta1)) - cos(theta4)*(sin(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) + cos(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z)))) + l_AV*cos(theta7)*(cos(theta2)*(sin(theta1)*(sin(phi_x)*sin(phi_z) - cos(phi_x)*cos(phi_z)*sin(phi_y)) + cos(phi_x)*cos(phi_y)*cos(theta1)) - sin(theta2)*(cos(phi_z)*sin(phi_x) + cos(phi_x)*sin(phi_y)*sin(phi_z))) + l_AV*cos(phi_x)*cos(phi_y)*cos(theta1)],
        [                                                                                                                                                                                                                                                                                                                                          0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    1]
        ])

        # Ausrichtung des Referenzvektors auf der Arbeitsplattform csE6.R ist die zusätzliche Drehung des Referenzvektors
        x_AP = (R*transpose(self.csE6.R))[:,0]
        y_AP = (R*transpose(self.csE6.R))[:,1]
        z_AP = (R*transpose(self.csE6.R))[:,2]


        n=Matrix([T_vor[0,0],T_vor[1,0],T_vor[2,0]])
        s=Matrix([T_vor[0,1],T_vor[1,1],T_vor[2,1]])
        a=Matrix([T_vor[0,2],T_vor[1,2],T_vor[2,2]])

        # implizite Beschreibung des Gelenkes zur AP

        G_1=transpose(s)*y_AP
        G_2=transpose(s)*z_AP

        # Gleichungen für das Gleichungssystem

        G_1=G_1[0]
        G_2=G_2[0]
        G_3=T_vor[0,3]-l[0]
        G_4=T_vor[1,3]-l[1]
        G_5=T_vor[2,3]-l[2]

        links = [theta1,theta2,theta4,d4,theta7] #Liste der aufzulösenden Variablen vorher mit add Variable an Graph gegeben
        rechts = [G_1, G_2, G_3, G_4, G_5]  #Liste der zu lösenden Gleichungen
        graph.addEquation(links, rechts,implicit=True)

		# Kopie aus Verzeichnis

        Gq = Matrix([
		[  0,  0,  0, (cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) + (cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)), cos(q4)*cos(q5)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - sin(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) + cos(q5)*sin(q4)*sin(q6)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)), (cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) - (cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + cos(q5)*cos(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))],
		[  0,  0,  0,                                                         cos(q4)*cos(q5)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) - cos(q5)*sin(q4)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)),                       - cos(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) - cos(q4)*sin(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - sin(q4)*sin(q5)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)),                                                                                                                                                                                                                                                                                                                       0],
		[ -1,  0,  0,                                                                                                                                                                                                                     0,                                                                                                                                                                                                                      AP_x*cos(q6)*sin(q5) - AP_z*cos(q5) - AP_y*sin(q5)*sin(q6),                                                                                                                                                                                                                                                                                   cos(q5)*(AP_y*cos(q6) + AP_x*sin(q6))],
		[  0, -1,  0,                                                                                            AP_x*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + AP_y*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + AP_z*cos(q4)*cos(q5),                                                                                                                                                                                                           -sin(q4)*(AP_z*sin(q5) + AP_x*cos(q5)*cos(q6) - AP_y*cos(q5)*sin(q6)),                                                                                                                                                                                                                     AP_y*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5)) - AP_x*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))],
		[  0,  0, -1,                                                                                            AP_z*cos(q5)*sin(q4) - AP_y*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - AP_x*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5)),                                                                                                                                                                                                            cos(q4)*(AP_z*sin(q5) + AP_x*cos(q5)*cos(q6) - AP_y*cos(q5)*sin(q6)),                                                                                                                                                                                                                     AP_y*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) - AP_x*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))]
		])

        Gw = Matrix([

		[ (cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - cos(q5)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)), cos(q5)*sin(q6)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0)) - (cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1)) - (cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)), cos(v4)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(v1)*cos(v4)*sin(v2)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) + cos(q5)*cos(v4)*sin(q6)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)),               0, - (cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4)) - (sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - cos(q5)*sin(q6)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))],
		[                                     sin(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) + cos(q4)*cos(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)),                                                                 cos(q5)*sin(q4)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1)) - cos(q4)*cos(q5)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) - sin(q5)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0)),                                                                 cos(q4)*cos(q5)*cos(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(v4)*sin(q5)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q5)*cos(v1)*cos(v4)*sin(q4)*sin(v2),               0,                                                                   sin(q5)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0)) - cos(q4)*cos(q5)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q5)*sin(q4)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4))],
		[                                                                                                cos(v0)*l_AV - sin(v4)*((sin(v0)*sin(v2))*l_AV + (cos(v0)*cos(v2)*sin(v1))*l_AV) + v3*cos(v0)*cos(v1) + (cos(v0)*cos(v1)*cos(v4))*l_AV,                                                                                                                                                                                                       -(sin(v0)*(cos(v4)*sin(v1) + 10*v3*sin(v1) + cos(v1)*cos(v2)*sin(v4)))*l_AV,                                                                                                                                                                                    (sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)))*l_AV, cos(v1)*sin(v0),                                                                                                                                                                                                                              cos(v4)*((cos(v0)*sin(v2))*l_AV - (cos(v2)*sin(v0)*sin(v1))*l_AV) - (cos(v1)*sin(v0)*sin(v4))*l_AV],
		[                                                                                                                                                                                                                             0,                                                                                                                                                                                                                (cos(v2)*sin(v1)*sin(v4))*l_AV - v3*cos(v1) - (cos(v1)*cos(v4))*l_AV,                                                                                                                                                                                                                (cos(v1)*sin(v2)*sin(v4))*l_AV,        -sin(v1),                                                                                                                                                                                                                                                                       (sin(v1)*sin(v4))*l_AV - (cos(v1)*cos(v2)*cos(v4))*l_AV],
		[                                                                                              - sin(v0)*l_AV - sin(v4)*((cos(v0)*sin(v2))*l_AV - (cos(v2)*sin(v0)*sin(v1))*l_AV) - v3*cos(v1)*sin(v0) - (cos(v1)*cos(v4)*sin(v0))*l_AV,                                                                                                                                                                                                       -(cos(v0)*(cos(v4)*sin(v1) + 10*v3*sin(v1) + cos(v1)*cos(v2)*sin(v4)))*l_AV,                                                                                                                                                                                   -(sin(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)))*l_AV, cos(v0)*cos(v1),                                                                                                                                                                                                                            - cos(v4)*((sin(v0)*sin(v2))*l_AV + (cos(v0)*cos(v2)*sin(v1))*l_AV) - (cos(v0)*cos(v1)*sin(v4))*l_AV]
		])

        G_dot = Matrix([

		[ 0, 0, 0, q4d*((cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) - (cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))) - q6d*((sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) + (cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))) + v4d*((cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4)) - (sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))) - v1d*((cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - (cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) - q5d*(cos(q5)*sin(q4)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q4)*cos(q5)*sin(q6)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) + v2d*(cos(v4)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(v1)*cos(v4)*sin(v2)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))) + v0d*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)), v4d*(sin(q5)*sin(q6)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0)) - cos(q4)*cos(q5)*sin(q6)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q5)*sin(q4)*sin(q6)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4))) - q5d*(cos(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) + cos(q4)*sin(q5)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) + sin(q4)*sin(q5)*sin(q6)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) - v1d*(sin(q5)*sin(q6)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0)) + cos(q4)*cos(q5)*sin(q6)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) - cos(q5)*sin(q4)*sin(q6)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) + v0d*(sin(q5)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) + cos(q4)*cos(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))) + v2d*(cos(q4)*cos(q5)*cos(v4)*sin(q6)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(v4)*sin(q5)*sin(q6)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q5)*cos(v1)*cos(v4)*sin(q4)*sin(q6)*sin(v2)) - q4d*(cos(q5)*sin(q4)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q4)*cos(q5)*sin(q6)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) + q6d*(cos(q4)*cos(q5)*cos(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q6)*sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) + cos(q5)*cos(q6)*sin(q4)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))), v2d*(cos(q5)*cos(q6)*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) - cos(v4)*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(v1)*cos(v4)*sin(v2)*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))) - v0d*((cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + cos(q5)*cos(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))) - q4d*((sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) + (cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))) + v4d*((cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4)) + (sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) - cos(q5)*cos(q6)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))) - q6d*((cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - (cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) + cos(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))) + q5d*(cos(q4)*cos(q5)*cos(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q6)*sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) + cos(q5)*cos(q6)*sin(q4)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) + v1d*((cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + (cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1)) + cos(q5)*cos(q6)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0))), q5d*(sin(q5)*sin(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) + cos(q4)*cos(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))) - v0d*((cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + cos(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))) - v4d*((sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - cos(q5)*sin(q6)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))) - q6d*((cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + cos(q5)*cos(q6)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))) + v1d*((sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + cos(q5)*sin(q6)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))) + v2d*(cos(v4)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) - cos(q5)*cos(v4)*sin(q6)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2))) + q4d*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)), v2d*(cos(v4)*sin(v1)*sin(v2)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - cos(v0)*cos(v1)*cos(v4)*sin(v2)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + cos(q5)*cos(v1)*cos(v4)*sin(q6)*sin(v0)*sin(v2)) - q4d*((cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - (cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) - q5d*(sin(q5)*sin(q6)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0)) + cos(q4)*cos(q5)*sin(q6)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) - cos(q5)*sin(q4)*sin(q6)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) + v0d*((sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + cos(q5)*sin(q6)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))) + q6d*((cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + (cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1)) + cos(q5)*cos(q6)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0))) + v1d*((cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) - (cos(v0)*cos(v1)*sin(v4) + cos(v0)*cos(v2)*cos(v4)*sin(v1))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + cos(q5)*sin(q6)*(cos(v1)*sin(v0)*sin(v4) + cos(v2)*cos(v4)*sin(v0)*sin(v1))) - v4d*((cos(v0)*cos(v4)*sin(v1) + cos(v0)*cos(v1)*cos(v2)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + (cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(cos(v1)*cos(v4) - cos(v2)*sin(v1)*sin(v4)) - cos(q5)*sin(q6)*(cos(v4)*sin(v0)*sin(v1) + cos(v1)*cos(v2)*sin(v0)*sin(v4))), q6d*(cos(q5)*cos(q6)*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) - cos(v4)*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(v1)*cos(v4)*sin(v2)*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))) - v2d*(cos(v4)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v1)*cos(v2)*cos(v4)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) + cos(q5)*cos(v4)*sin(q6)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1))) - v4d*(sin(v4)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(v1)*sin(v2)*sin(v4)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) + cos(q5)*sin(q6)*sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2))) + v1d*(cos(v4)*sin(v1)*sin(v2)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - cos(v0)*cos(v1)*cos(v4)*sin(v2)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + cos(q5)*cos(v1)*cos(v4)*sin(q6)*sin(v0)*sin(v2)) + q5d*(cos(q4)*cos(q5)*cos(v4)*sin(q6)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(v4)*sin(q5)*sin(q6)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q5)*cos(v1)*cos(v4)*sin(q4)*sin(q6)*sin(v2)) + q4d*(cos(v4)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(v1)*cos(v4)*sin(v2)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))) + v0d*(cos(v4)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) - cos(q5)*cos(v4)*sin(q6)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2))),                                           0, q5d*(sin(q5)*sin(q6)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0)) - cos(q4)*cos(q5)*sin(q6)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q5)*sin(q4)*sin(q6)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4))) - v0d*((sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - cos(q5)*sin(q6)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))) + q4d*((cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4)) - (sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))) - v2d*(sin(v4)*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(v1)*sin(v2)*sin(v4)*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) + cos(q5)*sin(q6)*sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2))) + q6d*((cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4)) + (sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4))*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) - cos(q5)*cos(q6)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))) - v4d*((cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - (cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)) + cos(q5)*sin(q6)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))) - v1d*((cos(v0)*cos(v4)*sin(v1) + cos(v0)*cos(v1)*cos(v2)*sin(v4))*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + (cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))*(cos(v1)*cos(v4) - cos(v2)*sin(v1)*sin(v4)) - cos(q5)*sin(q6)*(cos(v4)*sin(v0)*sin(v1) + cos(v1)*cos(v2)*sin(v0)*sin(v4)))],
		[ 0, 0, 0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           v1d*(cos(q5)*sin(q4)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) + cos(q4)*cos(q5)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) + q5d*(sin(q4)*sin(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q4)*sin(q5)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) - v2d*(cos(q5)*cos(v4)*sin(q4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(q4)*cos(q5)*cos(v1)*cos(v4)*sin(v2)) - q4d*(cos(q4)*cos(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) + cos(q5)*sin(q4)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) + v4d*(cos(q5)*sin(q4)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q4)*cos(q5)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4))) - v0d*cos(q5)*sin(q4)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)),                                                                                                                                                                                                                                                                                                                                                                                                                         q4d*(sin(q4)*sin(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q4)*sin(q5)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) - v2d*(cos(q5)*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q4)*cos(v4)*sin(q5)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(v1)*cos(v4)*sin(q4)*sin(q5)*sin(v2)) - q5d*(cos(q4)*cos(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) + cos(q5)*sin(q4)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) + v4d*(cos(q5)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0)) + cos(q4)*sin(q5)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) - sin(q4)*sin(q5)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4))) - v1d*(cos(q5)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0)) - cos(q4)*sin(q5)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) + sin(q4)*sin(q5)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) + v0d*(cos(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q4)*sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     0,                                                                                                                                                                                                                                                                                                                                                                                                                                   v2d*(cos(v4)*sin(q5)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(q4)*cos(q5)*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2))) - v1d*(sin(q5)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) - cos(q4)*cos(q5)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0))) + q5d*(cos(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - cos(q4)*sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4))) + v0d*(sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) - cos(q4)*cos(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4))) - v4d*(sin(q5)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q4)*cos(q5)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))) - q4d*cos(q5)*sin(q4)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             q4d*(cos(q5)*sin(q4)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) + cos(q4)*cos(q5)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))) - v0d*(sin(q5)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) - cos(q4)*cos(q5)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0))) - v2d*(cos(q5)*cos(v4)*sin(q4)*sin(v1)*sin(v2) + cos(v1)*cos(v4)*sin(q5)*sin(v0)*sin(v2) + cos(q4)*cos(q5)*cos(v0)*cos(v1)*cos(v4)*sin(v2)) - v1d*(sin(q5)*(cos(v1)*sin(v0)*sin(v4) + cos(v2)*cos(v4)*sin(v0)*sin(v1)) + cos(q4)*cos(q5)*(cos(v0)*cos(v1)*sin(v4) + cos(v0)*cos(v2)*cos(v4)*sin(v1)) + cos(q5)*sin(q4)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) - v4d*(sin(q5)*(cos(v4)*sin(v0)*sin(v1) + cos(v1)*cos(v2)*sin(v0)*sin(v4)) + cos(q4)*cos(q5)*(cos(v0)*cos(v4)*sin(v1) + cos(v0)*cos(v1)*cos(v2)*sin(v4)) - cos(q5)*sin(q4)*(cos(v1)*cos(v4) - cos(v2)*sin(v1)*sin(v4))) - q5d*(cos(q5)*(sin(v0)*sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4)*sin(v0)) - cos(q4)*sin(q5)*(cos(v0)*sin(v1)*sin(v4) - cos(v0)*cos(v1)*cos(v2)*cos(v4)) + sin(q4)*sin(q5)*(cos(v1)*sin(v4) + cos(v2)*cos(v4)*sin(v1))),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         v0d*(cos(v4)*sin(q5)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(q4)*cos(q5)*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2))) - v4d*(cos(q4)*cos(q5)*sin(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - sin(q5)*sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q5)*cos(v1)*sin(q4)*sin(v2)*sin(v4)) - v1d*(cos(q5)*cos(v4)*sin(q4)*sin(v1)*sin(v2) + cos(v1)*cos(v4)*sin(q5)*sin(v0)*sin(v2) + cos(q4)*cos(q5)*cos(v0)*cos(v1)*cos(v4)*sin(v2)) - q5d*(cos(q5)*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q4)*cos(v4)*sin(q5)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) + cos(v1)*cos(v4)*sin(q4)*sin(q5)*sin(v2)) - q4d*(cos(q5)*cos(v4)*sin(q4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - cos(q4)*cos(q5)*cos(v1)*cos(v4)*sin(v2)) + v2d*(cos(v4)*sin(q5)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(q4)*cos(q5)*cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(q5)*cos(v1)*cos(v2)*cos(v4)*sin(q4)),                                           0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     q5d*(cos(q5)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0)) + cos(q4)*sin(q5)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) - sin(q4)*sin(q5)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4))) - v4d*(cos(q4)*cos(q5)*(cos(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) + cos(v0)*cos(v1)*sin(v4)) - sin(q5)*(cos(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) - cos(v1)*sin(v0)*sin(v4)) + cos(q5)*sin(q4)*(sin(v1)*sin(v4) - cos(v1)*cos(v2)*cos(v4))) - v2d*(cos(q4)*cos(q5)*sin(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)) - sin(q5)*sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)) + cos(q5)*cos(v1)*sin(q4)*sin(v2)*sin(v4)) - v1d*(sin(q5)*(cos(v4)*sin(v0)*sin(v1) + cos(v1)*cos(v2)*sin(v0)*sin(v4)) + cos(q4)*cos(q5)*(cos(v0)*cos(v4)*sin(v1) + cos(v0)*cos(v1)*cos(v2)*sin(v4)) - cos(q5)*sin(q4)*(cos(v1)*cos(v4) - cos(v2)*sin(v1)*sin(v4))) - v0d*(sin(q5)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q4)*cos(q5)*(sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)) + cos(v1)*cos(v4)*sin(v0))) + q4d*(cos(q5)*sin(q4)*(sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)) - cos(v0)*cos(v1)*cos(v4)) + cos(q4)*cos(q5)*(cos(v4)*sin(v1) + cos(v1)*cos(v2)*sin(v4)))],
		[ 0, 0, 0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      q5d*(AP_z*sin(q5) + AP_x*cos(q5)*cos(q6) - AP_y*cos(q5)*sin(q6)) - q6d*(AP_y*cos(q6)*sin(q5) + AP_x*sin(q5)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 q6d*(AP_x*cos(q5)*cos(q6) - AP_y*cos(q5)*sin(q6)) - q5d*(AP_y*cos(q6)*sin(q5) + AP_x*sin(q5)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 v3d*cos(v0)*cos(v1) - v0d*(sin(v0)*l_AV + sin(v4)*((cos(v0)*sin(v2))*l_AV - (cos(v2)*sin(v0)*sin(v1))*l_AV) + v3*cos(v1)*sin(v0) + (cos(v1)*cos(v4)*sin(v0))*l_AV) - v4d*(cos(v4)*((sin(v0)*sin(v2))*l_AV + (cos(v0)*cos(v2)*sin(v1))*l_AV) + (cos(v0)*cos(v1)*sin(v4))*l_AV) - (v2d*sin(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)))*l_AV - v1d*(v3*cos(v0)*sin(v1) + (cos(v0)*cos(v4)*sin(v1))*l_AV + (cos(v0)*cos(v1)*cos(v2)*sin(v4))*l_AV),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              v4d*((sin(v0)*sin(v1)*sin(v4))*l_AV - (cos(v1)*cos(v2)*cos(v4)*sin(v0))*l_AV) - v0d*(v3*cos(v0)*sin(v1) + (cos(v0)*cos(v4)*sin(v1))*l_AV + (cos(v0)*cos(v1)*cos(v2)*sin(v4))*l_AV) - v1d*(v3*cos(v1)*sin(v0) + (cos(v1)*cos(v4)*sin(v0))*l_AV - (cos(v2)*sin(v0)*sin(v1)*sin(v4))*l_AV) - v3d*sin(v0)*sin(v1) + (v2d*cos(v1)*sin(v0)*sin(v2)*sin(v4))*l_AV,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         (v4d*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)))*l_AV - (v0d*sin(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)))*l_AV - (v2d*sin(v4)*(cos(v0)*sin(v2) - cos(v2)*sin(v0)*sin(v1)))*l_AV + (v1d*cos(v1)*sin(v0)*sin(v2)*sin(v4))*l_AV,   v0d*cos(v0)*cos(v1) - v1d*sin(v0)*sin(v1),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    v1d*((sin(v0)*sin(v1)*sin(v4))*l_AV - (cos(v1)*cos(v2)*cos(v4)*sin(v0))*l_AV) - v4d*(sin(v4)*((cos(v0)*sin(v2))*l_AV - (cos(v2)*sin(v0)*sin(v1))*l_AV) + (cos(v1)*cos(v4)*sin(v0))*l_AV) - v0d*(cos(v4)*((sin(v0)*sin(v2))*l_AV + (cos(v0)*cos(v2)*sin(v1))*l_AV) + (cos(v0)*cos(v1)*sin(v4))*l_AV) + (v2d*cos(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)))*l_AV],
		[ 0, 0, 0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 q6d*(AP_x*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - AP_y*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5))) + q4d*(AP_x*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5)) + AP_y*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - AP_z*cos(q5)*sin(q4)) - q5d*(AP_z*cos(q4)*sin(q5) + AP_x*cos(q4)*cos(q5)*cos(q6) - AP_y*cos(q4)*cos(q5)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   q6d*(AP_y*cos(q5)*cos(q6)*sin(q4) + AP_x*cos(q5)*sin(q4)*sin(q6)) - q5d*(AP_z*cos(q5)*sin(q4) - AP_x*cos(q6)*sin(q4)*sin(q5) + AP_y*sin(q4)*sin(q5)*sin(q6)) - q4d*(AP_z*cos(q4)*sin(q5) + AP_x*cos(q4)*cos(q5)*cos(q6) - AP_y*cos(q4)*cos(q5)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             q4d*(AP_x*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) - AP_y*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5))) + q6d*(AP_x*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5)) + AP_y*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6))) + q5d*(AP_y*cos(q5)*cos(q6)*sin(q4) + AP_x*cos(q5)*sin(q4)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       v1d*((cos(v4)*sin(v1))*l_AV + v3*sin(v1) + (cos(v1)*cos(v2)*sin(v4))*l_AV) + v4d*((cos(v1)*sin(v4))*l_AV + (cos(v2)*cos(v4)*sin(v1))*l_AV) - v3d*cos(v1) - (v2d*sin(v1)*sin(v2)*sin(v4))*l_AV,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        (v2d*cos(v1)*cos(v2)*sin(v4))*l_AV + (v4d*cos(v1)*cos(v4)*sin(v2))*l_AV - (v1d*sin(v1)*sin(v2)*sin(v4))*l_AV,                                -v1d*cos(v1),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              v1d*((cos(v1)*sin(v4))*l_AV + (cos(v2)*cos(v4)*sin(v1))*l_AV) + v4d*((cos(v4)*sin(v1))*l_AV + (cos(v1)*cos(v2)*sin(v4))*l_AV) + (v2d*cos(v1)*cos(v4)*sin(v2))*l_AV],
		[ 0, 0, 0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 q4d*(AP_x*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + AP_y*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6)) + AP_z*cos(q4)*cos(q5)) - q6d*(AP_x*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - AP_y*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))) - q5d*(AP_z*sin(q4)*sin(q5) + AP_x*cos(q5)*cos(q6)*sin(q4) - AP_y*cos(q5)*sin(q4)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   q5d*(AP_z*cos(q4)*cos(q5) - AP_x*cos(q4)*cos(q6)*sin(q5) + AP_y*cos(q4)*sin(q5)*sin(q6)) - q6d*(AP_y*cos(q4)*cos(q5)*cos(q6) + AP_x*cos(q4)*cos(q5)*sin(q6)) - q4d*(AP_z*sin(q4)*sin(q5) + AP_x*cos(q5)*cos(q6)*sin(q4) - AP_y*cos(q5)*sin(q4)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             q6d*(AP_x*(sin(q4)*sin(q6) - cos(q4)*cos(q6)*sin(q5)) + AP_y*(cos(q6)*sin(q4) + cos(q4)*sin(q5)*sin(q6))) - q4d*(AP_x*(cos(q4)*cos(q6) - sin(q4)*sin(q5)*sin(q6)) - AP_y*(cos(q4)*sin(q6) + cos(q6)*sin(q4)*sin(q5))) - q5d*(AP_y*cos(q4)*cos(q5)*cos(q6) + AP_x*cos(q4)*cos(q5)*sin(q6)),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 v1d*((cos(v4)*sin(v0)*sin(v1))*l_AV + v3*sin(v0)*sin(v1) + (cos(v1)*cos(v2)*sin(v0)*sin(v4))*l_AV) - v0d*(cos(v0)*l_AV - sin(v4)*((sin(v0)*sin(v2))*l_AV + (cos(v0)*cos(v2)*sin(v1))*l_AV) + v3*cos(v0)*cos(v1) + (cos(v0)*cos(v1)*cos(v4))*l_AV) - v4d*(cos(v4)*((cos(v0)*sin(v2))*l_AV - (cos(v2)*sin(v0)*sin(v1))*l_AV) - (cos(v1)*sin(v0)*sin(v4))*l_AV) - (v2d*sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)))*l_AV - v3d*cos(v1)*sin(v0),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              v4d*((cos(v0)*sin(v1)*sin(v4))*l_AV - (cos(v0)*cos(v1)*cos(v2)*cos(v4))*l_AV) - v1d*(v3*cos(v0)*cos(v1) + (cos(v0)*cos(v1)*cos(v4))*l_AV - (cos(v0)*cos(v2)*sin(v1)*sin(v4))*l_AV) + v0d*((cos(v4)*sin(v0)*sin(v1))*l_AV + v3*sin(v0)*sin(v1) + (cos(v1)*cos(v2)*sin(v0)*sin(v4))*l_AV) - v3d*cos(v0)*sin(v1) + (v2d*cos(v0)*cos(v1)*sin(v2)*sin(v4))*l_AV,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         (v2d*sin(v4)*(sin(v0)*sin(v2) + cos(v0)*cos(v2)*sin(v1)))*l_AV - (v0d*sin(v4)*(cos(v0)*cos(v2) + sin(v0)*sin(v1)*sin(v2)))*l_AV - (v4d*cos(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)))*l_AV + (v1d*cos(v0)*cos(v1)*sin(v2)*sin(v4))*l_AV, - v0d*cos(v1)*sin(v0) - v1d*cos(v0)*sin(v1),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    v1d*((cos(v0)*sin(v1)*sin(v4))*l_AV - (cos(v0)*cos(v1)*cos(v2)*cos(v4))*l_AV) - v0d*(cos(v4)*((cos(v0)*sin(v2))*l_AV - (cos(v2)*sin(v0)*sin(v1))*l_AV) - (cos(v1)*sin(v0)*sin(v4))*l_AV) + v4d*(sin(v4)*((sin(v0)*sin(v2))*l_AV + (cos(v0)*cos(v2)*sin(v1))*l_AV) - (cos(v0)*cos(v1)*cos(v4))*l_AV) - (v2d*cos(v4)*(cos(v2)*sin(v0) - cos(v0)*sin(v1)*sin(v2)))*l_AV]
		])

       

        Gwinv= inv(Gw)


        p_dot=Matrix([q1d,q2d,q3d,q4d,q5d,q6d,v0d,v1d,v2d,v3d,v4d])


        # TODO:

        v0 = theta1
        v1 = theta2
        v2 = theta4
        v3 = d4
        v4 = theta7

        Bvu = -Gwinv*Gq
        Bvu.simplify()

        b_prime=-Gwinv*G_dot*p_dot
        b_prime.simplify()



        return ([v0,v1,v2,v3,v4],Bvu,b_prime)


