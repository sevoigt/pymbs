import unittest

from pymbs.input import MbsSystem
from pymbs.common.functions import transpose

class TestInput(unittest.TestCase):

    def setUp(self):
        self.world = MbsSystem()
        self.m = self.world.addParam('m', 5)

    def test_getParentBody(self):

        body1 = self.world.addBody(self.m)
        body2 = self.world.addBody(self.m)

        body1.addFrame('CS1')
        body1.CS1.addFrame('CS2')
        body1.CS1.CS2.addFrame('CS3')
        parentBody = body1.CS1.CS2.CS3.getParentBody()

        self.assertTrue(body1 is parentBody)

    def test_expression(self):
        a = self.world.addParam('a', [0, 0, 0])
        exp1 = self.world.addExpression('Exp1', a*transpose(a)) 
        self.assertEqual(exp1.shape(), (3, 3))

    def test_gravity(self):
        # perfectly valid
        self.world.addParam('h', 4)
        # but g is already taken as a symbol by gravity
        with self.assertRaises(Exception):
            self.world.addParam('g', 4)

    def test_double_symbol(self):
        # can't have a symbol twice
        with self.assertRaises(Exception):
            self.world.addParam('m', 4)

    def test_addBody(self):
        free_body = self.world.addBody(self.m)
        body_list = list(self.world.bodyDict.values())
        self.assertEqual(len(body_list), 1)

    def test_addJoint(self):
        body = self.world.addBody(self.m)
        body.addFrame('joint_frame')
        self.world.addJoint(self.world, body.joint_frame,
                       ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'])

        jointList = list(self.world.jointDict.values())
        self.assertEqual(len(jointList), 6)

    def test_double_joint(self):
        # it should not be possible to use one body twice as parentBody of
        # joint-end-frames
        body = self.world.addBody(self.m)
        self.world.addJoint(self.world, body, 'Rx')
        with self.assertRaises(ValueError):
            self.world.addJoint(self.world, body, 'Tx')



if __name__ == '__main__':
    unittest.main()
