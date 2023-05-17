"""
Procedurally generate a planar belt. The belt segments have a longitudinal
and bending stiffness. They are each connected by a 1D rotational joint and
a 1D translational joint.

This could also serve as chain or rope model if the stiffnesses are adjusted
accordingly.

This model is also used as benchmark. The original model with 16 segments
was created in SimulationX 4.0 and all parameters have been taken from there. 
"""

import time

from pymbs.input import *

world = MbsSystem([0, 0, -1])


pitch = 0.131  # [m] pitch (length of a segment)
mass = 15.943      # [kg] mass per length
nseg = 16      # number of segments

c = 120e6   # [N/m] translational stiffness
d =   1e6   # [Ns/m] translational damping

c_r = 1000  # [Nm/rad] rotational stiffness
d_r = 1500  # [Nms/rad] rotational damping



# Initialize lists
bodies = [None]*nseg
joints_Tx = [None]*nseg
joints_Ry = [None]*nseg

# Parameters
I = diag([2.2896, 0.0194 + mass*pitch**2, 2.2896])

# Create bodies and connect them
for i in range(0, nseg):
    # Create Body and Frame
    bodies[i] = world.addBody(mass, cg=[pitch, 0, -0.037], inertia=I)
    bodies[i].addFrame(name='end', p=[pitch, 0, 0])

    # Create joints
    if (i==0):
        joints_Ry[i], joints_Tx[i] = world.addJoint(world, bodies[i], ['Ry', 'Tx'])        
    else:
        joints_Ry[i], joints_Tx[i] = world.addJoint(bodies[i-1].end, bodies[i], ['Ry', 'Tx'])
        #joints_Ry[i] = world.addJoint(bodies[i-1].end, bodies[i], 'Ry')

    world.addVisualisation.Line(bodies[i], pitch)
    world.addVisualisation.Sphere(bodies[i].end, 0.01, color=[0.8, 0.5, 0.5])

    # Add force    
    #if i>0:
    dx = world.addSensor.Joint(symbol=f'tx_{i}', joint=joints_Tx[i])
    f = world.addExpression(symbol_str=f'force_{i}', exp= -c*dx[0] - d*dx[1])
    world.addLoad.Joint(joint=joints_Tx[i], symbol=f)
    
    # Add torque
    dphi = world.addSensor.Joint(symbol = f'ry_{i}', joint=joints_Ry[i])
    t = world.addExpression(symbol_str=f'tau_{i}', exp= -c_r*dphi[0] - d_r*dphi[1])
    world.addLoad.Joint(joint=joints_Ry[i], symbol=t)


t = time.time()

world.addVisualisation.Frame(world, 0.5)
world.genEquations.Recursive()
print('Time needed for generating equations: %5.2f s' % (time.time() - t))

world.show('belt')
