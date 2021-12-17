from pymbs.common.mbselement import MbsElement

eps = 1e-10

class Loop(MbsElement):
    '''
    Super Class for all Loops
    '''


    def __init__(self, name):
        '''
        Doesn't do anything really
        '''
        
        MbsElement.__init__(self, name)



    def calc(self, graph):
        '''
        Calculate v, Bvu und b_prime
        '''
        raise NotImplementedError
    

    
    def applyConstraintLoads(self):
        '''
        Apply Constraint Forces at the ends of cut joint
        '''
        raise NotImplementedError
        