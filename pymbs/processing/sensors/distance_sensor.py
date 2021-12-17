from pymbs.processing.sensors.sensor import Sensor
from pymbs.common.functions import transpose, sqrt
from pymbs.common.simple import simple

from pymbs.symbolics import Matrix 


class DistanceSensor(Sensor):
    '''
    Calculates the Distance and its Derivative between two Coordinate Systems
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()
        
        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        r = self.parent.I_r - self.child.I_r
        v = self.parent.I_v - self.child.I_v
        # Explicit if r is a Matrix
        if (isinstance(r, Matrix)):
            d = r.norm()
            d = simple(d)
            
            d_dot = transpose((r/d))*v
            d_dot = simple(d_dot[0,0])
        else:
            r_sym = graph.addEquation('r_%s'%self.name, r)
            l_sym = graph.addEquation('l_%s'%self.name, sqrt( transpose(r_sym) * r_sym ) )                                       
            v_sym = graph.addEquation('v_%s'%self.name, v)
            d = l_sym 
            d_dot = transpose(r_sym)*v_sym / l_sym 
        
        # Add Result to the List of Equations
        self.writeResult(graph, Matrix([d, d_dot]))
        
        
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (2,)