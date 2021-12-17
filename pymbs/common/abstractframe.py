from .mbselement import MbsElement
from pymbs.symbolics import Matrix, Basic


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

