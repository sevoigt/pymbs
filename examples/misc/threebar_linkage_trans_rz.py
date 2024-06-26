# -*- coding: utf-8 -*-
"""
Model of a three-bar linkage with a prismatic joint, like a
cylinder erecting a boom

The rotational joint acts around the z-axis, thus the mechanism is in
the local x-y plane.
"""

from pymbs.input import MbsSystem, diag, rot_mat, pi


world = MbsSystem([0, -1, 0], name='Bar1')

# Parameter
l1 = world.addParam('l1', 0.13, name='Length 1')

m2 = world.addParam('m2', 0.6, name='Mass 2')
l2 = world.addParam('l2', 0.174, name='Length 2')
I2 = world.addParam('I2', m2 / 12 * l2**2, name='Inertia 2')

m3a = world.addParam('m3a', 0.25, name='Mass 3a')
l3a = world.addParam('l3a', 0.1, name='Length 3a')
I3a = world.addParam('I3a', m3a / 12 * l3a**2, name='Inertia 3a')

m3b = world.addParam('m3b', 0.25, name='Mass 3b')
l3b = world.addParam('l3b', 0.1, name='Length 3b')
I3b = world.addParam('I3b', m3b / 12 * l3b**2, name='Inertia 3b')

# Create bodies with coordinate systems
# Bar1 = inertial system
bar1_A = world.addFrame(name='CS_A')
bar1_C = world.addFrame(name='CS_C', p=[l1, 0, 0])

bar2 = world.addBody(name='Bar2', mass=m2, cg=[l2 / 2, 0, 0], inertia=diag([0, I2, I2]))
bar2.addFrame(name='CS_A')
bar2.addFrame(name='CS_B', p=[l2, 0, 0])
bar2.addFrame(name='CS_Graphic', p=[l2 / 2, 0, 0])
bar2.addFrame(name='CS_B_Graphic', p=[l2, 0, 0], R=rot_mat(pi / 2, 'x'))

bar3a = world.addBody(name='Bar3a', mass=m3a, cg=[l3a / 2, 0, 0], inertia=diag([0, I3a, I3a]))
bar3a.addFrame(name='CS_B')
bar3a.addFrame(name='CS_Graphic', p=[-l3a / 2, 0, 0])

bar3b = world.addBody(name='Bar3b', mass=m3b, cg=[l3b / 2, 0, 0], inertia=diag([0, I3b, I3b]))
bar3b.addFrame(name='CS_C')
bar3b.addFrame(name='CS_Graphic', p=[l3b / 2, 0, 0])

# Insert Joints
jA = world.addJoint(world.CS_A, bar2.CS_A, 'Rz', 0.01, name='jA')
jC = world.addJoint(world.CS_C, bar3b.CS_C, 'Rz', name='jC')
jCB = world.addJoint(bar3b.CS_C, bar3a.CS_B, 'Tx', name='jCB')

# Constraint
dae = False
if dae:
    world.addConstraint(bar2.CS_B, bar3a.CS_B, [1, 1, 0], [0, 0, 0], name='Constr_FB')
else:
    world.addLoop.ThreeBarTrans(bar2.CS_B, bar3a.CS_B, name='ThreeBarLinkageTrans')


# Sensors
world.addSensor.Position('r1_A', world.CS_A, world.CS_A, world.CS_A, name='r1_A')
world.addSensor.Position('r1_C', world.CS_A, world.CS_C, world.CS_A, name='r1_C')

world.addSensor.Position('r2_A', world.CS_A, bar2.CS_A, world.CS_A, name='r2_A')
world.addSensor.Position('r2_B', world.CS_A, bar2.CS_B, world.CS_A, name='r2_B')

world.addSensor.Position('r3a_B', world.CS_A, bar3a.CS_B, world.CS_A, name='r3a_B')
world.addSensor.Position('r3b_C', world.CS_A, bar3b.CS_C, world.CS_A, name='r3b_C')

print("System has been assembled")


# Visualisation
world.addVisualisation.Box(bar2.CS_Graphic, 0.174, 0.005, 0.005, name='bar2')
world.addVisualisation.Box(bar3a.CS_Graphic, 0.1, 0.005, 0.005, name='bar3a')
world.addVisualisation.Box(bar3b.CS_Graphic, 0.1, 0.01, 0.01, name='bar3b')
world.addVisualisation.Cylinder(bar2.CS_B_Graphic, 0.005, 0.005, name='bar2_cyl')

print("System has been assembled")

world.genEquations.Explicit()
world.show('threebar_linkage_trans')
