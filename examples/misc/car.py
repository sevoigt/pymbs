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
Created on 28.07.2009

@author: frenkel

model-description for a simple car with one chassi an 4 wheels
'''

from PyMbs.Input import *

world = MbsSystem([0, 0, -1])

# Add Parameters
m_k = world.addParam('m_k', 920, name='Mass_BodyShell')
m_r = world.addParam('m_r', 10, name='Mass_Wheel')
m_h = world.addParam('m_h', 1, name='Mass_Hub')

phi_k = world.addParam('phi_k', [100, 300, 300], name='Inertia_BodyShell')
phi_r = world.addParam('phi_r', [2, 5, 5], name='Inertia_Wheel')
phi_h = world.addParam('phi_h', [1, 1, 1], name='Inertia_Hub')

l_F = world.addParam('l_F', [1.559, 0.904, 0],
                     name='Distance_BodyShell_Wheel_F')
l_B = world.addParam('l_B', [-1.524, 0.904, 0],
                     name='Distance_BodyShell_Wheel_B')
l_D = world.addParam('l_D', [-0.3, 0.2, 0.01],
                     name='Distance_BodyShell_Driver')
l_W = world.addParam('l_W', [0.1, 0.2, 0.01],
                     name='Distance_BodyShell_SteeringWheel')

# Add forces and torque
# Steering
M_L = world.addInput('M_L', (1, 1), name='Torque_L')
# front left
FC_FL = world.addInput('FC_FL', (3,), name='Force_CFL')
# front right
FC_FR = world.addInput('FC_FR', (3,), name='Force_CFR')
# Drive
MA_FL = world.addInput('MA_FL', (1, 1), name='Torque_AFL')
MA_FR = world.addInput('MA_FR', (1, 1), name='Torque_AFR')
MA_BL = world.addInput('MA_BL', (1, 1), name='Torque_ABL')
MA_BR = world.addInput('MA_BR', (1, 1), name='Torque_ABR')


# Add Body and Coordinate Systems
world.addFrame('CS_world', zeros((3,)), eye(3))
# <!-- Buggy -->
body = world.addBody(m_k, zeros((3,)), diag(phi_k), name='BodyShell')
body.addFrame('CS_cg')
body.addFrame('BodyShellFL', l_F, eye(3))
body.addFrame('BodyShellFR', diag([1, -1, 1])*l_F, eye(3))
body.addFrame('BodyShellBL', l_B, eye(3))
body.addFrame('BodyShellBR', diag([1, -1, 1])*l_B, eye(3))
body.addFrame('Driver', l_D, eye(3))
body.addFrame('SteeringWheel', l_W, eye(3))
# <!-- Wheels -->
wFL = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelFL')
wFL.addFrame('CS_cg')
hFL = world.addBody(m_h, zeros((3,)), diag(phi_h), name='HubFL')
hFL.addFrame('CS_cg')
wFR = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelFR')
wFR.addFrame('CS_cg')
hFR = world.addBody(m_h, zeros((3,)), diag(phi_h), name='HubFR')
hFR.addFrame('CS_cg')
wBL = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelBL')
wBL.addFrame('CS_cg')
wBR = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelBR')
wBR.addFrame('CS_cg')


world.addJoint(world.CS_world, body.CS_cg,
               ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'], name='DOF6')

j1 = world.addJoint(body.BodyShellFL, hFL.CS_cg, ['Rz'], name='RotLenkL')
j2 = world.addJoint(body.BodyShellFR, hFR.CS_cg, ['Rz'], name='RotLenkR')
world.addJoint(hFL.CS_cg, wFL.CS_cg, ['Ry'], name='RotWheelFL')
world.addJoint(hFR.CS_cg, wFR.CS_cg, ['Ry'], name='RotWheelFR')
world.addJoint(body.BodyShellBL, wBL.CS_cg, ['Ry'], name='RotWheelBL')
world.addJoint(body.BodyShellBR, wBR.CS_cg, ['Ry'], name='RotWheelBR')

world.addLoop.Transmission(j1, j2, 1, 'Fix')

# forces and torque
# <!-- Drive -->
eMA_FL = world.addExpression('eMA_FL', [0, MA_FL, 0], name='Force_MA_FL')
eMA_FR = world.addExpression('eMA_FR', [0, MA_FR, 0], name='Force_MA_FR')
eMA_BL = world.addExpression('eMA_BL', [0, MA_BL, 0], name='Force_MA_BL')
eMA_BR = world.addExpression('eMA_BR', [0, MA_BR, 0], name='Force_MA_BR')

world.addLoad.CmpTorque(eMA_FL, wFL.CS_cg, hFL.CS_cg, name='Antrieb_FL')
world.addLoad.CmpTorque(eMA_FR, wFR.CS_cg, hFR.CS_cg, name='Antrieb_FR')
world.addLoad.CmpTorque(eMA_BL, wBL.CS_cg, body.BodyShellBL, name='Antrieb_BL')
world.addLoad.CmpTorque(eMA_BR, wBR.CS_cg, body.BodyShellBR, name='Antrieb_BR')
# <!-- Steering -->
eM_L = world.addExpression('eM_L', [0, 0, M_L], name='Force_M_L')
world.addLoad.CmpTorque(eM_L, body.BodyShellFL, wFL.CS_cg, name='Lenkung')
# <!-- Wheel FL -->
world.addLoad.CmpForce(FC_FL, wFL.CS_cg, world.CS_world, 'FCWheel_FL',
                       hFL.CS_cg)
# <!-- Wheel FR -->
world.addLoad.CmpForce(FC_FR, wFR.CS_cg, world.CS_world, 'FCWheel_FR',
                       hFR.CS_cg)

#  ''' Loads and Sensors for Wheels '''
world.addTyreSensorsLoads('Wheel_FL', hFL.CS_cg, wFL)
world.addTyreSensorsLoads('Wheel_FR', hFR.CS_cg, wFR)
world.addTyreSensorsLoads('Wheel_BL', body.BodyShellBL, wBL)
world.addTyreSensorsLoads('Wheel_BR', body.BodyShellBR, wBR)

# Motion System
world.addMotionSystemSensors(body.Driver)

# Visualisation
world.addVisualisation.Box(body, 3, 1.2, 0.5, name="PO_Chassis")
world.addVisualisation.Cylinder(wFL, 0.5, 0.25, name="PO_WheelFL")
world.addVisualisation.Cylinder(wFR, 0.5, 0.25, name="PO_WheelFR")
world.addVisualisation.Cylinder(wBL, 0.5, 0.25, name="PO_WheelBL")
world.addVisualisation.Cylinder(wBR, 0.5, 0.25, name="PO_WheelBR")
world.addVisualisation.Frame(body.Driver, 1, name="PO_Driver")
world.addVisualisation.Frame(body.SteeringWheel, 1, name="PO_SteeringWheel")

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Modelica('FahrzeugMBS', './Output')
world.genCode.Matlab('FahrzeugMBS', './Output')
#world.genEquations(explicit=True)
#world.genCode('m', 'FahrzeugMBS_exp', '.\Output', debugMode=True)
world.show('Car')
