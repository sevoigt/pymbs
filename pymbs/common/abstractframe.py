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
from PyMbs.Symbolics import Matrix, Basic


class AbstractFrame(MbsElement):
    '''
    Base Class For All Coordinate Systems (Processing & Parent)
    '''
    def __init__(self, name, p, R, graph=None):
        # Type Conversions
        if ( isinstance(p, list) ):
            for el in p:
                assert( isinstance(el, Basic) or isinstance(el, int) or isinstance(el, float) )
            p = Matrix(p)
        if ( isinstance(R, list) ):
            for row in R:
                for el in row:
                    assert( isinstance(el, Basic) or isinstance(el, int) or isinstance(el, float) )
            R = Matrix(R)

        # Assertions
        assert isinstance(p, Basic) and (p.shape() in ((3,), (3,1))), "p - Type: %s, Shape: %s, Value: %s"%(str(p.__class__), str(p.shape()), str(p))
        assert isinstance(R, Basic) and (R.shape() == (3,3))
#        for el in R:
#            assert( isinstance(el, Basic) )

        MbsElement.__init__(self, name, graph)
        self.p=p
        self.R=R

