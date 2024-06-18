"""
Implementation of an explicit loop for a crank-slider-mechanism
"""

from pymbs.processing.loops.loop import Loop, eps
from pymbs.common.functions import sqrt
from pymbs.processing import Frame

from pymbs.symbolics import Matrix, eye, cos, sin, asin, zeros, transpose

AL = 'FB_%s_AL'
L1 = 'FB_%s_L1'
L2 = 'FB_%s_L2'


class CrankSlider(Loop):
    """
    Explicit treatment of a crank-slider-mechanism

    Sketch:

        Ba|Bb
       /    \
      1      2
     /        \
    A     [--B|C|3--]

    1 - crank
    2 - rod
    3 - slider

    A - rotational joint between crankshaft and crank
    B - rotational joint between rod and slider
    C - translational joint of the slider
    Ba|Bb - coordinate systems at the end of the crank and the rod to close the loop
    """

    def __init__(self, name, csBa, csBb):
        '''
        Constructor
        @param name: name of the object
        @param csBa: coordinate system at the end of the crank
        @param csBb: coordinate system at the end of the rod
        '''

        # Assertions
        assert isinstance(csBa, Frame)
        assert isinstance(csBb, Frame)

        # Check parents
        if csBa.parentBody.joint is None:
            raise ValueError(f'Loop {name}: Coordinate System {csBa.name} is directly '\
                              'connected to the world!')
        if csBb.parentBody.joint is None:
            raise ValueError(f'Loop {name}: Coordinate System {csBb.name} is directly '\
                              'connected to the world!')

        # Build complete crank slider mechanism
        j1 = csBa.parentBody.joint
        j2 = csBb.parentBody.joint
        if j2.coordSys.parentBody.joint is None:
            j1 = csBb.parentBody.joint
            j2 = csBa.parentBody.joint
            j3 = j1.coordSys.parentBody.joint
            csB1 = csBb
            csB2 = csBa
        else:
            j3 = j2.coordSys.parentBody.joint
            csB1 = csBa
            csB2 = csBb

        # Do the joints have the same axis of rotation and lie in the same plane?
        Test1 = j1.coordSys.p-j2.coordSys.p
        if j1.Phi == Matrix([1, 0, 0]):
            delta = Test1[0]
            x = 1
            if j2.Phi == Matrix([1, 0, 0]):
                if abs(delta) < eps:
                    self.sign = 1
                    self.pick = Matrix([[0, 1, 0],
                                        [0, 0, 1]])
                else:
                    raise ValueError(f'Loop {name}: Endings of prismatic joints must have the '\
                                      'same values for x. There is a difference of {delta}')
            else:
                raise ValueError(f'Loop {name}: Both axes of rotation must be the same.')

        elif j1.Phi == Matrix([0, 1, 0]):
            delta = Test1[1]
            x = 2
            if j2.Phi == Matrix([0, 1, 0]):
                if abs(delta) < eps:
                    self.sign = -1
                    self.pick = Matrix([[1, 0, 0],
                                        [0, 0, 1]])
                else:
                    raise ValueError(f'Loop {name}: Endings of prismatic joints must have the '\
                                      'same values for y. There is a difference of {delta}')
            else:
                raise ValueError(f'Loop {name}: Both axes of rotation must be the same.')

        elif j1.Phi == Matrix([0, 0, 1]):
            delta = Test1[2]
            x = 3
            if j2.Phi == Matrix([0, 0, 1]):
                if abs(delta) < eps:
                    self.sign = 1
                    self.pick = Matrix([[1, 0, 0],
                                        [0, 1, 0]])
                else:
                    raise ValueError(f'Loop {name}: Endings of prismatic joints must have the '\
                                      'same values for z. There is a difference of {delta}')
            else:
                raise ValueError(f'Loop {name}: Both axes of rotation must be the same.')
        else:
            raise ValueError(f'Loop {name}: Axis of rotation must be either x,y or z!')

        # Does the translation joint lie in the right plane?
        if x == 1:
            # ValueError(f'Loop {name}: Axis of rotation is not allowed.')
            if j3.Psi == Matrix([0, 1, 0]):
                y = 2
            elif j3.Psi == Matrix([0, 0, 1]):
                y = 3
            else:
                raise ValueError('Loop "%s": Axis of translation is wrong.')
        elif x == 2:
            if j3.Psi == Matrix([1, 0, 0]):
                y = 1
            elif j3.Psi == Matrix([0, 0, 1]):
                y = 3
                # ValueError(f'Loop {name}: Only axis of translation Tx is allowed.')
            else:
                raise ValueError(f'Loop {name}: Axis of translation is wrong.')
        elif x == 3:
            # ValueError(f'Loop {name}: Axis of rotation is not allowed.')
            if j3.Psi == Matrix([1, 0, 0]):
                y = 1
            elif j3.Psi == Matrix([0, 1, 0]):
                y = 2
            else:
                raise ValueError(f'Loop {name}: Axis of translation is wrong.')
        else:
            raise ValueError(f'Loop {name}: Axis of translation must be either x,y or z!')

        assert j1.Psi.norm() == 0
        assert j2.Psi.norm() == 0
        assert j3.Phi.norm() == 0
        assert j3.Psi.norm() == 1
        assert j3.Psi.norm() == j1.Phi.norm()
        assert j1.Phi.norm() == j2.Phi.norm()

        # Are all coordinate systems aligned like their parent body?
        assert (j1.coordSys.R - eye(3)) == zeros(3)
        assert (j2.coordSys.R - eye(3)) == zeros(3)

        # Check that bodies between joints are the same
        assert j1.coordSys.parentBody == j3.coordSys.parentBody
        assert j1.body == csB1.parentBody
        assert j2.body == csB2.parentBody
        assert j3.body == j2.coordSys.parentBody

        assert j1.coordSys.p.norm() == 0

        # Superclass constructor
        Loop.__init__(self, name)

        # Save parameters
        self.j1 = j1
        self.j2 = j2
        self.j3 = j3
        self.csB1 = csB1
        self.csB2 = csB2

        # Independent coordinates
        self.u = [j1.q]
        self.ud = [j1.qd]
        self.udd = [j1.qdd]

        # Dependent coordinates
        self.v = [j3.q, j2.q]
        self.vd = [j3.qd, j2.qd]
        self.vdd = [j3.qdd, j2.qdd]


    def calc(self, graph):
        '''
        Returns precalculated v(u), Bvu and b_prime, s.t.
        q = [u,v]', where
            u: independent coordinates
            v: dependent coordinates
        Starting from the Constraint Equation: g(q) = 0,
        One Obtains by Differentiation:
        (dg/du)*u_dot + (dg/dv)*v_dot = 0
        Gu*u_dot + Gv*v_dot = 0
        Thus, v_dot = -(inv(Gu)*Gv)*u_dot = Bvu*u_dot, with Gv = dg/dv and Gu = dg/du
        Differentiating once more, yields
        v_ddot = Bvu*u_ddot + Bvu_dot*u_dot
               = Bvu*u_ddot + b_prime, with b_prime = Bvu_dot*u_dot

        Finally one can transform the Equation of Motion
        M*q_ddot + h = f + W'*mu
        M*(Jvu*u_ddot + bvu) + h = f + W'*mu    with Jvu = [1, Bvu']' and bvu = [0,b_prime']'
        (Jvu'*M*Jvu)*u_ddot + Jvu'*M*bvu + Jvu'*h = Jvu'*f, since Jvu'*W' = 0
        M_star*u_ddot + h_star = f_star
        M_star = (Jvu'*M*Jvu)
        h_star = Jvu'*M*bvu + Jvu'*h
        f_star = Jvu'*f
        '''

        assert graph is not None

        # Generalised coordinates
        q1 = self.j1.q     # u[0]    # angle
        q1d = self.j1.qd
        q2 = self.j3.q     # v[0]    # distance
        q2d = self.j3.qd
        q3 = self.j2.q     # v[1]    # angle
        q3d = self.j2.qd

        # Length of bars
        l1_vec = self.csB1.p
        l1_vec2 = self.pick*l1_vec
        l1 = graph.addEquation(L1 % self.name, sqrt((transpose(l1_vec2)*l1_vec2)[0]))

        l2_vec = self.csB2.p
        l2_vec2 = self.pick*l2_vec
        l2 = graph.addEquation(L2 % self.name, sqrt((transpose(l2_vec2)*l2_vec2)[0]))

        # Calculate v
        alpha = graph.addEquation(AL % self.name, asin((l1/l2)*sin(q1)))

        v1 = alpha                  # q3
        v0 = l1*cos(q1)-l2*cos(q3)  # q2

        alphad = q3d

        Bvu = Matrix([[-l1*sin(alpha+q1)/cos(alpha)], [l1*cos(q1)/(l2*cos(alpha))]])

        b_prime = Matrix([-l1*q1d*(2*alphad*cos(q1)+q1d*cos(q1)+q1d*cos(2*alpha+q1))/(2*cos(alpha)*cos(alpha)),
                          (-l1*q1d*q1d*cos(alpha)*sin(q1)+l1*alphad*q1d*cos(q1)*sin(alpha))/(l2*cos(alpha)*cos(alpha))])

        return ([v0, v1], Bvu, b_prime)
