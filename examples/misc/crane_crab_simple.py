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

# import PyMbs
from PyMbs.Input import *

# set up inertial frame
world = MbsSystem([0, 0, -1])

# add parameters
m1 = world.addParam('m1', 1.0)
m2 = world.addParam('m2', 1.0)
l2 = world.addParam('l2', 1.0)
I2 = world.addParam('I2', (m2*l2**2)/12)

# add bodies
trolley = world.addBody(mass=m1, name='Trolley')
pend = world.addBody(mass=m2, inertia=diag([0, I2, 0]), name='Pendulum')

# add frames
pend.addFrame(name='joint' , p=[0, 0, l2])
pend.addFrame(name='middle', p=[0, 0, l2/2], R=rotMat(pi/2,'x'))

# add joints
world.addJoint(world, trolley, dofList='Tx', startVals=1)
world.addJoint(trolley, pend.joint, dofList='Ry', startVals=-1)

# add visualisation
world.addVisualisation.Box(trolley, 1, 0.5, 0.1)
world.addVisualisation.Cylinder(pend.middle, 0.01, 1)
world.addVisualisation.Sphere(pend, 0.1)

# generate equations
world.genEquations.Recursive()
world.show('CraneTrolley')
