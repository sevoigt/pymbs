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
Created on 30.06.2009

@author: Christian Schubert
'''

from PyMbs.Common.MbsElement import MbsElement
from PyMbs.Common.Functions import transpose, sin, cos
from . import Frame
from . import Body

from PyMbs.Symbolics import Graph

from PyMbs.Symbolics import Basic, Matrix, zeros, eye

class Joint(MbsElement):
    '''
    Joint connecting a Coordinate System and a Body
    '''


    def __init__(self, name, coordSys, body, Psi, Phi, q0 = 0, qd0 = 0, graph=None):
        '''
        Constructor
        name: Name of the Joint
        coordSys: Parent Frame
        body: Child Body
        Psi: Axis of Translation (normalised 3x1 Vector)
        Phi: Axis of Rotation (normalised 3x1 Vector)
        '''
        # Type Conversions
        if ( isinstance(Psi, list) ): Psi = Matrix(Psi)
        if ( isinstance(Phi, list) ): Phi = Matrix(Phi)

        assert graph is not None

        # Assertions
        assert isinstance(coordSys, Frame.Frame)
        assert isinstance(body, (Body.Body,Body.FlexibleBody))
        assert isinstance(Psi, Basic) and (Psi.shape() in ((3,), (3,1)) ), "Psi - Type: %s, Shape %s, Value %s"%(str(Psi.__class__), str(Psi.shape()), str(Psi))
        assert isinstance(Phi, Basic) and (Phi.shape() in ((3,), (3,1)) )
        assert isinstance(q0, (int, float, Basic))
        assert isinstance(qd0, (int, float, Basic))

        # Call Constructor of MbsElement
        MbsElement.__init__(self, name, graph)

        # Assign Parameters
        self.coordSys = coordSys
        self.body = body
        self.Psi = Psi
        self.Phi = Phi
        self.IsIndependent = True


        # Calculate Orientation, Depending on the Generalised Coordinate
        if ((Phi == zeros((3,))) or (Psi == zeros((3,)))):
            self.q = graph.addVariable(name='q_%s' % self.name, shape=tuple())
            self.qd = graph.addVariable(name='qd_%s' % self.name, shape=tuple())
            self.qdd = graph.addVariable(name='qdd_%s' % self.name, shape=tuple())
        else:
            self.q = 0
            self.qd = 0
            self.qdd = 0

        self.q0 = q0;
        self.qd0 = qd0;

        if (Phi == zeros((3,))):
            self.R = eye((3,3))
        else:
            v1 = Phi[0]
            v2 = Phi[1]
            v3 = Phi[2]
            sq = sin(self.q)
            cq = cos(self.q)
            ecq = 1 - cos(self.q)
            # R rotates a vector around [v1, v2, v3] at a magnitude of q
            self.R = Matrix([[cq+v1**2*ecq    , v1*v2*ecq+v3*sq, v1*v3*ecq-v2*sq],
                             [v2*v1*ecq-v3*sq, cq+v2**2*ecq    , v2*v3*ecq+v1*sq],
                             [v3*v1*ecq+v2*sq, v3*v2*ecq-v1*sq, cq+v3**2*ecq]])
            # R transforms a vector from child to parent. i.e. R_parent_child
            self.R = transpose(self.R)

        # Initialise Constraint Forces/Torques
        self.F = zeros((3,))
        self.L = zeros((3,))
