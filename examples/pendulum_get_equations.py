# -*- coding: utf-8 -*-

"""
Dieses Skript verdeutlich wie Carsten sich den Zugang zu den
Bewegungsgleichungen und ggf. anderen relevanten Gleichungen vorstellt

"""



# set up PyMbs and the multi-body system
from PyMbs.Input import *
world = MbsSystem([0, 0, -1])

# add parameters
m = world.addParam('m', 1)
c = world.addParam('c', 10)
d = world.addParam('d', 0.2)

# add the body and constrain its movement
movingBody = world.addBody(m)
movingBody2 = world.addBody(m)
J1 = world.addJoint(world, movingBody, 'Tz', startVals=1)

# nur zum Test
#J2 = world.addJoint(world, movingBody2, ['Tx', 'Ty'], startVals=[3, 4])
#from IPython import embed as IPS
#IPS()


# add force to simulate a spring-damper combination
l = world.addSensor.Distance('l', movingBody, world)
F = world.addExpression('F', -(c*l[0] + d*l[1]))
world.addLoad.PtPForce(F, movingBody, world)

# add visualisations
world.addVisualisation.Box(movingBody, 1, 1, 1)
world.addVisualisation.Frame(world)

# generate equations
world.genEquations.Recursive()



#############################################################################
# liefert die Symbole der (minimalen) Koordinaten, und der zugehörigen ersten zwei Zeitableitungen
# Die Joint-Objekte haben nach der Ausführung von genEquations die zugehörigen Symbole als Attribute gesetzt:
q = list()
qd = list()
qdd = list()
for joint in list(world.jointDict.values()):
    if (joint.IsIndependent):
        q.append(joint.coord)
        qd.append(joint.vel)
        qdd.append(joint.acc)


eqns_mo = world.getMotionEquations()
# liefert eine Liste von symbolischen Ausdrücken aus denen man qdd für gegebene q, qd berechnen kann
# Mein Ziel ist es, diese Ausdrücke nach sympy zu konvertieren und zum Beispiel auch zu linearisieren
print('--- getMotionEquations ---')
for eqn in eqns_mo:
    print(eqn.lhs, ' := ', eqn.rhs)

# (optional)
eqns_sens = world.getSensorEquations()
print('--- getSensorEquations ---')
for eqn in eqns_sens:
    print(eqn.lhs, ' := ', eqn.rhs)

# liefert eine Liste von symbolischen Ausdrücken aus denen man jeden Sensor-Wert für gegebene q, qd Werte berechnen kann


#############################################################################


world.show('Pendulum')