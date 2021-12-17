"""
Tests for the symbolic math C++ extension
"""

import unittest
from math import pi

import pymbs.symbolics as symbolics    # C++ Module


class TestSymbolics(unittest.TestCase):

    def test_graph(self):
        graph = symbolics.Graph()
        a = graph.addVariable('a', (2, 2), 0, None, "No")
        self.assertIsInstance(a, symbolics.Symbol)

    def test_symbol(self):
        a = symbolics.Symbol('a')
        self.assertEqual(a.shape(), ())

        b = symbolics.Symbol('b', (3, 2))
        self.assertEqual(b.shape(), (3, 2))

    def test_matrix(self):
        A = symbolics.Matrix((2, 3))
        self.assertEqual(A.shape(), (2, 3))

        b = symbolics.Matrix( [1,2,3] )
        self.assertEqual(b.shape(), (3, ))

        B = symbolics.Matrix( [[1,2], [3,4], [5,6]] )
        self.assertEqual(B.shape(), (3, 2))

        # access elements
        A[1,2] = 1
        self.assertEqual(A[1, 2], 1)

    def test_operators(self):
        a = symbolics.Symbol('a')
        A = symbolics.Matrix( [[a,2], [3,4]] )
        B = symbolics.Matrix( [[2,3], [4,5]] )

        AB = A + B
        AB.simplify()
        self.assertEqual(AB[0, 0], (a + 2))

    def test_sin(self):
        a = symbolics.Symbol('a')
        sa = symbolics.sin(a)

        zero = symbolics.sin(0.0)
        self.assertEqual(zero, 0.0)

        one = symbolics.sin(0.5*pi)
        self.assertEqual(one, 1.0)

        two_sine = sa + sa
        two_sine.simplify()
        self.assertEqual(two_sine, 2*symbolics.sin(a))

    def test_der(self):
        a = symbolics.Symbol('a')
        da = symbolics.der(a)

        zero = symbolics.der(0.0)
        self.assertEqual(zero, 0)

        one = symbolics.der(1.0)
        self.assertEqual(one, 0)

        two_da = da + da
        two_da.simplify()
        self.assertEqual(two_da, 2*symbolics.der(a))

        b = symbolics.Symbol('b')
        db = symbolics.der(b)
        self.assertEqual(symbolics.der(a+b), da + db)
        self.assertEqual(symbolics.der(a*b), da*b + a*db)

    def test_jacobian(self):
        a = symbolics.Symbol('a')
        b = symbolics.Symbol('b')
        c = symbolics.Symbol('c')

        mat = symbolics.Matrix( [a,b,c] )
        jac_mat = symbolics.jacobian(mat,mat)
        self.assertEqual(jac_mat, symbolics.Matrix([[1, 0, 0],
                                                    [0, 1, 0],
                                                    [0, 0, 1]]))

        mat_2 = symbolics.Matrix( [a*a,b*b,c*c] )
        jac_mat_2 = symbolics.jacobian(mat_2,mat)
        self.assertEqual(jac_mat_2, symbolics.Matrix([[2*a,   0,   0],
                                                      [  0, 2*b,   0],
                                                      [  0,   0, 2*c]]))

    def test_get_set(self):
        A = symbolics.Matrix( (3,3) )
        self.assertIsInstance(A, symbolics.Matrix)
        A[0,0] = 1
        self.assertEqual(A[0, 0], 1)

        z = symbolics.zeros( (3,3) )
        self.assertIsInstance(z, symbolics.zeros)
        z[0,0] = 1
        self.assertEqual(z[0, 0], 1)
        self.assertIsInstance(z, symbolics.Matrix)

        E = symbolics.eye( (3,3) )
        self.assertIsInstance(E, symbolics.eye)
        E[0,0] = 1
        self.assertEqual(E, symbolics.eye((3, 3)))

    def test_if(self):
        a = symbolics.Symbol('a')
        b = symbolics.Symbol('b')
        condition = symbolics.If(symbolics.Less(a,b),a,b)
        self.assertIsInstance(condition, symbolics.If)

if __name__ == '__main__':
    unittest.main()
