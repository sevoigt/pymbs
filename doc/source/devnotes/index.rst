Developer Notes
===============

Development History
-------------------

PyMbs started off as a tool to generate the equations of motions of multibody
system for external tools, such as MATLAB or Modelica. This is still at the
core of PyMbs. Initially, sympy was used to generate these equations. This
turned out to be inadequate in terms of performance for larger systems, so a
dedicated symbolic computation module has been written in C++. Over time, a
graphical user interface with the ability to simulate the model has been added,
as well the ability to define sensors and kinematic loops. Recently, the
ability to influence the behaviour of your system by use of a controller has
been implemented.

Structure of the Repository
---------------------------

To get a feel for the project and where a particular peace of code is residing,
here is a quick rundown of the most commonly used files and directories.

``setup.py`` is the starting point for building, distributing and installing
a Python module, this goes for PyMbs as well.

``builder.py`` handles the extra steps required to build the symbolics module
from C++ using CMake. It is called from ``setup.py`` when necessary.

``common/`` contains the base classes for bodies, frames, etc.

``doc/`` contains the Sphinx documentation for PyMbs.

``examples/`` contains various examples of PyMbs models. Take a look at
``examples/controlled`` for models using a controller. Under
``examples/misc`` you'll find old examples that may or may not work
with current versions of PyMbs.

``Graphics/`` includes all the code responsible for creating the graphical
user interface.

``input/`` is the main module that defines the user interface of PyMbs
scripting. In particular, ``input/mbs_system.py`` is the starting point in all
PyMbs scripts. Start here if you want to find out how a particular feature is
implemented.

``processing/`` the code for :ref:`loads`, :ref:`sensors`, :ref:`loops`, etc.
resides here.

``symbolics/`` all of the "heavy lifting" is done in this C++ module. If you
want to change the output for a code generator, you'll have to modify the
respective file in ``symbolics/writer/``.

``test/`` contains the unittests. Some of them are obsolete, others need
expansion. Writing new tests is a great way to get started on improving PyMbs!

Building PyMbs under Windows
----------------------------

To build PyMbs under Windows, you need `CMake <www.cmake.org>`_ and `Visual
Studio (Express) <www.visualstudio.com>`_. Once you have VS installed, start a
Visual Studio command prompt (the normal windows command prompt will not work,
take a look under Visual Studio in the Start Menu), ``cd`` to your PyMbs source
directory and execute::

    python setup.py install

This should generate all the files necessary and copy them to the appropriate
locations. Alternatively, you can use the CMake GUI and generate a Visual
Studio project file which you can then build using the Visual Studio IDE.

Running the Tests
-----------------

There are a couple of files containing unittests. Running each of these
commands should result in no error::

    python test/test_symbolics.py
    python test/test_input.py
    python symbolics/test_symbolics.py
    python examples/test_examples.py


Working on the Documentation
----------------------------

A very good, short introduction to the Sphinx syntax can be found at
`Documenting Your Project Using Sphinx <https://pythonhosted.org/an_example_pypi_project/sphinx.html#full-code-example>`_.

Building the Documentation
""""""""""""""""""""""""""

The documentation at `Read the Docs <http://pymbs.readthedocs.org/en/latest/>`_
is automatically build on every commit.

For building a local copy of the HTML documentation, you can call Sphinx like
this::

    sphinx-build <documentation-directory> <target-directory>

Commonly, if you are in the PyMbs source directory, you would call::

    sphinx-build doc doc/.build

To then take a look at your newly built documentation, direct your browser to
``doc/.build/index.html``.

If you want to build the PDF documentation, you need to have LaTeX installed
as well. Then you need to call Sphinx with the option ``-b latex`` to select
the LaTeX builder, e.g.::

    sphinx-build -b latex doc doc/.latex

This generates all the necessary ``.tex`` files, but does not execute PDF
generation. To do this, go to the target directory (``doc/.latex``) and
call ``make``. This will call ``pdflatex`` a number of times and result in
a file called ``PyMbs.pdf`` in that directory.

Sphinx Hints
""""""""""""

In some cases it has been necessary to use some of the more obscure features
of Sphinx. These are documented here.

-----

**Avoid substitutions in autodoced function definitions**

The *autodoc* extension will execute some of your Python code as it imports
your module/class. In some cases, this will lead to unwanted results.
Consider this example code::

    x = 3
    def my_function(foo_bar=x):
        pass

This would be rendered as

.. function:: my_function(foo_bar=3)

by *autodoc*. If you want to circumvent this, you have to duplicate the
function definition in the first line of the docstrings, i.e.::

    x = 3
    def my_function2(foo_bar=x):
        """
        my_function2(foo_bar=x)
        """
        pass

which now renders as intended

.. function:: my_function2(foo_bar=x)

In PyMbs, this has been used to document e.g.
:func:`pymbs.input.MbsSystem.addFrame`.

-----

**Document instance variables**

*Autodoc* extracts the docstrings of your classes and methods to build parts of
the documentation, in our case most of the :ref:`reference` section.  Variables
and instance variables usually don't have docstrings though.  However, we
expose the interfaces to :ref:`loads`, :ref:`loops`, :ref:`sensors`, etc. using
instance variables. To document these entities, precede them with Python
comments, followed by a colon. For example, the interface
:class:`PyMbs.Input.MbsSystem.addLoad` has been documented like this::

    #: addLoad contains a :class:`PyMbs.Input.MbsSystem.AddLoad` instance,
    #: see :ref:`loads` for the different load types available
    #:
    #: >>> world.addLoad.PtPForce(F, body_one, body_two)
    self.addLoad = AddLoad(self)

Alternativly use can add an `ivar` (instance variable) info field to the
docstring of the class. However, these can not be linked to and should
therefore be using sparingly.

-----

**Animation in HTML output, static image in PDF**

The examples have a small animation of the simulation attached to them. They
are in the form of ``.gif`` files; however, they can't be included in the
LaTeX version of the documentation. Because of this, a different file is to
be included depending on the builder::

    .. only:: html

        .. image:: simple_pendulum.gif

    .. only:: latex

        .. image:: simple_pendulum.jpg            :scale: 50 %

-----

**Source code from Examples**

The :ref:`examples` are documented by including the actual source code from
``<pymbs-root>/examples`` with the ``literalinclude`` directive, e.g.::

    .. literalinclude:: ../../examples/simple_pendulum.py
        :lines: 29-31

This includes the lines 29 through 31 in the documentation.

.. warning::
    When you change the example ``.py`` files in ``<pymbs-root>/examples``,
    make sure you also change the line numbers in the documentation
    accordingly!
