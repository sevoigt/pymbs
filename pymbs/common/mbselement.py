from pymbs.symbolics import Graph

class MbsElement(object):
    """
    Basic Class Of All Mbs Elements For Calculation Purposes
    """

    def __init__(self, name, graph=None):
        """
        Constructor
        """
        assert isinstance(name, str), 'name must be a string'
        assert (isinstance(graph, Graph) or (graph is None)), \
            'Graph must be a symbolics.Graph object'
                
        self.name = name.replace(" ", "_")
        self.graph = graph
        

    def __repr__(self):
        """
        Return a human readable string representation of this object

        e.g. <Frame: 'CS_pendulum'>
        """
        # maybe self.__class__.__module__ is also of interest
        return f'<{self.__class__.__name__}: "{self.name}">'


#    def __eq__(self, other):
#        """
#        If the 'thing' it shall be compared to is a string, than compare it to its name
#        otherwise do a normal compare (whatever that is - does not work)
#        """
#        if (isinstance(other, str)):
#            return (self.name == other)
#        else:
#            return super(MbsElement, self) == other
