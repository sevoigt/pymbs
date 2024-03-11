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

        # Type conversions
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
        assert(isinstance(mass, float) or 
               isinstance(mass, int) or
               isinstance(mass, symbolics.Variable) or 
               isinstance(mass, symbolics.Basic))
        assert isinstance(cg, symbolics.Basic) and (cg.shape() in ((3,), (3,1))) , \
            f'cg - Type {cg.__class__}, Shape: {cg.shape()}, Value: {cg}'
        assert isinstance(inertia, symbolics.Basic) and (inertia.shape() == (3,3)), \
            f'Inertia - Type {inertia.__class__}, Shape: {inertia.shape()}, Value: {inertia}'

        # Test for symmetric inertia
        if not ( transpose(inertia) == inertia ):
            print(f'Warning (Body "{name}"): Inertia matrix does not seem to be symmetric!')

        self.mass=mass
        self.cg=cg
        self.inertia=inertia
