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
pend.addFrame('middle', [0, 0, l2/2], rotMat(pi/2,'x'))

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

world.genCode.Modelica('CraneCrab', '.\Output')
world.genCode.Matlab('CraneCrab', 'Output')
world.genCode.Python('CraneCrab', 'Output')
world.genCode.C('CraneCrabRec', 'Output')

# show system
world.genMatlabAnimation('CraneCrab')
world.show('CraneCrab')