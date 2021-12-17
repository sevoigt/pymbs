from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose

import pymbs.symbolics as sym


class AngAccelerationSensor(Sensor):
    '''
    Calculates the Relative Angular Acceleration between Parent and Child w.r.t. ref
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()

        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        if (isinstance(self.parent.I_a, sym.Matrix)):
            alpha = transpose(self.ref.I_R)*(self.child.I_R*self.child.K_al - self.parent.I_R*self.parent.K_al)
        else:
            alpha = transpose(self.ref.I_R)*(self.child.I_al - self.parent.I_al)
            if (alpha == 0):
                alpha = sym.zeros(self.getShape())

        # Add Result to the List of Equations
        self.writeResult(graph, alpha)


    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (3,)