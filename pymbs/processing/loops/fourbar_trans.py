from numpy import pi

from pymbs.processing.loops.loop import Loop, eps
from pymbs.common.functions import sqrt, transpose
from pymbs.processing import Frame

from pymbs.symbolics import Matrix, eye, cos, sin, atan, atan2, acos, asin, zeros


AL = 'FB_%s_AL'
BE = 'FB_%s_BE'
GA = 'FB_%s_GA'
DE = 'FB_%s_DE'

L1 = 'FB_%s_L1'
L2 = 'FB_%s_L2'
L3 = 'FB_%s_L3'
L4 = 'FB_%s_L4'

PHI = 'FB_%s_PHI'
PSI = 'FB_%s_PSI'
THETA = 'FB_%s_THETA'

A = 'FB_%s_A'
B = 'FB_%s_B'
C = 'FB_%s_C'



class FourBarTrans(Loop):
    '''
    Explicit Treatment of a Three Bar Linkage Mechanism with Slider
    '''

    '''
    Sketch:
          B--3---C
        /         \
       /          4a
      2            \
     /              4b
    /                \
   A---------1--------D

    it does not have a posture, since it two angles are used
    as independent coordinates, which determine the posture
    '''
    def __init__(self, name, csCa, csCb):

        '''
        Constructor
        @param setup: Four Bar Linkage has two setups: -1, 1
        '''

        # Assertions
        assert ( isinstance(csCa, Frame) )
        assert ( isinstance(csCb, Frame) )

        # Check parents
        if (csCa.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCa.name))
        if (csCb.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csCb.name))

        # Build complete FourBarLinkageTrans
        jDC = csCb.parentBody.joint
        if (jDC.Psi.norm() == 1):
            jB = csCa.parentBody.joint
            jA = jB.coordSys.parentBody.joint
            jD = jDC.coordSys.parentBody.joint
            csC3 = csCa
            csC4a = csCb
        else:
            jDC = csCa.parentBody.joint
            jB = csCb.parentBody.joint
            jA = jB.coordSys.parentBody.joint
            jD = jDC.coordSys.parentBody.joint
            csC3 = csCb
            csC4a = csCa


        # Do the Joints have the same axis of Rotation and lie in the same plane
        jC = jA.coordSys.p+jB.coordSys.p+csC3.p
        if (jA.Phi == Matrix([1,0,0])):
            delta = jC[0] - jD.coordSys.p[0]
            if (abs(delta) < eps):
                self.sign = 1
                self.pick = Matrix([[0,1,0],
                                    [0,0,1]])
            else:
                raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for x. There is a difference of %f'%(name,delta))
        elif (jA.Phi == Matrix([0,1,0])):
            delta = jC[1] - jD.coordSys.p[1]
            if (abs(delta) < eps):
                self.sign = -1
                self.pick = Matrix([[1,0,0],
                                    [0,0,1]])
            else:
                raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for y. There is a difference of %f'%(name,delta))
        elif (jA.Phi == Matrix([0,0,1])):
            delta = jC[2] - jD.coordSys.p[2]
            if (abs(delta) < eps):
                self.sign = 1
                self.pick = Matrix([[1,0,0],
                                    [0,1,0]])
            else:
                raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for z. There is a difference of %f'%(name,delta))
        else:
            raise ValueError('Loop "%s": Axis of Rotation must be either x,y or z!'%name)

        assert( jA.Phi == jD.Phi )
        assert( jA.Phi == jB.Phi )
        assert( jA.Psi.norm() == 0 )
        assert( jB.Psi.norm() == 0 )
        assert( jD.Psi.norm() == 0 )
        assert( jDC.Phi.norm() == 0 )
        assert( jDC.Psi.norm() == 1 )
        assert( not (jDC.Psi == jD.Phi ))

        # Are All Coordinate Systems Aligned like Their parentBody?
        assert( (jA.coordSys.R - eye(3)) == zeros(3) )
        assert( (jB.coordSys.R - eye(3)) == zeros(3) )
        assert( (jD.coordSys.R - eye(3)) == zeros(3) )


        # Check that bodies between joints are the same
        assert( jA.coordSys.parentBody == jD.coordSys.parentBody )
        assert( jA.body == jB.coordSys.parentBody )
        assert( jB.body == csC3.parentBody )
        assert( jD.body == jDC.coordSys.parentBody )
        assert( jDC.body == csC4a.parentBody )

        assert( jDC.coordSys.p.norm() == 0 )
        assert( csC4a.p.norm() == 0)

        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.jA = jA
        self.jB = jB
        self.jD = jD
        self.jDC = jDC
        self.csC3 = csC3
        self.csC4 = csC4a

        # Independent Coordinates
        self.u = [jA.q, jB.q]
        self.ud = [jA.qd, jB.qd]
        self.udd = [jA.qdd, jB.qdd]

        # Dependent Coordinates
        self.v = [jDC.q, jD.q]
        self.vd = [jDC.qd, jD.qd]
        self.vdd = [jDC.qdd, jD.qdd]

    def calc(self, graph):
        '''
        Returns precalculated v(u), Bvu and b_prime, s.t.
        q = [u,v]', where
            u: independent coordinates
            v: dependent coordinates
        Starting from the Constraint Equation: Phi(q) = 0,
        One Obtains by Differentiation:
        (d(Phi)/du)*u_dot + (d(Phi)/dv)*v_dot = 0
        Ju*u_dot + Jv+v_dot = 0
        Thus, v_dot = -(inv(Jv)*Ju)*u_dot = Bvu*u_dot, with Jv = d(Phi)/dv and Ju = d(Phi)/du
        Differentiating once more, yields
        Ju*u_ddot + Jv*v_ddot + [Ju_dot, Jv_dot]*[u_dot,v_dot]' = 0
        Ju*u_ddot + Jv*v_ddot + J_dot*q_dot = 0
        Using this relations, one may obtain an expression for v_ddot
        v_ddot = -(inv(Jv)*Ju)*u_ddot - inv(Jv)*J_dot*q_dot
               = Bvu*u_ddot + b_prime, with b_prime = -inv(Jv)*J_dot*q_dot

        Finally one can transform the Equation of Motion
        M*q_ddot + h = f + W'*mu
        M*(J*u_ddot + b) + h = f + W'*mu    with J = [1, Bvu']' and b = [0,b_prime']'
        (J'*M*J)*u_ddot + J'*M*b + J'*h = J'*f, since J'*W' = 0
        M_star*u_ddot + h_star = f_star
        M_star = (J'*M*J)
        h_star = J'*M*b + J'*h
        f_star = J'*f
        '''

        assert( graph is not None )

        # Abbrevations
        s = self.sign

        # Generalised Coordinates
        q1 = self.jA.q     # u[0]    # angle between x-axes
        q1d = self.jA.qd
        q2 = self.jB.q     # u[1]    # angle between x-axes
        q2d = self.jB.qd
        q3 = self.jDC.q     # v[0]    # distance
        q3d = self.jDC.qd
        q4 = self.jD.q     # v[1]    # angle between x-axes
        q4d = self.jD.qd

        # Length of bars and angle between x-axis and bar
        l1_vec = self.jD.coordSys.p - self.jA.coordSys.p
        l1_vec2 = self.pick*l1_vec
        l1 = graph.addEquation(L1%self.name, sqrt((transpose(l1_vec2)*l1_vec2)))
        alpha = graph.addEquation(AL%self.name, s*atan2(l1_vec2[1],l1_vec2[0]))

        l2_vec = self.jB.coordSys.p
        l2_vec2 = self.pick*l2_vec
        l2 = graph.addEquation(L2%self.name, sqrt((transpose(l2_vec2)*l2_vec2)))
        beta = graph.addEquation(BE%self.name, s*atan2(l2_vec2[1],l2_vec2[0]))

        l3_vec = self.csC3.p
        l3_vec2 = self.pick*l3_vec
        l3 = graph.addEquation(L3%self.name, sqrt((transpose(l3_vec2)*l3_vec2)))
        gamma = graph.addEquation(GA%self.name, s*atan2(l3_vec2[1],l3_vec2[0]))

        l4_vec2 = self.pick*self.jDC.Psi
        delta = graph.addEquation(DE%self.name, s*atan2(l4_vec2[1],l4_vec2[0]))


        # angle between bars
        phi_prime = graph.addEquation(PHI%self.name, q1 + beta - alpha)
        theta_prime = graph.addEquation(THETA%self.name, q2 + gamma - beta)



        # Calculate v
        l4_vec = Matrix([ l2*cos(phi_prime)+l3*cos(phi_prime+theta_prime)-l1 , s*l2*sin(phi_prime)+s*l3*sin(phi_prime+theta_prime) ])
        l4 = graph.addEquation(L4%self.name, sqrt((transpose(l4_vec)*l4_vec)))

        # A = P1, B = P2, C = P3
        P1 = graph.addEquation(A%self.name, 2*l4*(l1-l2*cos(phi_prime)))
        P2 = graph.addEquation(B%self.name, -2*l2*l4*sin(phi_prime))
        P3 = graph.addEquation(C%self.name, l1**2+l2**2-l3**2+l4**2-2*l1*l2*cos(phi_prime))

        psi_prime = graph.addEquation(PSI%self.name, ((l2*sin(phi_prime)+l3*sin(phi_prime+theta_prime))/abs(l2*sin(phi_prime)+l3*sin(phi_prime+theta_prime)))*acos((l2*cos(phi_prime)+l3*cos(phi_prime+theta_prime)-l1)/l4))

        v1 = (psi_prime + alpha - delta)
        v0 = (l4)

        Bvu = Matrix( [ [-(l2*sin(phi_prime-psi_prime)+l3*sin(phi_prime+theta_prime-psi_prime)), -l3*sin(phi_prime+theta_prime-psi_prime)], [(l2*cos(phi_prime-psi_prime)+l3*cos(phi_prime+theta_prime-psi_prime))/l4, l3*cos(phi_prime+theta_prime-psi_prime)/l4] ] )

        b_prime = Matrix( [-q1d**2*l2*cos(phi_prime-psi_prime)-l3*cos(phi_prime+theta_prime-psi_prime)*(q1d+q2d)**2+q4d**2*l4, -(q1d**2*l2*sin(phi_prime-psi_prime)+l3*sin(phi_prime+theta_prime-psi_prime)*(q1d+q2d)**2+2*q4d*q3d)/l4] )

        return ([v0,v1],Bvu,b_prime)


