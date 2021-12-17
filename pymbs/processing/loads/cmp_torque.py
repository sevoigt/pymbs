from pymbs.processing.loads.load import Load

from pymbs.symbolics import eye

class CmpTorque(Load):
    '''
    Point to Point Force
    '''


    def __init__(self, name, parent, child, torque, ref):
        '''
        Constructor
        '''

        Load.__init__(self, name, parent, child, None, torque, ref)
        
        
        
    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        '''

        self.Wf = None        
        self.Wt = eye((3,3))       # Rotation
