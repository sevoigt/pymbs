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
Created on 08.08.2011

@author: Jens Scheffler

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
R_AP=0.5
R_BP=0.5
R_Zyl_stange=0.02
R_Zyl_geh=0.04
l_zy=0.6
m_z_geh = 0.1
m_z_st = 0.1

c=world.addParam('c',10)
c1=world.addParam('c1',5)

m1=world.addParam('m1', 1.0)
R1=world.addParam('R1', 0.5)


m2=world.addParam('m2', 1.0)
R2=world.addParam('R2', 0.3)
H2=world.addParam('H2',hoehe)

I2x=world.addParam( 'I2x', (m2*hoehe**2)/12) # Traägheit eines Vollzylinders um x die x-Achse
I2y=world.addParam( 'I2y', (m2*hoehe**2)/12) # Traägheit eines Vollzylinders um y die x-Achse
I2z=world.addParam( 'I2z', (m2*R_AP**2)/2) # Traägheit eines Vollzylinders um z die x-Achse

l_AV=world.addParam('l_AV',0.075)

m_Gelenk=world.addParam('m_Gelenk',3.4)

I_Gelenk_x=world.addParam( 'I_Gelenk_x', 0.00361)
I_Gelenk_y=world.addParam( 'I_Gelenk_y', 0.0077)
I_Gelenk_z=world.addParam( 'I_Gelenk_z', 0.00615)

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

phi_BP_1 =-pi/3 +pi/18
phi_BP_2 = pi/3 -pi/18
phi_BP_3 = phi_BP_1 + 2*pi/3
phi_BP_4 = phi_BP_2 + 2*pi/3
phi_BP_5 = phi_BP_3 + 2*pi/3
phi_BP_6 = phi_BP_4 + 2*pi/3

phi_AP_1 =  - pi/18
phi_AP_2 = phi_AP_1 + pi/9
phi_AP_3 = phi_AP_1 + 2*pi/3
phi_AP_4 = phi_AP_2 + 2*pi/3
phi_AP_5 = phi_AP_3 + 2*pi/3
phi_AP_6 = phi_AP_4 + 2*pi/3

################
#    Hexapod   #
################

#################################
#         Bodies & KS           #
#################################


#Ground = world.addBody('Ground',mass=1)
#Ground.addFrame('KS_1',p=[0,0,0])

#BP = Ground.KS_1

world.addFrame(name='BP_visual', p=[0,0,0],R=rotMat(pi/2,'x'))

world.addFrame(name='BP_Anlenkpunkt_1', p=[R1*cos(phi_BP_1),R1*sin(phi_BP_1),0],R=rotMat(0,'z')*rotMat(pi/18,'x'))
world.addFrame(name='BP_Anlenkpunkt_2', p=[R1*cos(phi_BP_2),R1*sin(phi_BP_2),0],R=rotMat(0,'z')*rotMat(pi/18,'x'))
world.addFrame(name='BP_Anlenkpunkt_3', p=[R1*cos(phi_BP_3),R1*sin(phi_BP_3),0],R=rotMat(0,'z')*rotMat(pi/18,'x'))
world.addFrame(name='BP_Anlenkpunkt_4', p=[R1*cos(phi_BP_4),R1*sin(phi_BP_4),0],R=rotMat(0,'z')*rotMat(pi/18,'x'))
world.addFrame(name='BP_Anlenkpunkt_5', p=[R1*cos(phi_BP_5),R1*sin(phi_BP_5),0],R=rotMat(0,'z')*rotMat(pi/18,'x'))
world.addFrame(name='BP_Anlenkpunkt_6', p=[R1*cos(phi_BP_6),R1*sin(phi_BP_6),0],R=rotMat(0,'z')*rotMat(pi/18,'x'))

world.addFrame('BP_Feder',p=[0,0,1.1])


################################################################################

AP = world.addBody(name='Arbeitsplattform', mass=m2,inertia=diag([I2x,I2y,I2z]))

AP.addFrame(name='AP_visual', p=[0,0,0],R=rotMat(pi/2,'x'))
AP.addFrame(name='AP_Anlenkpunkt_1', p=[R2*cos(phi_AP_1),R2*sin(phi_AP_1),0],R=rotMat(pi/2,'z'))
AP.addFrame(name='AP_Anlenkpunkt_2', p=[R2*cos(phi_AP_2),R2*sin(phi_AP_2),0],R=rotMat(pi/2,'z'))
AP.addFrame(name='AP_Anlenkpunkt_3', p=[R2*cos(phi_AP_3),R2*sin(phi_AP_3),0],R=rotMat(pi/2,'z'))
AP.addFrame(name='AP_Anlenkpunkt_4', p=[R2*cos(phi_AP_4),R2*sin(phi_AP_4),0],R=rotMat(pi/2,'z'))
AP.addFrame(name='AP_Anlenkpunkt_5', p=[R2*cos(phi_AP_5),R2*sin(phi_AP_5),0],R=rotMat(pi/2,'z'))
AP.addFrame(name='AP_Anlenkpunkt_6', p=[R2*cos(phi_AP_6),R2*sin(phi_AP_6),0],R=rotMat(pi/2,'z'))

################################################################################


Zyl_geh_1 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_1')
Zyl_geh_1.addFrame('Zyl_geh_1_visual', p=[0,0,0])
Zyl_geh_1.addFrame('Zyl_geh_1_cs', p=[0,0,0])
Zyl_geh_1.addFrame('Zyl_geh_1_cs_2', p=[0,0,0])

Zyl_geh_2 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_2')
Zyl_geh_2.addFrame('Zyl_geh_2_visual', p=[0,0,0])
Zyl_geh_2.addFrame('Zyl_geh_2_cs', p=[0,0,0])
Zyl_geh_2.addFrame('Zyl_geh_2_cs_2', p=[0,0,0])

Zyl_geh_3 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_3')
Zyl_geh_3.addFrame('Zyl_geh_3_visual', p=[0,0,0])
Zyl_geh_3.addFrame('Zyl_geh_3_cs', p=[0,0,0])
Zyl_geh_3.addFrame('Zyl_geh_3_cs_2', p=[0,0,0])

Zyl_geh_4 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_4')
Zyl_geh_4.addFrame('Zyl_geh_4_visual', p=[0,0,0])
Zyl_geh_4.addFrame('Zyl_geh_4_cs', p=[0,0,0])
Zyl_geh_4.addFrame('Zyl_geh_4_cs_2', p=[0,0,0])

Zyl_geh_5 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_5')
Zyl_geh_5.addFrame('Zyl_geh_5_visual', p=[0,0,0])
Zyl_geh_5.addFrame('Zyl_geh_5_cs', p=[0,0,0])
Zyl_geh_5.addFrame('Zyl_geh_5_cs_2', p=[0,0,0])

Zyl_geh_6 = world.addBody( mass=m_Zyl_Geh,cg=[cg_Zyl_Geh_x,0,0], inertia=diag([I_Zyl_Geh_x,I_Zyl_Geh_y,I_Zyl_Geh_z]),name='Zyl_geh_6')
Zyl_geh_6.addFrame('Zyl_geh_6_visual', p=[0,0,0])
Zyl_geh_6.addFrame('Zyl_geh_6_cs', p=[0,0,0])
Zyl_geh_6.addFrame('Zyl_geh_6_cs_2', p=[0,0,0])

################################################################################

Zyl_stange_1 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_1')
Zyl_stange_1.addFrame('Zyl_stange_1_visual', p=[0,0,0])
Zyl_stange_1.addFrame('Zyl_stange_1_cs', p=[0,0,0])
Zyl_stange_1.addFrame('Zyl_stange_1_cs_2', p=[0,0,0])

Zyl_stange_2 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_2')
Zyl_stange_2.addFrame('Zyl_stange_2_visual', p=[0,0,0])
Zyl_stange_2.addFrame('Zyl_stange_2_cs', p=[0,0,0])
Zyl_stange_2.addFrame('Zyl_stange_2_cs_2', p=[0,0,0])

Zyl_stange_3 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_3')
Zyl_stange_3.addFrame('Zyl_stange_3_visual', p=[0,0,0])
Zyl_stange_3.addFrame('Zyl_stange_3_cs', p=[0,0,0])
Zyl_stange_3.addFrame('Zyl_stange_3_cs_2', p=[0,0,0])

Zyl_stange_4 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_4')
Zyl_stange_4.addFrame('Zyl_stange_4_visual', p=[0,0,0])
Zyl_stange_4.addFrame('Zyl_stange_4_cs', p=[0,0,0])
Zyl_stange_4.addFrame('Zyl_stange_4_cs_2', p=[0,0,0])

Zyl_stange_5 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_5')
Zyl_stange_5.addFrame('Zyl_stange_5_visual', p=[0,0,0])
Zyl_stange_5.addFrame('Zyl_stange_5_cs', p=[0,0,0])
Zyl_stange_5.addFrame('Zyl_stange_5_cs_2', p=[0,0,0])

Zyl_stange_6 = world.addBody( mass=m_Zyl_Stange,cg=[cg_Zyl_Stange_x,0,0], inertia=diag([I_Zyl_Stange_x,I_Zyl_Stange_y,I_Zyl_Stange_z]),name='Zyl_stange_6')
Zyl_stange_6.addFrame('Zyl_stange_6_visual', p=[0,0,0])
Zyl_stange_6.addFrame('Zyl_stange_6_cs', p=[0,0,0])
Zyl_stange_6.addFrame('Zyl_stange_6_cs_2', p=[0,0,0])

###########################
# Gelenke mit Achsversatz #
###########################


Gelenk_1_AP=world.addBody(name='Gelenk_1_AP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_1_AP.addFrame(name='Gelenk_1_AP_visual', p=[0,0,-l_AV/2])
Gelenk_1_AP.addFrame(name='Gelenk_1_AP_CS_1', p=[0,0,-l_AV/2])
Gelenk_1_AP.addFrame(name='Gelenk_1_AP_CS_2', p=[0,0,l_AV/2])

Gelenk_1_BP=world.addBody(name='Gelenk_1_BP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_1_BP.addFrame(name='Gelenk_1_BP_visual', p=[0,0,l_AV/2])
Gelenk_1_BP.addFrame(name='Gelenk_1_BP_CS_1', p=[0,0,-l_AV/2])
Gelenk_1_BP.addFrame(name='Gelenk_1_BP_CS_2', p=[0,0,l_AV/2])

Gelenk_2_AP=world.addBody(name='Gelenk_2_AP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_2_AP.addFrame(name='Gelenk_2_AP_visual', p=[0,0,-l_AV/2])
Gelenk_2_AP.addFrame(name='Gelenk_2_AP_CS_1', p=[0,0,-l_AV/2])
Gelenk_2_AP.addFrame(name='Gelenk_2_AP_CS_2', p=[0,0,l_AV/2])

Gelenk_2_BP=world.addBody(name='Gelenk_2_BP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_2_BP.addFrame(name='Gelenk_2_BP_visual', p=[0,0,l_AV/2])
Gelenk_2_BP.addFrame(name='Gelenk_2_BP_CS_1', p=[0,0,-l_AV/2])
Gelenk_2_BP.addFrame(name='Gelenk_2_BP_CS_2', p=[0,0,l_AV/2])

Gelenk_3_AP=world.addBody(name='Gelenk_3_AP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_3_AP.addFrame(name='Gelenk_3_AP_visual', p=[0,0,-l_AV/2])
Gelenk_3_AP.addFrame(name='Gelenk_3_AP_CS_1', p=[0,0,-l_AV/2])
Gelenk_3_AP.addFrame(name='Gelenk_3_AP_CS_2', p=[0,0,l_AV/2])

Gelenk_3_BP=world.addBody(name='Gelenk_3_BP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_3_BP.addFrame(name='Gelenk_3_BP_visual', p=[0,0,l_AV/2])
Gelenk_3_BP.addFrame(name='Gelenk_3_BP_CS_1', p=[0,0,-l_AV/2])
Gelenk_3_BP.addFrame(name='Gelenk_3_BP_CS_2', p=[0,0,l_AV/2])

Gelenk_4_AP=world.addBody(name='Gelenk_4_AP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_4_AP.addFrame(name='Gelenk_4_AP_visual', p=[0,0,-l_AV/2])
Gelenk_4_AP.addFrame(name='Gelenk_4_AP_CS_1', p=[0,0,-l_AV/2])
Gelenk_4_AP.addFrame(name='Gelenk_4_AP_CS_2', p=[0,0,l_AV/2])

Gelenk_4_BP=world.addBody(name='Gelenk_4_BP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_4_BP.addFrame(name='Gelenk_4_BP_visual', p=[0,0,l_AV/2])
Gelenk_4_BP.addFrame(name='Gelenk_4_BP_CS_1', p=[0,0,-l_AV/2])
Gelenk_4_BP.addFrame(name='Gelenk_4_BP_CS_2', p=[0,0,l_AV/2])

Gelenk_5_AP=world.addBody(name='Gelenk_5_AP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_5_AP.addFrame(name='Gelenk_5_AP_visual', p=[0,0,-l_AV/2])
Gelenk_5_AP.addFrame(name='Gelenk_5_AP_CS_1', p=[0,0,-l_AV/2])
Gelenk_5_AP.addFrame(name='Gelenk_5_AP_CS_2', p=[0,0,l_AV/2])

Gelenk_5_BP=world.addBody(name='Gelenk_5_BP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_5_BP.addFrame(name='Gelenk_5_BP_visual', p=[0,0,l_AV/2])
Gelenk_5_BP.addFrame(name='Gelenk_5_BP_CS_1', p=[0,0,-l_AV/2])
Gelenk_5_BP.addFrame(name='Gelenk_5_BP_CS_2', p=[0,0,l_AV/2])

Gelenk_6_AP=world.addBody(name='Gelenk_6_AP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_6_AP.addFrame(name='Gelenk_6_AP_visual', p=[0,0,-l_AV/2])
Gelenk_6_AP.addFrame(name='Gelenk_6_AP_CS_1', p=[0,0,-l_AV/2])
Gelenk_6_AP.addFrame(name='Gelenk_6_AP_CS_2', p=[0,0,l_AV/2])

Gelenk_6_BP=world.addBody(name='Gelenk_6_BP',mass=m_Gelenk,inertia=diag([I_Gelenk_x,I_Gelenk_y,I_Gelenk_z]))
Gelenk_6_BP.addFrame(name='Gelenk_6_BP_visual', p=[0,0,l_AV/2])
Gelenk_6_BP.addFrame(name='Gelenk_6_BP_CS_1', p=[0,0,-l_AV/2])
Gelenk_6_BP.addFrame(name='Gelenk_6_BP_CS_2', p=[0,0,l_AV/2])
##########
# Joints #
##########

#world.addJoint('fix_BP', world, BP)
#world.addJoint('fix_BP', world, Ground)

jAP=world.addJoint(world, AP,['Tx', 'Ty', 'Tz','Rx', 'Ry', 'Rz'],[0,0,1,0,0,0],name='free_AP')

#world.addSensor.Joint('jAP',jAP)




Gelenk_1_BP_an_BP=world.addJoint(world.BP_Anlenkpunkt_1,Gelenk_1_BP.Gelenk_1_BP_CS_1,'Ry',name='Gelenk_1_BP_an_BP')
Zyl_geh_1_an_Gelenk_1_BP=world.addJoint(Gelenk_1_BP.Gelenk_1_BP_CS_2,Zyl_geh_1.Zyl_geh_1_cs_2, 'Rx',name='Zyl_geh_1_an_Gelenk_1_BP')
Zyl_stange_1_an_Zyl_geh_1=world.addJoint(Zyl_geh_1.Zyl_geh_1_cs,Zyl_stange_1.Zyl_stange_1_cs_2, ['Tz','Rz'],name='Zyl_stange_1_an_Zyl_geh_1')
Gelenk_1_AP_an_Zyl_stange_1=world.addJoint(Zyl_stange_1.Zyl_stange_1_cs,Gelenk_1_AP.Gelenk_1_AP_CS_1,'Rx',name='Gelenk_1_AP_an_Zyl_stange_1')

Gelenk_2_BP_an_BP=world.addJoint(world.BP_Anlenkpunkt_2,Gelenk_2_BP.Gelenk_2_BP_CS_1,'Ry',name='Gelenk_2_BP_an_BP')
Zyl_geh_2_an_Gelenk_2_BP=world.addJoint(Gelenk_2_BP.Gelenk_2_BP_CS_2,Zyl_geh_2.Zyl_geh_2_cs_2, 'Rx',name='Zyl_geh_2_an_Gelenk_2_BP')
Zyl_stange_2_an_Zyl_geh_2=world.addJoint(Zyl_geh_2.Zyl_geh_2_cs,Zyl_stange_2.Zyl_stange_2_cs_2, ['Tz','Rz'],name='Zyl_stange_2_an_Zyl_geh_2')
Gelenk_2_AP_an_Zyl_stange_2=world.addJoint(Zyl_stange_2.Zyl_stange_2_cs,Gelenk_2_AP.Gelenk_2_AP_CS_1,'Rx',name='Gelenk_2_AP_an_Zyl_stange_2')

world.addJoint(world.BP_Anlenkpunkt_3,Gelenk_3_BP.Gelenk_3_BP_CS_1,'Ry',name='Gelenk_3_BP_an_BP')
world.addJoint(Gelenk_3_BP.Gelenk_3_BP_CS_2,Zyl_geh_3.Zyl_geh_3_cs_2, 'Rx',name='Zyl_geh_3_an_Gelenk_3_BP')
world.addJoint(Zyl_geh_3.Zyl_geh_3_cs,Zyl_stange_3.Zyl_stange_3_cs_2, ['Tz','Rz'],name='Zyl_stange_3_an_Zyl_geh_3')
world.addJoint(Zyl_stange_3.Zyl_stange_3_cs,Gelenk_3_AP.Gelenk_3_AP_CS_1,'Rx',name='Gelenk_3_AP_an_Zyl_stange_3')

world.addJoint(world.BP_Anlenkpunkt_4,Gelenk_4_BP.Gelenk_4_BP_CS_1,'Ry',name='Gelenk_4_BP_an_BP')
world.addJoint(Gelenk_4_BP.Gelenk_4_BP_CS_2,Zyl_geh_4.Zyl_geh_4_cs_2, 'Rx',name='Zyl_geh_4_an_Gelenk_4_BP')
world.addJoint(Zyl_geh_4.Zyl_geh_4_cs,Zyl_stange_4.Zyl_stange_4_cs_2, ['Tz','Rz'],name='Zyl_stange_4_an_Zyl_geh_4')
world.addJoint(Zyl_stange_4.Zyl_stange_4_cs,Gelenk_4_AP.Gelenk_4_AP_CS_1,'Rx',name='Gelenk_4_AP_an_Zyl_stange_4')

world.addJoint(world.BP_Anlenkpunkt_5,Gelenk_5_BP.Gelenk_5_BP_CS_1,'Ry',name='Gelenk_5_BP_an_BP')
world.addJoint(Gelenk_5_BP.Gelenk_5_BP_CS_2,Zyl_geh_5.Zyl_geh_5_cs_2, 'Rx',name='Zyl_geh_5_an_Gelenk_5_BP')
world.addJoint(Zyl_geh_5.Zyl_geh_5_cs,Zyl_stange_5.Zyl_stange_5_cs_2, ['Tz','Rz'],name='Zyl_stange_5_an_Zyl_geh_5')
world.addJoint(Zyl_stange_5.Zyl_stange_5_cs,Gelenk_5_AP.Gelenk_5_AP_CS_1,'Rx',name='Gelenk_5_AP_an_Zyl_stange_5')

world.addJoint(world.BP_Anlenkpunkt_6,Gelenk_6_BP.Gelenk_6_BP_CS_1,'Ry',name='Gelenk_6_BP_an_BP')
world.addJoint(Gelenk_6_BP.Gelenk_6_BP_CS_2,Zyl_geh_6.Zyl_geh_6_cs_2, 'Rx',name='Zyl_geh_6_an_Gelenk_6_BP')
world.addJoint(Zyl_geh_6.Zyl_geh_6_cs,Zyl_stange_6.Zyl_stange_6_cs_2, ['Tz','Rz'],name='Zyl_stange_6_an_Zyl_geh_6')
world.addJoint(Zyl_stange_6.Zyl_stange_6_cs,Gelenk_6_AP.Gelenk_6_AP_CS_1,'Rx',name='Gelenk_6_AP_an_Zyl_stange_6')


########################
# Constraints or Loops #
########################

world.addLoop.Hexapod_m_AV(AP.AP_Anlenkpunkt_1, Gelenk_1_AP.Gelenk_1_AP_CS_2, 'Verbindung_1')
world.addLoop.Hexapod_m_AV(AP.AP_Anlenkpunkt_2, Gelenk_2_AP.Gelenk_2_AP_CS_2, 'Verbindung_2')
world.addLoop.Hexapod_m_AV(AP.AP_Anlenkpunkt_3, Gelenk_3_AP.Gelenk_3_AP_CS_2, 'Verbindung_3')
world.addLoop.Hexapod_m_AV(AP.AP_Anlenkpunkt_4, Gelenk_4_AP.Gelenk_4_AP_CS_2, 'Verbindung_4')
world.addLoop.Hexapod_m_AV(AP.AP_Anlenkpunkt_5, Gelenk_5_AP.Gelenk_5_AP_CS_2, 'Verbindung_5')
world.addLoop.Hexapod_m_AV(AP.AP_Anlenkpunkt_6, Gelenk_6_AP.Gelenk_6_AP_CS_2, 'Verbindung_6')

#####################
# add visualisation #
#####################

world.addVisualisation.Cylinder(world.BP_visual,R_BP, hoehe)
world.addVisualisation.Cylinder(AP.AP_visual,R_AP, hoehe)


world.addVisualisation.Cylinder(Zyl_geh_1.Zyl_geh_1_visual, 0.08,0.6)
world.addVisualisation.Cylinder(Zyl_geh_2.Zyl_geh_2_visual, 0.08,0.6)
world.addVisualisation.Cylinder(Zyl_geh_3.Zyl_geh_3_visual, 0.08,0.6)
world.addVisualisation.Cylinder(Zyl_geh_4.Zyl_geh_4_visual, 0.08,0.6)
world.addVisualisation.Cylinder(Zyl_geh_5.Zyl_geh_5_visual, 0.08,0.6)
world.addVisualisation.Cylinder(Zyl_geh_6.Zyl_geh_6_visual, 0.08,0.6)


world.addVisualisation.Cylinder(Zyl_stange_1.Zyl_stange_1_visual, 0.04,0.6)
world.addVisualisation.Cylinder(Zyl_stange_2.Zyl_stange_2_visual, 0.04,0.6)
world.addVisualisation.Cylinder(Zyl_stange_3.Zyl_stange_3_visual, 0.04,0.6)
world.addVisualisation.Cylinder(Zyl_stange_4.Zyl_stange_4_visual, 0.04,0.6)
world.addVisualisation.Cylinder(Zyl_stange_5.Zyl_stange_5_visual, 0.04,0.6)
world.addVisualisation.Cylinder(Zyl_stange_6.Zyl_stange_6_visual, 0.04,0.6)

world.addVisualisation.Box(Gelenk_1_BP, 0.05,0.05,0.075)
world.addVisualisation.Box(Gelenk_1_AP, 0.05,0.05,0.075)

world.addVisualisation.Box(Gelenk_2_BP, 0.05,0.05,0.075)
world.addVisualisation.Box(Gelenk_2_AP, 0.05,0.05,0.075)

world.addVisualisation.Box(Gelenk_3_BP, 0.05,0.05,0.075)
world.addVisualisation.Box(Gelenk_3_AP, 0.05,0.05,0.075)

world.addVisualisation.Box(Gelenk_4_BP, 0.05,0.05,0.075)
world.addVisualisation.Box(Gelenk_4_AP, 0.05,0.05,0.075)

world.addVisualisation.Box(Gelenk_5_BP, 0.05,0.05,0.075)
world.addVisualisation.Box(Gelenk_5_AP, 0.05,0.05,0.075)

world.addVisualisation.Box(Gelenk_6_BP, 0.05,0.05,0.075)
world.addVisualisation.Box(Gelenk_6_AP, 0.05,0.05,0.075)




#####################
# add visualisation #
#####################



print("System has been assembled")



###############
# add Sensors #
###############

world.addHydCylSensorsLoads('HP_Zylinder1', Gelenk_1_BP.Gelenk_1_BP_CS_2,Gelenk_1_AP.Gelenk_1_AP_CS_1)
world.addHydCylSensorsLoads('HP_Zylinder2', Gelenk_2_BP.Gelenk_2_BP_CS_2,Gelenk_2_AP.Gelenk_2_AP_CS_1)
world.addHydCylSensorsLoads('HP_Zylinder3', Gelenk_3_BP.Gelenk_3_BP_CS_2,Gelenk_3_AP.Gelenk_3_AP_CS_1)
world.addHydCylSensorsLoads('HP_Zylinder4', Gelenk_4_BP.Gelenk_4_BP_CS_2,Gelenk_4_AP.Gelenk_4_AP_CS_1)
world.addHydCylSensorsLoads('HP_Zylinder5', Gelenk_5_BP.Gelenk_5_BP_CS_2,Gelenk_5_AP.Gelenk_5_AP_CS_1)
world.addHydCylSensorsLoads('HP_Zylinder6', Gelenk_6_BP.Gelenk_6_BP_CS_2,Gelenk_6_AP.Gelenk_6_AP_CS_1)


#world.addSensor.Position(world,AP.AP_Anlenkpunkt_1,"P_AP_1")

'''
world.addSensor.ConstraintForce(symbol='F2_1',joint=Gelenk_2_BP_an_BP,name='Sensor_1_Gelenk_2')
world.addSensor.ConstraintForce(symbol='F2_2',joint=Zyl_geh_2_an_Gelenk_2_BP,name='Sensor_2_Gelenk_2')
world.addSensor.ConstraintForce(symbol='F2_3',joint=Zyl_stange_2_an_Zyl_geh_2[0],name='Sensor_3_Gelenk_2')
world.addSensor.ConstraintForce(symbol='F2_4',joint=Zyl_stange_2_an_Zyl_geh_2[1],name='Sensor_4_Gelenk_2')
world.addSensor.ConstraintForce(symbol='F2_5',joint=Gelenk_2_AP_an_Zyl_stange_2,name='Sensor_5_Gelenk_2')
'''

#world.addSensor.Energy(symbol_str='E_ges',body=Gelenk_1_AP,name='Sensor_E_ges')


#####################
# add Imput & Load  #
#####################
'''
l = world.addSensor.Distance( 'L',world.BP_Feder,AP, name='DistanceSensor')

l1 = world.addSensor.Distance( 'L1',Zyl_geh_1.Zyl_geh_1_cs_2,Zyl_stange_1.Zyl_stange_1_cs, name='DistanceSensor_Zyl_1')
l2 = world.addSensor.Distance( 'L2',Zyl_geh_2.Zyl_geh_2_cs_2,Zyl_stange_2.Zyl_stange_2_cs, name='DistanceSensor_Zyl_2')
l3 = world.addSensor.Distance( 'L3',Zyl_geh_3.Zyl_geh_3_cs_2,Zyl_stange_3.Zyl_stange_3_cs, name='DistanceSensor_Zyl_3')
l4 = world.addSensor.Distance( 'L4',Zyl_geh_4.Zyl_geh_4_cs_2,Zyl_stange_4.Zyl_stange_4_cs, name='DistanceSensor_Zyl_4')
l5 = world.addSensor.Distance( 'L5',Zyl_geh_5.Zyl_geh_5_cs_2,Zyl_stange_5.Zyl_stange_5_cs, name='DistanceSensor_Zyl_5')
l6 = world.addSensor.Distance( 'L6',Zyl_geh_6.Zyl_geh_6_cs_2,Zyl_stange_6.Zyl_stange_6_cs, name='DistanceSensor_Zyl_6')
#lz = world.addSensor.Distance(BP,AP, 'lz', 'DistanceSensor_Cylinder')
'''
#c=100
'''
F_c = world.addExpression('F_c', -c*l[0],name='SpringForce')

F_c_1 = world.addExpression('F_c_1', -c1*l1[0],name='SpringForce_1')
F_c_2 = world.addExpression('F_c_2', -c1*l2[0],name='SpringForce_2')
F_c_3 = world.addExpression('F_c_3', -c1*l3[0],name='SpringForce_3')
F_c_4 = world.addExpression('F_c_4', -c1*l4[0],name='SpringForce_4')
F_c_5 = world.addExpression('F_c_5', -c1*l5[0],name='SpringForce_5')
F_c_6 = world.addExpression('F_c_6', -c1*l6[0],name='SpringForce_6')
'''
#world.addLoad.PtPForce(F_c,world.BP_Feder,AP,  name='Spring')
#world.addLoad.CmpForce(world.BP_Feder,AP,[0,0,F_c], name='Spring')
'''
world.addLoad.PtPForce(F_c_1,Zyl_geh_1.Zyl_geh_1_cs_2,Zyl_stange_1.Zyl_stange_1_cs,  name='Spring_1')
world.addLoad.PtPForce(F_c_2,Zyl_geh_2.Zyl_geh_2_cs_2,Zyl_stange_2.Zyl_stange_2_cs,  name='Spring_2')
world.addLoad.PtPForce(F_c_3,Zyl_geh_3.Zyl_geh_3_cs_2,Zyl_stange_3.Zyl_stange_3_cs,  name='Spring_3')
world.addLoad.PtPForce(F_c_4,Zyl_geh_4.Zyl_geh_4_cs_2,Zyl_stange_4.Zyl_stange_4_cs,  name='Spring_4')
world.addLoad.PtPForce(F_c_5,Zyl_geh_5.Zyl_geh_5_cs_2,Zyl_stange_5.Zyl_stange_5_cs,  name='Spring_5')
world.addLoad.PtPForce(F_c_6,Zyl_geh_6.Zyl_geh_6_cs_2,Zyl_stange_6.Zyl_stange_6_cs,  name='Spring_6')

'''

#################################
# generate equations & sim Code #
#################################

world.genEquations.Recursive()
#world.genEquations(explicit=False)
#world.genEquations(explicit=False, diff=[0,1,2])
#world.genCode.Modelica('hexapod_z_m_AV_kpl','.\HP_Output',inputsAsInputs=False, debugMode=False)


#world.show('hexapod_z_m_AV')
