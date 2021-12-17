from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose



class VelocitySensor(Sensor):
    '''
    Calculates the Relative Velocity between Parent and Child w.r.t. ref
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()
        
        # Calculate Vector Pointing From Parent to Child (w.r.t. Inertial Frame)
        v = transpose(self.ref.I_R)*(self.child.I_v - self.parent.I_v)
        if (v == 0):
            v = pymbs.Symbolics.zeros(self.getShape())
        
        # Add Result to the List of Equations
        self.writeResult(graph, v)
        
        
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (3,1)