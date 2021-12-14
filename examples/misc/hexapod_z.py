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
Created on 13.05.2011

@author: Jens Scheffler

Pfade für Visualisierung anpassen !!!

'''
#################################
# import PyMbs & Lib.           #
#################################

from PyMbs.Input import *

from PyMbs.Symbolics import Matrix,cos,sin
pi = 3.1415926535897932384626433832795

#################################
# set up inertial frame         #
#################################

world=MbsSystem([0,0,-1])

#################################
# Parameters                    #
#################################

# Länge der Zylinderstangen und Gehäuse


hoehe = 0.01
R_AP=0.3
R_BP=0.5
R_Zyl_stange=0.02
R_Zyl_geh=0.04
l_zyl=0.6
m_z_geh = 0.1
m_z_st = 0.1

c=world.addParam('c',10)
c1=world.addParam('c1',5)

m1=world.addParam('m1', 1.0)
R1=world.addParam('R1', R_BP)


m2=world.addParam('m2', 50)
R2=world.addParam('R2', R_AP)
H2=world.addParam('H2',hoehe)

I2x=world.addParam( 'I2x', (m2*H2**2)/12)   # Traägheit eines Vollzylinders um x die x-Achse
I2y=world.addParam( 'I2y', (m2*H2**2)/12)   # Traägheit eines Vollzylinders um y die x-Achse
I2z=world.addParam( 'I2z', (m2*R2**2)/2)    # Traägheit eines Vollzylinders um z die x-Achse


################################################

m_Zyl_Geh=world.addParam('m_Zyl_Geh', 18.6)
l_Zyl_Geh=world.addParam('l_Zyl_Geh',0.74)

cg_Zyl_Geh_x=world.addParam('cg_Zyl_Geh_x',0.353)

I_Zyl_Geh_x=world.addParam( 'I_Zyl_Geh_x', 0.027)
I_Zyl_Geh_y=world.addParam( 'I_Zyl_Geh_y', 1.061)
I_Zyl_Geh_z=world.addParam( 'I_Zyl_Geh_z', 1.061)

m_Zyl_Stange=world.addParam('m_Zyl_Stange', 8.4)
l_Zyl_Stange=world.addParam('l_Zyl_Stange',0.66)

cg_Zyl_Stange_x=world.addParam('cg_Zyl_Stange_x',-0.347)

I_Zyl_Stange_x=world.addParam('I_Zyl_Stange_x', 0.003)
I_Zyl_Stange_y=world.addParam('I_Zyl_Stange_y', 0.433)
I_Zyl_Stange_z=world.addParam('I_Zyl_Stange_z', 0.432)

###############
# Anordnungen #
###############

phi_BP_1 = pi/2-pi/18
phi_BP_2 = phi_BP_1 + pi/9
phi_BP_3 = phi_BP_1 + 2*pi/3
phi_BP_4 = phi_BP_2 + 2*pi/3
phi_BP_5 = phi_BP_3 + 2*pi/3
phi_BP_6 = phi_BP_4 + 2*pi/3

phi_AP_1 = pi/6+pi/18
phi_AP_2 = phi_AP_1 + 2*pi/3-pi/9
phi_AP_3 = phi_AP_1 + 2*pi/3
phi_AP_4 = phi_AP_3 + 2*pi/3-pi/9
phi_AP_5 = phi_AP_3 + 2*pi/3
phi_AP_6 = phi_AP_4 + 2*pi/3


################
#    Hexapod   #
################

#################################
#         Bodies & KS           #
#################################


Ground = world.addBody(name='Ground',mass=1)
Ground.addFrame(name='KS_1',p=[0,0,0])

BP = Ground.KS_1

BP.addFrame(name='BP_visual', p=[0,0,0],R=rotMat(pi/2,'x'))

BP.addFrame(name='BP_Anlenkpunkt_1', p=[R1*cos(phi_BP_1),R1*sin(phi_BP_1),0])
BP.addFrame(name='BP_Anlenkpunkt_2', p=[R1*cos(phi_BP_2),R1*sin(phi_BP_2),0])
BP.addFrame(name='BP_Anlenkpunkt_3', p=[R1*cos(phi_BP_3),R1*sin(phi_BP_3),0])
BP.addFrame(name='BP_Anlenkpunkt_4', p=[R1*cos(phi_BP_4),R1*sin(phi_BP_4),0])
BP.addFrame(name='BP_Anlenkpunkt_5', p=[R1*cos(phi_BP_5),R1*sin(phi_BP_5),0])
BP.addFrame(name='BP_Anlenkpunkt_6', p=[R1*cos(phi_BP_6),R1*sin(phi_BP_6),0])

BP.addFrame(name='BP_Feder',p=[0,0,1.1])


################################################################################

AP = world.addBody(name='Arbeitsplattform', mass=m2,inertia=diag([I2x,I2y,I2z]))

AP.addFrame(name='AP_visual', p=[0,0,0],R=rotMat(pi/2,'x'))

AP.addFrame(name='AP_Anlenkpunkt_1', p=[R2*cos(phi_AP_1),R2*sin(phi_AP_1),0])
AP.addFrame(name='AP_Anlenkpunkt_2', p=[R2*cos(phi_AP_2),R2*sin(phi_AP_2),0])
AP.addFrame(name='AP_Anlenkpunkt_3', p=[R2*cos(phi_AP_3),R2*sin(phi_AP_3),0])
AP.addFrame(name='AP_Anlenkpunkt_4', p=[R2*cos(phi_AP_4),R2*sin(phi_AP_4),0])
AP.addFrame(name='AP_Anlenkpunkt_5', p=[R2*cos(phi_AP_5),R2*sin(phi_AP_5),0])
AP.addFrame(name='AP_Anlenkpunkt_6', p=[R2*cos(phi_AP_6),R2*sin(phi_AP_6),0])

################################################################################

'''
#Für Visualisierung im Dymola

Zyl_geh_1 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_1')
Zyl_geh_1.addFrame('Zyl_geh_1_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_geh_1.addFrame('Zyl_geh_1_cs', p=[0,0,0])
Zyl_geh_1.addFrame('Zyl_geh_1_cs_2', p=[0,0,0])

Zyl_geh_2 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_2')
Zyl_geh_2.addFrame('Zyl_geh_2_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_geh_2.addFrame('Zyl_geh_2_cs', p=[0,0,0])
Zyl_geh_2.addFrame('Zyl_geh_2_cs_2', p=[0,0,0])

Zyl_geh_3 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_3')
Zyl_geh_3.addFrame('Zyl_geh_3_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_geh_3.addFrame('Zyl_geh_3_cs', p=[0,0,0])
Zyl_geh_3.addFrame('Zyl_geh_3_cs_2', p=[0,0,0])

Zyl_geh_4 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_4')
Zyl_geh_4.addFrame('Zyl_geh_4_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_geh_4.addFrame('Zyl_geh_4_cs', p=[0,0,0])
Zyl_geh_4.addFrame('Zyl_geh_4_cs_2', p=[0,0,0])

Zyl_geh_5 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_5')
Zyl_geh_5.addFrame('Zyl_geh_5_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_geh_5.addFrame('Zyl_geh_5_cs', p=[0,0,0])
Zyl_geh_5.addFrame('Zyl_geh_5_cs_2', p=[0,0,0])

Zyl_geh_6 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_6')
Zyl_geh_6.addFrame('Zyl_geh_6_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_geh_6.addFrame('Zyl_geh_6_cs', p=[0,0,0])
Zyl_geh_6.addFrame('Zyl_geh_6_cs_2', p=[0,0,0])

################################################################################

Zyl_stange_1 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_1')
Zyl_stange_1.addFrame('Zyl_stange_1_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_stange_1.addFrame('Zyl_stange_1_cs', p=[0,0,0])
Zyl_stange_1.addFrame('Zyl_stange_1_cs_2', p=[0,0,0])

Zyl_stange_2 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_2')
Zyl_stange_2.addFrame('Zyl_stange_2_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_stange_2.addFrame('Zyl_stange_2_cs', p=[0,0,0])
Zyl_stange_2.addFrame('Zyl_stange_2_cs_2', p=[0,0,0])

Zyl_stange_3 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_3')
Zyl_stange_3.addFrame('Zyl_stange_3_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_stange_3.addFrame('Zyl_stange_3_cs', p=[0,0,0])
Zyl_stange_3.addFrame('Zyl_stange_3_cs_2', p=[0,0,0])

Zyl_stange_4 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_4')
Zyl_stange_4.addFrame('Zyl_stange_4_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_stange_4.addFrame('Zyl_stange_4_cs', p=[0,0,0])
Zyl_stange_4.addFrame('Zyl_stange_4_cs_2', p=[0,0,0])

Zyl_stange_5 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_5')
Zyl_stange_5.addFrame('Zyl_stange_5_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_stange_5.addFrame('Zyl_stange_5_cs', p=[0,0,0])
Zyl_stange_5.addFrame('Zyl_stange_5_cs_2', p=[0,0,0])

Zyl_stange_6 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_6')
Zyl_stange_6.addFrame('Zyl_stange_6_visual', p=[0,0,0],R=rotMat(pi/2,'y')*rotMat(pi/2,'x'))
Zyl_stange_6.addFrame('Zyl_stange_6_cs', p=[0,0,0])
Zyl_stange_6.addFrame('Zyl_stange_6_cs_2', p=[0,0,0])
'''

# Für Visualisierung im PyMbs
Zyl_geh_1 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_1')
Zyl_geh_1.addFrame('Zyl_geh_1_visual', p=[0,0,l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_geh_1.addFrame('Zyl_geh_1_cs', p=[0,0,0])
Zyl_geh_1.addFrame('Zyl_geh_1_cs_2', p=[0,0,0])

Zyl_geh_2 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_2')
Zyl_geh_2.addFrame('Zyl_geh_2_visual', p=[0,0,l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_geh_2.addFrame('Zyl_geh_2_cs', p=[0,0,0])
Zyl_geh_2.addFrame('Zyl_geh_2_cs_2', p=[0,0,0])

Zyl_geh_3 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_3')
Zyl_geh_3.addFrame('Zyl_geh_3_visual', p=[0,0,l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_geh_3.addFrame('Zyl_geh_3_cs', p=[0,0,0])
Zyl_geh_3.addFrame('Zyl_geh_3_cs_2', p=[0,0,0])

Zyl_geh_4 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_4')
Zyl_geh_4.addFrame('Zyl_geh_4_visual', p=[0,0,l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_geh_4.addFrame('Zyl_geh_4_cs', p=[0,0,0])
Zyl_geh_4.addFrame('Zyl_geh_4_cs_2', p=[0,0,0])

Zyl_geh_5 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_5')
Zyl_geh_5.addFrame('Zyl_geh_5_visual', p=[0,0,l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_geh_5.addFrame('Zyl_geh_5_cs', p=[0,0,0])
Zyl_geh_5.addFrame('Zyl_geh_5_cs_2', p=[0,0,0])

Zyl_geh_6 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_6')
Zyl_geh_6.addFrame('Zyl_geh_6_visual', p=[0,0,l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_geh_6.addFrame('Zyl_geh_6_cs', p=[0,0,0])
Zyl_geh_6.addFrame('Zyl_geh_6_cs_2', p=[0,0,0])

################################################################################

Zyl_stange_1 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_1')
Zyl_stange_1.addFrame('Zyl_stange_1_visual', p=[0,0,-l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_stange_1.addFrame('Zyl_stange_1_cs', p=[0,0,0])
Zyl_stange_1.addFrame('Zyl_stange_1_cs_2', p=[0,0,0])

Zyl_stange_2 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_2')
Zyl_stange_2.addFrame('Zyl_stange_2_visual', p=[0,0,-l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_stange_2.addFrame('Zyl_stange_2_cs', p=[0,0,0])
Zyl_stange_2.addFrame('Zyl_stange_2_cs_2', p=[0,0,0])

Zyl_stange_3 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_3')
Zyl_stange_3.addFrame('Zyl_stange_3_visual', p=[0,0,-l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_stange_3.addFrame('Zyl_stange_3_cs', p=[0,0,0])
Zyl_stange_3.addFrame('Zyl_stange_3_cs_2', p=[0,0,0])

Zyl_stange_4 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_4')
Zyl_stange_4.addFrame('Zyl_stange_4_visual', p=[0,0,-l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_stange_4.addFrame('Zyl_stange_4_cs', p=[0,0,0])
Zyl_stange_4.addFrame('Zyl_stange_4_cs_2', p=[0,0,0])

Zyl_stange_5 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_5')
Zyl_stange_5.addFrame('Zyl_stange_5_visual', p=[0,0,-l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_stange_5.addFrame('Zyl_stange_5_cs', p=[0,0,0])
Zyl_stange_5.addFrame('Zyl_stange_5_cs_2', p=[0,0,0])

Zyl_stange_6 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_6')
Zyl_stange_6.addFrame('Zyl_stange_6_visual', p=[0,0,-l_zyl/2],R=rotMat(pi/2,'x'))
Zyl_stange_6.addFrame('Zyl_stange_6_cs', p=[0,0,0])
Zyl_stange_6.addFrame('Zyl_stange_6_cs_2', p=[0,0,0])


#################################
# Joints                        #
#################################

#world.addJoint('fix_BP', world, BP)
world.addJoint( world, Ground, name='fix_BP')

jAP=world.addJoint(world, AP,['Tx', 'Ty', 'Tz','Rx', 'Ry', 'Rz'],[0,0,1,0,0,0],name='free_AP')


world.addJoint(BP.BP_Anlenkpunkt_1,Zyl_geh_1.Zyl_geh_1_cs_2,['Rz', 'Ry'],[0,0],name='Zyl_geh_1_an_BP_1')
world.addJoint(BP.BP_Anlenkpunkt_2,Zyl_geh_2.Zyl_geh_2_cs_2,['Rz', 'Ry'],[0,0],name='Zyl_geh_1_an_BP_2')
world.addJoint(BP.BP_Anlenkpunkt_3,Zyl_geh_3.Zyl_geh_3_cs_2,['Rz', 'Ry'],[0,0],name='Zyl_geh_1_an_BP_3')
world.addJoint(BP.BP_Anlenkpunkt_4,Zyl_geh_4.Zyl_geh_4_cs_2,['Rz', 'Ry'],[0,0],name='Zyl_geh_1_an_BP_4')
world.addJoint(BP.BP_Anlenkpunkt_5,Zyl_geh_5.Zyl_geh_5_cs_2,['Rz', 'Ry'],[0,0],name='Zyl_geh_1_an_BP_5')
world.addJoint(BP.BP_Anlenkpunkt_6,Zyl_geh_6.Zyl_geh_6_cs_2,['Rz', 'Ry'],[0,0],name='Zyl_geh_1_an_BP_6')


world.addJoint(Zyl_geh_1.Zyl_geh_1_cs,Zyl_stange_1.Zyl_stange_1_cs_2,'Tz',0,name='Zyl_stange_1_an_Zyl_geh_1')
world.addJoint(Zyl_geh_2.Zyl_geh_2_cs,Zyl_stange_2.Zyl_stange_2_cs_2,'Tz',0,name='Zyl_stange_1_an_Zyl_geh_2')
world.addJoint(Zyl_geh_3.Zyl_geh_3_cs,Zyl_stange_3.Zyl_stange_3_cs_2,'Tz',0,name='Zyl_stange_1_an_Zyl_geh_3')
world.addJoint(Zyl_geh_4.Zyl_geh_4_cs,Zyl_stange_4.Zyl_stange_4_cs_2,'Tz',0,name='Zyl_stange_1_an_Zyl_geh_4')
world.addJoint(Zyl_geh_5.Zyl_geh_5_cs,Zyl_stange_5.Zyl_stange_5_cs_2,'Tz',0,name='Zyl_stange_1_an_Zyl_geh_5')
world.addJoint(Zyl_geh_6.Zyl_geh_6_cs,Zyl_stange_6.Zyl_stange_6_cs_2,'Tz',0,name='Zyl_stange_1_an_Zyl_geh_6')



########################
# Constraints or Loops #
########################

world.addLoop.Hexapod(AP.AP_Anlenkpunkt_1, Zyl_stange_1.Zyl_stange_1_cs, 'Verbindung_1')
world.addLoop.Hexapod(AP.AP_Anlenkpunkt_2, Zyl_stange_2.Zyl_stange_2_cs, 'Verbindung_2')
world.addLoop.Hexapod(AP.AP_Anlenkpunkt_3, Zyl_stange_3.Zyl_stange_3_cs, 'Verbindung_3')
world.addLoop.Hexapod(AP.AP_Anlenkpunkt_4, Zyl_stange_4.Zyl_stange_4_cs, 'Verbindung_4')
world.addLoop.Hexapod(AP.AP_Anlenkpunkt_5, Zyl_stange_5.Zyl_stange_5_cs, 'Verbindung_5')
world.addLoop.Hexapod(AP.AP_Anlenkpunkt_6, Zyl_stange_6.Zyl_stange_6_cs, 'Verbindung_6')


#####################
# add visualisation #
#####################

world.addVisualisation.Cylinder(BP.BP_visual,R_BP, hoehe)
world.addVisualisation.Cylinder(AP.AP_visual,R_AP, hoehe)

'''
# Für Visualisierung im Dymola

world.addVisualisation.File(Zyl_geh_1.Zyl_geh_1_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_geh_001.stl',1,name='Zylinder_geh_1')
world.addVisualisation.File(Zyl_geh_2.Zyl_geh_2_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_geh_001.stl',1,name='Zylinder_geh_2')
world.addVisualisation.File(Zyl_geh_3.Zyl_geh_3_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_geh_001.stl',1,name='Zylinder_geh_3')
world.addVisualisation.File(Zyl_geh_4.Zyl_geh_4_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_geh_001.stl',1,name='Zylinder_geh_4')
world.addVisualisation.File(Zyl_geh_5.Zyl_geh_5_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_geh_001.stl',1,name='Zylinder_geh_5')
world.addVisualisation.File(Zyl_geh_6.Zyl_geh_6_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_geh_001.stl',1,name='Zylinder_geh_6')

world.addVisualisation.File(Zyl_stange_1.Zyl_stange_1_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_stange_001.stl',1,name='Zylinder_stange_1')
world.addVisualisation.File(Zyl_stange_2.Zyl_stange_2_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_stange_001.stl',1,name='Zylinder_stange_2')
world.addVisualisation.File(Zyl_stange_3.Zyl_stange_3_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_stange_001.stl',1,name='Zylinder_stange_3')
world.addVisualisation.File(Zyl_stange_4.Zyl_stange_4_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_stange_001.stl',1,name='Zylinder_stange_4')
world.addVisualisation.File(Zyl_stange_5.Zyl_stange_5_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_stange_001.stl',1,name='Zylinder_stange_5')
world.addVisualisation.File(Zyl_stange_6.Zyl_stange_6_visual, 'C:\\Users\JeSche\Desktop\Diplom_Arbeit\Hexapod/zyl_stange_001.stl',1,name='Zylinder_stange_6')
'''

# Für Visualisierung im Dymola

world.addVisualisation.Cylinder(Zyl_geh_1.Zyl_geh_1_visual, R_Zyl_geh,l_zyl)
world.addVisualisation.Cylinder(Zyl_geh_2.Zyl_geh_2_visual, R_Zyl_geh,l_zyl)
world.addVisualisation.Cylinder(Zyl_geh_3.Zyl_geh_3_visual, R_Zyl_geh,l_zyl)
world.addVisualisation.Cylinder(Zyl_geh_4.Zyl_geh_4_visual, R_Zyl_geh,l_zyl)
world.addVisualisation.Cylinder(Zyl_geh_5.Zyl_geh_5_visual, R_Zyl_geh,l_zyl)
world.addVisualisation.Cylinder(Zyl_geh_6.Zyl_geh_6_visual, R_Zyl_geh,l_zyl)


world.addVisualisation.Cylinder(Zyl_stange_1.Zyl_stange_1_visual, R_Zyl_stange,l_zyl)
world.addVisualisation.Cylinder(Zyl_stange_2.Zyl_stange_2_visual, R_Zyl_stange,l_zyl)
world.addVisualisation.Cylinder(Zyl_stange_3.Zyl_stange_3_visual, R_Zyl_stange,l_zyl)
world.addVisualisation.Cylinder(Zyl_stange_4.Zyl_stange_4_visual, R_Zyl_stange,l_zyl)
world.addVisualisation.Cylinder(Zyl_stange_5.Zyl_stange_5_visual, R_Zyl_stange,l_zyl)
world.addVisualisation.Cylinder(Zyl_stange_6.Zyl_stange_6_visual, R_Zyl_stange,l_zyl)

world.addVisualisation.Frame(AP,0.4)
#world.addVisualisation.Frame(BP.BP_Feder,1)
world.addVisualisation.Frame(Ground,0.6)


#################################
# add visualisation             #
#################################



print("System has been assembled")



#################################
# add Sensors                   #
#################################

#world.addSensor.Position(world,AP.AP_Anlenkpunkt_1,"P_AP_1")

#world.addSensor.Energy(AP,'E_AP')


#####################
# add Imput & Load  #
#####################

#l = world.addSensor.Distance(AP,BP.BP_Feder, 'l', 'DistanceSensor')
#lz = world.addSensor.Distance(BP,AP, 'lz', 'DistanceSensor_Cylinder')

#c=50

#F_c = world.addExpression('SpringForce', 'F_c', -c*l[0])

#world.addLoad.PtPForce(AP,BP.BP_Feder, F_c, name='Spring')


#################################
# generate equations & sim Code #
#################################

world.genEquations.Recursive()

#world.genCode.Modelica('hexapod_z_kpl','.\HP_Output',inputsAsInputs=True, debugMode=False)


world.show('hexapod_z_kpl')
