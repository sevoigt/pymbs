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
# Warning: The source code of the examples is quoted in the documentation. If
# you change this file, you'll have to change the corresponding file in the
# documentation (see doc/examples).

# import PyMbs functionality
from PyMbs.Input import *

# create main object and inital reference frame 
world = MbsSystem([0, 0, -1])

# mass, length and inertia of the rod
m = world.addParam('m', 1.0)
l = world.addParam('l', 1.0)
I = world.addParam('Inertia', (m*l**2)/12)

# add pendulum
pend = world.addBody(mass=m, inertia=diag([0, I, 0]))
# add additional coordinate system at upper end of the rod
pend.addFrame('link', [0,0,0.5*l])

# add joint to constrain motion of the pendulum
world.addJoint(world, pend.link, 'Ry')

# use a Box to represente the pendulum
world.addVisualisation.Box(pend, length=0.1, width=0.1, height=1.0)

# generate equations of motion
world.genEquations.Recursive()

# show gui
world.show('SimplePendulum')
