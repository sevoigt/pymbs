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

#from ipHelp import IPS, ip_syshook, ST
#ip_syshook(1)



# Unitests

import unittest

print("-------- Unittests --------------\n")
import PyMbs.Common.Functions
from PyMbs.Common.Functions import skew, subs, element, solve, symmetricMatrix
import PyMbs.Common.Simple
from PyMbs.Symbolics import __version__, eye, Matrix, Symbol, cos, sin
from numpy import cross

from math import pi as math_pi



from PyMbs.Common.Expression import Expression, VarKind
from PyMbs.Common.MbsEquations import MbsEquations
from PyMbs.Common.State import State
from PyMbs.Graph.Graph import Graph

import PyMbs.Common.GraphReps as gr

# flag used to skip slow tests
FASTRUN = False

class TestFunctions(unittest.TestCase):
    # Tests without any asserts shall check
    # whether an exception is raised

    def setUp(self):


        global a, b, c, d
        #a, b, c, d = symbols("abcd", commutative = False)
        a = PyMbs.Symbolics.Symbol('a', commutative = False)
        b = PyMbs.Symbolics.Symbol('b', commutative = False)
        c = PyMbs.Symbolics.Symbol('c', commutative = False)
        d = PyMbs.Symbolics.Symbol('d', commutative = False)

    def testPyMbs_Symbolicsversion(self):
        # if this test fails, maybe some PyMbs.Symbolics behavior has changed
#        print "PyMbs.Symbolics Version: %s"%__version__
        self.assertTrue(__version__ in ('0.6.5', '0.6.6', '0.6.7', '0.6.8', '0.7.0') )

    def testsubs_mul(self):

        newexp=subs(a*b*c, b, d)

        self.assertTrue(newexp == a*d*c)
        self.assertFalse(newexp == a*c*d)

    def testsubs_div(self):

        newexp=subs(a/b*c, b, d)

        self.assertTrue(newexp == a/d*c)
        self.assertFalse(newexp == a*c/d)


    def testsubs_pow(self):

        newexp=subs(a**b*c, b, d)

        self.assertTrue(newexp == a**d*c)
        self.assertFalse(newexp == c*a**d)


    def test_sympify(self):
        # tests whether specifying a local variable helps
        
        # sympify without locals
        tmp1 = PyMbs.Symbolics.sympify('transpose(a)')

        # sympify with locals
        local_dict = {}
        exec('from PyMbs.Common.Functions import *', local_dict)
        tmp2 = PyMbs.Symbolics.sympify('transpose(a)', locals=local_dict)
        tmp3 = PyMbs.Common.Functions.transpose(a)
        
        # Testen
        # self.assertTrue( tmp1.__class__ != tmp2.__class__ )
        self.assertTrue( tmp2.__class__ == tmp3.__class__ )
        

    def test_transpose(self):
        # transposes a Symbol:
        transpose = PyMbs.Common.Functions.transpose
        exp=a*transpose(b)

        # transpose a Matrix
        E=eye(3)
        exp2=a*transpose(E)

        self.assertTrue(exp2 == a*E)

        M=Matrix([[a,b],[c,d]])

        self.assertTrue(b*transpose(M) == b*M.T)

        self.assertTrue( transpose(5) == 5 )
        self.assertTrue( transpose(0) == 0 )
        Zero = PyMbs.Symbolics.sympify(0)
        self.assertTrue( transpose(Zero) == Zero )

        A = PyMbs.Common.Functions.sym('A')
        B = PyMbs.Common.Functions.sym('B')

        t = transpose(A)*B - B*transpose(A)
        self.assertFalse(t == 0)
        
        # Test simplification
        t1 = transpose(a) + transpose(-a)
        self.assertTrue(t1 == 0)
        
        # Put a transpose into a transpose
        t2 = transpose(transpose(a))
        self.assertTrue(t2 == a)

    def test_skew(self):
        # check that skew(v)*a = v x a
        v1 = Symbol('v1')
        v2 = Symbol('v2')
        v3 = Symbol('v3')
        v = [v1,v2,v3]

        a1 = Symbol('a1')
        a2 = Symbol('a2')
        a3 = Symbol('a3')
        a = [a1,a2,a3]
        cp = Matrix(cross(v,a)).transpose()
        s = skew(Matrix(v))
        a = Matrix(a)
        self.assertTrue( s*a == cp )

        A = PyMbs.Common.Functions.sym('A')
        B = PyMbs.Common.Functions.sym('B')

        t = skew(A)*B - B*skew(A)
        self.assertFalse(t == 0)


    def testsubs_skew(self):
        # check that subs(skew, s, v) == [[...],[...],[...]]
        s = Symbol('s')
        exp = skew(s)

        v0 = Symbol('v0')
        v1 = Symbol('v1')
        v2 = Symbol('v2')
        v = Matrix([v0,v1,v2])

        self.assertTrue( subs(exp, s, v) == skew(v) )


    def testsubs_transpose(self):
        # check that subs(skew, s, v) == [[...],[...],[...]]
        s = Symbol('s')
        t = Symbol('t')
        exp = PyMbs.Common.Functions.transpose(s)

        self.assertTrue( subs(exp, PyMbs.Common.Functions.transpose(s), t) == t )

    def test_element(self):
        # check that subs(skew, s, v) == [[...],[...],[...]]
        s = Symbol('s')
        exp = element(s, 2, 0)

        v0 = Symbol('v0')
        v1 = Symbol('v1')
        v2 = Symbol('v2')
        v = Matrix([v0,v1,v2])

        self.assertTrue( element(v, 2, 0) == v2 )

    def test_solve(self):
        '''
        Test solve command
        '''

        A = Symbol('A')
        b = Symbol('b')

        t1 = solve(A,b)

        A_exp = Matrix([[8,1,6],
                        [3,5,7],
                        [4,9,2]])
        b_exp = Matrix([28,34,28])

        x_exp = solve(A_exp, b_exp)
        self.assertTrue(x_exp == Matrix([1,2,3]))

#        a,b,c,d,e,f = symbols('abcdef')
        a = PyMbs.Symbolics.Symbol('a')
        b = PyMbs.Symbolics.Symbol('b')
        c = PyMbs.Symbolics.Symbol('c')
        d = PyMbs.Symbolics.Symbol('d')
        e = PyMbs.Symbolics.Symbol('e')
        f = PyMbs.Symbolics.Symbol('f')


        A_sym = Matrix([[a,b],
                        [c,d]])
        b_sym = Matrix([e,f])

        x_sym = solve(A_sym, b_sym)

        t = A_sym*x_sym
        t.simplify()
        self.assertTrue(t == b_sym)


    def test_symmetricMatrix(self):

        a = PyMbs.Symbolics.Symbol('a')
        b = PyMbs.Symbolics.Symbol('b')
        c = PyMbs.Symbolics.Symbol('c')
        d = PyMbs.Symbolics.Symbol('d')
        e = PyMbs.Symbolics.Symbol('e')
        f = PyMbs.Symbolics.Symbol('f')


        M=symmetricMatrix([a, b, c, d, e, f])

        M2=Matrix([[a, b, d],
                   [b, c, e],
                   [d, e, f]])

        self.assertTrue(M == M.T)
        self.assertTrue(M == M2)


    def test_expression(self):

        exp1 = Expression("Test1", Symbol("exp1"), 0)
        exp2 = Expression("Test2", Symbol("exp2"), 0, VarKind.STATE)
        exp3 = Expression("Test2", Symbol("exp3"), 0, [VarKind.STATE, VarKind.Sensor])

        self.assertTrue( VarKind.STATE in exp2.category )
        self.assertTrue( VarKind.Sensor not in exp2.category )

        self.assertTrue( VarKind.STATE in exp3.category )
        self.assertTrue( VarKind.Sensor in exp3.category )

    def test_Graph_addExpression(self):

        exp0 = Expression("Test1", Symbol("exp0"), Matrix([1,1]))
        exp1 = Expression("Test1", Symbol("exp1"), Matrix([5,5]))
        exp2 = Expression("Test2", Symbol("exp2"), exp0.symbol-exp1.symbol, VarKind.STATE)
        exp3 = Expression("Test2", Symbol("exp3"), exp0.symbol*PyMbs.Common.Functions.transpose(exp1.symbol), [VarKind.STATE, VarKind.Sensor])
        exp4 = Expression("Test2", Symbol("exp4"), exp3.symbol**2, [VarKind.STATE, VarKind.Sensor])

        g = Graph()
        g.addExpression(exp0)
        g.addExpression(exp1)
        g.addExpression(exp2)
        g.addExpression(exp3)
        g.addExpression(exp4)

        self.assertTrue( exp4.shape == (2,2) )

    def test_Graph(self):
        if FASTRUN: return

        exp0 = Expression("Test0", Symbol("exp0"), Matrix([1,1]), VarKind.PARAMS)
        exp1 = Expression("Test1", Symbol("exp1"), Matrix([5,5]), VarKind.PARAMS)
        exp2 = Expression("Test2", Symbol("exp2"), exp0.symbol-exp1.symbol, VarKind.PARAMS)
        exp3 = Expression("Test3", Symbol("exp3"), exp0.symbol*PyMbs.Common.Functions.transpose(exp1.symbol), VarKind.PARAMS)
        exp4 = Expression("Test4", Symbol("exp4"), exp3.symbol**2, VarKind.PARAMS)

        exp5 = Expression("Test5", Symbol("exp5"), exp0.symbol)
        exp6 = Expression("Test6", Symbol("exp6"), exp1.symbol)
        exp7 = Expression("Test7", Symbol("exp7"), exp5.symbol-exp6.symbol)
        exp8 = Expression("Test8", Symbol("exp8"), exp5.symbol*PyMbs.Common.Functions.transpose(exp6.symbol), [VarKind.Sensor])
        exp9 = Expression("Test9", Symbol("exp9"), exp8.symbol**2, [VarKind.STATE, VarKind.Sensor])

        s = State()
        mbseqn = MbsEquations(s,[exp0,exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9])
        g = Graph(mbseqn)

        g.buildGraph()
        g.reduceGraph()

        mbs = g.getMbsEqns([VarKind.PARAMS])

        self.assertTrue( exp4.shape == (2,2) )


        mbs = g.getMbsEqns([VarKind.STATE,VarKind.Sensor], [VarKind.PARAMS])
        self.assertTrue( exp9.shape == (2,2) )
        self.assertTrue( len(mbs.eqns) == 2)

        for eqn in mbs.eqns:
            self.assertTrue( VarKind.PARAMS not in eqn.category )
            self.assertTrue( VarKind.Sensor in eqn.category )
            self.assertTrue( VarKind.DER_STATE not in eqn.category )

        mbs = g.getMbsEqns(VarKind.STATE)
        self.assertTrue( len(mbs.eqns) == 4)

        mbs = g.getMbsEqns(VarKind.STATE,VarKind.PARAMS)
        self.assertTrue( len(mbs.eqns) == 2)

        mbs = g.getMbsEqns(VarKind.Sensor)
        self.assertTrue( len(mbs.eqns) == 4)

    def test_Graphreduce(self):
        if FASTRUN: return

        exp0 = Expression("Test0", Symbol("exp0"), Matrix([[1,0],[0,1]]), VarKind.PARAMS)
        exp1 = Expression("Test1", Symbol("exp1"), Matrix([[0,0],[0,0]]), VarKind.PARAMS)
        exp2 = Expression("Test2", Symbol("exp2"), Matrix([1,1]), VarKind.PARAMS)
        exp3 = Expression("Test3", Symbol("exp3"), Matrix([5,5]), VarKind.PARAMS)
        exp4 = Expression("Test4", Symbol("exp4"), Matrix([[1,1],[0,1]]), VarKind.PARAMS)

        exp5 = Expression("Test5", Symbol("exp5"), exp0.symbol + exp4.symbol)
        exp6 = Expression("Test6", Symbol("exp6"), exp1.symbol + exp4.symbol)
        exp7 = Expression("Test7", Symbol("exp7"), exp0.symbol*exp4.symbol)
        exp8 = Expression("Test8", Symbol("exp8"), exp1.symbol*exp4.symbol, [VarKind.Sensor])
        exp9 = Expression("Test9", Symbol("exp9"), exp7.symbol, [VarKind.STATE, VarKind.Sensor])
        exp10 = Expression("Test10", Symbol("exp10"), exp9.symbol+exp4.symbol, [VarKind.STATE])

        s = State()
        mbseqn = MbsEquations(s,[exp0,exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9,exp10])
        g = Graph(mbseqn)

        g.buildGraph()
        g.reduceGraph()

        self.assertTrue( len(g.getNode(exp0.symbol).childs) == 2)
        self.assertTrue( len(g.getNode(exp1.symbol).childs) == 2)
        self.assertTrue( len(g.getNode(exp2.symbol).childs) == 0)
        self.assertTrue( len(g.getNode(exp3.symbol).childs) == 0)
        self.assertTrue( len(g.getNode(exp4.symbol).childs) == 5)
        self.assertTrue( len(g.getNode(exp5.symbol).childs) == 0)
        self.assertTrue( len(g.getNode(exp6.symbol).childs) == 0)
        self.assertTrue( len(g.getNode(exp7.symbol).childs) == 1)
        self.assertTrue( len(g.getNode(exp8.symbol).childs) == 0)
        self.assertTrue( len(g.getNode(exp9.symbol).childs) == 1)
        self.assertTrue( len(g.getNode(exp10.symbol).childs) == 0)
        self.assertTrue( len(g.getNode(exp0.symbol).parents) == 0)
        self.assertTrue( len(g.getNode(exp1.symbol).parents) == 0)
        self.assertTrue( len(g.getNode(exp2.symbol).parents) == 0)
        self.assertTrue( len(g.getNode(exp3.symbol).parents) == 0)
        self.assertTrue( len(g.getNode(exp4.symbol).parents) == 0)
        self.assertTrue( len(g.getNode(exp5.symbol).parents) == 2)
        self.assertTrue( len(g.getNode(exp6.symbol).parents) == 2)
        self.assertTrue( len(g.getNode(exp7.symbol).parents) == 2)
        self.assertTrue( len(g.getNode(exp8.symbol).parents) == 2)
        self.assertTrue( len(g.getNode(exp9.symbol).parents) == 1)
        self.assertTrue( len(g.getNode(exp10.symbol).parents) == 2)

        mbs = g.getMbsEqns(VarKind.STATE,VarKind.PARAMS)

#        self.assertTrue( len(mbs.eqns) == 5)

        for eqn in mbs.eqns:
            self.assertTrue( VarKind.PARAMS not in eqn.category )

    def test_GraphsetValue(self):
        if FASTRUN: return

        exp0 = Expression("Test0", Symbol("exp0"), 1, VarKind.PARAMS)
        exp1 = Expression("Test1", Symbol("exp1"), 2, VarKind.PARAMS)
        exp2 = Expression("Test2", Symbol("exp2"), exp0.symbol+exp1.symbol, VarKind.PARAMS)
        exp3 = Expression("Test3", Symbol("exp3"), exp2.symbol*exp0.symbol, VarKind.PARAMS)
        exp4 = Expression("Test4", Symbol("exp4"), exp3.symbol**2, VarKind.PARAMS)


        # states
        q = Symbol("q")
        q0 = Symbol("q0")
        q1 = Symbol("q1")
        #state expression
        sexpq = Expression("sexp0", q, PyMbs.Symbolics.Matrix([0,0]))
        sexp0 = Expression("sexp0", q0, element(q,0,0))
        sexp1 = Expression("sexp1", q1, element(q,PyMbs.Symbolics.sympify(1),PyMbs.Symbolics.sympify(0)))
        s = State([q0,q1])

        exp5 = Expression("Test5", Symbol("exp5"), exp0.symbol + q0)
        exp6 = Expression("Test6", Symbol("exp6"), exp1.symbol + q1)
        exp7 = Expression("Test7", Symbol("exp7"), exp5.symbol*exp6.symbol)
        exp8 = Expression("Test8", Symbol("exp8"), exp5.symbol+exp6.symbol, [VarKind.Sensor])
        exp9 = Expression("Test9", Symbol("exp9"), exp7.symbol, [VarKind.STATE, VarKind.Sensor])
        exp10 = Expression("Test10", Symbol("exp10"), exp9.symbol+exp4.symbol, [VarKind.STATE])


        mbseqn = MbsEquations(s,[exp0,exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9,exp10,sexp0,sexp1,sexpq])
        g = Graph(mbseqn)

        g.buildGraph()

        self.assertTrue( g.getValue(exp4.symbol) == 9)

        g.changeValue(exp0.symbol,2)
        self.assertTrue( g.getValue(exp4.symbol) == 64)
        self.assertTrue( g.getValue(exp9.symbol) == 4)
        self.assertTrue( g.getValue(exp10.symbol) == 68)

        g.changeValue(q, PyMbs.Symbolics.Matrix([1,1]))
        self.assertTrue( g.getValue(exp10.symbol) == 73)

        g.changeValue(q0, 0)
        self.assertTrue( g.getValue(exp10.symbol) == 70)

        # Shape==(0,0) represents a scalar symbol
        self.assertTrue( g.getShape(exp9.exp) == (0,0))

    def test_trigsimp(self):

#        simp = lambda x: PyMbs.Symbolics.trigsimp(x, deep=True, recursive=True)
#        simp = ownTrigSimp
#        simp = simpleTrigSimp
#        s = SimpleMATLAB()
        simp = PyMbs.Common.Simple.simple

        #x,y = symbols('xy')
        x = PyMbs.Symbolics.Symbol('x')
        y = PyMbs.Symbolics.Symbol('y')

        # some trivial tests:
        self.assertTrue(simp(x) == x)
        self.assertTrue(simp(cos(x)) == cos(x))
        self.assertTrue(simp(sin(x)) == sin(x))
#        self.assertTrue(simp(cos(x)*sin(x)) == cos(x)*sin(x))
        t = simp(cos(x)*sin(x))
        self.assertTrue( (t == sin(2*x)/2) or (t == cos(x)*sin(x)) )
        self.assertTrue(simp(cos(x)*sin(y)) == cos(x)*sin(y))
#        self.assertTrue(simp(cos(y)*sin(x)) == cos(y)*sin(x))

        t1 = cos(x)**2+sin(x)**2
        self.assertTrue(simp(t1) == 1)

#        t2 = (cos(x)**2+sin(x)**2)*(cos(y)**2+sin(y)**2)
        t2 = (cos(x)**2)*(cos(y)**2) + (cos(x)**2)*(sin(y)**2) + (sin(x)**2)*(cos(y)**2) + (sin(x)**2)*(sin(y)**2)
        self.assertTrue(simp(t2) == 1)

        t3 = cos(x)*cos(y) + sin(x)*sin(y)
        res3 = simp(t3)
        self.assertTrue((res3 == cos(x-y)) or (res3 == cos(y-x)))

        t4 = cos(x)*cos(y) - sin(x)*sin(y)
        self.assertTrue(simp(t4) == cos(x+y))

        t5 = sin(x)*cos(y) + cos(x)*sin(y)
        self.assertTrue(simp(t5) == sin(x+y))

        t6 = -sin(x)*cos(y) - cos(x)*sin(y)
        self.assertTrue(simp(t6) == -sin(x+y))

        # Now it is getting really hard!
        v0 = Symbol('v0')
        v1 = Symbol('v1')
        v2 = Symbol('v2')
        v = Matrix([v0,v1,v2])

        al1 = Symbol('al1')
        al2 = Symbol('al2')
        be = Symbol('be')
        ga = Symbol('ga')

        Tal1 = rotMat(al1, 'x')
        Tal2 = rotMat(al2, 'x')
        Tbe = rotMat(be, 'y')
        Tga = rotMat(ga, 'z')

        t7 = Tal1*Tal2*Tbe*Tga*v
        t7 = Tga.T*Tbe.T*Tal2.T*Tal1.T*t7
        # it just hangs here, since term.match is too clever/stupid
        res7 = simp(t7)
        self.assertTrue(res7 == v)


from PyMbs.Input import *

class TestInput(unittest.TestCase):
    """
    Test Input functionality
    mainly the tests consists in the fact that no exception is thrown
    """

    def setUp(self):

        global world, m, I

        world=MbsSystem([0,0,-1])

        m=world.addParam('mass', 'm', 1)
        I=world.addParam('inertia', 'J', 1)

    def test_getParentBody(self):

        body1 = world.addBody('body1', 0)
        body2 = world.addBody('body2', 0)

        body1.addFrame('CS1')
        body1.CS1.addFrame('CS2')
        body1.CS1.CS2.addFrame('CS3')
        parentBody = body1.CS1.CS2.CS3.getParentBody()

        self.assertTrue(body1 is parentBody)


    def test_addParam1(self):
        """
        """


        cg_pend=world.addParam('cg Pendulum', 'cg_pend', [0, 0, 0])

        zeromatrix=world.addParam('test', 'test_matrix', zeros((3,3)))

        # now a arbitrary shaped matrix
        z2=world.addParam('test', 'test_matrix', zeros((5,7)))
        self.assertTrue(z2[2,5].name.endswith('2_5'))

        I_chass = world.addParam('inertia chassis', 'I_chass',
                         [617.79, 0.35, 1206.98, -74.05, -0.17, 1106.6])

        pend=world.addBody('Pendulum', m, cg_pend, diag([0,0,I]))

        self.assertRaises(ValueError,
              world.addParam, 'mass2', 'm', 4)

        self.assertRaises(ValueError,
              world.addParam, 'mass', 'm2', 7)

    def test_addParam2(self):
        # gravity and g are reserved

        self.assertRaises(ValueError,
              world.addParam, 'gravity', 'g_', 4)

        self.assertRaises(ValueError,
              world.addParam, 'gravity_', 'g', 4)

    def test_addParam3(self):
        # test setGravity

        self.assertTrue(world.expressionDict['gravity'].exp == 9.81)

        world.setGravity(15.03)

        self.assertTrue(world.expressionDict['gravity'].exp == 15.03)

    def test_addParam4(self):
        # tests Bug 003
        F1=world.addParam('force', 'F1', 1)
        world.addFrame('CS_A', zeros((3,)), eye(3))
        world.addFrame('CS_B', zeros((3,)), eye(3))
        world.addLoad.PtPForce(world.CS_A, world.CS_B, F1, name='Spring')


    def test_addBody(self):
        chass=world.addBody('chassis', m, zeros((3,)), diag([0,0,I]))
        # Prevent name-doubling
        self.assertRaises(ValueError,
              world.addBody, 'chassis', m, zeros((3,)), diag([0,0,I]))

    def test_addJoint(self):

        world.addFrame('CS_world')
        freeBody = world.addBody('free_body',
                                 m, zeros((3,)), diag([0,0,I]))
        freeBody.addFrame('CS_A')
        world.addJoint('DOF6', world.CS_world,
                       freeBody.CS_A,
                       ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'])

        bodyList=list(world.bodyDict.values())
        self.assertTrue(len(bodyList) == 6)

        jointList=list(world.jointDict.values())
        self.assertTrue(len(jointList) == 6)


    def test_addJoint2(self):
        # it should not be possible to use one body twice as parentBody of
        # joint-end-frames

        world.addFrame('CS_A')
        world.addFrame('CS_B', [0, 1, 0])

        freeBody = world.addBody('free_body',
                                 m, zeros((3,)), diag([0,0,I]))
        freeBody.addFrame('CS_A')
        freeBody.addFrame('CS_B')
        world.addJoint('revJoint1', world.CS_A, freeBody.CS_A, 'Rx')
        self.assertRaises(ValueError, world.addJoint, 'revJoint2',
                                      world.CS_A, freeBody.CS_A, 'Tx')

    def test_addJoint3(self):
        world.addFrame('CS_A')
        world.addFrame('CS_B', [0, 1, 0])

        freeBody = world.addBody('free_body',
                                 m, zeros((3,)), diag([0,0,I]))
        body2 = world.addBody('body2',
                                 m, zeros((3,)), diag([0,0,I]))

        freeBody.addFrame('CS_A')
        jointList = world.addJoint('Joint1', world.CS_A,
                                   freeBody.CS_A, ['Rx', 'Tz'], [10, 0], [-1.4, 12.3])

        # test that a sequence is returned
        self.assertTrue(len(jointList) == 2)

        lastJoint = jointList[-1]

        self.assertTrue(lastJoint.q0 == 0)
        self.assertTrue(lastJoint.qd0 == 12.3)

        j0=world.jointDict['Joint1_Rx']

        self.assertTrue(j0.q0 == 10)
        self.assertTrue(j0.qd0 == -1.4)

        body2.addFrame('CS_A')
        j2=world.addJoint('Joint2', world.CS_A,
                                  body2.CS_A, 'Ty')

        self.assertTrue(j2.q0 == 0)
        self.assertTrue(j2.qd0 == 0)

    def test_addJoint4(self):

        #multiple DOF and initial values
        x0 = world.addParam('x0', 'x0', 0.8)
        y0 = world.addParam('y0', 'y0', 0.3)
        phi0 = world.addParam('phi0', 'z0', -0.1)

        world.addFrame('CS_A')
        pend = world.addBody('pendulum',
                                 m, zeros((3,)), diag([0,0,I]))
        pend.addFrame('CS_0')

        world.addJoint('J_Planar', world.CS_A,
                       pend.CS_0, ['Tx', 'Ty', 'Rz'],
                       [x0, y0, phi0], [1,2,3])

        jTx=world.jointDict['J_Planar_Tx']
        jTy=world.jointDict['J_Planar_Ty']
        jRz=world.jointDict['J_Planar_Rz']

        self.assertTrue(jTx.q0 == x0)
        self.assertTrue(jTx.qd0 == 1)

        self.assertTrue(jTy.q0 == y0)
        self.assertTrue(jTy.qd0 == 2)

        self.assertTrue(jRz.q0 == phi0)
        self.assertTrue(jRz.qd0 == 3)

        pend2 = world.addBody('pendulum2',
                                 m, zeros((3,)), diag([0,0,I]))

        pend2.addFrame('CS_0')

        # already existing name
        self.assertRaises(ValueError, world.addJoint,
                       'J_Planar', world.CS_A, pend2.CS_0,
                       ['Tx', 'Ty', 'Tz'])

        # invalid lengths of initvalue Lists
        self.assertRaises(AssertionError, world.addJoint,
                       'J_Planar2', world.CS_A, pend2.CS_0,
                       ['Tx', 'Ty', 'Rz'], [x0, y0], [1,2,3])


        self.assertRaises(AssertionError, world.addJoint,
                       'J_Planar3', world.CS_A, pend2.CS_0,
                       ['Tx', 'Ty', 'Rz'], [x0, y0, phi0], [1,2])

    def test_addJoint5(self):

        # use bodies as CS

        world.addFrame('CS_A')
        pend1 = world.addBody('pendulum1',
                                 m, zeros((3,)), diag([0,0,I]))

        pend2 = world.addBody('pendulum2',
                                 m, zeros((3,)), diag([0,0,I]))

        # too lazy to create additional Frametems

        world.addJoint('myJoint', pend1, pend2, 'Rz')

    def test_addJoint6(self):
        # inspired from Bug 007:
        pend1 = world.addBody('pendulum1',
                                 m, zeros((3,)), diag([0,0,I]))

        # a joint on one Body:
        self.assertRaises(AssertionError, world.addJoint,
                          'myJoint', pend1, pend1, 'Rx')



    def test_setJointRange(self):
        x0 = world.addParam('x0', 'x0', 0.5)
        y0 = world.addParam('y0', 'y0', 1.3)

        pend1 = world.addBody('pendulum1',
                                 m, zeros((3,)), diag([0,0,I]))

        pend2 = world.addBody('pendulum2',
                                 m, zeros((3,)), diag([0,0,I]))

        box = world.addBody('box',
                                 m, zeros((3,)), diag([0,0,I]))


        j1 = world.addJoint('myJoint', pend1, pend2, 'Rz')
        j2 = world.addJoint('myJoint2', pend2, box, 'Tx')

        # test the default values

        rangeTuple = world.jointRangeDict[j1]
        self.assertTrue(rangeTuple == (-math_pi, math_pi))

        rangeTuple = world.jointRangeDict[j2]
        self.assertTrue(rangeTuple == (-1, 1))

        # valid vaulues
        world.setJointRange(j1, 0, .1)
        world.setJointRange(j1, x0, 1)
        world.setJointRange(j1, 1, y0)

        rangeTuple = world.jointRangeDict[j1]
        self.assertTrue(rangeTuple == (1, y0))

        # invalidValues

        self.assertRaises(ValueError, world.setJointRange, j1, 1, 0)
        self.assertRaises(ValueError, world.setJointRange, j1, 1.0, x0)
        self.assertRaises(ValueError, world.setJointRange, j1, y0, x0)



    def test_addExpression(self):

        # just add some nonsens expressions

        expr_symbol = world.addExpression('testExp', 'gamma', m**2)
        self.assertTrue(expr_symbol.name == 'gamma')

        expr_object = world.symbolDict[expr_symbol]
        # Shape==(0,0) represents a scalar symbol
        self.assertTrue(expr_object.shape == (0,0))

        vect = Matrix([1,I, 0])
        expr_symbol2 = world.addExpression('testExp2', 'psi', m**2*vect)
        expr_object = world.symbolDict[expr_symbol2]
        self.assertTrue(expr_object.shape == (3,1))

        vect = Matrix([[0,I],[m*4,5]])
        expr_symbol3 = world.addExpression('testExp3', 'phi', m**2*vect)
        expr_object = world.symbolDict[expr_symbol3]
        self.assertTrue(expr_object.shape == (2,2))

        self.assertRaises(ValueError, world.addExpression, 'testExp',
                                                           'gamma_', 0*815)
        self.assertRaises(ValueError, world.addExpression, 'testExp_',
                                                           'gamma', 0*815)

        # check whether all the expected names are keys in the dict
        boolList=[name in list(world.expressionDict.keys()) \
                              for name in ['testExp', 'testExp2', 'testExp3']]
        self.assertTrue(all(boolList))

        allExpressions = list(world.expressionDict.values())
        userExpressions = [ex for ex in allExpressions if VarKind.USER_EXP in ex.category]
        self.assertTrue(len(userExpressions) == 3)

    def test_addLoad(self):

        # dummy
        world.addFrame('CS_Load1')
        world.addFrame('CS_Load2', [1,0,0])

        force1 = world.addExpression('weight', 'Fg',
                                        m*world.gravity_const)

        world.addLoad.PtPForce(world.CS_Load1,world.CS_Load2,
                               force1, name='weight_force')

        Ez=Matrix([0,0,1])
        forceVect = world.addExpression('F_bucket', 'Fbucket',
                                        Ez*150)

        world.addLoad.CmpForce(world.CS_Load1, world.CS_Load2,
                               forceVect, name='bucket_force')

        torqueVect = world.addExpression('T_bucket', 'Tbucket',
                                        Ez*150)

        world.addLoad.CmpTorque(world.CS_Load1, world.CS_Load2,
                                torqueVect, name='bucket_torque')

        # invalid shape of Expr.
        self.assertRaises(AssertionError, world.addLoad.CmpForce, 
                                      world.CS_Load1, world.CS_Load2,
                                      force1, 'foo_name')
        self.assertRaises(AssertionError, world.addLoad.CmpTorque,
                                      world.CS_Load1, world.CS_Load2,
                                      force1, 'foo_name_')

        # existing Name
        self.assertRaises(ValueError, world.addLoad.CmpForce, 
                          world.CS_Load1, world.CS_Load2,
                           forceVect, 'bucket_force')

        boolList=[name in list(world.loadDict.keys()) for name in\
                                  ['weight_force', 'bucket_force', 'bucket_torque']]

        self.assertTrue(all(boolList))
        self.assertTrue(len(list(world.loadDict.keys())) == 3)

    def test_addLoad2(self):
        # use Body as CS
        ball1 = world.addBody('Ball1',
                             m, zeros((3,)), diag([0,0,I]))
        ball2 = world.addBody('Ball2',
                             m, zeros((3,)), diag([0,0,I]))
        ball3 = world.addBody('Ball3',
                             m, zeros((3,)), diag([0,0,I]))


        force1 = world.addExpression('weight', 'Fg', -m*world.gravity_const)
        world.addLoad.PtPForce(world, ball1, force1, name='gravity_force')

        Ez=Matrix([0,0,1])
        forceVect = world.addExpression('F_kick', 'Fk', Ez*150)
        world.addLoad.CmpForce(world, ball2, forceVect, name='kick_force')


        torqueVect = world.addExpression('T_kick', 'Tk', Ez*-15)
        world.addLoad.CmpTorque(world, ball3, torqueVect, name='kick_torque')


    def test_addJointLoad(self):
        box = world.addBody('box', mass = 5)
        j = world.addJoint('TransBody', world, box, 'Rx')
        Fm = world.addParam('MotorForce', 'Fm', 10)
        world.addJointLoad('jointLoad1', j, Fm)

        # this joint already has a load
        self.assertRaises(AssertionError, world.addJointLoad,\
                                              'jointLoad2', j, Fm)

        box2 = world.addBody('box2', mass = 5)
        j2 = world.addJoint('TransBody2', world, box2, 'Rx')


        # invalid arg: loadsymbol
        self.assertRaises(AssertionError, world.addJointLoad,\
                                              'jointLoad2', j2, Fm*3)



    def test_addInput(self):

        world.addInput("testInput", "u1")

        self.assertRaises(ValueError, world.addInput, "testInput", "u1_")
        self.assertRaises(ValueError, world.addInput, "testInput_", "u1")

        inputSymb = world.addInput("testInput2", "u2", (3,2))
        self.assertTrue(inputSymb.name == "u2")
        exprObject = world.symbolDict[inputSymb]
        self.assertTrue(exprObject.shape == (3,2))

        #self.assertTrue(len(world.inputSymbolList) == 2)

    def test_addSensor(self):

        world.addFrame('CS_A')
        body1 = world.addBody('cube', 10)
        body1.addFrame('CS_A')

        dist = world.addSensor.Position(body1.CS_A, world.CS_A,
                                        'dist', name='body1_dist')
        pos = world.addSensor.Position(body1.CS_A, world.CS_A, 
                                       'pos', name='body1_pos')
        vel = world.addSensor.Velocity(body1.CS_A, world.CS_A, 
                                       'vel', name='body1_vel')
        acc = world.addSensor.Acceleration(body1.CS_A, world.CS_A,
                                           'acc', name='body1_acc')

        ori = world.addSensor.Orientation(body1.CS_A, world.CS_A, 
                                          'ori', name='body1_ori')
        angvel = world.addSensor.AngularVelocity(body1.CS_A, world.CS_A,
                                                 'angvel', name='body1_angvel')
        angacc = world.addSensor.AngularAcceleration(body1.CS_A, world.CS_A, 
                                                     'angacc', name='body1_angacc')

        # existing name
        self.assertRaises(ValueError, world.addSensor.Position,
                              body1.CS_A, world.CS_A, 'dist_', name='body1_dist')

        # existing symbol
        self.assertRaises(ValueError, world.addSensor.Position, 
                              body1.CS_A, world.CS_A, 'dist', name='body1_dist_')

        self.assertTrue( pos.shape == (3,1) )
        self.assertTrue( vel.shape == (3,1) )
        self.assertTrue( acc.shape == (3,1) )

        self.assertTrue( ori.shape == (3,3) )
        self.assertTrue( angvel.shape == (3,1) )
        self.assertTrue( angacc.shape == (3,1) )

        # now try to use some sensor_values in Expressions
        F1 = world.addExpression('Fspring', 'F1', pos[2]*15)
        F2 = world.addExpression('Fdamper', 'F2', vel[2]*10)


    def test_addSensor2(self):
        # use Body as CS

        world.addFrame('CS_A')
        body1 = world.addBody('cube', 10)
        body1.addFrame('CS_A')

        dist = world.addSensor.Position(body1, world, 
                                        'dist', name='body1_dist')

        pos = world.addSensor.Position(body1, world, 
                                       'pos', name='body1_pos')
        vel = world.addSensor.Velocity(body1, world, 
                                       'vel', name='body1_vel')
        acc = world.addSensor.Acceleration(body1, world, 
                                           'acc', name='body1_acc')

        ori = world.addSensor.Orientation(body1, world, 
                                          'ori', name='body1_ori')
        angvel = world.addSensor.AngularVelocity(body1, world, 
                                                 'angvel', name='body1_angvel')
        angacc = world.addSensor.AngularAcceleration(body1, world, 
                                                     'angacc', name='body1_angacc')


        # control some samples


        ori = world.sensorDict['body1_ori']
        self.assertTrue(ori.parent == body1._CS_0)
        self.assertTrue(ori.child == world._CS_0)

        acc = world.sensorDict['body1_acc']
        self.assertTrue(acc.parent == body1._CS_0)
        self.assertTrue(acc.child == world._CS_0)

        pos = world.sensorDict['body1_pos']
        self.assertTrue(pos.parent == body1._CS_0)
        self.assertTrue(pos.child == world._CS_0)
        
    def test_addJointSensor(self):

        box1 = world.addBody('box1', 10)
        box2 = world.addBody('box2', 10)
        box3 = world.addBody('box3', 10)

        j1 = world.addJoint('Joint1', world, box1, 'Tx')
        j2 = world.addJoint('Joint2', world, box2, 'Ry')
        jList3 = world.addJoint('Joint3', world, box3, ['Tx','Ty','Tz'])

        s1 = world.addSensor.Joint(j1, 'x_j1')
        s2 = world.addSensor.Joint(j2, 'phi_j2')

        s3x = world.addSensor.Joint(jList3[0], 'x_j3')
        s3y = world.addSensor.Joint(jList3[1], 'y_j3')
        s3z = world.addSensor.Joint(jList3[2], 'z_j3')

        self.assertTrue(len(list(world.sensorDict.keys())) == 5)

        self.assertTrue(s1.shape == (2, 1))
        self.assertTrue(s2.shape == (2, 1))
        self.assertTrue(s3x.shape == (2, 1))
        self.assertTrue(s3y.shape == (2, 1))
        self.assertTrue(s3z.shape == (2, 1))

    def test_addJointSensor2(self):
        box1 = world.addBody('box1', 10)
        box2 = world.addBody('box2', 10)
        box3 = world.addBody('box3', 10)

        j1 = world.addJoint('Joint1', world, box1, 'Tx')
        j2 = world.addJoint('Joint2', world, box2, 'Ry')

        s1 = world.addSensor.Joint(j1, 'x_j1', name='jSens1')


        # additional Sensor to the same joint (j1):
        self.assertRaises(AssertionError, world.addSensor.Joint,
                                              j1, 'x_j1_2')

        # already used name
        self.assertRaises(ValueError, world.addSensor.Joint,
                                              j2, 'phi_j2', 'jSens1')


        # already used symbol
        self.assertRaises(AssertionError, world.addSensor.Joint,
                                              j2, 'x_j1')



    def test_addConstraint(self):


        world.addFrame('CS_A')
        fixBody = world.addBody('fixBody', m, zeros((3,)), diag([0,0,I]))
        fixBody.addFrame('CS_A')

        world.addConstraint('Fixation', world.CS_A, fixBody.CS_A, [1,1,1], [1,1,1])

    def test_addConstraint2(self):

        # use Body as CS

        fixBody = world.addBody('fixBody', m, zeros((3,)), diag([0,0,I]))
        world.addConstraint('Fixation', world, fixBody, [1,1,1], [1,1,1])

        constr = world.constraintDict['Fixation']
        self.assertTrue(constr.parent == world._CS_0)
        self.assertTrue(constr.child == fixBody._CS_0)


    def test_addVisualisation1(self):

        body_cube = world.addBody('cub', m, zeros((3,)), diag([0,0,I]))
        body_cylinder = world.addBody('cyl', m, zeros((3,)), diag([0,0,I]))
        body_cylinder2 = world.addBody('cyl2', m, zeros((3,)), diag([0,0,I]))
        body_sphere = world.addBody('sph', m, zeros((3,)), diag([0,0,I]))
        body_sphere2 = world.addBody('sph2', m, zeros((3,)), diag([0,0,I]))
        body_complex = world.addBody('cpl', m, zeros((3,)), diag([0,0,I]))

        body_cube.addFrame('CS_show', [1.3, 0, 0])


        world.addVisualisation.Box(body_cube, 2, 2, 2)

        # use default res
        world.addVisualisation.Cylinder(body_cylinder, 0.5, 2)
        # res = 40
        world.addVisualisation.Cylinder(body_cylinder2, 0.5, 2, 40)

        # use default res
        world.addVisualisation.Sphere(body_sphere, .3)
        # res = 80
        world.addVisualisation.Sphere(body_sphere2, .3, 80)

        world.addVisualisation.File(body_complex, 'themodel.stl')

        world.addVisualisation.Frame(body_cube.CS_show, 1.5)

        # use more than one GraphRep Object for the same CS:
        world.addVisualisation.Sphere(body_cube.CS_show, 0.5)
        world.addVisualisation.Box(body_cube.CS_show, 0.5, 1, 2)



        graphRepList = list(world.graphRepDict.values())
        self.assertTrue(len(graphRepList) == 9)

        for elt in graphRepList:
            self.assertTrue(isinstance(elt, gr.AbstractGraphRep))

    def test_addVisualisation2(self):

        # different bodys to prevent name clashes of the graphic sensors
        cube = world.addBody('cub', m, zeros((3,)), diag([0,0,I]))
        cube2 = world.addBody('cub2', m, zeros((3,)), diag([0,0,I]))
        cube3 = world.addBody('cub3', m, zeros((3,)), diag([0,0,I]))
        cube4 = world.addBody('cub4', m, zeros((3,)), diag([0,0,I]))

        # wrong number of args
        #self.assertRaises(AssertionError, world.addVisualisation.Box, cube2, 0.3)
        #self.assertRaises(AssertionError, world.addVisualisation.Box, cube3, .3, .3)
        self.assertRaises(AssertionError, world.addVisualisation.Box, cube4, .3, .3, .3, .3)


        #TODO: complete that for other GraphReps

    def test_addVisualisation3(self):
        # use parameter-objects ( = Symbols) as size and shape parameeters

        p1 = world.addParam('length', 'l1', 1.4)
        p2 = world.addParam('factor', 'f', 2.0)
        exp1 = world.addExpression('product', 'e', p1*p2)


        body_cube = world.addBody('cub', m, zeros((3,)), diag([0,0,I]))
        body_cylinder = world.addBody('cyl', m, zeros((3,)), diag([0,0,I]))
        body_cylinder2 = world.addBody('cyl2', m, zeros((3,)), diag([0,0,I]))
        body_sphere = world.addBody('sph', m, zeros((3,)), diag([0,0,I]))
        body_sphere2 = world.addBody('sph2', m, zeros((3,)), diag([0,0,I]))
        body_complex = world.addBody('cpl', m, zeros((3,)), diag([0,0,I]))

        body_cube.addFrame('CS_show', [1.3, 0, 0])

        world.addVisualisation.Box(body_cube, p1, p1, exp1)

        # use default res
        world.addVisualisation.Cylinder(body_cylinder, p1, p1)
        # res = p1
        world.addVisualisation.Cylinder(body_cylinder2, p1, p1, p1)

        # use default res
        world.addVisualisation.Sphere(body_sphere, p1)
        # res = exp1
        world.addVisualisation.Sphere(body_sphere2, p1, exp1)

        #ST()
        world.addVisualisation.File(body_complex, 'themodel.stl', p1)

        world.addVisualisation.Frame(body_cube.CS_show, exp1)

    def test__isscalar(self):
        p1 = world.addParam('length', 'l1', 1.4)
        p2 = world.addParam('factor', 'f', 2.0)
        exp1 = world.addExpression('product', 'e', p1*p2)

        zeromatrix=world.addParam('test', 'test_matrix', zeros((3,3)))

        self.assertTrue(world._isscalar(1))
        self.assertTrue(world._isscalar(1.5))

        self.assertTrue(world._isscalar(p1))
        self.assertTrue(world._isscalar(exp1))

        self.assertFalse(world._isscalar(zeromatrix))


        #unknown Symbols
        gamma=Symbol('gamma')
        self.assertRaises(ValueError, world._isscalar, gamma)

        # wrong types
        self.assertRaises(TypeError, world._isscalar, [1, 2])
        self.assertRaises(TypeError, world._isscalar, "1")



    def test_complex1(self):

        u1 = world.addInput("testInput", "u1")
        u2 = world.addInput("testInput2", "u2")

        # use input in Expression
        f1 = world.addExpression('formula1', 'f1', u1*m)

        world.addFrame('CS1')
        world.addFrame('CS2', [1,0,0])

        world.addLoad.PtPForce(world.CS1, world.CS2, f1)

        # use  input directly
        world.addLoad.PtPForce(world.CS1, world.CS2, u2)


    def test_checkName(self):

        # validNames:

        world._checkName('validName')
        world._checkName('valid[Name]')
        world._checkName('valid.Name')

        # invalid Names
        self.assertRaises(ValueError, world._checkName,
                          'invalid Name')
        self.assertRaises(ValueError, world._checkName,
                          'invalid+Name')
        self.assertRaises(ValueError, world._checkName,
                          'invalid:Name')
        self.assertRaises(ValueError, world._checkName,
                          'invalid(Name)')
        self.assertRaises(ValueError, world._checkName,
                          'invalid{Name}')


if __name__ == '__main__':
    import sys

    debug=False
    if len(sys.argv)>1:
        if sys.argv[1] == '-d':
            sys.argv.pop(1)
            debug = True
    if '-f' in sys.argv:
        FASTRUN = True
        print("omit slow tests")
        sys.argv.remove('-f') # dont confuse unittest

    if debug is True:
        #allow (commandline) debugging
        # does not work right now
        unittest.main()

    else:
        unittest.main()

