"""
Model of an overhead crane with a trolley and load pendulum.
"""

from pymbs.input import *
from controller import trolley_force, rope_force

ropelength = 1
world=MbsSystem([0,0,-1])

# Parameters
trolley_mass = world.addParam('trolley_mass', 1.0)
rope_mass = world.addParam('rope_mass', 0.1)
load_mass = world.addParam('load_mass', 1.0)

# Moving bodies and frames
trolley = world.addBody(trolley_mass)
trolley.addFrame('hook', [0, 0, -0.2])

rope_one = world.addBody(rope_mass)
rope_one.addFrame('joint', [0, 0, 0.5*ropelength])
rope_one.addFrame('vis', R = rotMat(pi/2, 'x'))

rope_two = world.addBody(rope_mass)
rope_two.addFrame('joint', [0, 0, 0.5*ropelength])
rope_two.addFrame('vis', R = rotMat(pi/2, 'x'))
rope_two.addFrame('load', [0, 0, -0.5*ropelength])

load = world.addBody(load_mass)

# Visualisation
world.addVisualisation.Box(trolley, length=0.5, width=0.3, height=0.3)
world.addVisualisation.Box(trolley.hook, length=0.2, width=0.2, height=0.4)
world.addVisualisation.Cylinder(rope_one.vis, radius=0.01, height=ropelength)
world.addVisualisation.Cylinder(rope_two.vis, radius=0.01, height=ropelength)
world.addVisualisation.Box(load, length=0.3, width=0.3, height=0.3)

# Joints
world.addJoint(world, trolley, 'Tx', startVals = 1)
hook = world.addJoint(trolley.hook, rope_one.joint, 'Ry')
world.addJoint(rope_one, rope_two.joint, 'Tz')
world.addJoint(rope_two.load, load)

# Controller
F_trolley = world.addController('F_trolley', trolley_force, shape=(3, ))
world.addLoad.CmpForce(F_trolley, trolley, world)
F_rope = world.addController('F_rope', rope_force, shape=(3, ))
world.addLoad.CmpForce(F_rope, rope_two, rope_one)

# Sensors
world.addSensor.Distance('l', trolley, load) # ropelength
world.addSensor.Distance('x', world, trolley) # trolley position
world.addSensor.Joint('phi', hook) # rope angle

# Ground
world.addFrame('beam')
world.addVisualisation.Box(world.beam, length=7, width =0.1, height=0.1)
world.addFrame('ground', p = [0, 0, -2.5])
world.addVisualisation.Box(world.ground, length=7, width=5, height=0.1)
world.addFrame('obstacle', p = [-0.5, 0, -2.25])
world.addVisualisation.Box(world.obstacle, length=1, width=2, height=0.5)
world.addFrame('support_one', p = [3.45, 0, -1.25])
world.addVisualisation.Box(world.support_one, length=0.1, width=0.1, height=2.5)
world.addFrame('support_two', p = [-3.45, 0, -1.25])
world.addVisualisation.Box(world.support_two, length=0.1, width=0.1, height=2.5)

# Generate equations of motion and show model
world.genEquations.Recursive()
world.show('crane')
