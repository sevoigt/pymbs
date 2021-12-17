from numpy import pi

from pymbs.processing.loops.loop import Loop, eps
from pymbs.common.functions import sqrt
from pymbs.processing import Frame

from pymbs.symbolics import Matrix, eye, cos, sin, atan, atan2, acos, asin, zeros


AL = 'FB_%s_AL'
#BE = 'FB_%s_BE'

#L0 = 'FB_%s_L0'
L1 = 'FB_%s_L1'
L2 = 'FB_%s_L2'

#PHI = 'FB_%s_PHI'
#PSI = 'F_%s_PSI'



class CrankSlider(Loop):
    '''
    Explicit Treatment of a Three Bar Linkage Mechanism with Slider
    '''

    '''
    Sketch:
         B
       /   \
      1     2
    /        \
   A-----0----C
    '''
    def __init__(self, name, csBa, csBb):

        '''
        Constructor
        @param setup: Four Bar Linkage has two setups: -1, 1
        '''

        # Assertions
        assert ( isinstance(csBa, Frame) )
        assert ( isinstance(csBb, Frame) )

        # Check parents
        if (csBa.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csBa.name))
        if (csBb.parentBody.joint is None):
            raise ValueError('Loop "%s": Coordinate System "%s" is directly connected to the world!'%(name,csBb.name))

        # Build complete CrankSlider
        j1 = csBa.parentBody.joint
        j2 = csBb.parentBody.joint
        if (j2.coordSys.parentBody.joint == None):
            j1 = csBb.parentBody.joint
            j2 = csBa.parentBody.joint
            j3 = j1.coordSys.parentBody.joint
            csB1 = csBb
            csB2 = csBa
        else:
            j3 = j2.coordSys.parentBody.joint
            csB1 = csBa
            csB2 = csBb


        # Do the Joints have the same axis of Rotation and lie in the same plane
        Test1 = j1.coordSys.p-j2.coordSys.p
        if (j1.Phi == Matrix([1,0,0])):
            delta = Test1[0]
            x = 1
            if (j2.Phi == Matrix([1,0,0])):
                if ( abs(delta) < eps ):
                    self.sign = 1
                    self.pick = Matrix([[0,1,0],
                                        [0,0,1]])
                else:
                    raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for x. There is a difference of %f'%(name,delta))
            else:
                raise ValueError('Loop "%s": The both axis of Rotation must be the same.')
        elif (j1.Phi == Matrix([0,1,0])):
            delta = Test1[1]
            x = 2
            if (j2.Phi == Matrix([0,1,0])):
                if ( abs(delta) < eps ):
                    self.sign = -1
                    self.pick = Matrix([[1,0,0],
                                        [0,0,1]])
                else:
                    raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for y. There is a difference of %f'%(name,delta))
            else:
                raise ValueError('Loop "%s": The both axis of Rotation must be the same.')
        elif (j1.Phi == Matrix([0,0,1])):
            delta = Test1[2]
            x = 3
            if (j2.Phi == Matrix([0,0,1])):
                if (abs(delta) < eps):
                    self.sign = 1
                    self.pick = Matrix([[1,0,0],
                                        [0,1,0]])
                else:
                    raise ValueError('Loop "%s": Endings of prismatic joints must have the same values for z. There is a difference of %f'%(name,delta))
            else:
                raise ValueError('Loop "%s": The both axis of Rotation must be similar.')
        else:
            raise ValueError('Loop "%s": Axis of Rotation must be either x,y or z!'%name)

        # Does the translation joint lie in the right plane?

        if (x==1):
            ValueError('Loop "%s": Axis of Rotation is not allowed.')
            if (j3.Psi == Matrix([0,1,0])):
                y=2
            elif (j3.Psi == Matrix([0,0,1])):
                y=3
            else:
                raise ValueError('Loop "%s": Axis of Translation is wrong.')
        elif (x==2):
            if (j3.Psi == Matrix([1,0,0])):
                y=1
            elif (j3.Psi == Matrix([0,0,1])):
                y=3
                ValueError('Loop "%s": Just Axis of Translation Tx is allowed.')
            else:
                raise ValueError('Loop "%s": Axis of Translation is wrong.')
        elif (x==3):
            ValueError('Loop "%s": Axis of Rotation is not allowed.')
            if (j3.Psi == Matrix([1,0,0])):
                y=1
            elif (j3.Psi == Matrix([0,1,0])):
                y=2
            else:
                raise ValueError('Loop "%s": Axis of Translation is wrong.')
        else:
            raise ValueError('Loop "%s": Axis of Translation be either x,y or z!')



        assert( j1.Psi.norm() == 0 )
        assert( j2.Psi.norm() == 0 )
        assert( j3.Phi.norm() == 0 )
        assert( j3.Psi.norm() == 1 )
        assert( j3.Psi.norm() == j1.Phi.norm() )
        assert( j1.Phi.norm() == j2.Phi.norm() )


        # Are All Coordinate Systems Aligned like Their parentBody?
        assert( (j1.coordSys.R - eye(3)) == zeros(3) )
        assert( (j2.coordSys.R - eye(3)) == zeros(3) )


        # Check that bodies between joints are the same
        assert( j1.coordSys.parentBody == j3.coordSys.parentBody )
        assert( j1.body == csB1.parentBody )
        assert( j2.body == csB2.parentBody )
        assert( j3.body == j2.coordSys.parentBody )

        assert( j1.coordSys.p.norm() == 0 )


        # Super Constructor
        Loop.__init__(self, name)

        # Save Parameters
        self.j1 = j1
        self.j2 = j2
        self.j3 = j3
        self.csB1 = csB1
        self.csB2 = csB2


        # Independent Coordinates
        self.u = [j1.q]
        self.ud = [j1.qd]
        self.udd = [j1.qdd]

        # Dependent Coordinates
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

        assert( graph is not None )

        # Abbrevations
        s = self.sign

        # Generalised Coordinates
        q1 = self.j1.q     # u[0]    # angle
        q1d = self.j1.qd
        q2 = self.j3.q     # v[0]    # distance
        q2d = self.j3.qd
        q3 = self.j2.q     # v[1]    # angle
        q3d = self.j2.qd

        # Length of bars

        l1 = sym(L1%self.name, True)
        l1_vec = self.csB1.p
        l1_vec2 = self.pick*l1_vec
        graph += [Expression(str(l1), l1, sqrt((l1_vec2.transpose()*l1_vec2)[0]))]

        l2 = sym(L2%self.name, True)
        l2_vec =self.csB2.p
        l2_vec2 = self.pick*l2_vec
        graph += [Expression(str(l2), l2, sqrt((l2_vec2.transpose()*l2_vec2)[0]))]



        # Calculate v
        alpha = sym(AL%self.name)
        graph += [Expression(str(alpha), alpha, asin((l1/l2)*sin(q1)))]


        v1 = alpha              # q3
        v0 = l1*cos(q1)-l2*cos(q3) # q2

        alphad = q3d

        Bvu = Matrix( [[-l1*sin(alpha+q1)/cos(alpha)], [l1*cos(q1)/(l2*cos(alpha))]] )

        b_prime = Matrix( [-l1*q1d*(2*alphad*cos(q1)+q1d*cos(q1)+q1d*cos(2*alpha+q1))/(2*cos(alpha)*cos(alpha)),
                        (-l1*q1d*q1d*cos(alpha)*sin(q1)+l1*alphad*q1d*cos(q1)*sin(alpha))/(l2*cos(alpha)*cos(alpha))] )

        return ([v0,v1],Bvu,b_prime)


