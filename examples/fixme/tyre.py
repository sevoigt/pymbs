# -*- coding: utf-8 -*-
'''
Created on 15.03.2012

@author: TU-Dresden - Jens Frenkel

ftv.py
'''

import sys

from PyMbs.Input import *
from PyMbs.Symbolics import Matrix, transpose, asin, Less, If
from PyMbs.Common.Functions import norm, skew
import math

''' Definitions '''

teilchenzahl = 30
radius = 1


rimjointmode = 3
# 0  fixed
# 1  Tx,Ty,Tz,Rx,Ry,Rz
# 2  Tz,Ry
# 3  Tx,Tz,Ry

''' Model '''

# World Object anlegen mit Richtung der Anziehungskraft in -Z-Achse
world = MbsSystem(gravity_vect=[0,0,-1])

''' SpringConstants '''
c_r = world.addParam(symbol_str='c_r',defaultValue=100)
d_r = world.addParam(symbol_str='d_r',defaultValue=3)

c_t = world.addParam(symbol_str='c_t',defaultValue=100)
d_t = world.addParam(symbol_str='d_t',defaultValue=3)

c_p = world.addParam(symbol_str='c_p',defaultValue=100)
d_p = world.addParam(symbol_str='d_p',defaultValue=3)

c_pp = world.addParam(symbol_str='c_pp',defaultValue=10)

c_w = world.addParam(symbol_str='c_w',defaultValue=1000)
d_w = world.addParam(symbol_str='d_w',defaultValue=30)
d_x = world.addParam(symbol_str='d_x',defaultValue=1.5)


''' Rim '''
rim = world.addBody(mass=1, inertia=[1.0,0.0,1.0,0.0,0.0,1.0], cg=[0.0,0.0,0.0],name='Rim')
rim.addFrame(name='Nullsystem')
rim.addFrame(name='KS_Hub', p=[0,0,0])
rim.addFrame(name='KS_Vis', p=[0,0,0],R=rotMat(math.pi/2.0,'y'))

world.addVisualisation.Cylinder(rim.KS_Vis,name='Rim_VIS',radius=radius/2.0, height=0.2)


''' World - Rim '''
if (rimjointmode==1):
    J_World_Rim = world.addJoint(name='World_Rim_6DOF', CS1=world, CS2=rim.Nullsystem, dofList=['Tx','Ty','Tz','Rx','Ry','Rz'], startVals=[0,0,0,0,0,0])
if (rimjointmode==2):
    J_World_Rim = world.addJoint(name='World_Rim_2DOF', CS1=world, CS2=rim.Nullsystem, dofList=['Tz','Ry'], startVals=[2.0*radius,0])
if (rimjointmode==3):
    J_World_Rim = world.addJoint(name='World_Rim_3DOF', CS1=world, CS2=rim.Nullsystem, dofList=['Tx','Tz','Ry'], startVals=[0,2.0*radius,0])
else:
    J_World_Rim = world.addJoint(name='World_Rim_Fixed', CS1=world, CS2=rim.Nullsystem, dofList=[])



phidelta = 2*math.pi/float(teilchenzahl)
gamma_0 = (math.pi*(float(teilchenzahl)-2))/float(teilchenzahl);
first = None
previos = None

x1 = math.sin(phidelta*0)*radius
z1 = math.cos(phidelta*0)*radius
x2 = math.sin(phidelta*1)*radius
z2 = math.cos(phidelta*1)*radius
d = math.sqrt((x2-x1)**2 + (z2-z1)**2)

particles = list()

for i in range(teilchenzahl):

    #x = math.sin(phidelta*i)*radius
    #z = math.cos(phidelta*i)*radius

    ''' Particle '''
    Particle = world.addBody(mass=0.1, inertia=[0.1,0.0,0.1,0.0,0.0,0.1], cg=[0.0,0.0,0.0],name='Particle_%s'%i)
    Particle.addFrame(name='Nullsystem')
    particles.append(Particle)

    world.addVisualisation.Sphere(cs=Particle.Nullsystem,radius=radius/10.0,name='Particle_Vis_%s'%i)

    # Rim - Particle
    cs = rim.addFrame(name='KS_P_%s'%i, p=[0,0,0],R=rotMat(phidelta*i,'y'))
    sval = radius
    if (True):
     cs = cs.addFrame(name='KS_P__%s'%i, p=[radius,0,0])
     sval = 0
    #world.addVisualisation.Frame(cs,size=0.2)
    J_Rim_Particle = world.addJoint(name='Rim_Particle_%s'%i, CS1=cs, CS2=Particle.Nullsystem, dofList=['Tx','Tz'], startVals=[sval,0])

    ''' Forces - Rim - Particle '''
    s = world.addSensor.Position(symbol='S_Rim_P%s'%i, CS1=cs, CS2=Particle.Nullsystem)
    ds = world.addSensor.Velocity(symbol='dS_Rim_P%s'%i, CS1=cs, CS2=Particle.Nullsystem)
    F=world.addExpression(symbol_str='F_Rim_P%s'%i,exp=Matrix([c_r*(s[0]-radius)+d_r*ds[0],0.0,c_t*s[2]+d_t*ds[2]]),name='F_Rim_P%s'%i)
    world.addLoad.CmpForce(symbol=F, CS1=cs, CS2=Particle.Nullsystem)

    ''' Forces - Particle - Particle '''

    if (first is None):
        first = Particle.Nullsystem
    else:
        s = world.addSensor.Distance(symbol='S_P%s_P%s'%(i-1,i), CS1=previos, CS2=Particle.Nullsystem)
        Fp=world.addExpression(symbol_str='F_P%s_P%s'%(i-1,i),exp=c_p*(d-s[0])-d_p*s[1],name='F_P%s_P%s'%(i-1,i))
        world.addLoad.PtPForce(symbol=Fp, CS1=previos, CS2=Particle.Nullsystem)

    previos = Particle.Nullsystem

    ''' Forces - Particle - World '''
    s = world.addSensor.Position(symbol='r_P%s'%i, CS1=world, CS2=Particle.Nullsystem)
    v = world.addSensor.Velocity(symbol='v_P%s'%i, CS1=world, CS2=Particle.Nullsystem)

    Fz=world.addExpression(symbol_str='F_Pz%s'%(i),exp=If(Less(s[2],0.0),c_w*s[2]+d_w*v[2],0),name='F_P%s'%(i))
    Fx=world.addExpression(symbol_str='F_Px%s'%(i),exp=-d_x*v[0]*Fz,name='F_P%s'%(i))
    Fw=world.addExpression(symbol_str='Fw_P%s'%i,exp=Matrix([Fx,0.0,Fz]),name='Fw_P%s'%i)
    world.addLoad.CmpForce(symbol=Fw, CS1=world, CS2=Particle.Nullsystem)


''' first with last '''
s = world.addSensor.Distance(symbol='S_P%s_P%s'%(0,i), CS1=first, CS2=previos)
Fp=world.addExpression(symbol_str='F_P%s_P%s'%(0,i),exp=c_p*(d-s[0])-d_p*s[1],name='F_P%s_P%s'%(0,i))
world.addLoad.PtPForce(symbol=Fp, CS1=first, CS2=previos)


''' Sensoren fuer Drehfedern '''

for i in range(1,len(particles)-1):
    p_0 = particles[i-1]
    p  = particles[i]
    p_1 = particles[i+1]
    r_1 = world.addSensor.Position(symbol='r_P%s_1'%i, CS1=p.Nullsystem, CS2=p_0.Nullsystem)
    r_2 = world.addSensor.Position(symbol='r_P%s_2'%i, CS1=p.Nullsystem, CS2=p_1.Nullsystem)
    gamma=world.addExpression(symbol_str='gamma%s'%i,exp=asin(norm(skew(r_1)*r_2)/(norm(r_1)*norm(r_2))),name='gamma%s'%i)
    F=world.addExpression(symbol_str='F_Rot_P%s'%i,exp=Matrix([c_pp*(gamma_0-gamma),0.0,0.0]),name='F_Rot_P%s'%i)
    world.addLoad.CmpForce(symbol=F, CS1=p.Nullsystem, CS2=p_0.Nullsystem)
    world.addLoad.CmpForce(symbol=F, CS1=p.Nullsystem, CS2=p_1.Nullsystem)

p_0 = particles[len(particles)-1]
p  = particles[0]
p_1 = particles[1]
r_1 = world.addSensor.Position(symbol='r_P0_1', CS1=p.Nullsystem, CS2=p_0.Nullsystem)
r_2 = world.addSensor.Position(symbol='r_P0_2', CS1=p.Nullsystem, CS2=p_1.Nullsystem)
gamma=world.addExpression(symbol_str='gamma0',exp=asin(norm(skew(r_1)*r_2)/(norm(r_1)*norm(r_2))),name='gamma0')
F=world.addExpression(symbol_str='F_Rot_P0',exp=Matrix([c_pp*(gamma_0-gamma),0.0,0.0]),name='F_Rot_P0')
world.addLoad.CmpForce(symbol=F, CS1=p.Nullsystem, CS2=p_0.Nullsystem)
world.addLoad.CmpForce(symbol=F, CS1=p.Nullsystem, CS2=p_1.Nullsystem)

p_0 = particles[len(particles)-2]
p  = particles[len(particles)-1]
p_1 = particles[0]
r_1 = world.addSensor.Position(symbol='r_P%s_1'%(len(particles)-1), CS1=p.Nullsystem, CS2=p_0.Nullsystem)
r_2 = world.addSensor.Position(symbol='r_P%s_2'%(len(particles)-1), CS1=p.Nullsystem, CS2=p_1.Nullsystem)
gamma=world.addExpression(symbol_str='gamma%s'%(len(particles)-1),exp=asin(norm(skew(r_1)*r_2)/(norm(r_1)*norm(r_2))),name='gamma%s'%(len(particles)-1))
F=world.addExpression(symbol_str='F_Rot_P%s'%(len(particles)-1),exp=Matrix([c_pp*(gamma_0-gamma),0.0,0.0]),name='F_Rot_P%s'%(len(particles)-1))
world.addLoad.CmpForce(symbol=F, CS1=p.Nullsystem, CS2=p_0.Nullsystem)
world.addLoad.CmpForce(symbol=F, CS1=p.Nullsystem, CS2=p_1.Nullsystem)


''' Gleichungen generieren '''
#world.genEquations.Explicit(kinematicsOnly=False,graphOptimizations=True)
world.genEquations.Recursive(kinematicsOnly=False,graphOptimizations=True)

modelName = 'Rad_%s'%teilchenzahl

# generate simulation code
world.genCode.Modelica(modelname=modelName,dirname='./Output',genVisualFile='Rad_%s_Con'%teilchenzahl)

# show system
#world.genMatlabAnimation(modelName)
world.show(modelName)
print("Fertig")

