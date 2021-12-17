from .mbselement import MbsElement
from pymbs.symbolics import Basic


class State(MbsElement):
    '''
    State Object holds all Elements which are part of the state, 
    including algebraic variables
    '''

    def __init__(self, q=None, qd=None, qdd=None, mu=None):
        '''
        Constructor
        '''

        if (q is None):
            q = list()
        if (qd is None):
            qd = list()
        if (qdd is None):
            qdd = list()
        if (mu is None):
            mu = list()

        # Achtung Assertions: prüfen auf int muss noch geändert werden
        # Assertions
        assert( isinstance(q, list) )
        for e in q:
            assert( isinstance(e, (Basic,int)) )
        assert( isinstance(qd, list) )
        for e in qd:
            assert( isinstance(e, (Basic,int)) )
        assert( isinstance(qdd, list) )
        for e in qdd:
            assert( isinstance(e, (Basic,int)) )
        assert( isinstance(mu, list) )
        for e in mu:
            assert( isinstance(e, (Basic,int)) )

        # Constructor of Super Class
        MbsElement.__init__(self, 'State')

        # Save Parameters
        self.q = q              # Positions
        self.qd = qd            # Velocities
        self.qdd = qdd          # Accelerations
        self.mu = mu            # Constraint Forces/Torques
