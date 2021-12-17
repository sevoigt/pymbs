from pymbs.processing.loads.load import Load
from pymbs.common.functions import transpose, sqrt

from pymbs.symbolics import Matrix, zeros


class PtPForce(Load):
    '''
    Point to Point Force
    '''


    def __init__(self, name, parent, child, force, ref = None):
        '''
        Constructor
        '''

        # Assertions
        assert(parent is not None)
        assert(child is not None)

        # Constructor of super class (ref=none, means ref = inertial frame)
        Load.__init__(self, name, parent, child, force, torque = None)
        # Setting ref to none means that the inertial frame is the reference frame
        self.ref = None


    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        Overwrites Function in Super Class
        '''

        # Calculate vector pointing from child to parent (w.r.t. inertial frame)
        r = self.parent.I_r - self.child.I_r
        if (isinstance(r, Matrix)):
            l = r.norm()
            r = r/l
        else:
            assert( graph is not None )
            r_sym = graph.addEquation( 'r_%s'%self.name, r )
            l = sqrt( transpose(r_sym) * r_sym )
            r = r_sym/l

        # Update Wf, Wft
        self.Wf = r
        self.Wt = zeros((3,))
