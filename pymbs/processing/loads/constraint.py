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
Created on 07.07.2009

@author: Christian Schubert
'''

from PyMbs.Common.State import State

from PyMbs.Processing.LoadElements.Load import Load
from PyMbs.Processing.Frame import Frame
from PyMbs.Common.Functions import transpose, element

from PyMbs.Common.Simple import simple

from PyMbs.Symbolics import Matrix, eye


class Constraint(Load):
    '''
    Constraint, Constrains the Relative Movements Between a Parent and a Child Coordinate System
    '''

    def __init__(self, name, parent, child, transLock=[0,0,0], rotLock=[0,0,0], active=True):
        '''
        Constructor
        name: Name of the Constraint
        parent: Parent Coordinate System
        child: Child Coordinate System
        transLock: which Relative Movements are Free (=0) or Locked (=1), 3x1 Vector [Tx, Ty, Tz]
        rotLock: which Kardan Rotation (X-Y-Z) are Free (=0) or Locked (=1), 3x1 Vector [Rx, Ry, Rz]
        '''

        # Assertions
        assert isinstance(transLock, (list,Matrix)), "rotLock was supposed to be a list but is %s (%s)"%(transLock,transLock.__class__)
        assert len(transLock)==3, "transLock must be list of length 3, not %s with "%(len(transLock), transLock)
        assert isinstance(rotLock, (list,Matrix)), "rotLock was supposed to be a list but is %s (%s)"%(rotLock,rotLock.__class__)
        assert len(rotLock)==3, "rotLock must be list of length 3, not length %s with %s"%(len(rotLock), rotLock)

        # Call Constructor of Load
        Load.__init__(self, name, parent, child, force=None, torque=None, ref=parent)

        # Assign Parameters
        self.transLock = transLock
        self.rotLock = rotLock
        self.active = active



    def calc(self, graph, states):
        '''
        Everything has happened in initialize already
        '''
        try:
            if (self.force is not None):
                return True
        except:
            return False



    def initialize(self, graph, states):
        '''
        Calculcate Wf and Wt
        '''

        # Init
        self.transPick = []
        self.rotPick = []
        constraint_force = []
        constraint_torque = []

        # Constraints
        axis = ['X', 'Y', 'Z']
        for i in [0,1,2]:
            # Translation
            if (self.transLock[i]):
                tmp = [0,0,0]; tmp[i] = 1
                self.transPick.append(tmp)
                constraint_force.append(graph.addVariable('F_%s_%s'%(axis[i], self.name)))

            # Rotation
            if (self.rotLock[i]):
                tmp = [0,0,0]; tmp[i] = 1
                self.rotPick.append(tmp)
                constraint_torque.append(graph.addVariable('T_%s_%s'%(axis[i],self.name)))

        # Number of Constraints
        nf = len(constraint_force)
        nt = len(constraint_torque)

        # Add Forces/Torques to the State
        states.mu.extend( constraint_force )
        states.mu.extend( constraint_torque )

        # Set Up Constraint Forces
        if (nf == 0):
            self.force = None
        elif (nf == 1):
            self.force = constraint_force[0]
        else:
            self.force = Matrix(constraint_force)

        # Set Up Constraint Torques
        if (nt == 0):
            self.torque = None
        elif (nt == 1):
            self.torque = constraint_torque[0]
        else:
            self.torque = Matrix(constraint_torque)

        # Define Constraint - Force
        if (self.force is not None):
            self.Wf = transpose(Matrix(self.transPick))
        else:
            self.Wf = None

        # Define Constraint - Force
        if (self.torque is not None):
            # Compare Bases of Parent and Child
            # The two Axes of Rotation must always be Perpendicular to Each Other
            # Bases are Hidden Within the Transformation Matrices
            # Positions of Matrix Elements which have to be zero

            # Init Torque Distribution Matrix
            self.Wt = transpose(Matrix(self.rotPick))
        else:
            self.Wt = None


    def getConstraint(self, graph, n):
        '''
        Returns the Constraint, differentiated n times
        '''
        assert graph is not None
        assert isinstance(n, int)
        assert n in [0,1,2]
        phi = []

        # Define Constraints - Translation
        if (self.force is not None):
            # Position
            if (n == 0):
                phiTrans = self.parent.I_r - self.child.I_r
            # Velocity
            elif (n == 1):
                phiTrans = self.parent.I_v - self.child.I_v
            # Acceleration
            elif (n == 2):
                phiTrans = self.parent.I_a - self.child.I_a
            else:
                raise AttributeError('n must be either 0, 1, 2')

            if (isinstance(self.parent.I_R, Matrix)):
                print("Simplifying Translational Constraint %s"%self.name)
                phiTrans = simple(phiTrans)

            phi_t = graph.addEquation('phi_trans_%s_%i'%(self.name,n), phiTrans)
        else:
            phi_t = None


        # Define Constraints - Rotation
        if (self.torque is not None):
            # Position
            if (n == 0):
                phiRot = self.parent.I_R*transpose(self.child.I_R)
            # Velocity
            elif (n == 1):
                phiRot = self.parent.I_om - self.child.I_om
            # Acceleration
            elif (n == 2):
                phiRot = self.parent.I_al - self.child.I_al
            else:
                raise AttributeError('n must be either 0, 1, 2')

            if (isinstance(self.parent.I_R, Matrix)):
                print("Simplifying Rotational Constraint %s"%self.name)
                phiRot = simple(phiRot)

            phi_r = graph.addEquation('phi_rot_%s_%i'%(self.name,n), phiRot)
        else:
            phi_r = None

        # Add Constraint to list of Equations
        rotPosPick = [(2,1), (2,0), (1,0) ]
        for i in [0,1,2]:
            if (self.transLock[i]):
                phi.append( phi_t[i] )
        for i in [0,1,2]:
            if (self.rotLock[i]):
                if (n == 0):
                    index = rotPosPick[i]
                    phi.append( phi_r[ index ] )
                else:
                    phi.append( phi_r[i] )

        return phi