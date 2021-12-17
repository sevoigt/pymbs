from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose

import pymbs.symbolics as sym

class OrientationSensor(Sensor):
    '''
    Calculates the Relative Orientation between Parent and Child yielding a Transformation Matrix (Parent <- Child)
    '''

    def calc(self, graph):
        '''
        Calculate Transformation Matrix
        '''
        assert( graph is not None )        
        self.check()
        
        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        if (isinstance(self.parent.I_R, sym.Matrix)):
            R = transpose(self.parent.I_R)*self.child.I_R
        else:
            R = transpose(self.parent.I_R)*self.child.I_R
        
        # Add Result to the List of Equations
        self.writeResult(graph, R)
        
                
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (3,3)