from pymbs.common.mbselement import MbsElement

class MbsInputElement(MbsElement):
    """
    This class provides basic properties for all mbs-elements:
        name
        predecessor (parent)
        successor (child)
    """

    def __init__(self, name, parent):
        MbsElement.__init__(self, name, None)
        self.parent = parent
        self.children = []

        # Reference to the coresponding processing object
        self.referenceToNew = None

    def _getChild(self, idx=0):
        return self.children[idx]

    def _setChild(self, val):
        if len(self.children) == 0:
            self.children.append(val)
        else:
            self.children[0] = val

    # make "child" a shortcut to "self.children[0]"
    child = property(_getChild, _setChild)
