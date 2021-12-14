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
Created on 26.08.2009

@author: Christian
'''

from PyMbs.Input import *

world = MbsSystem([0, -1, 0])

b = world.addParam('b', 1, name='Length_b')
B = world.addFrame('B', [0, -b, 0])

l = world.addParam('l', 1, name='Length_l')
J = world.addParam('J', 1, name='Inertia')
m = world.addParam('m', 1, name='Mass')
arm = world.addBody(m, [l, 0, 0], diag([0, 0, J]), name='Arm')

a = world.addParam('a', 1, name='Length_a')
A = arm.addFrame('A', p=[a, 0, 0])

world.addJoint(world, arm, 'Rz', name='RotArm')

world.addVisualisation.Box(arm, 10, 1, 1)

world.genEquations.Recursive()
world.genCode.Modelica('Arm', './output')

world.show('Arm')
