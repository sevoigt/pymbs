"""
Model description for a simple pendulum adapted from Matlab-example
"""

from pymbs.input import MbsSystem, diag, eye, zeros

world = MbsSystem([0, -1, 0])

m1 = world.addParam('m1', 1, name='mass1')
l1 = world.addParam('l1', 1, name='lengthOfRod1')
I1 = world.addParam('I1', m1*l1**2/12, name='inertia1')
m2 = world.addParam('m2', 1, name='mass2')
l2 = world.addParam('l2', 1, name='lengthOfRod2')
I2 = world.addParam('I2', m2*l2**2/12, name='inertia2')

world.addFrame('CS_p1', zeros((3,)), eye(3))

pend1 = world.addBody(m1, [0, 0, 0], diag([0, 0, I1]), name='Pendulum1')
pend1.addFrame('CS_joint', [0, l1, 0], eye(3))
pend1.addFrame('CS_tip', [0, 0, 0], eye(3))

pend2 = world.addBody(m2, [0, 0, 0], diag([0, 0, I2]), name='Pendulum2')
pend2.addFrame('CS_joint', [0, l2, 0], eye(3))
pend2.addFrame('CS_tip', [0, 0, 0], eye(3))

j1 = world.addJoint(world.CS_p1, pend1.CS_joint, 'Rz', name='RevPendulum1')
j2 = world.addJoint(world.CS_p1, pend2.CS_joint, 'Rz', name='RevPendulum2')

world.addLoop.Transmission(j1, j2, 1, 'Fix')

world.genEquations.Explicit()
world.genCode.Matlab('coupled_pendulum', './output')
