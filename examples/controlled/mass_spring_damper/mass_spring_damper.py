"""
Damped oscillator

Single mass being moved like it is connected to a spring and a damper by a
force calculated from sensor data in `controller.py`. This is basically the
same as `examples/mass_spring_damper`, but using the `addController` method
instead of `addExpression`, which is much more flexible due to allowing to
execute arbitrary python code.
"""

from pymbs.input import *
from controller import controlForce

world=MbsSystem([0,0,-1])

m = world.addParam('m', 1.0)

mass = world.addBody(mass = m)

world.addJoint(world, mass, 'Tx', startVals=1)

# add controller
F = world.addController('F', controlForce, shape=(3, ))
world.addLoad.CmpForce(F, world, mass)

world.addVisualisation.Box(mass)
x = world.addSensor.Position('x', world, mass)
v = world.addSensor.Velocity('v', world, mass)

# Generate equations of motion and show model
world.genEquations.Recursive()
world.show('MassSpringDamper')
