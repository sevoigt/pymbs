.. _inverted_pendulum:

Inverted Pendulum
-----------------

In this example, we would like to introduce the controller functionality of
PyMbs. With it, you can introduce loads depending on the state of the system
using arbitrary python code. We want to simulate a simple pendulum hanging from
a travelling trolley, with a force acting on the trolley to keeep the pendulum
in an upright position. If you want to try it first, or look at the complete
source code, see :download:`inverted_pendulum.py
<../../../examples/controlled/inverted_pendulum/inverted_pendulum.py>`.
Note: You'll also need the corresponding :download:`controller.py
<../../../examples/controlled/inverted_pendulum/controller.py>`
to run the example.

We will start again with importing PyMbs, but also importing the
``controlForce`` residing in our ``controller.py``. We also set up our initial
frame of reference:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
    :lines: 14-22

We add the masses of the trolley and the load, as well as the two bodies
themselves:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
    :lines: 24-28

The trolley can move translational and the pendulum rotates around the mounting
point on the trolley:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
    :lines: 30-31

Now the interesting bit. First, we add a controller, which gets a symbol ``'F'``,
the handle of the function describing the controller ``controlForce`` and for
non-scalar controllers, the resulting shape ``(3, )``, i.e. this controller
returns a vector:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
    :lines: 33

Now we can use this object in any expression or load we would like. Here we add
a ``CmpForce`` between the world and the trolley:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
    :lines: 34

Let's take a quick look at the controller itself. The source code is

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/controller.py

If you want to supply your own controller, the function has to adhere to the
format ``functionname(time, coordinatevec, sensors)``, where ``time`` is the
current simulation time, ``coordinatevec`` is a vector containing the
generalised coordinates and their time derivative. In this example, we take
the difference between the current generalised coordinates ``y`` and the
point we would like to reach ``y0`` and multiply this by the vector ``k``,
which was predetermined for the particular parameters of this example.
Of course, you could just as well calculate it in this function. The result
is the x-component of our force.

All that is left is to define the visualisations, generate our equations of
motion and take a look at the system:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
    :lines: 36-43

This results from running the simulation:

.. only:: html

    .. image:: inverted_pendulum.gif

.. only:: latex

    .. image:: inverted_pendulum.jpg
        :scale: 50 %

In the end, the complete source looks like this:

.. literalinclude:: ../../../examples/controlled/inverted_pendulum/inverted_pendulum.py
