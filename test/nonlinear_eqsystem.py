'''
Test case for solving a nonlinear system of equations within pymbs-generated
code.
'''

from pymbs.input import *


from pymbs.common.functions import newton, sym
from pymbs.symbolics import Expression, VarKind

from pymbs.output.PythonWriter import PythonWriter


# dummy mbs system to get some equations
world = MbsSystem()
b1 = world.addBody('body', mass=1.)
world.addJoint('j1', world, b1, ['Tx'])

# some expressions
a = sym('a')
b = sym('b')
r = sym('r')

ex1 = Expression('a', a, 1, VarKind.DER_STATE)
ex2 = Expression('b', b, 2, VarKind.DER_STATE)

n = newton((3*a**2-2*b, a-1.5*b**1.5+1), (a,b), (0,0))

ex3 = Expression('r', r, newton((3*a**2-2*b, a-1.5*b**1.5+1), (a,b), (0,0)), VarKind.DER_STATE)

# gen code and add expressions
graph = world.genEquations()
graph.addExpression(ex1)
graph.addExpression(ex2)
graph.addExpression(ex3)

# graph in pythonwriter stecken und code erzeugen
pw = PythonWriter(graph, 'test')
pw.writeStateDerivative()
