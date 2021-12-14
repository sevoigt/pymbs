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

import unittest
import os
import sys

from mock import MagicMock
from PyMbs.Input import MbsSystem

class TestExamples(unittest.TestCase):
    
    def setUp(self):
        # Mock GUI call, to iterate over all examples without the need to close
        # each window
        MbsSystem.show = MagicMock()
        self.examples_dir = os.path.split(sys.argv[0])[0]
        self.controlled_examples_dir = os.path.join(self.examples_dir,
                                                    'controlledExamples')

    def test_simple_pendulum(self):
        script = os.path.join(self.examples_dir, 'SimplePendulum.py')
        exec(compile(open(script).read(), script, 'exec'))

    def test_mass_spring_damper(self):
        script = os.path.join(self.examples_dir, 'MassSpringDamper.py')
        exec(compile(open(script).read(), script, 'exec'))

    def test_four_bar_linkage(self):
        script = os.path.join(self.examples_dir, 'FourBarLinkage.py')
        exec(compile(open(script).read(), script, 'exec'))

    def test_rope(self):
        script = os.path.join(self.examples_dir, 'Rope.py')
        exec(compile(open(script).read(), script, 'exec'))

    def test_inverted_pendulum(self):
        controller_path = os.path.join(self.controlled_examples_dir,
                                       'InvertedPendulum')
        sys.path.append(controller_path)
        exec(compile(open(os.path.join(controller_path,'InvertedPendulum.py')).read(), os.path.join(controller_path,'InvertedPendulum.py'), 'exec'))

    def test_controlled_mass_spring_damper(self):
        controller_path = os.path.join(self.controlled_examples_dir,
                                       'MassSpringDamper')
        sys.path.append(controller_path)
        exec(compile(open(os.path.join(controller_path,'MassSpringDamper.py')).read(), os.path.join(controller_path,'MassSpringDamper.py'), 'exec'))

    @unittest.skip("Doesn't work, don't know why yet")
    def test_overhead_crane(self):
        controller_path = os.path.join(self.controlled_examples_dir,
                                       'OverheadCrane')
        sys.path.append(controller_path)
        print(sys.path)
        exec(compile(open(os.path.join(controller_path,'OverheadCrane.py')).read(), os.path.join(controller_path,'OverheadCrane.py'), 'exec'))

if __name__ == '__main__':
    unittest.main()
