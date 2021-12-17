from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose

from pymbs.symbolics import zeros


class PositionSensor(Sensor):
    '''
    Calculates the Relative Position between Parent and Child w.r.t. ref
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()

        # Calculate Vector Pointing From Parent to Child (w.r.t. Inertial Frame)
        r = transpose(self.ref.I_R)*(self.child.I_r - self.parent.I_r)
        if (r == 0):
            r = zeros(self.getShape())

        # Add Result to the List of Equations
        self.writeResult(graph, r)


    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (3,)