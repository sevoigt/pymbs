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

# set up PyMbs and the multi-body system
from PyMbs.Input import *
world = MbsSystem([0, 0, -1])

# add parameters
m = world.addParam('m', 1)
c = world.addParam('c', 10)
d = world.addParam('d', 0.2)

# add the body and constrain its movement
movingBody = world.addBody(m)
world.addJoint(world, movingBody, 'Tz', startVals=1)

# add force to simulate a spring-damper combination
l = world.addSensor.Distance('l', movingBody, world)
F = world.addExpression('F', -(c*l[0] + d*l[1]))
world.addLoad.PtPForce(F, movingBody, world)

# add visualisations
world.addVisualisation.Box(movingBody, 1, 1, 1)
world.addVisualisation.Frame(world)

# generate equations and show the system
world.genEquations.Recursive()
world.show('MassSpringDamper')
