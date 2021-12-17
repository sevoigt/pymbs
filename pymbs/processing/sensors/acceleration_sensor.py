from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose
import pymbs.symbolics as sym


class AccelerationSensor(Sensor):
    '''
    Calculates the Relative Acceleration between Parent and Child w.r.t. ref
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()
        
        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        a = transpose(self.ref.I_R)*(self.child.I_a - self.parent.I_a)
        if (a == 0):
            a = sym.zeros(self.getShape())
        
        # Add Result to the List of Equations
        self.writeResult(graph, a)
        
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (3,1)