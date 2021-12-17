import pymbs.symbolics as symbolics
from pymbs.symbolics import zeros, eye
from .frame import Frame
from pymbs.common.sidfilereader import SID, SIDFormatException

from pymbs.common.abstractbody import AbstractBody

import numpy as np

class Body(AbstractBody, Frame):
    """

    """
    def __init__(self, name, mass, cg, inertia):

        # super constructor (AbstractBody)
        AbstractBody.__init__(self, name, mass, cg, inertia)

        # super constructor (CoordianteSystem)
        Frame.__init__(self, name, parent=None, p=zeros((3,)), R=eye((3,3)))

        # save wheter a joint ends on this body
        # and prevent that more than one joint is connected to it
        self.endOfJoint = None

        # create a CS which may be used when that body
        # (by conveniece) is passed somewhere where a CS is expected

        self.addFrame('_CS_0')

    def _insertCS(self, cs):
        """
        similar to addFrame but instead of creating a new one
        here we take an existing instance
        """
        # this is needed to make all CS top-Level during transformation

        assert ((cs.__class__ is Frame) or (cs.__class__ is Body) or (cs.__class__ is FlexibleBody))

        self.children.append(cs) #!! not needed (may lead to confusion)
        self.coordList.append(cs)

        #?? not added to the objectnamespace because
        # this was just for writing convinience in the input file

    def getParentBody(self):
        return self



class FlexibleBody(AbstractBody, Frame):
    """

    """
    def __init__(self,name,filepath):

        # read a SID-File from the given path
        f = open(filepath, "r")
        try:
            # create a sid object which includes the informations for
            # generating a flexible body
            self.sid = SID(f)
        except SIDFormatException as fe:
            print("Datei konnte nicht eingelesen werden: " + fe.message)
        except:
            print("Unbekannter Fehler!")
        finally:
            # close SID-file
            f.close()


        cg = [0,0,0]
        inertia=symbolics.zeros((3,3))
        mass = 0

        # super constructor (AbstractBody)
        AbstractBody.__init__(self, name, mass, cg, inertia)

        # super constructor (CoordianteSystem)
        Frame.__init__(self, name, parent=None, p=zeros((3,)), R=eye((3,3)))


        # save wheter a joint ends on this body
        # and prevent that more than one joint is connected to it
        self.endOfJoint = None

        # create a CS which may be used when that body
        # (by conveniece) is passed somewhere where a CS is expected
        self.sid.node_List = list()
        for node in self.sid.modal.frame.Knoten:
            '''
            creating one frame per node
            '''
            # position of node i of undeflected body / zero order of originmatrix
            pos_node_numpy = node.origin.originmatrix.M0
            pos_node = np.array(pos_node_numpy).reshape(-1,).tolist()

            node_number = node.node_number
            node.frame = Frame.addFrame(self, name = 'node_%i'%node_number, p = pos_node,R = eye((3,3)))
            self.sid.node_List.append(node)

        self.node_list = self.sid.node_List

    def node(self, i):
        '''
        returns frame for node i
        '''
        if ((i < 1) or (i > self.sid.nNodes)):
            raise NotImplementedError("Node %i doesn´t exist! Index must be in range [1,%i]",(i,self.sid.nNodes))
        node_i = self.sid.node_List[i-1]
        return node_i.frame





##        self.node_List = list()
##        for nodes in xrange(self.sid.nNodes):
##            '''
##            creating one frame per node
##            '''
##            node_number = nodes+1
##            frame = Frame.addFrame(self, name = 'node_%i'%node_number, p = zeros((3,)),R = eye((3,3)))
##            self.node_List.append(frame)
##
##
##    def node(self, i):
##        '''
##        returns frame for node i
##        '''
##        if ((i < 1) or (i > self.sid.nNodes)):
##            raise NotImplementedError("Node %i doesn´t exist! Index must be in range [1,%i]",(i,self.sid.nNodes))
##        return self.node_List[i-1]


    def _insertCS(self, cs):
        """
        similar to addFrame but instead of creating a new one
        here we take an existing instance
        """
        # this is needed to make all CS top-Level during transformation

        assert ((cs.__class__ is Frame) or (cs.__class__ is Body) or (cs.__class__ is FlexibleBody))

        self.children.append(cs) #!! not needed (may lead to confusion)
        self.coordList.append(cs)

        #?? not added to the objectnamespace because
        # this was just for writing convinience in the input file

    def getParentBody(self):
        return self


    def addFrame(self, *args, **kwargs):
        '''
        Calling addFrame is not permitted for a flexible body
        '''
        raise NotImplementedError("Calling addFrame is not permitted for a flexible body")