# -*- coding: utf-8 -*-
'''
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert,
                     Jens Frenkel, Sebastian Voigt
'''

'''
Created on 05.07.2009

@author: Christian Schubert

'''

USE_NO_INPUTS = False

# import PyMbs
from PyMbs.Input import *

# set up inertial frame
world=MbsSystem([0,0,-1])

# add inputs and parameters
if (not USE_NO_INPUTS):
    F=world.addInput('F', limits=[-10, 10], name='DrivingForce')
m1=world.addParam('m1', 1.0)
m2=world.addParam('m2', 1.0)
l2=world.addParam('l2', 1.0)
I2=world.addParam('I2', (m2*l2**2)/12)

# add bodies
crab=world.addBody(mass=m1, name='Crab')
pend=world.addBody(mass=m2, inertia=diag([0,I2,0]), name='Pendulum')
pend.addFrame(name='joint' , p=[0, 0, l2])
pend.addFrame(name='middle', p=[0, 0, l2/2], R=rotMat(pi/2,'x'))

# add joints
jT = world.addJoint(world, crab, 'Tx', 1, name='TransCrab')
jR = world.addJoint(crab, pend.joint, 'Ry', -1, name='RotPendulum')

# add load element and sensor
if (not USE_NO_INPUTS):
    world.addLoad.CmpForce([F,0,0], crab, world)
    world.addSensor.Distance('d', crab, world)
world.addSensor.ConstraintForce('F_RotPendulum', jR)
world.addSensor.ConstraintTorque('L_RotPendulum', jR)

# add visualisation
world.addVisualisation.Box(crab, 1, 0.5, 0.1)
world.addVisualisation.Cylinder(pend.middle, 0.01, 1)
world.addVisualisation.Sphere(pend, 0.1)

# generate equations
#world.genEquations.Explicit(kinematicsOnly=False,graphOptimizations=True)
world.genEquations.Recursive(kinematicsOnly=False,graphOptimizations=True)

# generate simulation code
# world.genCode.Modelica('CraneCrab', './Output')
# world.genCode.Fortran90('CraneCrab', './Output')
# world.genCode.Matlab('CraneCrab', './Output')
# world.genCode.C('CraneCrab', './Output')
# world.genCode.Python('CraneCrab', './Output')

# show system
#world.genMatlabAnimation('CraneCrab')
world.show('CraneCrab')
print("Done")