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
from PyMbs.Common.Functions import rotMat
from PyMbs.Symbolics import VarKind

'''
Created on 31.07.2009

@author: Christian Schubert

Wheel Loader
'''

from PyMbs.Input import *
from PyMbs.Symbolics import Matrix

world = MbsSystem([0, 0, -1])

# Federsteifigkeit und -Daempfung
c = world.addParam('c', 1e8, positive=True)
l_KiZy = world.addParam('l_KiZy', 1.64, positive=True)
l_HuZy = world.addParam('l_HuZy', 1.72, positive=True)

# Vorderrahmen (world)
VoRa_A_x = world.addParam('VoRa_A_x', 1.21, positive=True)
VoRa_A_z = world.addParam('VoRa_A_z', 1.17, positive=True)
VoRa_B_x = world.addParam('VoRa_B_x', 0.71, positive=True)
VoRa_B_z = world.addParam('VoRa_B_z', 1.08, positive=True)
VoRa_C_x = world.addParam('VoRa_C_x', 0.67, positive=True)
VoRa_C_z = world.addParam('VoRa_C_z', 0.35, positive=True)

world.addFrame(name='A', p=[VoRa_A_x, 0, VoRa_A_z])
world.addVisualisation.Frame(world.A, 0.5)
world.addFrame(name='B', p=[VoRa_B_x, 0, VoRa_B_z])
world.addVisualisation.Frame(world.B, 0.5)
world.addFrame(name='C', p=[VoRa_C_x, 0, VoRa_C_z])
world.addVisualisation.Frame(world.C, 0.5)

# Hubrahmen (HuRa)
HuRa_m = world.addParam('HuRa_m', 1500, positive=True)
HuRa_I = world.addParam('HuRa_I', 1000, positive=True)
HuRa_x = world.addParam('HuRa_x', 1.5, positive=True)
HuRa_F_x = world.addParam('HuRa_F_x', 1.67, positive=True)
HuRa_H_x = world.addParam('HuRa_H_x', 1.96, positive=True)
HuRa_H_z = world.addParam('HuRa_H_z', 0.62, positive=True)
HuRa_L_x = world.addParam('HuRa_L_x', 3.05, positive=True)

HuRa = world.addBody(name='HuRa', mass=HuRa_m, inertia=diag([0, HuRa_I, 0]),
                     cg=[HuRa_x, 0, 0])
HuRa.addFrame(name='B')
HuRa.addFrame(name='B_prime', R=rotMat(0.3064, 'y')) # atan2(HuRa_H_x,HuRa_H_z)
HuRa.addFrame(name='F', p=[HuRa_F_x, 0, 0])
HuRa.addFrame(name='H', p=[HuRa_H_x, 0, HuRa_H_z])
HuRa.addFrame(name='L', p=[HuRa_L_x, 0, 0])
world.addVisualisation.Line(HuRa, 3.05)
world.addVisualisation.Line(HuRa.B_prime, 2.05)# sqrt(HuRa_H_x**2, HuRa_H_z**2)

# Hubzylinder - Gehaeuse (HuZyG)
HuZyG_m = world.addParam('HuZyG_m', 150, positive=True)
HuZyG_I = world.addParam('HuZyG_I', 30, positive=True)
HuZyG_x = world.addParam('HuZyG_x', 0.6, positive=True)
HuZyG_E_x = world.addParam('HuZyG_E_x', 1.2, positive=True)

HuZyG = world.addBody(HuZyG_m, inertia=diag([0, HuZyG_I, 0]),
                      cg=[HuZyG_x, 0, 0], name='HuZyG')
HuZyG.addFrame(name='C')
HuZyG.addFrame(name='E', p=[HuZyG_E_x, 0, 0])
world.addVisualisation.Line(HuZyG, 1.2)

# Hubzylinder - Stange (HuZyS)
HuZyS_m = world.addParam('HuZyS_m', 100, positive=True)
HuZyS_I = world.addParam('HuZyS_I', 20, positive=True)
HuZyS_x = world.addParam('HuZyS_x', 0.6, positive=True)
HuZyS_E_x = world.addParam('HuZyS_E_x', 1.2, positive=True)

HuZyS = world.addBody(HuZyS_m, inertia=diag([0, HuZyS_I, 0]),
                      cg=[-HuZyS_x, 0, 0], name='HuZyS')
HuZyS.addFrame(name='F')
HuZyS.addFrame(name='E', p=[-HuZyS_E_x, 0, 0])
world.addVisualisation.Line(HuZyS, -1.2)

# Kippzylinder - Gehaeuse (KiZyG)
KiZyG_m = world.addParam('KiZyG_m', 150, positive=True)
KiZyG_I = world.addParam('KiZyG_I', 20, positive=True)
KiZyG_x = world.addParam('KiZyG_x', 0.6, positive=True)
KiZyG_D_x = world.addParam('KiZyG_D_x', 1.2, positive=True)

KiZyG = world.addBody(KiZyG_m, inertia=diag([0, KiZyG_I, 0]),
                      cg=[KiZyG_x, 0, 0], name='KiZyG')
KiZyG.addFrame(name='A')
KiZyG.addFrame(name='D', p=[KiZyG_D_x, 0, 0])
world.addVisualisation.Line(KiZyG, 1.2)

# Kippzylinder - Stange (KiZyS)
KiZyS_m = world.addParam('KiZyS_m', 100, positive=True)
KiZyS_I = world.addParam('KiZyS_I', 20, positive=True)
KiZyS_x = world.addParam('KiZyS_x', 0.5, positive=True)
KiZyS_D_x = world.addParam('KiZyS_D_x', 1, positive=True)

KiZyS = world.addBody(KiZyS_m, inertia=diag([0, KiZyS_I, 0]),
                      cg=[-KiZyS_x, 0, 0], name='KiZyS')
KiZyS.addFrame(name='G')
KiZyS.addFrame(name='D', p=[-KiZyS_D_x, 0, 0])
world.addVisualisation.Line(KiZyS, -1)

# Kipphebel (KiHe)
KiHe_m = world.addParam('KiHe_m', 500, positive=True)
KiHe_I = world.addParam('KiHe_I', 120, positive=True)
KiHe_G_x = world.addParam('KiHe_G_x', 0.9, positive=True)
KiHe_I_x = world.addParam('KiHe_I_x', 0.9, positive=True)

KiHe = world.addBody(KiHe_m, inertia=diag([0, KiHe_I, 0]), cg=[0, 0, 0],
                     name='KiHe')
KiHe.addFrame(name='H')
KiHe.addFrame(name='G', p=[-KiHe_G_x, 0, 0])
KiHe.addFrame(name='I', p=[ KiHe_I_x, 0, 0])
world.addVisualisation.Line(KiHe.G, 1.8)

# Kippstange (KiSt)
KiSt_m = world.addParam('KiSt_m', 100, positive=True)
KiSt_I = world.addParam('KiSt_I', 10, positive=True)
KiSt_x = world.addParam('KiSt_x', 0.4, positive=True)
KiSt_K_x = world.addParam('KiSt_K_x', 0.8, positive=True)

KiSt = world.addBody(KiSt_m, inertia=diag([0, KiSt_I, 0]), cg=[KiSt_x, 0, 0],
                     name='KiSt')
KiSt.addFrame(name='I')
KiSt.addFrame(name='K', p=[KiSt_K_x, 0, 0])
world.addVisualisation.Line(KiSt, 0.8)

# Schaufel (Scha)
Scha_m = world.addParam('Scha_m', 3000, positive=True)
Scha_I = world.addParam('Scha_I', 1000, positive=True)
Scha_x = world.addParam('Scha_x', 0.2, positive=True)
Scha_z = world.addParam('Scha_z', 0.7, positive=True)
Scha_K_x = world.addParam('Scha_K_x', 0.4, positive=True)

Scha = world.addBody(Scha_m, inertia=diag([0, Scha_I, 0]),
                     cg=[Scha_z, 0, Scha_x], name='Schaufel')
Scha.addFrame(name='L')
Scha.addFrame(name='K', p=[Scha_K_x, 0, 0])
world.addVisualisation.Line(Scha, 0.4)

# Gelenke
J_A = world.addJoint(name='Rot_A', CS1=world.A, CS2=KiZyG.A, dofList='Ry')
J_B = world.addJoint(name='Rot_B', CS1=world.B, CS2=HuRa.B, dofList='Ry',
                     startVals=0.2)
J_C = world.addJoint(name='Rot_C', CS1=world.C, CS2=HuZyG.C, dofList='Ry')
J_D = world.addJoint(name='Trans_D', CS1=KiZyG, CS2=KiZyS, dofList='Tx')
J_E = world.addJoint(name='Trans_E', CS1=HuZyG, CS2=HuZyS, dofList='Tx')
J_H = world.addJoint(name='Rot_H', CS1=HuRa.H, CS2=KiHe.H, dofList='Ry',
                     startVals=1.1)
J_I = world.addJoint(name='Rot_I', CS1=KiHe.I, CS2=KiSt.I, dofList='Ry')
J_L = world.addJoint(name='Rot_L', CS1=HuRa.L, CS2=Scha.L, dofList='Ry')

# Berechnung der kinematischen Schleifen
loops = True

if (loops):
    world.addLoop.FourBarTrans(KiHe.G, KiZyS.G, posture=1, name='KippKinematik')
    world.addLoop.FourBar(KiSt.K, Scha.K, posture=-1, name='SchaufelKinematik')
    world.addLoop.ThreeBarTrans(HuRa.F, HuZyS.F, name='HubKinematik')
else:
    world.addConstraint('KippKinematik', KiHe.G, KiZyS.G, transLock=[1, 0, 1])
    world.addConstraint('SchaufelKinematik', KiSt.K, Scha.K,
                        transLock=[1, 0, 1])
    world.addConstraint('HubKinematik', HuRa.F, HuZyS.F, transLock=[1, 0, 1])

# Federn anstelle der Zylinder einbauen
d_KiZy = world.addSensor.Joint('d_KiZy', J_D)
F_KiZy = world.addExpression('F_Kizy', c*(d_KiZy[0] - l_KiZy))
world.addLoad.PtPForce(F_KiZy, KiZyG.D, KiZyS.D)
d_HuZy = world.addSensor.Joint('d_HuZy', J_E)
F_HuZy = world.addExpression('F_Huzy', c*(d_HuZy[0] - l_HuZy))
world.addLoad.PtPForce(F_HuZy, HuZyG.E, HuZyS.E)

# Energy-Sensor
E_bodies = world.addSensor.Energy('E_bodies', world)

# Energie in Federn
E_KiZy = world.addExpression('E_Kizy', 0.5*c*(d_KiZy[0] - l_KiZy)**2)
E_HuZy = world.addExpression('E_Huzy', 0.5*c*(d_HuZy[0] - l_HuZy)**2)
E = world.addExpression('E', E_bodies + E_KiZy + E_HuZy,
                        category=VarKind.Sensor)

# Gleichungen generieren
print("System erstellt")

if (loops):
    world.genEquations.Explicit()
    world.genCode.Matlab('Arbeitskinematik_ODE', './Output')
#   world.genMatlabAnimation('Arbeitskinematik_ODE', './Output')
else:
    world.genEquations.Explicit(diff=[0, 1, 2])
    world.genCode.Matlab('Arbeitskinematik_DAE', './Output')
#   world.genMatlabAnimation('Arbeitskinematik_DAE', './Output')
print("Fertig")

world.show('Arbeitskinematik')
