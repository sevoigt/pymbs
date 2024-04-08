"""
Cardan Pendulum
"""

from pymbs.input import MbsSystem, diag

world=MbsSystem([0,0,-1])

m=world.addParam('m', 1, name='mass')
l=world.addParam('l', 1, name='lengthOfRod')
Jx=world.addParam('Jx', 1, name='Jx')
Jy=world.addParam('Jy', 1, name='Jy')

world.addFrame('CS_0')

pend=world.addBody(m, [0,0,l/2], diag([Jx,Jy,0]), name='Pendulum')
pend.addFrame('CS_vis', [0,0,l/2])

world.addJoint(world.CS_0, pend, ['Rx', 'Ry'], name='RevPendulum')
world.addVisualisation.Box(pend.CS_vis, 0.05, 0.05, 1)

world.genEquations.Recursive()
world.genCode.Matlab('cardan_pendulum_rec', './output')

# world.genEquations.Explicit()
# world.genCode.Matlab('cardan_pendulum_exp', '.\output', debugMode=True)

world.show('cardan_pendulum')
