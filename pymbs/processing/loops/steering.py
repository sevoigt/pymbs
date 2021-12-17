from numpy import pi

from pymbs.processing.loops.loop import Loop, eps
from pymbs.common.functions import sqrt, transpose

from pymbs.processing import Frame


from pymbs.symbolics import Basic, Matrix, eye, cos, sin, atan, atan2, acos, zeros, asin


K0 = 'int_K0_%s'
K1 = 'int_K1_%s'
K = 'int_K_%s'
H0 = 'int_H0_%s'
L0 = 'int_L0_%s'
L1 = 'int_L1_%s'
R2 = 'int_R2_%s'
R3 = 'int_R3_%s'

Q4 = 'int_Q4_%s'
Q4P = 'int_Q4P_%s'

V0 = 'int_V0_%s'
V1 = 'int_V1_%s'

PRIME = 'int_D_%s'
DPRIME = 'int_DPRIME_%s'

SIN12 = 'int_sin_12_%s'
SIN30 = 'int_sin_30_%s'
COS12 = 'int_cos_12_%s'
COS30 = 'int_cos_30_%s'

PHIP12 = 'int_phip_12_%s'
PHIP23 = 'int_phip_23_%s'


class Steering(Loop):
    '''
    Explicit Treatment of a Steering Mechanism
    '''

    '''
    Sketch:
                     C (Rz)
                   /     |
                  2      |
                 /       |
    A (Ty) --1--B (Rz)   3
    |                    |
    |                    |
    |                    |
    0-----Da (Rz - cut) Db
    |
    |
    parent (closest to world)
    '''
    def __init__(self, name, csDa, csDb, setUpH=1, setUpS=1):

        '''
        Constructor
        @param setup_H: Steering has two setups for H_0: -1, 1
        @param setup_S: Steering has two setups for side: -1, 1
        '''

        # Assertions
        assert ( csDa is not None ), "csDa must not be None"
        assert ( csDb is not None ), "csDb must not be None"
        assert ( isinstance(csDa, Frame) ), "csDa must be a Frame, not a " + str(csDa.__class__)
        assert ( isinstance(csDb, Frame) ), "csDb must be a Frame, not a " + str(csDb.__class__)
        assert ((setUpH == 1) or (setUpH == -1)), "setUpH of Steering Mechanism '" + name + "' must be either 1 or -1, not " + str(setUpH)
        assert ((setUpS== 1) or (setUpS== -1)), "setUpS of Steering Mechanism '" + name + "' must be either 1 or -1, not " + str(setUpS)

        # Check parents
        b0a = csDa.parentBody
        assert b0a is not None, "Unexpected Structure"
        b3 = csDb.parentBody
        assert b3 is not None, "Unexpected Structure"
        jC = b3.joint
        assert jC is not None, "Unexpected Structure"
        f2 = jC.coordSys
        assert f2 is not None, "Unexpected Structure"
        b2 = f2.parentBody
        assert b2 is not None, "Unexpected Structure"
        jB = b2.joint
        assert jB is not None, "Unexpected Structure"
        f1 = jB.coordSys
        assert f1 is not None, "Unexpected Structure"
        b1 = f1.parentBody
        assert b1 is not None, "Unexpected Structure"
        jA = b1.joint
        assert jA is not None, "Unexpected Structure"
        f0 = jA.coordSys
        assert f0 is not None, "Unexpected Structure"
        b0b = f0.parentBody
        assert b0b is not None, "Unexpected Structure"

        assert b0a == b0b, "Unexpected Structure"

        # Calculations have been done for axis of rotation == [0,0,1]
        assert (jA.Phi == Matrix([0,0,0])), "Joint " + str(jB) + "must be pure Translational!"
        assert (jA.Psi == Matrix([0,1,0])), "Joint " + str(jB) + "must be Ty!"
        assert (jB.Phi == Matrix([0,0,1])), "Joint " + str(jB) + "must be Rz!"
        assert (jB.Psi == Matrix([0,0,0])), "Joint " + str(jB) + "must be pure Rotational!"
        assert (jC.Phi == Matrix([0,0,1])), "Joint " + str(jC) + "must be Rz!"
        assert (jC.Psi == Matrix([0,0,0])), "Joint " + str(jC) + "must be pure Rotational!"

        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.jA = jA
        self.jB = jB
        self.jC = jC
        self.csDa = csDa
        self.csDb = csDb
        self.sH = setUpH
        self.sS = setUpS


        # Independent Coordinates
        self.u = [jA.q]
        self.ud = [jA.qd]
        self.udd = [jA.qdd]

        # Dependent Coordinates
        self.v = [jB.q, jC.q]   # phi_12, phi_23 !!!
        self.vd = [jB.qd, jC.qd]
        self.vdd = [jB.qdd, jC.qdd]

    def calc(self, graph):
        '''
        Returns precalculated v(u), Bvu and b_prime, s.t.
        q = [u,v]', where
            u: independent coordinates
            v: dependent coordinates
        Starting from the Constraint Equation: Phi(q) = 0,
        One Obtains by Differentiation:
        (d(Phi)/du)*u_dot + (d(Phi)/dv)*v_dot = 0
        Ju*u_dot + Jv*v_dot = 0
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
        name = self.name


        # Generalised Coordinates
        q1 = self.jA.q     # u[0]    #
        q1d = self.jA.qd
        q2 = self.jB.q     # v[0]    # phi_12
        q2d = self.jB.qd
        q3 = self.jC.q     # v[1]    # phi_23 !!!
        q3d = self.jC.qd

        # Laengen
        L_vec = self.jA.coordSys.p - self.csDa.p
        L_0 = graph.addEquation(L0%name, abs(L_vec[1]))
        H_0 = graph.addEquation(H0%name, self.sH*abs(L_vec[0]))

        L_1 = self.jB.coordSys.p
        L_1 = graph.addEquation(L1%name, sqrt(L_1[0]*L_1[0]+L_1[1]*L_1[1]))
        if (isinstance(L_1, Basic)): L_1.simplify()

        R_2 = self.jC.coordSys.p
        R_2 = graph.addEquation(R2%name, sqrt(R_2[0]*R_2[0]+R_2[1]*R_2[1]))
        if (isinstance(R_2, Basic)): R_2.simplify()

        R_3 = self.csDb.p
        R_3 = graph.addEquation(R3%name, sqrt(R_3[0]*R_3[0]+R_3[1]*R_3[1]))
        if (isinstance(R_3, Basic)): R_3.simplify()

        # Calculate v
        k0 = graph.addEquation(K0%name, L_0 - L_1 - self.sS*q1)
        k1 = graph.addEquation(K1%name, (R_2*R_2-H_0*H_0-R_3*R_3-k0*k0)/(2*R_3))


        q4 = graph.addEquation(Q4%name,2.0*atan((H_0-self.sH*sqrt(H_0*H_0+k0*k0-k1*k1))/(k0+k1))) # q4 == phi_30
        v0 = graph.addEquation(V0%name, -self.sS*asin((H_0+R_3*sin(q4))/R_2))                           # phi_12
        v1 = graph.addEquation(V1%name, -self.sS*pi/2-self.sS*q4-v0)                                    # phi_23 !!!


        sin12 = graph.addEquation(SIN12%name, sin(v0))
        cos12 = graph.addEquation(COS12%name, cos(v0))
        sin30 = graph.addEquation(SIN30%name, sin(self.sS*q4))
        cos30 = graph.addEquation(COS30%name, cos(self.sS*q4))



        # Calculate Bvu
        D = graph.addEquation(PRIME%name, R_3*cos30*R_2*sin12+R_3*sin30*R_2*cos12)     # Determinante
        Bvu = Matrix( [[(self.sS*R_3*cos30)/D], \
                       [(self.sS*(R_2*cos12-R_3*cos30))/D]] )

        # Calculate b_prime
        # Ableitung der Determinante
        phip12 = graph.addEquation(PHIP12%name, Bvu[0,0]*q1d)
        phip23 = graph.addEquation(PHIP23%name, Bvu[1,0]*q1d)
        q4p    = graph.addEquation(Q4P%name, -self.sS*R_2*cos12/D*q1d)

        Dprime = graph.addEquation(DPRIME%name, R_3*R_2*((cos30*cos12-sin30*sin12)*phip12+(cos30*cos12-sin30*sin12)*q4p))

        b_prime = Matrix( [-self.sS*R_3*(sin30*q4p*D+cos30*Dprime)*q1d/(D*D), \
                           -self.sS*((R_2*sin12*phip12-R_3*sin30*q4p)*D+(R_2*cos12-R_3*cos30)*Dprime)*q1d/(D*D)] )

        return ([v0,v1],Bvu,b_prime)


