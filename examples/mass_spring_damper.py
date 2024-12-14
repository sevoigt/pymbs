"""
Model for a 1D mass attached to a parallel spring-damper.
"""

# Warning: The source code of the examples is quoted in the documentation. If
# you change this file, you'll have to change the corresponding file in the
# documentation (see doc/examples).

# set up PyMbs and the multi-body system
from pymbs.input import MbsSystem

world = MbsSystem([0, 0, -1])

# Add parameters
m = world.addParam('m', 1)
c = world.addParam('c', 10)
d = world.addParam('d', 0.2)

# Add the body and constrain its movement
movingBody = world.addBody(m)
world.addJoint(world, movingBody, 'Tz', startVals=1)

# Add force to simulate a spring-damper combination
l = world.addSensor.Distance('l', movingBody, world)
F = world.addExpression('F', -(c * l[0] + d * l[1]))
world.addLoad.PtPForce(F, movingBody, world)

# Add visualisations
world.addVisualisation.Box(movingBody, 1, 1, 1)
world.addVisualisation.Frame(world)

# Generate equations and show the system
world.genEquations.Recursive()
world.show('mass_spring_damper')
