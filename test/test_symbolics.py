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

import unittest
from numpy import cross
from PyMbs.Symbolics import Symbol, transpose, eye, Matrix, element, solve,\
                            SymmetricMatrix, skew

class TestFunctions(unittest.TestCase):

    def setUp(self):
        self.a = Symbol('a')
        self.b = Symbol('b')

    def test_transpose_symbol(self):
        exp = transpose(self.a)*self.b - transpose(self.b)*self.a
        self.assertFalse(exp == 0)

    def test_transpose_simplify(self):
        exp = transpose(self.a) + transpose(-self.a)
        exp = exp.simplify()
        self.assertEqual(exp, None)

    def test_transpose_eye(self):
        E = eye(3)
        exp3 = self.a*transpose(E)
        self.assertEqual(exp3, self.a*E)

    def test_transpose_matrix(self):
        m = Matrix([[0, 1],[0, 1]])
        mT = Matrix([[0, 0],[1, 1]])
        self.assertEqual(transpose(m), mT)
        self.assertEqual(m, transpose(transpose(m)))

    def test_transpose_number(self):
        self.assertEqual(transpose(5), 5)

    @unittest.skip("Not working yet")
    def test_skew(self):
        v1 = Symbol('v1')
        v2 = Symbol('v2')
        v3 = Symbol('v3')
        v = [v1,v2,v3]

        a1 = Symbol('a1')
        a2 = Symbol('a2')
        a3 = Symbol('a3')
        a = [a1,a2,a3]
        cp = Matrix([[- v3*a2 + v2*a3],
                     [  v3*a1 - v1*a3],
                     [- v2*a1 + v1*a2]])
        s = skew(Matrix(v))
        a = Matrix(a)
        self.assertEqual(s*a, cp)
    
    def test_element(self):
        v0 = Symbol('v0')
        v1 = Symbol('v1')
        v2 = Symbol('v2')
        v = Matrix([v0, v1, v2])

        self.assertEqual(element(v, 2, 0), v2)

    @unittest.skip("Not working yet")
    def test_solve_numbers(self):
        A = Matrix([[8,1,6],
                    [3,5,7],
                    [4,9,2]])
        b = Matrix([28,34,28])
        x = solve(A, b)

        self.assertEqual(x, Matrix([1, 2, 3]))

    @unittest.skip("Not working yet")
    def test_solve_symbols(self):
        a = Symbol('a')
        b = Symbol('b')
        c = Symbol('c')
        d = Symbol('d')
        e = Symbol('e')
        f = Symbol('f')

        A = Matrix([[a, b],
                    [c, d]])
        b = Matrix([e, f])
        x = solve(A, b)

        t = A*x
        t.simplify()
        self.assertEqual(t, b)
        
    @unittest.skip("Not working yet")
    def test_symmetricMatrix(self):
        a = Symbol('a')
        b = Symbol('b')
        c = Symbol('c')
        d = Symbol('d')
        e = Symbol('e')
        f = Symbol('f')

        M = SymmetricMatrix([a, b, c, d, e, f])

        M2 = Matrix([[a, b, d],
                     [b, c, e],
                     [d, e, f]])

        self.assertEqual(M, transpose(M))
        self.assertEqual(M, M2)

if __name__ == '__main__':
    unittest.main()
