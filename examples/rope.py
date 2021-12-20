"""
Procedurally generate a planar rope. The rope segments are ideally stiff
and connected by 1D rotational joints.
"""

# Warning: The source code of the examples is quoted in the documentation. If
# you change this file, you'll have to change the corresponding file in the
# documentation (see doc/examples).

import time

from pymbs.input import *

world = MbsSystem([0, 0, -1])

L = 3.0     # Total Length
d = 0.2     # Rotational Damping
rho = 500.0 # 500 kg/m^3
r = 0.1     # radius
segments = 15

# Choose between "Explicit", "OrderN" and "Recursive"
method = "Recursive"

# Initialize lists
bodies = [None]*segments
joints = [None]*segments

# Parameters
l = L/segments
m = rho*pi*r**2*l
I = 1/12*m*l**2*diag([1, 1, 1])

# Create bodies and connect them
for i in range(0, segments):
    # Create Body and Frame
    bodies[i] = world.addBody(m, cg=[l/2, 0, 0], inertia=I)
    bodies[i].addFrame(name='end', p=[l, 0, 0])

    # Create joints
    if (i==0):
        joints[i] = world.addJoint(world, bodies[i], 'Ry')
    else:
        joints[i] = world.addJoint(bodies[i-1].end, bodies[i], 'Ry')

    world.addVisualisation.Line(bodies[i], l)

    # Add damping
    s = world.addSensor.Joint(symbol='state%i_'%i, joint=joints[i])
    D = world.addExpression(symbol_str='T%i_'%i, exp=-d*s[1])
    world.addLoad.Joint(joint=joints[i], symbol=D)

t = time.time()
if method == "OrderN":
    world.genEquations.OrderN()
elif method == "Recursive":
    world.genEquations.Recursive()
elif method == "Explicit":
    world.genEquations.Explicit()
print("Time needed for generating equations: %5.2f s" % (time.time() - t))

world.show('Rope')
