Simulation
----------

If you choose to use the graphical user interface (usually invoked with
:func:`PyMbs.Input.MbsSystem.show`), you can use PyMbs to simulate the
behaviour of your system. Essentially, what this does is writing the equations
of motions to a Python file in a temporary directory using the usual code
generation mechanism of PyMbs. Then it uses an ODE integrator from scipy to
calculate the coordinates of the system at the next point in time. These can
then be visualised in the GUI, provided you have supplied visualisation
information in your script.

For larger systems, Python can become too slow to evaluate the time derivate
for a smooth display of the motion of the system. In this case, you can use the
buttons *Compile Model in Fortran* or *Compile Model in C* to speed up the
calculation. Basically, this writes the time derivate as a Fortran or C
function, as you would with :class:`PyMbs.Input.MbsSystem.GenCode`, compiles it
to a shared library, and calls this function using a Python wrapper.

Control
-------

The controller functionality provides the possibility to introduce loads whose
value is determined by arbitrary Python code.

When you use :func:`PyMbs.Input.MbsSystem.addController`, the control function
you supply is imported into the Python file containing the time derivative
function. So for each time step, your control function is called, and then the
result is used in the time derivative. This also works in conjunction with the
*Compile Model in C* button, where your function is called from the Python
wrapper. Currently, compiling a model with Fortran with a controller is not
supported.

For an introduction on how to use controllers take a look at the
:ref:`inverted_pendulum` example. Additional examples with controllers can be
found at ``examples/controlled``.
