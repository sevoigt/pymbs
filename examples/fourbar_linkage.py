"""
Model for a closed loop four-bar linkage.
"""

# Warning: The source code of the examples is quoted in the documentation. If
# you change this file, you'll have to change the corresponding file in the
# documentation (see doc/examples).

from pymbs.input import MbsSystem, diag, pi

world = MbsSystem([0, 0, -1])

# Parameter
l1 = world.addParam('l1', 1)

m2 = world.addParam('m2', 1)
l2 = world.addParam('l2', 4)
I2 = world.addParam('I2', m2 / 12 * l2**2)

m3 = world.addParam('m3', 1)
l3 = world.addParam('l3', 4)
I3 = world.addParam('I3', m3 / 12 * l3**2)

m4 = world.addParam('m4', 1)
l4 = world.addParam('l4', 5)
I4 = world.addParam('I4', m4 / 12 * l4**2)

# Create bodies with coordinate systems
world.addFrame(name='CS_A')
world.addFrame(name='CS_D', p=[l1, 0, 0])

bar2 = world.addBody(name='Bar2', mass=m2, cg=[l2 / 2, 0, 0],
                     inertia=diag([0, I2, I2]))
bar2.addFrame(name='CS_A')
bar2.addFrame(name='CS_B', p=[l2, 0, 0])

bar3 = world.addBody(name='Bar3', mass=m3, cg=[l3 / 2, 0, 0],
                     inertia=diag([0, I3, I3]))
bar3.addFrame(name='CS_B')
bar3.addFrame(name='CS_C', p=[l3, 0, 0])

bar4 = world.addBody(name='Bar4', mass=m4, cg=[l4 / 2, 0, 0],
                     inertia=diag([0, I4, I4]))
bar4.addFrame(name='CS_D')
bar4.addFrame(name='CS_C', p=[l4, 0, 0])

# Insert joints
jA = world.addJoint(world.CS_A, bar2.CS_A, 'Ry', pi / 2)
jB = world.addJoint(bar2.CS_B, bar3.CS_B, 'Ry')
jD = world.addJoint(world.CS_D, bar4.CS_D, 'Ry')

world.addLoop.FourBar(bar3.CS_C, bar4.CS_C, posture=1)

world.addVisualisation.Line(bar2, 4)
world.addVisualisation.Line(bar3, 4)
world.addVisualisation.Line(bar4, 5)

world.genEquations.Recursive()
world.show('fourbar_linkage')
