from pymbs.common.abstractbody import AbstractBody

from pymbs.symbolics import Matrix

from .frame import Frame

import pymbs.symbolics as symbolics


class Body(AbstractBody):
    '''
    Body holding mass and inertia properties
    '''


    def __init__(self, name, mass=0, cg=symbolics.zeros((3,)), inertia=symbolics.zeros((3,3)), graph=None):
        '''
        Constructor
        name: Name of the Body
        mass: Mass in kg (Scalar)
        cg: Centre Of Gravity (3x1 Vector)
        inertia: Inertia Tensor w.r.t. The Centre Of Gravity (symmetric 3x3 Matrix)
        '''

        # Call MbsElement Constructor
        assert graph is not None
        AbstractBody.__init__(self, name, mass, cg, inertia, graph)

        # additional attributes
        self.index = None   # body index, i.e. position in mass matrix
        self.children = []  # list of all children coordinate systems
        self.joint = None;  # reference to parent joint

        # attributes used for calculation
        self.I_r = None   # Position of Origin w.r.t. Inertial Frame
        self.I_v = None   # Velocity of Origin w.r.t. Inertial Frame
        self.I_a = None   # Acceleration of Origin w.r.t. Inertial Frame
        self.I_l = None   # Centre of Gravity w.r.t. Inertial Frame

        self.I_R =  None  # Transformation Matrix, Inertial Frame <- Body Frame
        self.K_om = None  # Angular Velocity w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)
        self.K_al = None  # Angular Acceleration w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)

        self.CS_0 = self.addFrame('_int_CS_0')


    def addFrame(self, name, p=symbolics.zeros((3,)), R=symbolics.eye((3,3))):
        '''
        Add A New Coordinate System To The List Of Children
        name: Name of the Coordinate System
        p: Position of the Coordinate System (3x1 Vector)
        R: Orientation of the Coordinate System (3x3 Matrix)
        '''
        # Create a New Coordinate System
        cs = Frame(name=name, parentBody=self, p=p, R=R, graph=self.graph)

        # Append it to the List of Children
        self.children += [cs]

        # return new Coordinate System
        return cs



class FlexibleBody(AbstractBody):
    '''
    Body holding mass and inertia properties
    '''

    def __init__(self, sid, name, mass=0, cg=symbolics.zeros((3,)), inertia=symbolics.zeros((3,3)), graph=None):
        '''
        Constructor
        name: Name of the Body
        mass: Mass in kg (Scalar)
        cg: Centre Of Gravity (3x1 Vector)
        inertia: Inertia Tensor w.r.t. The Centre Of Gravity (symmetric 3x3 Matrix)
        '''

        # Call MbsElement Constructor
        assert graph is not None
        AbstractBody.__init__(self, name, mass, cg, inertia, graph)
        # FlexibleBody object in Processing requires sid-object as well
        self.sid = sid
        # additional attributes
        self.index = None   # body index, i.e. position in mass matrix
        self.children = []  # list of all children coordinate systems
        self.joint = None;  # reference to parent joint

        # attributes used for calculation
        self.I_r = None   # Position of Origin w.r.t. Inertial Frame
        self.I_v = None   # Velocity of Origin w.r.t. Inertial Frame
        self.I_a = None   # Acceleration of Origin w.r.t. Inertial Frame
        self.I_l = None   # Centre of Gravity w.r.t. Inertial Frame

        self.I_R =  None  # Transformation Matrix, Inertial Frame <- Body Frame
        self.K_om = None  # Angular Velocity w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)
        self.K_al = None  # Angular Acceleration w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)

        #self.CS_0 = self.addFrame('_int_CS_0')

        # checking if the values of nelastq and nq (SID-File) are equal
        for nodes in self.sid.modal.frame.Knoten:
            if nodes.origin.originmatrix.nq != self.sid.modal.refmod.nelastq:
                raise NotImplementedError('the values of nelastq and nq (SID-File) must be equal')


        name_flexible_coordinates = 'flexible_coordinates'
        name_flexible_velocity = 'flexible_velocity'
        name_flexible_acceleration = 'flexible_acceleration'
        self.q = [graph.addVariable(name='q_%i_%s_%s' %(i+1,name_flexible_coordinates,self.name)) for i in range(self.sid.modal.refmod.nelastq)]
        self.q_vec = Matrix(self.q)
        self.qd = [graph.addVariable(name='qd_%i_%s_%s' %(i+1,name_flexible_velocity,self.name)) for i in range(self.sid.modal.refmod.nelastq)]
        self.qd_vec = Matrix(self.qd)
        self.qdd = [graph.addVariable(name='qdd_%i_%s_%s' %(i+1,name_flexible_acceleration,self.name)) for i in range(self.sid.modal.refmod.nelastq)]
        self.q0 = [0]*self.sid.modal.refmod.nelastq
        self.qd0 = [0]*self.sid.modal.refmod.nelastq


    def addFrame(self, name, p=symbolics.zeros((3,)), R=symbolics.eye((3,3))):
        '''
        Add A New Coordinate System To The List Of Children
        name: Name of the Coordinate System
        p: Position of the Coordinate System (3x1 Vector)
        R: Orientation of the Coordinate System (3x3 Matrix)
        '''
        # Create a New Coordinate System
        cs = Frame(name=name, parentBody=self, p=p, R=R, graph=self.graph)

        # Append it to the List of Children
        self.children += [cs]

        # return new Coordinate System
        return cs

