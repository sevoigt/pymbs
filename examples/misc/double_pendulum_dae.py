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
Created on 02.10.2009

@author: Christian Schubert
'''
from PyMbs.Input import *

world = MbsSystem([0,0,-1])

m1 = world.addParam( 'm1', 1, 'mass1')
l1 = world.addParam('l1', 1,'length1')
m2 = world.addParam('m2', 1,'mass2')
l2 = world.addParam('l2', 1,'length2')

pend1 = world.addBody(m1, [l1/2,0,0], diag([0,0,m1*l1**2/12]),name='Pendulum1')
pend1.addFrame('tip', p=[l1,0,0])
pend2 = world.addBody(m2, [l2/2,0,0], diag([0,0,m2*l2**2/12]),name='Pendulum2')

world.addJoint(world, pend1, 'Ry', 1, name='J_Rot1')

world.addJoint(world, pend2, ['Tx', 'Tz', 'Ry'], [0.5,-0.8,1],name='J_Planar')

world.addConstraint(pend1.tip, pend2, [1,0,1],name='Constr_Tip')

world.addVisualisation.Frame(world, 0.2)
world.addVisualisation.Line(pend1, 1)
world.addVisualisation.Line(pend2, 1)

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Modelica('DoublePendulumDAE_3',dirname='Output')

#world.show('DoublePendulum')
