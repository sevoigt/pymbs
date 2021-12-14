About
=====

[![Build Status](https://drone.io/bitbucket.org/pymbs/pymbs/status.png)](https://drone.io/bitbucket.org/pymbs/pymbs/latest)

PyMbs is a tool to generate the equations of motion of holonomic multibody
systems. The equations can be exported to several target languages like Matlab,
Modelica, Python or C++. Thus the output of PyMbs is a mathematical model of
the mechanical system which can be simulated by timestep integration.

The description of the mechanical system is text-based but held as easy and
convenient as possible. The user assembles the system from a range of
predefined objects like bodies, joints, loops, sensors etc. Anyway this method
may provoke (typing) errors, especially when modelling more complex systems.
Therefore it is possible to create a visualisation for the mechanical system
from CAD-files or geometric primitives to check the consistency of the model.
It is also possible to manipulate the degrees of freedom interactively.


Download
============

[Download PyMbs for Windows](https://bitbucket.org/pymbs/pymbs/downloads#available-downloads)

[Download the source code](https://bitbucket.org/pymbs/pymbs/downloads#branch-downloads)
and choose the default branch

Documentation
=============

The documentation for PyMbs can be found at <http://pymbs.readthedocs.org/>.

Installation
============

Installation on Windows
-----------------------

Simply execute the installer

Installation on Linux
---------------------

Please note: PyMbs was mainly developed from 2009 to 2012 and thus is written in Python2.7. It is not kompatible with Python3.x.
To build and run PyMbs, you need to install several dependencies:

- a C++ Compiler
- cmake
- several python packages
    - numpy
    - scipy
    - matplotlib
- python bindings for
    - vtk
    - qt4
    - tk

On Debian/Ubuntu, this can be accomplished by:
 
    sudo apt-get install build-essential cmake python-numpy python-scipy python-vtk python-qt4 python-h5py python-matplotlib python-tk

Then, all you have to do is:

    sudo python setup.py install

To check your installation, and see an example of PyMbs in action, try:

    python Examples/Rope.py
