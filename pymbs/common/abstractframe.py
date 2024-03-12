from pymbs.symbolics import Matrix, Basic
from .mbselement import MbsElement


class AbstractFrame(MbsElement):
    """
    Base Class For All Coordinate Systems (Processing & Parent)
    """

    def __init__(self, name, p, R, graph=None):
        # Type Conversions
        if isinstance(p, list):
            for el in p:
                assert isinstance(el, (Basic, int, float))
            p = Matrix(p)
        if isinstance(R, list):
            for row in R:
                for el in row:
                    assert isinstance(el, (Basic, int, float))
            R = Matrix(R)

        # Assertions
        assert isinstance(p, Basic) and (p.shape() in ((3,), (3, 1))), \
            f'p - Type: {p.__class__}, Shape: {p.shape()}, Value: {p}'
        assert isinstance(R, Basic) and (R.shape() == (3, 3))
        #for el in R:
        #     assert( isinstance(el, Basic) )

        MbsElement.__init__(self, name, graph)
        self.p = p
        self.R = R
