[![Documentation Status](https://readthedocs.org/projects/pymbs/badge/?version=latest)](https://pymbs.readthedocs.io/?badge=latest)

# PyMbs - Python Multibody Systems

## About

PyMbs is a tool to generate the equations of motion of holonomic multibody
systems. The equations can be exported to several target languages like Matlab,
Modelica, Python or C/C++. Thus the output of PyMbs is a mathematical model of
the mechanical system which can be simulated by timestep integration.

The description of the mechanical system is text-based (pure Python) but held as
easy and convenient as possible. The user assembles the system from a range of
predefined objects like bodies, joints, loops, sensors etc. Anyway this method
may provoke (typing) errors, especially when modelling more complex systems.
Therefore it is possible to create a visualisation for the mechanical system
from CAD-files or geometric primitives to check the consistency of the model.
It is also possible to manipulate the degrees of freedom interactively.

## Documentation

The documentation for PyMbs can be found at <http://pymbs.readthedocs.org/>.

## Installation

PyMbs is developed using Python 3.13. It will most likely work on other 3.x versions too as long as the dependencies (requirements.txt) are met.
It has been tested and works on Windows 10/11 including WSL, Fedora Linux and Apple M1.

There is currently no package available since PyMbs comes with a symbolic math
module written in C++ that needs to be compiled. To use PyMbs you have to

- clone the repo
- build the symbolics module for your platform using CMake
- add PyMbs to your environment/path so you can import it

To check your installation and see an example of PyMbs in action, try:

    python examples/rope.py
