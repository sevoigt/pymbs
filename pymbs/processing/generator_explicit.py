from pymbs.processing.generator import *
from pymbs.processing.loads.constraint import Constraint

from pymbs.common.functions import *

from pymbs.common.simple import simple

from pymbs.symbolics import Matrix, zeros, jacobian, VarKind


class Generator_Explicit(Generator):
    '''
    Generator That Calculates the Equations of Motion in Explicit Form
    '''


    def __init__(self, name, graph, inertial, g, sensors = None,
                 loops = None, diff = 0, kinematicsOnly = False):
        '''
        Constructor
        '''
        # Call Constructor of Super Class
        Generator.__init__(self, name, graph, inertial, g, sensors,
                           loops, diff, kinematicsOnly)


    def getSystemEqns(self):
        '''
        Calculate System Equations
        '''

        # Create Empty List of Equations
        graph = self.graph

        # Calculate All Positions w.r.t. Inertial Frame
        self.calcBodyStates(graph)

        # Calculate Sensors (before accelerations)
        for sens in self.sensors:
            if (not sens.delayCalc):
                sens.calc(graph)

        if (self.kinematicsOnly):
            self.handleLoops(graph)
        else:
            # Obtain Equations
            print(self.q, self.q.__class__)
            self.calcEquations(graph, self.inertial, self.bodies, self.joints, self.fels, self.loops, self.q, self.qd)

        # Calculate Sensors (after accelerations)
        for sens in self.sensors:
            if (sens.delayCalc):
                sens.calc(graph)

        # return all equations
        return True


    def calcBodyStates(self, graph):
        '''
        Calculates All Positions w.r.t. Inertial Frame
        Calculates All Rotations w.r.t. Body Frame
        '''

        for body in self.bodies:

            print("Obtaining Positions, Velocities and Accelerations of Body '%s'"%body.name)

            # Is it the Inertial Frame itself?
            if (body.joint is None):
                # Set Position and Angular Velocity to Zero, Rotational Matrix = Identity
                body.I_r = symbolics.zeros((3,))
                body.I_v = symbolics.zeros((3,))
                body.I_a = symbolics.zeros((3,))
                body.I_R = symbolics.eye((3,3))
                body.K_om = symbolics.zeros((3,))
                body.K_al = symbolics.zeros((3,))
            else:
                # Get Reference to Parent Body
                joint = body.joint
                parentCS = joint.coordSys

                # Calculate Position and Angular Velocity
                body.I_R = parentCS.I_R * joint.R

                body.I_r = parentCS.I_r + body.I_R*joint.Psi*joint.q
                body.I_v = jacobian(body.I_r, self.q)*self.qd
                body.I_a = jacobian(body.I_v, self.q)*self.qd + jacobian(body.I_v, self.qd)*self.qdd
                body.K_om = transpose(joint.R)*parentCS.K_om + joint.Phi*joint.qd
    # K_al = d(K_om)/dt = R'*d(R*K_om)/dt
    #            I_om = body.I_R*body.K_om
    #            I_al = I_om.jacobian(self.state.q)*self.state.qd + I_om.jacobian(self.state.qd)*self.state.qdd
    #            body.K_al = body.I_R.transpose()*I_al
    #            body.K_al = body.K_al.applyfunc(lambda x: pymbs.Symbolics.trigsimp(x, recursive=True))
                body.K_al = jacobian(body.K_om, self.q)*self.qd + jacobian(body.K_om, self.qd)*self.qdd

            # Update cg
            body.I_l = body.I_r + body.I_R*body.cg
            body.I_ldot = jacobian(body.I_l, self.q)*self.qd

            # Update Child Coordinate Systems
            for childCS in body.children:
                # Do the Update
                childCS.K_om = transpose(childCS.R)*body.K_om
                childCS.K_al = transpose(childCS.R)*body.K_al
                childCS.I_R = body.I_R*childCS.R
                childCS.I_r = body.I_r + body.I_R*childCS.p
                childCS.I_v = jacobian(childCS.I_r, self.q)*self.qd
                childCS.I_a = jacobian(childCS.I_v, self.q)*self.qd + jacobian(childCS.I_v, self.qd)*self.qdd


    def calcEquations(self, graph, inertial, bodies, joints, fels, loops, q, qd):
        '''
        Obtain Equations of Motion Using an Explicit Newton/Euler Algorithm
        '''

        # Dimensions
        N = len(bodies)-1
        n = len(joints)

        # Allocate Empty Matrices
        M1 = Matrix((3*N,3*N))         # Mass Matrix - Translation
        M2 = Matrix((3*N,3*N))         # Mass Matrix - Rotation
        J1 = Matrix((3*N,n))           # Jacobian Matrix - Translation
        J2 = Matrix((3*N,n))           # Jacobian Matrix - Rotation
        z2 = Matrix((3*N,))           # Angular Velocity
        z2_tilde = Matrix((3*N,3*N))   # Angular Velocity with Tilde Op
        f = Matrix((3*N,))            # Force Vector, Caused by Gravity

        # Iterate Through All Bodies
        print("Iterating Through All Bodies")
        for body in bodies:
            if (body.index is None): continue

            # Index in Matrix
            i = body.index * 3

            # Mass Matrices
            M1[i:i+3,i:i+3] = body.mass*symbolics.eye((3,3))
            M2[i:i+3,i:i+3] = body.inertia

            # Jacobian Matrices
            J1[i:i+3,:] = jacobian(body.I_l, q)
            J2[i:i+3,:] = jacobian(body.K_om, qd)

            # Auxiliary Variables
            z2[i:i+3,0] = body.K_om
            z2_tilde[i:i+3,i:i+3] = skew(body.K_om)

            # Gravity Force
            f[i:i+3,0] = body.mass * self.g

        # Calculate Reduced Mass Matrix, h Vector, f Vector
#        print "Simplifying Jacobian"
#        J1 = simple(J1)
#        J2 = simple(J2)

        print("Calculating Mass Matrix and h Vector")
        J1dqd = jacobian((J1*qd), q)*qd
        J2dqd = jacobian((J2*qd), q)*qd
        M_red = transpose(J1)*M1*J1 + transpose(J2)*M2*J2
        h_red = transpose(J1)*M1*J1dqd + transpose(J2)*(M2*J2dqd + z2_tilde*M2*z2)
        f_red = transpose(J1)*f

        # Handle Loops
        if (len(self.loops) > 0):
            print("Handling Loops")
            (J, b_prime) = self.handleLoops(graph)
        else:
            J = 1
            b_prime = 0

        # Iterate Through All Force Elements
        f_ext = 0
        for fel in fels:
            print("Calculating ForceElement '%s'"%fel.name)
            # Perform Calculations Within the Force Element
            fel.calc(graph, self.state)

            # Calculate Force/Torque Distribution Matrices
            if (fel.ref is None):
                Rref = symbolics.eye((3,3))
            else:
                Rref = fel.ref.I_R

            if (fel.Wf is None): fel.Wf = zeros( (3,3) )
            if (fel.Wt is None): fel.Wt = zeros( (3,3) )

            sWf = fel.Wf.shape()
            sWt = fel.Wt.shape()
            if (len(sWf) > 1):
                Wf = Matrix((n,sWf[1]))
            else:
                Wf = Matrix((n,))
            if (len(sWt) > 1):
                Wt = Matrix((n,sWt[1]))
            else:
                Wt = Matrix((n,))

            # Parent
            Wf_temp = Matrix((3,3))
            Wft_temp = Matrix((3,3))
            Wt_temp = Matrix((3,3))
            cs = fel.parent
            body = cs.parentBody
            if (body.index is not None):
                i = body.index*3
                Wf_temp = Rref * fel.Wf
                if (body.I_R == Rref):
                    Wft_temp = skew(cs.p - body.cg)*fel.Wf
                    Wt_temp = fel.Wt
                else:
                    Wft_temp = skew(cs.p - body.cg)*transpose(body.I_R)*Rref*fel.Wf
                    Wt_temp = transpose(body.I_R)*Rref*fel.Wt

                Wf += transpose(J1[i:i+3,:])*Wf_temp + transpose(J2[i:i+3,:])*Wft_temp
                Wt += transpose(J2[i:i+3,:])*Wt_temp

            # Child
            Wf_temp = Matrix((3,3))
            Wft_temp = Matrix((3,3))
            Wt_temp = Matrix((3,3))
            cs = fel.child
            if (cs is not None):
                body = cs.parentBody
                if (body.index is not None):
                    i = body.index*3
                    Wf_temp = Rref * fel.Wf
                    if (body.I_R == Rref):
                        Wft_temp = skew(cs.p - body.cg)*fel.Wf
                        Wt_temp = fel.Wt
                    else:
                        Wft_temp = skew(cs.p - body.cg)*transpose(body.I_R)*Rref*fel.Wf
                        Wt_temp = transpose(body.I_R)*Rref*fel.Wt

                    Wf -= transpose(J1[i:i+3,:])*Wf_temp + transpose(J2[i:i+3,:])*Wft_temp
                    Wt -= transpose(J2[i:i+3,:])*Wt_temp

            # Simplify Them
            print("Simplifying Load-Distribution Matrices")
            Wf = scalar_if_possible(simple(transpose(J)*Wf))
            Wt = scalar_if_possible(simple(transpose(J)*Wt))

            # Add Them to the List
            if (fel.force is not None):
                Wf_sym = graph.addEquation('WF_%s' % fel.name, Wf)
                f_ext += Wf_sym * fel.force
            if (fel.torque is not None):
                Wt_sym = graph.addEquation('WT_%s' % fel.name, Wt)
                f_ext += Wt_sym * fel.torque

        print("Simplifying M")
        M_red = simple(M_red)
        print("Simplifying h")
        h_red = simple(h_red)
        print("Simplifying f")
        f_red = simple(f_red)

        # Define State variables
        q = graph.addVariable(name='q', shape=self.reduce_shape( self.q.shape() ), initialValue=scalar_if_possible(self.q0) )
        qd = graph.addVariable(name='qd', shape=self.reduce_shape( self.qd.shape() ), initialValue=scalar_if_possible(self.qd0) )
        qdd = graph.addVariable(name='qdd', shape=self.reduce_shape( self.qdd.shape() ) )
        graph.addEquation(self.state.q, q)
        graph.addEquation(self.state.qd, qd)
        graph.addEquation(self.state.qdd, qdd)

        # Handle Loops
        if (len(self.loops) > 0):

            # Reduce Mass Matrix again
            M_red2 = transpose(J)*M_red*J
            h_red2 = transpose(J)*(M_red*b_prime + h_red)
            f_red2 = transpose(J)*f_red

            M_ = graph.addEquation('M_', scalar_if_possible(M_red2))
            h_ = graph.addEquation('h_', scalar_if_possible(h_red2))
            f_gravity_ = graph.addEquation('f_gravity_', scalar_if_possible(f_red2))
        else:
            M_ = graph.addEquation('M_', scalar_if_possible(M_red))
            h_ = graph.addEquation('h_', scalar_if_possible(h_red))
            f_gravity_ = graph.addEquation('f_gravity_', scalar_if_possible(f_red))

        if (f_ext==0):
            f_ext = Matrix( f_gravity_.shape() )

        f_ext_ = graph.addEquation('f_ext_', scalar_if_possible(f_ext))
        f_ = graph.addEquation('f_', f_gravity_+f_ext_)

        if (len(self.state.q) > 1):
            der_qd = graph.addEquation(qdd, solve(M_, f_-h_), varKind = VarKind.Der_State)
        else:
            der_qd = graph.addEquation(qdd, (f_-h_)/M_, varKind = VarKind.Der_State)

        graph.addEquation(der(q), qd)
        graph.addEquation(der(qd), qdd)

        self.handleConstraints(graph)
