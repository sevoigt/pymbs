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

# import PyMbs
from PyMbs.Input import *

# set up inertial frame
world=MbsSystem([0,0,-1])

# add inputs and parameters
F=world.addInput('DrivingForce', 'F')
m1=world.addParam('mass 1', 'm1', 1)
m2=world.addParam('mass 2', 'm2', 1)
l2=world.addParam('lengthOfRod 2', 'l2', 1)
I2=world.addParam('inertia 2', 'I2', m2*l2**2/12)

# add bodies
crab=world.addBody('Crab', mass=m1)
pend=world.addBody('Pendulum', mass=m2, inertia=diag([0,I2,0]))
pend.addFrame('joint' , [0, 0, l2])
pend.addFrame('middle', [0, 0, l2/2], rotMat(pi/2,'x'))

# add joints
world.addJoint('TransCrab', world, crab, 'Tx', 1)
world.addJoint('RotPendulum', crab, pend.joint, 'Ry', -1)

# add load element and sensor
world.addLoad.PtPForce(crab, world, F)
world.addSensor.Distance(crab, world, 'd')

# add visualisation
world.addVisualisation.Box(crab, 1, 0.5, 0.1)
world.addVisualisation.Cylinder(pend.middle, 0.01, 1)
world.addVisualisation.Sphere(pend, 0.1)



# generate simulation code
world.genEquations(explicit=True)
world.genCode('mo', 'CraneCrab', '.\Output')
world.genCode('m', 'CraneCrab', 'Output')
world.genCode('py', 'CraneCrab', 'Output')
world.genCode('cpp', 'CraneCrab', 'Output')

# generate equations
world.genEquations(explicit=False)
world.genCode('cpp', 'CraneCrabRec', 'Output')

# show system
#world.genMatlabAnimation('CraneCrab')
#world.show('CraneCrab')