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

import unittest
from math import pi
import PyMbs.Symbolics as Symbolics    # C++ Module

class TestSymbolics(unittest.TestCase):

    def test_graph(self):
        graph = Symbolics.Graph()
        a = graph.addVariable('a', (2, 2), 0, None, "No")
        self.assertIsInstance(a, Symbolics.Symbol)

    def test_symbol(self):
        a = Symbolics.Symbol('a')
        self.assertEqual(a.shape(), ())

        b = Symbolics.Symbol('b', (3, 2))
        self.assertEqual(b.shape(), (3, 2))

    def test_matrix(self):
        A = Symbolics.Matrix((2, 3))
        self.assertEqual(A.shape(), (2, 3))

        b = Symbolics.Matrix( [1,2,3] )
        self.assertEqual(b.shape(), (3, ))

        B = Symbolics.Matrix( [[1,2], [3,4], [5,6]] )
        self.assertEqual(B.shape(), (3, 2))

        # access elements
        A[1,2] = 1
        self.assertEqual(A[1, 2], 1)

    def test_operators(self):
        a = Symbolics.Symbol('a')
        A = Symbolics.Matrix( [[a,2], [3,4]] )
        B = Symbolics.Matrix( [[2,3], [4,5]] )

        AB = A + B
        AB.simplify()
        self.assertEqual(AB[0, 0], (a + 2))

    def test_sin(self):
        a = Symbolics.Symbol('a')
        sa = Symbolics.sin(a)

        zero = Symbolics.sin(0.0)
        self.assertEqual(zero, 0.0)

        one = Symbolics.sin(0.5*pi)
        self.assertEqual(one, 1.0)

        two_sine = sa + sa
        two_sine.simplify()
        self.assertEqual(two_sine, 2*Symbolics.sin(a))

    def test_der(self):
        a = Symbolics.Symbol('a')
        da = Symbolics.der(a)

        zero = Symbolics.der(0.0)
        self.assertEqual(zero, 0)

        one = Symbolics.der(1.0)
        self.assertEqual(one, 0)

        two_da = da + da
        two_da.simplify()
        self.assertEqual(two_da, 2*Symbolics.der(a))

        b = Symbolics.Symbol('b')
        db = Symbolics.der(b)
        self.assertEqual(Symbolics.der(a+b), da + db)
        self.assertEqual(Symbolics.der(a*b), da*b + a*db)

    def test_jacobian(self):
        a = Symbolics.Symbol('a')
        b = Symbolics.Symbol('b')
        c = Symbolics.Symbol('c')

        mat = Symbolics.Matrix( [a,b,c] )
        jac_mat = Symbolics.jacobian(mat,mat)
        self.assertEqual(jac_mat, Symbolics.Matrix([[1, 0, 0],
                                                    [0, 1, 0],
                                                    [0, 0, 1]]))

        mat_2 = Symbolics.Matrix( [a*a,b*b,c*c] )
        jac_mat_2 = Symbolics.jacobian(mat_2,mat)
        self.assertEqual(jac_mat_2, Symbolics.Matrix([[2*a,   0,   0],
                                                      [  0, 2*b,   0],
                                                      [  0,   0, 2*c]]))

    def test_get_set(self):
        A = Symbolics.Matrix( (3,3) )
        self.assertIsInstance(A, Symbolics.Matrix)
        A[0,0] = 1
        self.assertEqual(A[0, 0], 1)

        z = Symbolics.zeros( (3,3) )
        self.assertIsInstance(z, Symbolics.zeros)
        z[0,0] = 1
        self.assertEqual(z[0, 0], 1)
        self.assertIsInstance(z, Symbolics.Matrix)

        E = Symbolics.eye( (3,3) )
        self.assertIsInstance(E, Symbolics.eye)
        E[0,0] = 1
        self.assertEqual(E, Symbolics.eye((3, 3)))

    def test_if(self):
        a = Symbolics.Symbol('a')
        b = Symbolics.Symbol('b')
        condition = Symbolics.If(Symbolics.Less(a,b),a,b)
        self.assertIsInstance(condition, Symbolics.If)

if __name__ == '__main__':
    unittest.main()
