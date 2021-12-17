from pymbs.common.abstractframe import AbstractFrame

import pymbs.symbolics as symbolics


class Frame(AbstractFrame):
    '''
    Class Of A Coordinate System. Used To Mark Important Points On Bodies.
    '''


    def __init__(self, name, parentBody, p=symbolics.zeros((3,)), R=symbolics.eye((3,3)), graph = None):
        '''
        Constructor
        parentBody: Reference to ParentBody (Body)
        p: Position (3x1 Vector)
        R: Orientation (3x3 Vector)
        '''

        # TODO bad design, need this here to avoid circular import
        from pymbs.processing import Body, FlexibleBody

        # Assertions
        assert( isinstance(parentBody, (Body, FlexibleBody) ))

        # Constructor
        assert graph is not None
        AbstractFrame.__init__(self, name, p, R, graph)

        # Assign Parameters
        self.parentBody = parentBody    # reference to parentBody

        # Additional Attributes
        self.joints = []                # list of connected joints
        self.loads = []                 # list of connected loads

        # attributes used for calculation
#       self.p = p              # Position of Origin w.r.t. Parent Frame
        self.I_r = None         # Position of Origin w.r.t. Inertial Frame
        self.I_v = None         # Velocity of Origin w.r.t. Inertial Frame
        self.I_a = None         # Acceleration of Origin w.r.t. Inertial Frame

        self.I_R = None         # Transformation Matrix, Inertial Frame <- This Frame
#        self.R = R              # Transformation Matrix, Body Frame <- This Frame
        self.K_om = None        # Angular Velocity w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)
        self.K_al = None        # Angular Acceleration w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)


    def connectTo(self, body, type, axis, name = None, q0 = 0, qd0 = 0):
        '''
        Connect This Coordinate System to a Body
        body: Body which Shall Be Connected
        type: 'Rot' or 'Trans' for Rotational or Translational Joint
        axis: Axis of Translation/Rotation (normalised 3x1 vector)
        '''

        # TODO bad design, need this here to avoid circular import
        from pymbs.processing import Joint

        # Name of Bodies Involved
        pName = self.parentBody.name
        cName = body.name

        # Create a Joint Object
        if ((type == 'Rot') or (type == 'Rotation')):
            if (name is None): name = 'Rot_%s_%s' % (pName, cName)
            j = Joint(name, self, body, symbolics.zeros((3,)), axis, q0, qd0, self.graph)
        elif ((type == 'Trans') or (type == 'Translation')):
            if (name is None): name = 'Trans_%s_%s' % (pName, cName)
            j = Joint(name, self, body, axis, symbolics.zeros((3,)), q0, qd0, self.graph)
        else:
            raise NameError('Unknown Joint Type: %s' % type)

        # Connect Joint to Frame and Body
        self.joints += [j]
        body.joint = j

        return j
        