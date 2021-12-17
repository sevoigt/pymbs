from pymbs.symbolics import Graph

class MbsElement(object):
    '''
    Basic Class Of All Mbs Elements For Calculation Purposes
    '''

    def __init__(self, name, graph=None):
        '''
        Constructor
        '''
        assert isinstance(name, str), "Name must be a string"        
        assert (isinstance(graph, Graph) or (graph is None)), "Graph must be a Symbolics.Graph object"

        # Replace Blanks by Underscores
        name = name.replace(" ", "_")

        self.name = name
        self.graph = graph
        

    def __repr__(self):
        """
        return a human readable string representation of this object

        e.g. <Frame: 'CS_pendulum'>
        """
        # maybe self.__class__.__module__ is also of interest
        return "<%s: '%s'>" % (self.__class__.__name__, self.name)


#    def __eq__(self, other):
#        """
#        If the 'thing' it shall be compared to is a string, than compare it to its name
#        otherwise do a normal compare (whatever that is - does not work)
#        """
#        if (isinstance(other, str)):
#            return (self.name == other)
#        else:
#            return super(MbsElement, self) == other
