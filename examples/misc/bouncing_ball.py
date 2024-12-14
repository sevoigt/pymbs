"""
Classic bouncing ball example
"""

from pymbs.input import MbsSystem
from pymbs.symbolics import If, Less

world = MbsSystem([0, 0, -1])

r = 0.1     # radius of the ball
c = 500     # contact stiffness
d = 1.0     # contact damping

ball = world.addBody(mass=1, cg=[0,0,0], inertia=[r**2,0,r**2,0,0,r**2], name='ball')
ball.addFrame('contact', [0,0,-r])
world.addJoint(world, ball, 'Tz', 1)

x = world.addSensor.Position('d', world, ball.contact)
v = world.addSensor.Velocity('v', world, ball.contact)
Fc = world.addExpression('Fc', If(Less(x[2], 0), -x[2]*c - v[2]*d, 0))
world.addLoad.PtPForce(Fc, ball, world)

world.addVisualisation.Sphere(ball, r)
world.addVisualisation.Box(world, 1, 1, 0.01)

world.genEquations.Explicit()
world.show('BouncingBall')
