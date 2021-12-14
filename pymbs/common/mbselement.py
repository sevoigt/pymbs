# -*- coding: utf-8 -*-
'''
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
'''

'''
Created on 30.06.2009

@author: Christian Schubert
'''

from PyMbs.Symbolics import Graph

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

        self.name = name;
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
