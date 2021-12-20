"""
Model of a planar rope attaced to a sliding mass
"""

from pymbs.input import *
from controller import controlForce

world = MbsSystem([0, 0, -1])

L = 3.0     # total length
d = 0.2     # rotational damping
rho = 500.0 # 500 kg/m^3
r = 0.1     # radius
segments = 5

# Initialize lists
bodies = [None]*segments
joints = [None]*segments

# Parameters
l = L/segments
m = rho*pi*r**2*l
I = 1/12*m*l**2*diag([1, 1, 1])

# Add a trolley
trolley = world.addBody(m)
world.addVisualisation.Box(trolley, 0.1, 0.1, 0.1)
world.addJoint(world, trolley, 'Tx')

# Add controller
F = world.addController('F', controlForce, shape=(3, ))
world.addLoad.CmpForce(F, trolley, world, name='DrivingForce')

x = world.addSensor.Distance('x', world, trolley)

# Create rope segments and connect them
for i in range(0, segments):
    # Create body and frame
    bodies[i] = world.addBody(m, cg=[l/2, 0, 0], inertia=I)
    bodies[i].addFrame(name='end', p=[l, 0, 0])

    # Create joints
    if (i==0):
        joints[i] = world.addJoint(trolley, bodies[i], 'Ry')
    else:
        joints[i] = world.addJoint(bodies[i-1].end, bodies[i], 'Ry')

    world.addVisualisation.Line(bodies[i], l)

    # Add damping
    if (i==0):
        # Name first sensor alpha0 for use in controller
        s = world.addSensor.Joint(symbol='alpha0', joint=joints[i])
    elif (i==segments-1):
        s = world.addSensor.Joint(symbol='alphaN', joint=joints[i])
        # Add sensor for the position of the end tip of the rope
        pos = world.addSensor.Position('position', world, bodies[i])
    else:
        s = world.addSensor.Joint(symbol='state%i_'%i, joint=joints[i])
    D = world.addExpression(symbol_str='T%i_'%i, exp=-d*s[1])
    world.addLoad.Joint(joint=joints[i], symbol=D)

world.genEquations.Recursive()

world.show('Rope')
