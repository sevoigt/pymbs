from pymbs.common.mbselement import MbsElement

import pymbs.symbolics as symbolics


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
        assert isinstance(scale, (int, float, symbolics.Basic))

        AbstractGraphRep.__init__(self, name, r, T)

        self.pathToFile = pathToFile
        self.scale = scale


class Box(AbstractGraphRep):
    """
    Represents a box primitive
    """

    def __init__(self, name, r, T, lx, ly, lz):

        assert isinstance(lx, (float, int, symbolics.Symbol))
        assert isinstance(ly, (float, int, symbolics.Symbol))
        assert isinstance(lz, (float, int, symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.lx = lx
        self.ly = ly
        self.lz = lz


class Cylinder(AbstractGraphRep):
    """
    Represents a cylinder primitive
    """

    def __init__(self, name, r, T, radius, l, res=None):

        assert isinstance(radius, (float, int, symbolics.Symbol))
        assert isinstance(l, (float, int, symbolics.Symbol))

        if res == None:
            res=20

        assert isinstance(res, (float, int, symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.radius = radius
        self.l = l

        self.res = res

class Sphere(AbstractGraphRep):
    """
    Represents a sphere primitive
    """

    def __init__(self, name, r, T, radius, res=None):

        assert isinstance(radius, (float, int, symbolics.Symbol))

        if res == None:
            res=50

        assert isinstance(res, (float, int, symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.radius = radius

        self.res = res


class Line(AbstractGraphRep):
    """
    Represents a line of length L in x-Direction with its origin in the middle
    """

    def __init__(self, name, r, T, L):

        assert( isinstance(L, (float, int, symbolics.Symbol) ) )

        AbstractGraphRep.__init__(self, name, r, T)

        self.L = L


class Frame(AbstractGraphRep):
    """
    Represents a coordinate system
    """

    def __init__(self, name, r, T, size):

        assert isinstance(size, (float, int, symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.size = size

class Arrow(AbstractGraphRep):
    """
    Represents an arrow
    """

    def __init__(self, name, r, T, size):

        assert isinstance(size, (float, int, symbolics.Symbol))

        AbstractGraphRep.__init__(self, name, r, T)

        self.size = size

class Flexible_Body(AbstractGraphRep):
    """
    Represents a 'flexible body',line with starting point and endpoint
    """

    def __init__(self, name, r, T, points):

        AbstractGraphRep.__init__(self, name, r, T)

        self.positions = points







