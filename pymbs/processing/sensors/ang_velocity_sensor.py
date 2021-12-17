from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose

import pymbs.symbolics as sym


class AngVelocitySensor(Sensor):
    '''
    Calculates the Relative Angular Velocity between Parent and Child w.r.t. ref
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()

        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        if (isinstance(self.parent.I_a, sym.Matrix)):
            om = transpose(self.ref.I_R)*(self.child.I_R*self.child.K_om - self.parent.I_R*self.parent.K_om)
        else:
            om = transpose(self.ref.I_R)*(self.child.I_om - self.parent.I_om)
            if (om == 0):
                om = sym.zeros(self.getShape())

        # Add Result to the List of Equations
        self.writeResult(graph, om)


    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (3,)