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
from PyMbs.Processing.Joint import Joint

from PyMbs.Symbolics import Matrix

class Transmission(Loop):
    '''
    Defines a coupling between the generalised coordinates of two joints
    j2.q = ratio*j1.q
    '''

    def __init__(self, name, j1, j2, ratio):
        '''
        Constructor
        '''

        # Assertions
        assert ( isinstance(j1, Joint) )
        assert ( isinstance(j2, Joint) )
        assert ( isinstance(ratio, (int, float)) )

        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.j1 = j1
        self.j2 = j2
        self.ratio = ratio

        # Independent Coordinates
        self.u = [j1.q]
        self.ud = [j1.qd]
        self.udd = [j1.qdd]

        # Dependent Coordinates
        self.v = [j2.q]
        self.vd = [j2.qd]
        self.vdd = [j2.qdd]

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

        assert( graph is not None )

        # Generalised Coordinates
        q1 = self.j1.q     # u[0]    # angle between x-axes
        qd1 = self.j1.qd
        q2 = self.j2.q     # v[0]    # distance
        qd2 = self.j2.qd

        v = self.ratio*q1

        Bvu = Matrix( [[self.ratio]] )

        b_prime = Matrix( [0] )

        return ([v], Bvu, b_prime)


