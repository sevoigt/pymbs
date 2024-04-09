"""
Model of a crane crab with equation system in DAE form
"""

# import PyMbs
from pymbs.input import MbsSystem, diag, rot_mat

# set up inertial frame
world=MbsSystem([0,0,-1])

# add inputs and parameters
F=world.addInput('DrivingForce', 'F', limits=[-10, 10])
m1=world.addParam('mass 1', 'm1', 1.0)
I1=world.addParam('inertia 1', 'I1', 1.0)
m2=world.addParam('mass 2', 'm2', 1.0)
l2=world.addParam('lengthOfRod 2', 'l2', 1.0)
I2=world.addParam('inertia 2', 'I2', (m2*l2**2)/12)

# add bodies
crab=world.addBody('Crab', mass=m1, inertia=diag([0,I1,0]))
pend=world.addBody('Pendulum', mass=m2, inertia=diag([0,I2,0]))
pend.addFrame('joint' , [0, 0, l2])
pend.addFrame('middle', [0, 0, l2/2], rot_mat(pi/2,'x'))

# add joints
world.addJoint('TransCrab', world, crab, ['Tx', 'Tz', 'Ry'], [0,0,0])
world.addJoint('RotPendulum', world, pend, ['Tx', 'Tz', 'Ry'], [1,0,-float(pi)/2])

# introduce constraints
world.addConstraint('Constr_Crab', world, crab, transLock=[0,0,1], rotLock=[0,1,0])
world.addConstraint('Constr_Pend', crab, pend.joint, transLock=[1,0,1])

# add load element and sensor
world.addLoad.CmpForce(crab, world, [F,0,0])
world.addSensor.Distance(crab, world, 'd')

# add visualisation
world.addVisualisation.Box(crab, 1, 0.5, 0.1)
world.addVisualisation.Cylinder(pend.middle, 0.01, 1)
world.addVisualisation.Sphere(pend, 0.1)

# generate equations
world.genEquations(explicit=True, kinematicsOnly=False, diff=[0,1,2])

# generate simulation code
world.genCode('mo', 'CraneCrab_DAE', './output')
world.genCode('m', 'CraneCrab_DAE', './output')
#world.genCode('py', 'CraneCrab')

# show system
#world.genMatlabAnimation('CraneCrab')
world.show('CraneCrab_DAE')