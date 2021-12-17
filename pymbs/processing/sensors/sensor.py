from pymbs.common.mbselement import MbsElement
from pymbs.processing import Frame
from pymbs.common.simple import simple
from pymbs.symbolics import Matrix, Variable, VarKind


class Sensor(MbsElement):
    '''
    Base Class For All Sensors
    '''

    def __init__(self, name, parent, child, symbol, ref = None, category=VarKind.Sensor):
        '''
        Constructor
        name: Name of the Sensor
        @param name: Name of the SensorElement
        @param symbol: Symbol Which is Used For Storing the Sensor Result
        @param parent: Parent Coordinate System (Sensor Compares Parent to Child)
        @param child: Child Coordinate System (Sensor Compares Parent to Child)
        @param ref: Result is Written w.r.t. ref Frame, if it is a Vector
        '''

        # If Reference Frame is None, then Use Parent as Reference
        if (ref is None):
            ref = parent

        # Assertions
        assert( isinstance(parent, Frame) )
        assert( isinstance(child, Frame) )
        assert isinstance(symbol, Variable), "Symbol must be a variable, not a %s with content %s"%(str(symbol.__class__),str(symbol))
        assert( isinstance(ref, Frame) )
        assert( isinstance(category, (int, list)) )

        # Constructor of SuperClass
        MbsElement.__init__(self, name)

        if (isinstance(symbol, str)):
            symbol=sym(symbol)

        # Save Parameters
        self.parent = parent
        self.child = child
        self.symbol = symbol
        self.ref = ref
        self.category = category

        # Delay Calculation after acceleration and forces have been calculated
        self.delayCalc = False


    def check(self):
        '''
        Checks, whether bodies are connected to the rest of the world
        '''

        def __check(cs):
            if (cs is not None):
                if (cs.I_R is None):
                    raise AttributeError('Frame "%s" on Body "%s" is not connected to the world!'%(cs.name, cs.parentBody.name))

        # Check Parent, Child and Reference
        __check(self.parent)
        __check(self.child)
        __check(self.ref)


    def calc(self, graph):
        '''
        Function is Overwritten by Derived Classes
        '''

        raise NotImplementedError


    def writeResult(self, graph, result):
        '''
        Adds Result to the List of Equations
        '''
        # Check that Equations is not None
        assert(graph is not None)

        if ( isinstance(self.ref.I_R, Matrix) ):
            print("Simplifying Result of Sensor '%s'"%self.name)
            result = simple(result)

        # Add it to the List
        graph.addEquation(self.symbol, result, varKind=self.category)
        

    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        raise NotImplementedError
