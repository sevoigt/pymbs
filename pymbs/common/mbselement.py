"""
This module contains a common baseclass for all elements of a multibody system (mbs)
"""

from keyword import iskeyword
from pymbs.symbolics import Graph


class MbsElement:
    """
    Baseclass for all mbs elements for calculation purposes
    """

    def __init__(self, name, graph=None):
        """
        Constructor
        """
        assert isinstance(name, str), 'name must be a string'
        assert name.isidentifier(), 'name must be a valid identifier'
        assert not iskeyword(name), 'name must not be a python keyword'

        assert isinstance(graph, Graph) or (graph is None), \
            'Graph must be a symbolics.Graph object'

        self.name = name
        self.graph = graph

    def __repr__(self):
        """
        Return a human readable string representation of this object

        e.g. <Frame: 'CS_pendulum'>
        """

        return f'<{self.__class__.__name__}: \'{self.name}\'>'
