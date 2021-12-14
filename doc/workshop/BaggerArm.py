# -*- coding: utf-8 -*-
'''
Created on 31.07.2009

@author: Christian Schubert

BaggerArm
'''

from PyMbs.Input import *
world = MbsSystem([0,0,-1])

# Koordinatensysteme der Welt
ow_mb = world.addFrame('KS_Monoblock', p=[0.43,-0.067,0.567])
ow_Zyl_L = world.addFrame('KS_Zyl_L', p=[0.86,0.168,0.137])
ow_Zyl_R = world.addFrame('KS_Zyl_R', p=[0.86,-0.3025,0.137])

# Monoblock
mb = world.addBody(name='Monoblock', mass=1758, cg=[1.99,0,3.18], inertia=diag([2940.5,7768.9,4890.8]))
mb_Zyl_L = mb.addFrame(name='KS_Zyl_L', p=[0,0.235,2])
mb_Zyl_R = mb.addFrame(name='KS_Zyl_R', p=[0,-0.2355,2])

# Zylinder- Stange/Gehaeuse - Links
Zyl_L_Sta = world.addBody(name='Zyl_L_Sta', mass=53.6, cg=[-0.57,0,0], inertia=diag([0.06,11.5,11.4]))
Zyl_L_Geh = world.addBody(name='Zyl_L_Geh', mass=64.7, cg=[0.7,0,0], inertia=diag([0.2,15,15]))

# Zylinder- Stange/Gehaeuse - Rechts
Zyl_R_Sta = world.addBody(name='Zyl_R_Sta', mass=53.6, cg=[-0.57,0,0], inertia=diag([0.06,11.5,11.4]))
Zyl_R_Geh = world.addBody(name='Zyl_R_Geh', mass=64.7, cg=[0.7,0,0], inertia=diag([0.2,15,15]))

# Zusammenbau
world.addJoint(name='Rot_OW_MB', CS1=ow_mb, CS2=mb, dofList=['Ry'], startVals=[0])

world.addJoint(name='Rot_OW_Zyl_L', CS1=ow_Zyl_L, CS2=Zyl_L_Geh, dofList=['Ry'], startVals=[0])
world.addJoint(name='Trans_Zyl_L', CS1=Zyl_L_Geh, CS2=Zyl_L_Sta, dofList=['Tx'], startVals=[1])

world.addJoint(name='Rot_OW_Zyl_R', CS1=ow_Zyl_R, CS2=Zyl_R_Geh, dofList=['Ry'], startVals=[0])
world.addJoint(name='Trans_Zyl_R', CS1=Zyl_R_Geh, CS2=Zyl_R_Sta, dofList=['Tx'], startVals=[1])

# Kinematische Schleifen schließen
world.addLoop.ThreeBarTrans(name='Zyl_L_Loop', CS1=Zyl_L_Sta, CS2=mb_Zyl_L)
world.addLoop.ThreeBarTrans(name='Zyl_R_Loop', CS1=Zyl_R_Sta, CS2=mb_Zyl_R)

# Kraftelemente
world.addHydCylSensorsLoads(name='Zylinder_Links', CS1=Zyl_L_Geh, CS2=Zyl_L_Sta)
world.addHydCylSensorsLoads(name='Zylinder_Rechts', CS1=Zyl_R_Geh, CS2=Zyl_R_Sta)

# Visualisierung
path = 'STL/%s.STL'
world.addVisualisation.File(world, fileName=path%'Oberwagen', name='Oberwagen', scale=1e-3)
world.addVisualisation.File(mb, fileName=path%'Monoblock', name='Monoblock', scale=1e-3)
world.addVisualisation.File(cs=Zyl_L_Sta, fileName=path%'Zylinder_Stange', name='Zyl_L_Sta', scale=1e-3)
world.addVisualisation.File(cs=Zyl_L_Geh, fileName=path%'Zylinder_Gehaeuse', name='Zyl_L_Geh', scale=1e-3)
world.addVisualisation.File(cs=Zyl_R_Sta, fileName=path%'Zylinder_Stange', name='Zyl_R_Sta', scale=1e-3)
world.addVisualisation.File(cs=Zyl_R_Geh, fileName=path%'Zylinder_Gehaeuse', name='Zyl_R_Geh', scale=1e-3)

# Gleichungen generieren
world.genEquations.Recursive()

# GUI anzeigen
world.show('BaggerArm', AAFrames=0)




