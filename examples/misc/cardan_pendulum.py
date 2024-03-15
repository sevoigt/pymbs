"""
Cardan Pendulum
"""

from pymbs.input import *

world=MbsSystem([0,0,-1])

m=world.addParam('m', 1, name='mass')
l=world.addParam('l', 1, name='lengthOfRod')
Jx=world.addParam('Jx', 1, name='Jx')
Jy=world.addParam('Jy', 1, name='Jy')

world.addFrame('CS_0')

pend=world.addBody(m, zeros((3,)), diag([Jx,Jy,0]), name='Pendulum')
pend.addFrame('CS_joint', [0,0,l])

world.addJoint(world.CS_0, pend.CS_joint, ['Rx', 'Ry'], name='RevPendulum')

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Matlab('CardanPendulum_rec', '.\output')

# world.genEquations.Explicit()
# world.genCode.Matlab('CardanPendulum_exp', '.\output', debugMode=True)
