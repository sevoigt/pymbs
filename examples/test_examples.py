"""
Call several example scripts to test wether generation of equations works.
"""

import unittest
import os
import sys

from pymbs.input import MbsSystem


def mock_show(*args, **kwargs):
    """
    We use this to mock the GUI call, to iterate over all examples without
    the need to close each window.
    """
    return None


class TestExamples(unittest.TestCase):
    def setUp(self):
        MbsSystem.show = mock_show
        self.examples_dir = os.path.split(sys.argv[0])[0]
        self.controlled_examples_dir = os.path.join(self.examples_dir, "controlled")

    def test_simple_pendulum(self):
        script = os.path.join(self.examples_dir, "simple_pendulum.py")
        exec(compile(open(script).read(), script, "exec"))

    def test_mass_spring_damper(self):
        script = os.path.join(self.examples_dir, "mass_spring_damper.py")
        exec(compile(open(script).read(), script, "exec"))

    def test_four_bar_linkage(self):
        script = os.path.join(self.examples_dir, "fourbar_linkage.py")
        exec(compile(open(script).read(), script, "exec"))

    def test_rope(self):
        script = os.path.join(self.examples_dir, "rope.py")
        exec(compile(open(script).read(), script, "exec"))

    def test_inverted_pendulum(self):
        controller_path = os.path.join(self.controlled_examples_dir, "inverted_pendulum")
        sys.path.append(controller_path)
        exec(
            compile(
                open(os.path.join(controller_path, "inverted_pendulum.py")).read(),
                os.path.join(controller_path, "InvertedPendulum.py"),
                "exec",
            )
        )

    def test_controlled_mass_spring_damper(self):
        controller_path = os.path.join(self.controlled_examples_dir, "mass_spring_damper")
        sys.path.append(controller_path)
        exec(
            compile(
                open(os.path.join(controller_path, "mass_spring_damper.py")).read(),
                os.path.join(controller_path, "MassSpringDamper.py"),
                "exec",
            )
        )

    @unittest.skip("Doesn't work because module controller is already imported")
    def test_overhead_crane(self):
        controller_path = os.path.join(self.controlled_examples_dir, "overhead_crane")
        sys.path.append(controller_path)
        print(sys.path)
        exec(
            compile(
                open(os.path.join(controller_path, "overhead_crane.py")).read(),
                os.path.join(controller_path, "OverheadCrane.py"),
                "exec",
            )
        )


if __name__ == "__main__":
    unittest.main()
