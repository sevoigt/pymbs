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

edited on 2012/03/21: Sebastian Voigt
'''



from PyMbs.Processing.Loops.Loop import Loop, eps
from PyMbs.Common.Functions import sqrt, norm, transpose, rotMat

from PyMbs.Processing.Joint import Joint
from PyMbs.Processing.Frame import Frame


from PyMbs.Symbolics import Matrix, jacobian, inv, der, zeros, eye, \
                            sin, cos , tan, asin, acos, atan, atan2
from scipy import pi



alpha_Zyl = 'HP_%s_AL_Zyl'
beta_Zyl = 'HP_%s_BE_Zyl'
L_Zyl = 'HP_%s_L_Zyl'





class Hexapod(Loop):
    '''
    Explicit Treatment of a Hexapod Mechanism


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
        Constructor. Currently the loop is not entirely checked for validity
        (number and kind of dof of the involved bodies etc.).
        '''

        # assertions
        assert ( isinstance(csCa, Frame) )
        assert ( isinstance(csCb, Frame) )

        # check parents
        if (csCa.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCa.name))
        if (csCb.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCb.name))

        # superclass constructor
        Loop.__init__(self, name)


        # build complete hexapod axis
        self.csC3 = csCb
        self.csC4 = csCa
        self.jB = self.csC3.parentBody.joint


        if (self.jB.coordSys.parentBody.joint == None):

            self.csC3 = csCa
            self.csC4 = csCb

            self.jB = csC3.parentBody.joint


        self.jA1 = self.jB.coordSys.parentBody.joint
        self.jA2 = self.jA1.coordSys.parentBody.joint

        self.jD1 = self.csC4.parentBody.joint
        self.jD2 = self.jD1.coordSys.parentBody.joint
        self.jD3 = self.jD2.coordSys.parentBody.joint
        self.jD4 = self.jD3.coordSys.parentBody.joint
        self.jD5 = self.jD4.coordSys.parentBody.joint
        self.jD6 = self.jD5.coordSys.parentBody.joint


        # independent coordinates
        self.u = [self.jD6.q,self.jD5.q,self.jD4.q,
                  self.jD3.q,self.jD2.q,self.jD1.q]
        self.ud = [self.jD6.qd,self.jD5.qd,self.jD4.qd,
                   self.jD3.qd,self.jD2.qd,self.jD1.qd]
        self.udd = [self.jD6.qdd,self.jD5.qdd,self.jD4.qdd,
                    self.jD3.qdd,self.jD2.qdd,self.jD1.qdd]

        # dependent coordinates
        self.v = [self.jB.q,self.jA2.q,self.jA1.q]
        self.vd = [self.jB.qd,self.jA2.qd,self.jA1.qd]
        self.vdd = [self.jB.qdd,self.jA2.qdd,self.jA1.qdd]


        self.q = Matrix(self.u + self.v)
        self.qd = Matrix(self.ud + self.vd)



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


        # generalised coordinates
        u0  = self.u[0]     # Tx
        u0d = self.ud[0]
        u1  = self.u[1]     # Ty
        u1d = self.ud[1]
        u2  = self.u[2]     # Tz
        u2d = self.ud[2]

        u3  = self.u[3]     # Rx
        u3d = self.ud[3]
        u4  = self.u[4]     # Ry
        u4d = self.ud[4]
        u5  = self.u[5]     # Rz
        u5d = self.ud[5]

        #v0  = self.v[0]      # lenght of cylinder  - calculated below
        v0d = self.vd[0]
        #v1  = self.v[1]      # first rotation      - calculated below
        v1d = self.vd[1]
        #v2  = self.v[2]      # second rotation     - calculated below
        v2d = self.vd[2]


        # translational displacement of the moving platform (x,y,z)
        r_0_BP_0_AP = Matrix([u0,u1,u2])


        # rotation of the moving platform (kardan angles)
        Tx = rotMat(u3, 'x')
        Ty = rotMat(u4, 'y')
        Tz = rotMat(u5, 'z')

        T = transpose(Tx) * transpose(Ty) * transpose(Tz)


        # length of the hexapod axis
        r_0_BP_P_BP = self.jA2.coordSys.p # w.r.t. base platform
        r_0_AP_P_AP = T * self.csC4.p     # w.r.t. base platform

        l = graph.addEquation('L_%s' % self.name, r_0_BP_0_AP + r_0_AP_P_AP - r_0_BP_P_BP)

        l_Zyl = graph.addEquation(L_Zyl % self.name, norm(l))


        # The two rotational dof of the axis have to be calculated according
        # to the orientation of the reference frame of the base platform, i.e.
        # if the hexapod points in x-, y- or z-direction.

        # cylinder points in x, Ry and Rz are the rotational dof
        if self.jB.Psi == Matrix([1,0,0]):

            # rotation around y-axis
            Alpha_Zyl = graph.addEquation(alpha_Zyl % self.name,
                                          -atan2(l[2],l[0]))

            # rotation aroudn z-axis
            Beta_Zyl  = graph.addEquation(beta_Zyl % self.name,
                                          asin(l[1]/l_Zyl))

        else:
            print('sorry, Hexapod for this frame not yet implemented')
            Alpha_Zyl = 0
            Beta_Zyl = 0


        '''
        # cylinder points in y, Rx and Rz for axis
        if self.jB.Psi == Matrix([0,1,0]):
            Alpha_Zyl = graph.addEquation(alpha_Zyl % self.name,
                                          atan2(l[0],l[2]))
            Beta_Zyl  = graph.addEquation(beta_Zyl % self.name,
                                          acos(l[1]/l_Zyl))


        # cylinder points in z, Rx and Ry for axis
        if self.jB.Psi == Matrix([0,0,1]):
            Alpha_Zyl = graph.addEquation(alpha_Zyl % self.name,
                                          atan2(l[1],l[0]))
            Beta_Zyl  = graph.addEquation(beta_Zyl % self.name,
                                          acos(l[2]/l_Zyl))
        '''

        # assign dependent coordinates
        v0 = l_Zyl
        v1 = Alpha_Zyl
        v2 = Beta_Zyl

        # geometric parameters of base platform (bp) and tool platform (tp)
        bpx = self.jA2.coordSys.p[0]
        bpy = self.jA2.coordSys.p[1]
        bpz = self.jA2.coordSys.p[2]

        tpx = self.csC4.p[0]
        tpy = self.csC4.p[1]
        tpz = self.csC4.p[2]


        # and calculate Bvu and b_prime respectively
        Bvu = Matrix([
        [      cos(v1)*cos(v2),    sin(v2),      sin(v1)*cos(v2),       tpx*sin(u3)*sin(u4)*sin(v1)*cos(u5)*cos(v2) - tpx*sin(u3)*sin(u5)*sin(v2) + tpx*sin(u4)*sin(v2)*cos(u3)*cos(u5) + tpx*sin(u5)*sin(v1)*cos(u3)*cos(v2) - tpy*sin(u3)*sin(u4)*sin(u5)*sin(v1)*cos(v2) - tpy*sin(u3)*sin(v2)*cos(u5) - tpy*sin(u4)*sin(u5)*sin(v2)*cos(u3) + tpy*sin(v1)*cos(u3)*cos(u5)*cos(v2) - tpz*sin(u3)*sin(v1)*cos(u4)*cos(v2) - tpz*sin(v2)*cos(u3)*cos(u4),        tpx*sin(u3)*sin(v2)*cos(u4)*cos(u5) - tpx*sin(u4)*cos(u5)*cos(v1)*cos(v2) - tpx*sin(v1)*cos(u3)*cos(u4)*cos(u5)*cos(v2) - tpy*sin(u3)*sin(u5)*sin(v2)*cos(u4) + tpy*sin(u4)*sin(u5)*cos(v1)*cos(v2) + tpy*sin(u5)*sin(v1)*cos(u3)*cos(u4)*cos(v2) + tpz*sin(u3)*sin(u4)*sin(v2) - tpz*sin(u4)*sin(v1)*cos(u3)*cos(v2) + tpz*cos(u4)*cos(v1)*cos(v2),      -tpx*sin(u3)*sin(u4)*sin(u5)*sin(v2) + tpx*sin(u3)*sin(v1)*cos(u5)*cos(v2) + tpx*sin(u4)*sin(u5)*sin(v1)*cos(u3)*cos(v2) - tpx*sin(u5)*cos(u4)*cos(v1)*cos(v2) + tpx*sin(v2)*cos(u3)*cos(u5) - tpy*sin(u3)*sin(u4)*sin(v2)*cos(u5) - tpy*sin(u3)*sin(u5)*sin(v1)*cos(v2) + tpy*sin(u4)*sin(v1)*cos(u3)*cos(u5)*cos(v2) - tpy*sin(u5)*sin(v2)*cos(u3) - tpy*cos(u4)*cos(u5)*cos(v1)*cos(v2)],
        [-sin(v1)/(v0*cos(v2)),          0, cos(v1)/(v0*cos(v2)),                                                                                                                                                                                                                                      (tpx*sin(u3)*sin(u4)*cos(u5) + tpx*sin(u5)*cos(u3) - tpy*sin(u3)*sin(u4)*sin(u5) + tpy*cos(u3)*cos(u5) - tpz*sin(u3)*cos(u4))*cos(v1)/(v0*cos(v2)),                                                                                                                                                   (tpx*sin(u4)*sin(v1)*cos(u5) - tpx*cos(u3)*cos(u4)*cos(u5)*cos(v1) - tpy*sin(u4)*sin(u5)*sin(v1) + tpy*sin(u5)*cos(u3)*cos(u4)*cos(v1) - tpz*sin(u4)*cos(u3)*cos(v1) - tpz*sin(v1)*cos(u4))/(v0*cos(v2)),                                                                                                                                                                                (tpx*sin(u3)*cos(u5)*cos(v1) + tpx*sin(u4)*sin(u5)*cos(u3)*cos(v1) + tpx*sin(u5)*sin(v1)*cos(u4) - tpy*sin(u3)*sin(u5)*cos(v1) + tpy*sin(u4)*cos(u3)*cos(u5)*cos(v1) + tpy*sin(v1)*cos(u4)*cos(u5))/(v0*cos(v2))],
        [  -sin(v2)*cos(v1)/v0, cos(v2)/v0,  -sin(v1)*sin(v2)/v0, -(tpx*sin(u3)*sin(u4)*sin(v1)*sin(v2)*cos(u5) + tpx*sin(u3)*sin(u5)*cos(v2) - tpx*sin(u4)*cos(u3)*cos(u5)*cos(v2) + tpx*sin(u5)*sin(v1)*sin(v2)*cos(u3) - tpy*sin(u3)*sin(u4)*sin(u5)*sin(v1)*sin(v2) + tpy*sin(u3)*cos(u5)*cos(v2) + tpy*sin(u4)*sin(u5)*cos(u3)*cos(v2) + tpy*sin(v1)*sin(v2)*cos(u3)*cos(u5) - tpz*sin(u3)*sin(v1)*sin(v2)*cos(u4) + tpz*cos(u3)*cos(u4)*cos(v2))/v0, -(-tpx*sin(u3)*cos(u4)*cos(u5)*cos(v2) - tpx*sin(u4)*sin(v2)*cos(u5)*cos(v1) - tpx*sin(v1)*sin(v2)*cos(u3)*cos(u4)*cos(u5) + tpy*sin(u3)*sin(u5)*cos(u4)*cos(v2) + tpy*sin(u4)*sin(u5)*sin(v2)*cos(v1) + tpy*sin(u5)*sin(v1)*sin(v2)*cos(u3)*cos(u4) - tpz*sin(u3)*sin(u4)*cos(v2) - tpz*sin(u4)*sin(v1)*sin(v2)*cos(u3) + tpz*sin(v2)*cos(u4)*cos(v1))/v0, -(tpx*sin(u3)*sin(u4)*sin(u5)*cos(v2) + tpx*sin(u3)*sin(v1)*sin(v2)*cos(u5) + tpx*sin(u4)*sin(u5)*sin(v1)*sin(v2)*cos(u3) - tpx*sin(u5)*sin(v2)*cos(u4)*cos(v1) - tpx*cos(u3)*cos(u5)*cos(v2) + tpy*sin(u3)*sin(u4)*cos(u5)*cos(v2) - tpy*sin(u3)*sin(u5)*sin(v1)*sin(v2) + tpy*sin(u4)*sin(v1)*sin(v2)*cos(u3)*cos(u5) + tpy*sin(u5)*cos(u3)*cos(v2) - tpy*sin(v2)*cos(u4)*cos(u5)*cos(v1))/v0],
        ])

        b_prime = Matrix([
        -tpx*u3d**2*sin(u3)*sin(u4)*sin(v2)*cos(u5) - tpx*u3d**2*sin(u3)*sin(u5)*sin(v1)*cos(v2) + tpx*u3d**2*sin(u4)*sin(v1)*cos(u3)*cos(u5)*cos(v2) - tpx*u3d**2*sin(u5)*sin(v2)*cos(u3) + 2*tpx*u3d*u4d*sin(u3)*sin(v1)*cos(u4)*cos(u5)*cos(v2) + 2*tpx*u3d*u4d*sin(v2)*cos(u3)*cos(u4)*cos(u5) - 2*tpx*u3d*u5d*sin(u3)*sin(u4)*sin(u5)*sin(v1)*cos(v2) - 2*tpx*u3d*u5d*sin(u3)*sin(v2)*cos(u5) - 2*tpx*u3d*u5d*sin(u4)*sin(u5)*sin(v2)*cos(u3) + 2*tpx*u3d*u5d*sin(v1)*cos(u3)*cos(u5)*cos(v2) - tpx*u4d**2*sin(u3)*sin(u4)*sin(v2)*cos(u5) + tpx*u4d**2*sin(u4)*sin(v1)*cos(u3)*cos(u5)*cos(v2) - tpx*u4d**2*cos(u4)*cos(u5)*cos(v1)*cos(v2) - 2*tpx*u4d*u5d*sin(u3)*sin(u5)*sin(v2)*cos(u4) + 2*tpx*u4d*u5d*sin(u4)*sin(u5)*cos(v1)*cos(v2) + 2*tpx*u4d*u5d*sin(u5)*sin(v1)*cos(u3)*cos(u4)*cos(v2) - tpx*u5d**2*sin(u3)*sin(u4)*sin(v2)*cos(u5) - tpx*u5d**2*sin(u3)*sin(u5)*sin(v1)*cos(v2) + tpx*u5d**2*sin(u4)*sin(v1)*cos(u3)*cos(u5)*cos(v2) - tpx*u5d**2*sin(u5)*sin(v2)*cos(u3) - tpx*u5d**2*cos(u4)*cos(u5)*cos(v1)*cos(v2) + tpy*u3d**2*sin(u3)*sin(u4)*sin(u5)*sin(v2) - tpy*u3d**2*sin(u3)*sin(v1)*cos(u5)*cos(v2) - tpy*u3d**2*sin(u4)*sin(u5)*sin(v1)*cos(u3)*cos(v2) - tpy*u3d**2*sin(v2)*cos(u3)*cos(u5) - 2*tpy*u3d*u4d*sin(u3)*sin(u5)*sin(v1)*cos(u4)*cos(v2) - 2*tpy*u3d*u4d*sin(u5)*sin(v2)*cos(u3)*cos(u4) - 2*tpy*u3d*u5d*sin(u3)*sin(u4)*sin(v1)*cos(u5)*cos(v2) + 2*tpy*u3d*u5d*sin(u3)*sin(u5)*sin(v2) - 2*tpy*u3d*u5d*sin(u4)*sin(v2)*cos(u3)*cos(u5) - 2*tpy*u3d*u5d*sin(u5)*sin(v1)*cos(u3)*cos(v2) + tpy*u4d**2*sin(u3)*sin(u4)*sin(u5)*sin(v2) - tpy*u4d**2*sin(u4)*sin(u5)*sin(v1)*cos(u3)*cos(v2) + tpy*u4d**2*sin(u5)*cos(u4)*cos(v1)*cos(v2) - 2*tpy*u4d*u5d*sin(u3)*sin(v2)*cos(u4)*cos(u5) + 2*tpy*u4d*u5d*sin(u4)*cos(u5)*cos(v1)*cos(v2) + 2*tpy*u4d*u5d*sin(v1)*cos(u3)*cos(u4)*cos(u5)*cos(v2) + tpy*u5d**2*sin(u3)*sin(u4)*sin(u5)*sin(v2) - tpy*u5d**2*sin(u3)*sin(v1)*cos(u5)*cos(v2) - tpy*u5d**2*sin(u4)*sin(u5)*sin(v1)*cos(u3)*cos(v2) + tpy*u5d**2*sin(u5)*cos(u4)*cos(v1)*cos(v2) - tpy*u5d**2*sin(v2)*cos(u3)*cos(u5) + tpz*u3d**2*sin(u3)*sin(v2)*cos(u4) - tpz*u3d**2*sin(v1)*cos(u3)*cos(u4)*cos(v2) + 2*tpz*u3d*u4d*sin(u3)*sin(u4)*sin(v1)*cos(v2) + 2*tpz*u3d*u4d*sin(u4)*sin(v2)*cos(u3) + tpz*u4d**2*sin(u3)*sin(v2)*cos(u4) - tpz*u4d**2*sin(u4)*cos(v1)*cos(v2) - tpz*u4d**2*sin(v1)*cos(u3)*cos(u4)*cos(v2) + v0*v1d**2*cos(v2)**2 + v0*v2d**2*sin(v1)**2*cos(v2)**4 - 2*v0*v2d**2*sin(v1)**2*cos(v2)**2 + v0*v2d**2*sin(v1)**2 + v0*v2d**2*cos(v1)**2*cos(v2)**4 - 2*v0*v2d**2*cos(v1)**2*cos(v2)**2 + v0*v2d**2*cos(v1)**2 - v0*v2d**2*cos(v2)**4 + 2*v0*v2d**2*cos(v2)**2,
        (-tpx*u3d**2*sin(u3)*sin(u5)*cos(v1) + tpx*u3d**2*sin(u4)*cos(u3)*cos(u5)*cos(v1) + 2*tpx*u3d*u4d*sin(u3)*cos(u4)*cos(u5)*cos(v1) - 2*tpx*u3d*u5d*sin(u3)*sin(u4)*sin(u5)*cos(v1) + 2*tpx*u3d*u5d*cos(u3)*cos(u5)*cos(v1) + tpx*u4d**2*sin(u4)*cos(u3)*cos(u5)*cos(v1) + tpx*u4d**2*sin(v1)*cos(u4)*cos(u5) - 2*tpx*u4d*u5d*sin(u4)*sin(u5)*sin(v1) + 2*tpx*u4d*u5d*sin(u5)*cos(u3)*cos(u4)*cos(v1) - tpx*u5d**2*sin(u3)*sin(u5)*cos(v1) + tpx*u5d**2*sin(u4)*cos(u3)*cos(u5)*cos(v1) + tpx*u5d**2*sin(v1)*cos(u4)*cos(u5) - tpy*u3d**2*sin(u3)*cos(u5)*cos(v1) - tpy*u3d**2*sin(u4)*sin(u5)*cos(u3)*cos(v1) - 2*tpy*u3d*u4d*sin(u3)*sin(u5)*cos(u4)*cos(v1) - 2*tpy*u3d*u5d*sin(u3)*sin(u4)*cos(u5)*cos(v1) - 2*tpy*u3d*u5d*sin(u5)*cos(u3)*cos(v1) - tpy*u4d**2*sin(u4)*sin(u5)*cos(u3)*cos(v1) - tpy*u4d**2*sin(u5)*sin(v1)*cos(u4) - 2*tpy*u4d*u5d*sin(u4)*sin(v1)*cos(u5) + 2*tpy*u4d*u5d*cos(u3)*cos(u4)*cos(u5)*cos(v1) - tpy*u5d**2*sin(u3)*cos(u5)*cos(v1) - tpy*u5d**2*sin(u4)*sin(u5)*cos(u3)*cos(v1) - tpy*u5d**2*sin(u5)*sin(v1)*cos(u4) - tpz*u3d**2*cos(u3)*cos(u4)*cos(v1) + 2*tpz*u3d*u4d*sin(u3)*sin(u4)*cos(v1) + tpz*u4d**2*sin(u4)*sin(v1) - tpz*u4d**2*cos(u3)*cos(u4)*cos(v1) + 2*v0*v1d*v2d*sin(v2) - 2*v0d*v1d*cos(v2))/(v0*cos(v2)),
        -(tpx*u3d**2*sin(u3)*sin(u4)*cos(u5)*cos(v2) - tpx*u3d**2*sin(u3)*sin(u5)*sin(v1)*sin(v2) + tpx*u3d**2*sin(u4)*sin(v1)*sin(v2)*cos(u3)*cos(u5) + tpx*u3d**2*sin(u5)*cos(u3)*cos(v2) + 2*tpx*u3d*u4d*sin(u3)*sin(v1)*sin(v2)*cos(u4)*cos(u5) - 2*tpx*u3d*u4d*cos(u3)*cos(u4)*cos(u5)*cos(v2) - 2*tpx*u3d*u5d*sin(u3)*sin(u4)*sin(u5)*sin(v1)*sin(v2) + 2*tpx*u3d*u5d*sin(u3)*cos(u5)*cos(v2) + 2*tpx*u3d*u5d*sin(u4)*sin(u5)*cos(u3)*cos(v2) + 2*tpx*u3d*u5d*sin(v1)*sin(v2)*cos(u3)*cos(u5) + tpx*u4d**2*sin(u3)*sin(u4)*cos(u5)*cos(v2) + tpx*u4d**2*sin(u4)*sin(v1)*sin(v2)*cos(u3)*cos(u5) - tpx*u4d**2*sin(v2)*cos(u4)*cos(u5)*cos(v1) + 2*tpx*u4d*u5d*sin(u3)*sin(u5)*cos(u4)*cos(v2) + 2*tpx*u4d*u5d*sin(u4)*sin(u5)*sin(v2)*cos(v1) + 2*tpx*u4d*u5d*sin(u5)*sin(v1)*sin(v2)*cos(u3)*cos(u4) + tpx*u5d**2*sin(u3)*sin(u4)*cos(u5)*cos(v2) - tpx*u5d**2*sin(u3)*sin(u5)*sin(v1)*sin(v2) + tpx*u5d**2*sin(u4)*sin(v1)*sin(v2)*cos(u3)*cos(u5) + tpx*u5d**2*sin(u5)*cos(u3)*cos(v2) - tpx*u5d**2*sin(v2)*cos(u4)*cos(u5)*cos(v1) - tpy*u3d**2*sin(u3)*sin(u4)*sin(u5)*cos(v2) - tpy*u3d**2*sin(u3)*sin(v1)*sin(v2)*cos(u5) - tpy*u3d**2*sin(u4)*sin(u5)*sin(v1)*sin(v2)*cos(u3) + tpy*u3d**2*cos(u3)*cos(u5)*cos(v2) - 2*tpy*u3d*u4d*sin(u3)*sin(u5)*sin(v1)*sin(v2)*cos(u4) + 2*tpy*u3d*u4d*sin(u5)*cos(u3)*cos(u4)*cos(v2) - 2*tpy*u3d*u5d*sin(u3)*sin(u4)*sin(v1)*sin(v2)*cos(u5) - 2*tpy*u3d*u5d*sin(u3)*sin(u5)*cos(v2) + 2*tpy*u3d*u5d*sin(u4)*cos(u3)*cos(u5)*cos(v2) - 2*tpy*u3d*u5d*sin(u5)*sin(v1)*sin(v2)*cos(u3) - tpy*u4d**2*sin(u3)*sin(u4)*sin(u5)*cos(v2) - tpy*u4d**2*sin(u4)*sin(u5)*sin(v1)*sin(v2)*cos(u3) + tpy*u4d**2*sin(u5)*sin(v2)*cos(u4)*cos(v1) + 2*tpy*u4d*u5d*sin(u3)*cos(u4)*cos(u5)*cos(v2) + 2*tpy*u4d*u5d*sin(u4)*sin(v2)*cos(u5)*cos(v1) + 2*tpy*u4d*u5d*sin(v1)*sin(v2)*cos(u3)*cos(u4)*cos(u5) - tpy*u5d**2*sin(u3)*sin(u4)*sin(u5)*cos(v2) - tpy*u5d**2*sin(u3)*sin(v1)*sin(v2)*cos(u5) - tpy*u5d**2*sin(u4)*sin(u5)*sin(v1)*sin(v2)*cos(u3) + tpy*u5d**2*sin(u5)*sin(v2)*cos(u4)*cos(v1) + tpy*u5d**2*cos(u3)*cos(u5)*cos(v2) - tpz*u3d**2*sin(u3)*cos(u4)*cos(v2) - tpz*u3d**2*sin(v1)*sin(v2)*cos(u3)*cos(u4) + 2*tpz*u3d*u4d*sin(u3)*sin(u4)*sin(v1)*sin(v2) - 2*tpz*u3d*u4d*sin(u4)*cos(u3)*cos(v2) - tpz*u4d**2*sin(u3)*cos(u4)*cos(v2) - tpz*u4d**2*sin(u4)*sin(v2)*cos(v1) - tpz*u4d**2*sin(v1)*sin(v2)*cos(u3)*cos(u4) + v0*v1d**2*sin(v2)*cos(v2) - 2*v0d*v2d*sin(v1)**2*cos(v2)**2 + 2*v0d*v2d*sin(v1)**2 - 2*v0d*v2d*cos(v1)**2*cos(v2)**2 + 2*v0d*v2d*cos(v1)**2 + 2*v0d*v2d*cos(v2)**2)/v0,        ])

        return ([v0,v1,v2],Bvu,b_prime)