"""
Simple model of a rotating arm
"""

from pymbs.input import MbsSystem, diag

world = MbsSystem([0, -1, 0])

b = world.addParam('b', 1, name='Length_b')
B = world.addFrame('B', [0, -b, 0])

l = world.addParam('l', 1, name='Length_l')
J = world.addParam('J', 1, name='Inertia')
m = world.addParam('m', 1, name='Mass')
arm = world.addBody(m, [l, 0, 0], diag([0, 0, J]), name='Arm')

a = world.addParam('a', 1, name='Length_a')
A = arm.addFrame('A', p=[a, 0, 0])

world.addJoint(world, arm, 'Rz', name='RotArm')

world.addVisualisation.Box(arm, 10, 1, 1)

world.genEquations.Recursive()
world.genCode.Modelica('Arm', './output')

world.show('Arm')
