"""
Another version of the crane crab example
"""

# import PyMbs
from pymbs.input import *

# set up inertial frame
world=MbsSystem([0,0,-1])

# add inputs and parameters
F=world.addInput('DrivingForce')
m1=world.addParam('m1', 1, name='mass 1')
m2=world.addParam('m2', 1, name='mass 2')
l2=world.addParam('l2', 1, name='lengthOfRod 2')
I2=world.addParam('I2', m2*l2**2/12, name='inertia 2')

# add bodies
crab=world.addBody(mass=m1, name='Crab')
pend=world.addBody(mass=m2, inertia=diag([0,I2,0]), name='Pendulum')
pend.addFrame('joint' , [0, 0, l2])
pend.addFrame('middle', [0, 0, l2/2], rot_mat(pi/2,'x'))

# add joints
world.addJoint(world, crab, 'Tx', 1, name='TransCrab')
world.addJoint(crab, pend.joint, 'Ry', -1, name='RotPendulum')

# add load element and sensor
world.addLoad.PtPForce(F, crab, world)
world.addSensor.Distance('d', crab, world)

# add visualisation
world.addVisualisation.Box(crab, 1, 0.5, 0.1)
world.addVisualisation.Cylinder(pend.middle, 0.01, 1)
world.addVisualisation.Sphere(pend, 0.1)


# generate simulation code
world.genEquations.Explicit()

world.genCode.Modelica('CraneCrab', './output')
world.genCode.Matlab('CraneCrab', './output')
world.genCode.Python('CraneCrab', './output')
world.genCode.C('CraneCrabRec', './output')

# show system
world.genMatlabAnimation('CraneCrab')
world.show('CraneCrab')