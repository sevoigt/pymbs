from pymbs.processing.sensors.sensor import Sensor
from pymbs.processing import Joint

import pymbs.symbolics as sym


class JointSensor(Sensor):
    '''
    Obtains a Joint's State of Motion (Position, Velocity)
    '''

    def __init__(self, name, joint, symbol, category=sym.VarKind.Sensor):
        '''
        Constructor
        '''

        assert( isinstance(joint, Joint) )

        # !! this causes trouble because there is no parent and child
        #self.check()

        # Call Constructor of Super Class
        Sensor.__init__(self, name, joint.body.CS_0, joint.coordSys, symbol, None, category)
        self.joint = joint


    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )

        # Calculate Vector Pointing From Parent to Child (w.r.t. Inertial Frame)
        # we cannot include qdd here
        # it would cause a loop in the equations if this sensor was used to calculate a force or torque, since qdd = f(qdd, ...)
        self.writeResult(graph, sym.Matrix([self.joint.q, self.joint.qd]))


    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (2,1)