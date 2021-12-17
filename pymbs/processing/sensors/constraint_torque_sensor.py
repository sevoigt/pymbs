from pymbs.processing.sensors.sensor import Sensor
from pymbs.processing import Joint

import pymbs.symbolics as sym


class ConstraintTorqueSensor(Sensor):
    '''
    Obtains a the Constraint Torque exerted by a joint
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
        self.delayCalc = True


    def calc(self, graph):
        '''
        Read out the Force field of the joint object 
        '''
        assert( graph is not None )

        # Warning if explicit generator is used
        if ('L' not in self.joint.__dict__):
            print("Warning: Calculation of Constraint Torques not yet supported for 'explicit=True'")
        
        self.writeResult(graph, self.joint.L)


    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (3,1)