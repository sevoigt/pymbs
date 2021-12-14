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
Created on 08.08.2009

@author: knoll


Module containing classes modelling different types
of graphical representations of coordinate systems and bodies
'''

from PyMbs.Common.MbsElement import MbsElement

import PyMbs.Symbolics as Symbolics


# Questions:
# for all GraphRepObjects: middlepoint?

# Can one CS have more than one GraphReps?

class AbstractGraphRep(MbsElement):
    def __init__(self, name, r, T):
        MbsElement.__init__(self, name)

        assert isinstance(T, list)
        assert isinstance(r, list)

        self.T = T
        self.r = r
        self.color = None


class File(AbstractGraphRep):
    """
    Graphical representation is loaded from an stl-file
    (STereo Lithography format, can be exported by many CAD tools)
    Additional file-formats: *.obj
    """

    def __init__(self, name, r, T, pathToFile, scale=1):

        assert isinstance(pathToFile, str)
        #assert isinstance(scale, (int, float))
        assert isinstance(scale, (int, float, Symbolics.Basic))

        AbstractGraphRep.__init__(self, name, r, T)

        self.pathToFile = pathToFile
        self.scale = scale


class Box(AbstractGraphRep):
    """
    Represents a box primitive
    """

    def __init__(self, name, r, T, lx, ly, lz):

        assert isinstance(lx, (float, int, Symbolics.Symbol))
        assert isinstance(ly, (float, int, Symbolics.Symbol))
        assert isinstance(lz, (float, int, Symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.lx = lx
        self.ly = ly
        self.lz = lz


class Cylinder(AbstractGraphRep):
    """
    Represents a cylinder primitive
    """

    def __init__(self, name, r, T, radius, l, res=None):

        assert isinstance(radius, (float, int, Symbolics.Symbol))
        assert isinstance(l, (float, int, Symbolics.Symbol))

        if res == None:
            res=20

        assert isinstance(res, (float, int, Symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.radius = radius
        self.l = l

        self.res = res

class Sphere(AbstractGraphRep):
    """
    Represents a sphere primitive
    """

    def __init__(self, name, r, T, radius, res=None):

        assert isinstance(radius, (float, int, Symbolics.Symbol))

        if res == None:
            res=50

        assert isinstance(res, (float, int, Symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.radius = radius

        self.res = res


class Line(AbstractGraphRep):
    """
    Represents a line of length L in x-Direction with its origin in the middle
    """

    def __init__(self, name, r, T, L):

        assert( isinstance(L, (float, int, Symbolics.Symbol) ) )

        AbstractGraphRep.__init__(self, name, r, T)

        self.L = L


class Frame(AbstractGraphRep):
    """
    Represents a coordinate system
    """

    def __init__(self, name, r, T, size):

        assert isinstance(size, (float, int, Symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.size = size

class Arrow(AbstractGraphRep):
    """
    Represents an arrow
    """

    def __init__(self, name, r, T, size):

        assert isinstance(size, (float, int, Symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.size = size

class Flexible_Body(AbstractGraphRep):
    """
    Represents a 'flexible body',line with starting point and endpoint
    """

    def __init__(self, name, r, T, points):

        AbstractGraphRep.__init__(self, name, r, T)

        self.positions = points







