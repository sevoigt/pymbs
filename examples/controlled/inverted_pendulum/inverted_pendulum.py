# -*- coding: utf-8 -*-
'''
Controlled pendulum

Single pendulum with controlling force acting on the joint. The control force
is defined in `controller.py` and tries to keep the pendulum upright.
If you set the initial angle to 2.4 or more, the controller should reach
equilibrium.
'''
# Warning: The source code of the examples is quoted in the documentation. If
# you change this file, you'll have to change the corresponding file in the
# documentation (see doc/examples).
from PyMbs.Input import MbsSystem
from controller import controlForce

world = MbsSystem([0, 0, -1])

M1 = world.addParam('m1', 0.8)
M2 = world.addParam('m2', 0.3)

Trolley = world.addBody(M1)
Load = world.addBody(M2, cg=[0, 0, -0.5])

world.addJoint(world, Trolley, 'Tx')
world.addJoint(Trolley, Load, 'Ry')

F = world.addController('F', controlForce, shape=(3, ))
world.addLoad.CmpForce(F, Trolley, world, name='DrivingForce')

# Visualisierung
world.addVisualisation.Box(world, length=3, width=0.05, height=0.05)
world.addVisualisation.Box(Trolley, length=0.1, width=0.1, height=0.1)
world.addVisualisation.File(Load, 'Last_01.stl')

# Bewegungsgleichungen berechnen und Modell darstellen
world.genEquations.Recursive()
world.show('InvertedPendulum')
