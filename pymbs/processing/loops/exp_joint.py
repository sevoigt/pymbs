from pymbs.processing.loops.loop import Loop
from pymbs.processing import Joint

from pymbs.symbolics import Matrix, Basic


class ExpJoint(Loop):
    '''
    The user may give an expression for a joint variable
    j2.q = ...
    Currently only kinematics is supported
    '''

    def __init__(self, name, j1, exp):
        '''
        Constructor
        '''

        # Assertions
        assert ( isinstance(j1, Joint) )
        assert ( isinstance(exp, Basic) )

        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.j1 = j1
        self.exp = exp

        # Independent Coordinates
        self.u = [1]
        self.ud = [1]
        self.udd = [1]

        # Dependent Coordinates
        self.v = [j1.q]
        self.vd = [j1.qd]
        self.vdd = [j1.qdd]

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

        v = self.exp

        Bvu = Matrix( [[0]] )

        b_prime = Matrix( [0] )

        return ([v], Bvu, b_prime)


