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

@author: knoll

model-description for a simple pendulum
adapted from matlab-Example
'''
from PyMbs.Input import *

world = MbsSystem([0, 0, -1])

m = world.addParam('m', 1, 'mass')
I = world.addParam('inertia', 1, 'J')
l = world.addParam('L', 1, 'lengthOfRod')
F = world.addInput('F', name='force')
F_tip = world.addInput('F_tip', (3,), name='CmpForce_at_Tip')
T_tip = world.addInput('T_tip', (3,), name='CmpTorque_at_Tip')
cg_pend = world.addParam('cg_pend', [0, 0, 0], 'cg_Pendulum')

world.addFrame('CS_world', zeros((3,)), eye(3))
# where the spring will be connected to:
world.addFrame('CS_world_p1', [l*2, 0, 0], eye(3))

#pend=world.addBody('Pendulum', m, cg_pend, diag([0,0,I]))
pend = world.addBody(m, cg_pend, diag([0, I, 0]), name='Pendulum')
pend.addFrame('CS_joint', [0, 0, -l/2], eye(3))
pend.addFrame('CS_tip', [0, 0, l/2], eye(3))
pend.addFrame(name='middle', p=[0, 0, -l/4], R=rotMat(pi/2, 'x'))

world.addVisualisation.Cylinder(pend.middle, 0.01, 0.5)
world.addVisualisation.Sphere(pend, 0.1)

# TestCS
#pend.CS_tip.addFrame('testCS1', [0,-l,0], eye(3))
world.addJoint(world.CS_world, pend.CS_joint, 'Ry', startVals=1,
               name='RevPendulum')

world.addLoad.PtPForce(F, pend.CS_tip, world.CS_world_p1, name='Spring')
world.addLoad.CmpForce(F_tip, pend.CS_tip, world.CS_world, name='TipForce',
                       CSref=world)
world.addLoad.CmpTorque(T_tip, pend.CS_tip, world.CS_world, name='TipTorque',
                        CSref=world)

world.addSensor.Position('pos', world.CS_world_p1, pend.CS_tip, name='Pos')
world.addSensor.Orientation('rot', world.CS_world_p1, pend.CS_tip,
                            name='Orientation')
world.addSensor.Velocity('vel', world.CS_world_p1, pend.CS_tip, name='Vel')
world.addSensor.Distance('d', world.CS_world_p1, pend.CS_tip, name='D')

print("System has been assembled")

#world.genEquations.Explicit(kinematicsOnly=False,graphOptimizations=True)
world.genEquations.Recursive(kinematicsOnly=False, graphOptimizations=True)
#world.genCode.Modelica('Pendulum_exp', './Output', inputsAsInputs=True)
#world.genCode.Fortran90('Pendulum_exp', './Output', inputsAsInputs=True)
#world.genCode.Matlab('Pendulum_exp', './Output', inputsAsInputs=True)
#world.genCode.C('Pendulum_exp', '.\Output', inputsAsInputs=True)
world.genCode.FMU('Pendulum_exp', './Output', inputsAsInputs=True, util_folder_path="/home/psirus/Code/Python/pymbs/utils")

#world.show("Pendulum")
