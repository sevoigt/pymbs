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
Created on 20.07.2009

@author: Christian Schubert
'''

from PyMbs.Common.MbsElement import MbsElement

eps = 1e-10

class Loop(MbsElement):
    '''
    Super Class for all Loops
    '''


    def __init__(self, name):
        '''
        Doesn't do anything really
        '''
        
        MbsElement.__init__(self, name)



    def calc(self, graph):
        '''
        Calculate v, Bvu und b_prime
        '''
        raise NotImplementedError
    

    
    def applyConstraintLoads(self):
        '''
        Apply Constraint Forces at the ends of cut joint
        '''
        raise NotImplementedError
        