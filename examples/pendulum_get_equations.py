"""
Shows how to get access to the symbolic equations of motion and to the
symbolic expressions for the sensors.
"""

# Set up PyMbs and the multibody-system
from pymbs.input import *

world = MbsSystem([0, 0, -1])

# Add parameters
m = world.addParam('m', 1)
c = world.addParam('c', 10)
d = world.addParam('d', 0.2)

# Add the body and constrain its movement
movingBody = world.addBody(m)
movingBody2 = world.addBody(m)
J1 = world.addJoint(world, movingBody, 'Tz', startVals=1)

# Add force to simulate a spring-damper combination
l = world.addSensor.Distance('l', movingBody, world)
F = world.addExpression('F', -(c*l[0] + d*l[1]))
world.addLoad.PtPForce(F, movingBody, world)

# Add visualisations
world.addVisualisation.Box(movingBody, 1, 1, 1)
world.addVisualisation.Frame(world)

# Generate equations
world.genEquations.Recursive()



# -----------------------------------------------------------------------------
# Get the  symbols of the minimal coordinates and their first two time
# derivatives. After calling genEquations(), the joint-objects have the
# corresponding symbols set as attributes:

q = list()
qd = list()
qdd = list()
for joint in list(world.jointDict.values()):
    if (joint.IsIndependent):
        q.append(joint.coord)
        qd.append(joint.vel)
        qdd.append(joint.acc)


# Provides a list of symbolic expressions that can be used to calculate
# qdd from given (q, qd). The goal here is to convert these expressions
# to sympy, e.g. for linearisation.

eqns_mo = world.getMotionEquations()

print('--- getMotionEquations ---')
for eqn in eqns_mo:
    print(eqn.lhs, ' := ', eqn.rhs)


# (optional) provides a list of symbolic expressions to calculate all
# sensor values for give q, qd

eqns_sens = world.getSensorEquations()

print('--- getSensorEquations ---')
for eqn in eqns_sens:
    print(eqn.lhs, ' := ', eqn.rhs)

# -----------------------------------------------------------------------------

world.show('pendulum')
