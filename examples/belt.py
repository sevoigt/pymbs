"""
Procedurally generate a planar belt. The belt segments have a longitudinal
and bending stiffness. They are each connected by a 1D rotational joint and
a 1D translational joint.

This could also serve as chain or rope model if the stiffnesses are adjusted
accordingly.
"""

import time

from pymbs.input import *

world = MbsSystem([0, 0, -1])


pitch = 0.13 # pitch (length of a segment)
nseg = 16   # number of segments

c = 120000   # [N/m] translational stiffness
d =  10000   # [Ns/m] translational damping

c_r =  200  # [Nm/rad] rotational stiffness
d_r =  150  # [Nms/rad] rotational damping

ml = 15     # [kg/m] mass per length


# Initialize lists
bodies = [None]*nseg
joints_Tx = [None]*nseg
joints_Ry = [None]*nseg

# Parameters
m = ml * pitch
I = 1/12*m*pitch**2*diag([1, 1, 1])

# Create bodies and connect them
for i in range(0, nseg):
    # Create Body and Frame
    bodies[i] = world.addBody(m, cg=[pitch/2, 0, 0], inertia=I)
    bodies[i].addFrame(name='end', p=[pitch, 0, 0])

    # Create joints
    if (i==0):
        joints_Ry[i] = world.addJoint(world, bodies[i], ['Ry'])        
    else:
        joints_Ry[i], joints_Tx[i] = world.addJoint(bodies[i-1].end, bodies[i], ['Ry', 'Tx'])
        #joints_Ry[i] = world.addJoint(bodies[i-1].end, bodies[i], 'Ry')

    world.addVisualisation.Line(bodies[i], pitch)

    # Add force    
    if i>0:
        dx = world.addSensor.Joint(symbol=f'tx_{i}', joint=joints_Tx[i])
        f = world.addExpression(symbol_str=f'force_{i}', exp= -c*dx[0] - d*dx[1])
        world.addLoad.Joint(joint=joints_Tx[i], symbol=f)
    
    # Add torque
    dphi = world.addSensor.Joint(symbol = f'ry_{i}', joint=joints_Ry[i])
    t = world.addExpression(symbol_str=f'tau_{i}', exp= -c_r*dphi[0] - d_r*dphi[1])
    world.addLoad.Joint(joint=joints_Ry[i], symbol=t)


# Pin other end to world with loose spring
world.addFrame('pin', p=[nseg*pitch, 0, 0])
dist = world.addSensor.Distance(symbol='dist', CS1=world.pin, CS2=bodies[-1].end)
fd = world.addExpression(symbol_str='f_dist', exp= -100*dist[0] - 10*dist[1])
world.addLoad.PtPForce(symbol=fd, CS1=world.pin, CS2=bodies[-1].end)

t = time.time()

world.genEquations.Recursive()
print('Time needed for generating equations: %5.2f s' % (time.time() - t))

world.show('belt')
