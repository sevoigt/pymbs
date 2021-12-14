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
Created on 07.07.2009

@author: Christian Schubert
'''

from .MbsElement import MbsElement

from .Functions import transpose

import PyMbs.Symbolics as Symbolics


class AbstractBody(MbsElement):
    """
    Base Class For All Bodies (Input & Processing)
    """
    def __init__(self, name, mass, cg, inertia, graph=None):
        # super constructor (CoordianteSystem)
        MbsElement.__init__(self, name, graph)

        # Type Conversions
        if ( isinstance(cg, list) ):
            cg = Symbolics.Matrix(cg)
        if ( isinstance(inertia, list) ):
            inertia = Symbolics.Matrix(inertia)
        if ( isinstance(inertia, Symbolics.Matrix) and (inertia.shape() == (6,))):
            # inertia = [I_xx, I_xy, I_yy, I_xz, I_yz, I_zz]
            inertia = Symbolics.Matrix([[inertia[0], inertia[1], inertia[3]],
                                    [inertia[1], inertia[2], inertia[4]],
                                    [inertia[3], inertia[4], inertia[5]]])

        # Assertions
        assert( isinstance(mass, float) or isinstance(mass, int) or
                isinstance(mass, Symbolics.Variable) or (isinstance(mass, Symbolics.Basic)))
        assert isinstance(cg, Symbolics.Basic) and (cg.shape() in ((3,), (3,1))) , "cg - Type %s, Shape: %s, Value: %s"%(str(cg.__class__), str(cg.shape()), str(cg))
        assert isinstance(inertia, Symbolics.Basic) and (inertia.shape() == (3,3)), "Inertia - Type %s, Shape: %s, Value: %s"%(str(inertia.__class__), str(inertia.shape()), str(inertia))

        # Test for symmetric inertia
        if not ( transpose(inertia) == inertia ):
            print("Warning (Body '%s'): Inertia matrix does not seem to be symmetric!"%name)

        self.mass=mass
        self.cg=cg
        self.inertia=inertia
