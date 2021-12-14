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
Created on 14.09.2010

@author: frenkel

model-description for a simple truck with one chassi and 8 wheels and one bucket
'''

from PyMbs.Input import *

world = MbsSystem([0, 0, -1])

# Add Parameters 
m_b = world.addParam('m_b', 1000, name='Mass_Bowl')
m_k = world.addParam('m_k', 8000, name='Mass_BodyShell')
m_r = world.addParam('m_r', 50, name='Mass_Wheel') 
m_h = world.addParam('m_h', 10, name='Mass_Hub') 
phi_b = world.addParam('phi_b', [100, 200, 300], name='Inertia_Bowl')
phi_k = world.addParam('phi_k', [600, 800, 900], name='Inertia_BodyShell')
phi_r = world.addParam('phi_r', [20, 50, 50], name='Inertia_Wheel')
phi_h = world.addParam('phi_h', [1, 1, 1], name='Inertia_Hub')
l_y = world.addParam('l_y', 1.074, name='Distance_BodyShell_Wheel_y')  
l_z = world.addParam('l_z', -0.44, name='Distance_BodyShell_Wheel_z')  
l_x1 = world.addParam('l_x1', 2.01, name='Distance_BodyShell_Wheel_x1') 
l_x2 = world.addParam('l_x2', 0.11, name='Distance_BodyShell_Wheel_x2') 
l_x3 = world.addParam('l_x3', -3.06, name='Distance_BodyShell_Wheel_x3') 
l_x4 = world.addParam('l_x4', -4.59, name='Distance_BodyShell_Wheel_x4') 
l_D = world.addParam('l_D', [2.0, 0, 1.3], name='Distance_BodyShell_Driver') 
l_bx = world.addParam('l_bx', -5.418, name='Distance_BodyShell_Bowlx') 
l_bz = world.addParam('l_bz', 0.438, name='Distance_BodyShell_Bowlz') 
l_bdx = world.addParam('l_bdx', -2.97, name='Distance_BodyShell_BowlDrehPx') 
l_bdz = world.addParam('l_bdz', -0.21, name='Distance_BodyShell_BowlDrehPz') 

# Add forces and torque
# Steering  
M_L = world.addInput('M_L', (1, 1), name='Torque_L') 

# Drive 
MA_FFL = world.addInput('MA_FFL', (1, 1), name='Torque_AFFL')
MA_FFR = world.addInput('MA_FFR', (1, 1), name='Torque_AFFR')
MA_FBL = world.addInput('MA_FBL', (1, 1), name='Torque_AFBL') 
MA_FBR = world.addInput('MA_FBR', (1, 1), name='Torque_AFBR')

# Add Body and Coordinate Systems 
world.addFrame('CS_world', zeros((3,)), eye(3))

# Truck
body = world.addBody(m_k, zeros((3,)), diag(phi_k), name='BodyShell')
body.addFrame('CS_cg')
body.addFrame('BodyShellFFL', [l_x1, l_y, l_z], eye(3))
body.addFrame('BodyShellFFR', [l_x1, -l_y, l_z], eye(3))
body.addFrame('BodyShellFBL', [l_x2, l_y, l_z], eye(3))
body.addFrame('BodyShellFBR', [l_x2, -l_y, l_z], eye(3))
body.addFrame('BodyShellBFL', [l_x3, l_y, l_z], eye(3))
body.addFrame('BodyShellBFR', [l_x3, -l_y, l_z], eye(3))
body.addFrame('BodyShellBBL', [l_x4, l_y, l_z], eye(3))
body.addFrame('BodyShellBBR', [l_x4, -l_y, l_z], eye(3))
body.addFrame('BodyShellBowlDreh', [l_bx, 0, l_bz], eye(3))
body.addFrame('Driver', l_D, eye(3))

# Bowl
bowl = world.addBody(m_b, zeros((3,)), diag(phi_b), name='Bowl')
bowl.addFrame('Drehpunkt', [l_bdx, 0, l_bdz], eye(3))

# Wheels
wFFL = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelFFL') 
wFFL.addFrame('CS_cg')
wFFL.addFrame('CRadFFL', p=[0, 0, 0], R=rotMat(pi/2, 'x'))
hFFL = world.addBody(m_h, zeros((3,)), diag(phi_h), name='HubFFL')
hFFL.addFrame('CS_cg')

wFFR = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelFFR') 
wFFR.addFrame('CS_cg')
wFFR.addFrame('CRadFFR', p=[0, 0, 0], R=rotMat(pi/2, 'x'))
hFFR = world.addBody(m_h, zeros((3,)), diag(phi_h), name='HubFFR')
hFFR.addFrame('CS_cg')

wFBL = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelFBL') 
wFBL.addFrame('CS_cg')
wFBL.addFrame('CRadFBL', p=[0, 0, 0], R=rotMat(pi/2, 'x'))
hFBL = world.addBody(m_h, zeros((3,)), diag(phi_h), name='HubFBL')
hFBL.addFrame('CS_cg')

wFBR = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelFBR') 
wFBR.addFrame('CS_cg')
wFBR.addFrame('CRadFBR', p=[0, 0, 0], R=rotMat(pi/2, 'x'))
hFBR = world.addBody(m_h, zeros((3,)), diag(phi_h), name='HubFBR')
hFBR.addFrame('CS_cg')

wBFL = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelBFL') 
wBFL.addFrame('CS_cg')
wBFL.addFrame('CRadBFL', p=[0, 0, 0], R=rotMat(pi/2, 'x'))

wBFR = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelBFR') 
wBFR.addFrame('CS_cg')
wBFR.addFrame('CRadBFR', p=[0, 0, 0], R=rotMat(pi/2, 'x'))

wBBL = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelBBL') 
wBBL.addFrame('CS_cg')
wBBL.addFrame('CRadBBL', p=[0, 0, 0], R=rotMat(pi/2, 'x'))

wBBR = world.addBody(m_r, zeros((3,)), diag(phi_r), name='WheelBBR') 
wBBR.addFrame('CS_cg')
wBBR.addFrame('CRadBBR', p=[0, 0, 0], R=rotMat(pi/2, 'x'))

# Joints
world.addJoint(world.CS_world, body.CS_cg, ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'])

j1 = world.addJoint(body.BodyShellFFL, hFFL.CS_cg, ['Rz'], name='RotLenkLF')
j2 = world.addJoint(body.BodyShellFFR, hFFR.CS_cg, ['Rz'], name='RotLenkRF')
j3 = world.addJoint(body.BodyShellFBL, hFBL.CS_cg, ['Rz'], name='RotLenkLB')
j4 = world.addJoint(body.BodyShellFBR, hFBR.CS_cg, ['Rz'], name='RotLenkRB')
world.addJoint(hFFL.CS_cg, wFFL.CS_cg, ['Ry'], name='RotWheelFFL')
world.addJoint(hFFR.CS_cg, wFFR.CS_cg, ['Ry'], name='RotWheelFFR')
world.addJoint(hFBL.CS_cg, wFBL.CS_cg, ['Ry'], name='RotWheelFBL')
world.addJoint(hFBR.CS_cg, wFBR.CS_cg, ['Ry'], name='RotWheelFBR')
world.addJoint(body.BodyShellBFL, wBFL.CS_cg, ['Ry'], name='RotWheelBFL')
world.addJoint(body.BodyShellBFR, wBFR.CS_cg, ['Ry'], name='RotWheelBFR')
world.addJoint(body.BodyShellBBL, wBBL.CS_cg, ['Ry'], name='RotWheelBBL')
world.addJoint(body.BodyShellBBR, wBBR.CS_cg, ['Ry'], name='RotWheelBBR')
world.addJoint(body.BodyShellBowlDreh, bowl.Drehpunkt, ['Ry'], name='RotBowl')

world.addLoop.Transmission(j1, j2, 1, 'Fix1')
world.addLoop.Transmission(j1, j3, 1, 'Fix2')
world.addLoop.Transmission(j1, j4, 1, 'Fix3')

# Forces and Torque 
# Drive
eMA_FFL = world.addExpression('eMA_FFL', [0, MA_FFL, 0], name='Force_MA_FFL')
eMA_FFR = world.addExpression('eMA_FFR', [0, MA_FFR, 0], name='Force_MA_FFR')
eMA_FBL = world.addExpression('eMA_FBL', [0, MA_FBL, 0], name='Force_MA_FBL')
eMA_FBR = world.addExpression('eMA_FBR', [0, MA_FBR, 0], name='Force_MA_FBR')
world.addLoad.CmpTorque(eMA_FFL, hFFL.CS_cg, wFFL, name='Antrieb_FFL') 
world.addLoad.CmpTorque(eMA_FFR, hFFR.CS_cg, wFFR, name='Antrieb_FFR') 
world.addLoad.CmpTorque(eMA_FBL, hFBL.CS_cg, wFBL, name='Antrieb_FBL') 
world.addLoad.CmpTorque(eMA_FBR, hFBR.CS_cg, wFBR, name='Antrieb_FBR') 

# Steering
eM_L = world.addExpression('eM_L', [0, 0, M_L], name='Force_M_L')
world.addLoad.CmpTorque(eM_L, body.BodyShellFFL, wFFL.CS_cg, name='Lenkung')

# Loads and Sensors for Wheels
world.addTyreSensorsLoads('Wheel_FFL', hFFL.CS_cg, wFFL)
world.addTyreSensorsLoads('Wheel_FFR', hFFR.CS_cg, wFFR)
world.addTyreSensorsLoads('Wheel_FBL', hFBL.CS_cg, wFBL)
world.addTyreSensorsLoads('Wheel_FBR', hFBR.CS_cg, wFBR)
world.addTyreSensorsLoads('Wheel_BFL', body.BodyShellBFL, wBFL)
world.addTyreSensorsLoads('Wheel_BFR', body.BodyShellBFR, wBFR)
world.addTyreSensorsLoads('Wheel_BBL', body.BodyShellBBL, wBBL)
world.addTyreSensorsLoads('Wheel_BBR', body.BodyShellBBR, wBBR)

# Contact Reader
world.addContactSensorsLoads('CReifenFFL', wFFL.CRadFFL)
world.addContactSensorsLoads('CReifenFFR', wFFR.CRadFFR)
world.addContactSensorsLoads('CReifenFBL', wFBL.CRadFBL)
world.addContactSensorsLoads('CReifenFBR', wFBR.CRadFBR)
world.addContactSensorsLoads('CReifenBFL', wBFL.CRadBFL)
world.addContactSensorsLoads('CReifenBFR', wBFR.CRadBFR)
world.addContactSensorsLoads('CReifenBBL', wBBL.CRadBBL)
world.addContactSensorsLoads('CReifenBBR', wBBR.CRadBBR)

# Motion System
world.addMotionSystemSensors(body.Driver)

# Visualisation
world.addVisualisation.Box(body, 5, 1.2, 0.5, name="PO_Chassis")
world.addVisualisation.Cylinder(wFFL, 0.5, 0.25, name="PO_WheelFFL")
world.addVisualisation.Cylinder(wFFR, 0.5, 0.25, name="PO_WheelFFR")
world.addVisualisation.Cylinder(wFBL, 0.5, 0.25, name="PO_WheelFBL")
world.addVisualisation.Cylinder(wFBR, 0.5, 0.25, name="PO_WheelFBR")
world.addVisualisation.Cylinder(wBFL, 0.5, 0.25, name="PO_WheelBFL")
world.addVisualisation.Cylinder(wBFR, 0.5, 0.25, name="PO_WheelBFR")
world.addVisualisation.Cylinder(wBBL, 0.5, 0.25, name="PO_WheelBBL")
world.addVisualisation.Cylinder(wBBR, 0.5, 0.25, name="PO_WheelBBR")
world.addVisualisation.Frame(body.Driver, 1, name="PO_Driver")
world.addVisualisation.Box(bowl, 3, 1.2, 0.5, name="PO_Bowl")

print("System has been assembled")

# Generate Equations and write Modelica Code
world.genEquations.Recursive()
world.genCode.Modelica('LKWMBS', './Output')

world.show('LKW')
