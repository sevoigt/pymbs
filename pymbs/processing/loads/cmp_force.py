from pymbs.processing.loads.load import Load
from pymbs.symbolics import eye

class CmpForce(Load):
    '''
    Point to Point Force
    '''


    def __init__(self, name, parent, child, force, ref):
        '''
        Constructor
        '''

        Load.__init__(self, name, parent, child, force, None, ref)
        
        
    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        '''

        self.Wf = eye((3,3))       # Translation caused by force
        self.Wt = None
        