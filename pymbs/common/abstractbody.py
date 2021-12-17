from .mbselement import MbsElement
from .functions import transpose

import pymbs.symbolics as symbolics


class AbstractBody(MbsElement):
    """
    Base class for all bodies (input & processing)
    """
    def __init__(self, name, mass, cg, inertia, graph=None):
        # super constructor (CoordianteSystem)
        MbsElement.__init__(self, name, graph)

        # Type Conversions
        if ( isinstance(cg, list) ):
            cg = symbolics.Matrix(cg)
        if ( isinstance(inertia, list) ):
            inertia = symbolics.Matrix(inertia)
        if ( isinstance(inertia, symbolics.Matrix) and (inertia.shape() == (6,))):
            # inertia = [I_xx, I_xy, I_yy, I_xz, I_yz, I_zz]
            inertia = symbolics.Matrix([[inertia[0], inertia[1], inertia[3]],
                                    [inertia[1], inertia[2], inertia[4]],
                                    [inertia[3], inertia[4], inertia[5]]])

        # Assertions
        assert( isinstance(mass, float) or isinstance(mass, int) or
                isinstance(mass, symbolics.Variable) or (isinstance(mass, symbolics.Basic)))
        assert isinstance(cg, symbolics.Basic) and (cg.shape() in ((3,), (3,1))) , "cg - Type %s, Shape: %s, Value: %s"%(str(cg.__class__), str(cg.shape()), str(cg))
        assert isinstance(inertia, symbolics.Basic) and (inertia.shape() == (3,3)), "Inertia - Type %s, Shape: %s, Value: %s"%(str(inertia.__class__), str(inertia.shape()), str(inertia))

        # Test for symmetric inertia
        if not ( transpose(inertia) == inertia ):
            print("Warning (Body '%s'): Inertia matrix does not seem to be symmetric!"%name)

        self.mass=mass
        self.cg=cg
        self.inertia=inertia
