from pymbs.common.mbselement import MbsElement
from pymbs.processing import Frame
import pymbs.symbolics as symbolics


class Load(MbsElement):
    '''
    Load applied to a Coordinate System
    '''


    def __init__(self, name, parent, child, force=None, torque=None, ref = None):
        '''
        Constructor
        name: Name of the Load
        force: 3x1 Force Vector (w.r.t. ref)
        torque: 3x1 Torque Vector (w.r.t. ref)
        ref: Reference Frame
        '''

        # Type Conversions
        if (ref is None): ref = parent

        # Assertions
        assert( isinstance(name, str) )
        assert( isinstance(parent, Frame) )
        assert( isinstance(child, Frame) or (child is None) )
        assert( isinstance(force, symbolics.Symbol) or (force is None) )
        assert( isinstance(torque, symbolics.Symbol) or (torque is None) )
        assert( isinstance(ref, Frame) )


        # Call Constructor of MbsElement
        MbsElement.__init__(self, name)

        # Assign Parameters
        self.parent = parent
        self.child = child
        self.force = force
        self.torque = torque
        self.ref = ref

        # Connect to Parent and Child Coordinate System
        parent.loads.append(self)
        if (child is not None):
            child.loads.append(self)


    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        '''

        raise NotImplementedError
