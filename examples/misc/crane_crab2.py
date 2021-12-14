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

from PyMbs.Input import *

# Set up a new MbsSystem
world = MbsSystem([0, 0, -1])

# Define Input and Parameters
F = world.addInput('F', name='Force')
m1 = world.addParam('m1', 10, name='mass 1')
m2 = world.addParam('m2', 1, name='mass 2')
l2 = world.addParam('l2', 1, name='length')
I2 = world.addParam('I2', 1./12, name='inertia 2')

# Define Bodies and Coordinate Systems
crab = world.addBody(m1, name='Crab')
load = world.addBody(m2, inertia=diag([0, I2, 0]), name='Load')
load.addFrame('joint', p=[l2, 0, 0])

# Connect Bodies Through Joints
world.addJoint(world, crab, 'Tx', startVals=1, name='TransCrab')
world.addJoint(crab, load.joint, 'Rz', name='RotLoad')

# Add Sensors and Force Elements
world.addLoad.PtPForce(F, crab, world, 'DrivingForce')
world.addSensor.Position('d', crab, world, name='Distance')

# Add Visualisation
crab.addFrame('axis1', p=[-0.2, -0.15, 0], R=rotMat(pi/2, 'y'))
load.addFrame('rope', p=[l2/2, 0, 0])
world.addVisualisation.Box(crab, 0.5, 0.2, 0.3)
world.addVisualisation.Sphere(load, 0.1)
world.addVisualisation.Box(load.rope, 1, 0.03, 0.03)
world.addVisualisation.Cylinder(crab.axis1, 0.05, 0.3)

# Calculate Equations of Motion and Generate Code
world.genEquations.Explicit()
world.genCode.Modelica('CraneCrab', './Output')
world.show('CraneCrab')
