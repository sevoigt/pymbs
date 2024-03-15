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
from pymbs.symbolics import VarKind

'''
Created on 17.08.2009

@author: Christian Schubert

'''

from pymbs.input import *

world=MbsSystem([0,0,-1])

#################################
# Parameters                    #
#################################
m1=world.addParam('mass 1', 'm1', 5)
l1=world.addParam('lengthOfRod 1', 'l1', 0.5)
I1=world.addParam('inertia 1', 'I1', m1*l1**2/12)

m2=world.addParam('mass 2', 'm2', 0.5)
l2=world.addParam('lengthOfRod 2', 'l2', 2)
I2=world.addParam('inertia 2', 'I2', m2*l2**2/12)

m3=world.addParam('mass 3', 'm3', 200)

#################################
# Bodies                        #
#################################
#cs = world.addBody('CrankShaft', mass=m1, cg=[l1/2,0,0], inertia=diag([0,I1,0]))
cs = world.addBody('CrankShaft', mass=m1, cg=[l1/2,0,0], inertia=diag([0,m1*l1**2/12,0]))
cs.addFrame('cs_rod', p=[l1,0,0])

#rod = world.addBody('Rod', mass=m2, cg=[l2/2,0,0], inertia=diag([0,I2,0]))
rod = world.addBody('Rod', mass=m2, cg=[l2/2,0,0], inertia=diag([0,m2*l2**2/12,0]))
rod.addFrame('rod_cs', p=[l2,0,0])

piston = world.addBody('Piston', mass=m3)
piston.addFrame('piston_rod', p=[0,0,0])

#################################
# Joints                        #
#################################
world.addJoint('Rot_CS', world, cs, 'Ry', 0)
world.addJoint('Trans_Piston', world, piston, 'Tx', 2.5)
world.addJoint('Rot_Rod', piston.piston_rod, rod, 'Ry', pi)

#################################
# Constraints                   #
#################################
world.addConstraint('Rod_Piston', rod.rod_cs, piston, transLock=[1,0,1])
#world.addLoop.CrankSlider(cs.cs_rod, rod.rod_cs, 'Test_Loop')

print("System has been assembled")

world.addVisualisation.Line(cs,  0.5)
world.addVisualisation.Line(rod, 2)
world.addVisualisation.Box(piston, 0.3, 0.1, 0.1)

E_cs = world.addSensor.Energy(cs, 'E_cs', 'E_cs')
E_rod = world.addSensor.Energy(rod, 'E_rod', 'E_rod')
E_piston = world.addSensor.Energy(piston, 'E_piston', 'E_piston')
E = world.addExpression('Energy', 'E', E_cs+E_rod+E_piston, category=VarKind.Sensor)
world.genEquations(explicit=True, diff=2)
world.genCode('m', 'CrankSlider')
world.genCode('mo', 'CrankSlider')
world.genMatlabAnimation('CrankSlider')
world.show('CrankSlider')