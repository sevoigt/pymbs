"""
Model of a crane crab with a load, i.e. a sliding pendulum
"""

from pymbs.input import MbsSystem, rot_mat, diag, pi

# set up inertial frame
world = MbsSystem([0, 0, -1])

# add parameters
m1 = world.addParam('m1', 1.0)
m2 = world.addParam('m2', 1.0)
l2 = world.addParam('l2', 1.0)
I2 = world.addParam('I2', (m2*l2**2)/12)

# add bodies
trolley = world.addBody(mass=m1, name='Trolley')
pend = world.addBody(mass=m2, inertia=diag([0, I2, 0]), name='pendulum')

# add frames
pend.addFrame(name='joint' , p=[0, 0, l2])
pend.addFrame(name='middle', p=[0, 0, l2/2], R=rot_mat(pi/2,'x'))

# add joints
world.addJoint(world, trolley, dofList='Tx', startVals=0.1)
world.addJoint(trolley, pend.joint, dofList='Ry', startVals=-1)

# add visualisation
world.addVisualisation.Box(trolley, 1, 0.5, 0.1)
world.addVisualisation.Cylinder(pend.middle, 0.01, 1)
world.addVisualisation.Sphere(pend, 0.1)

# generate equations
world.genEquations.Recursive()
world.show('crane_crab')
