from pymbs.processing import Body
from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose

import pymbs.symbolics as sym


class EnergySensor(Sensor):
    '''
    Obtain the potential and kinetic energy of a body
    '''

    def __init__(self, name, body, symbol, gravity, category=sym.VarKind.Sensor):
        '''
        Constructor
        '''

        assert( isinstance(body, Body) )

        # Call Constructor of Super Class
        Sensor.__init__(self, name, body.CS_0, body.CS_0, symbol, None, category)
        self.gravity = gravity
        self.body = body

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )

        # get mass and Inertia of body
        m = self.body.mass
        I = self.body.inertia

        # get potential Energy
        assert self.body.I_l is not None, 'Position of Body "%s" has not been calculated!'%self.body
        E_pot = -m*transpose(self.gravity)*self.body.I_l

        # get kinetic energy
        assert self.body.I_ldot is not None, 'Velocity of Body "%s" has not been calculated!'%self.body
        E_kin = 0.5*m*transpose(self.body.I_ldot)*self.body.I_ldot + \
                0.5*transpose(self.body.K_om)*I*self.body.K_om

        # Total Energy
        E = E_pot + E_kin
        if isinstance(E, sym.Matrix):
            assert E.shape == (1,1)
            E = E[0]

        self.writeResult(graph, E)

    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (1,1)