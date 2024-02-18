Simple Pendulum
---------------

A very easy problem, and therefore used as an introduction here for
PyMbs, is that of a swinging pendulum. If you want to try it first, or look
at the complete source code, see
:download:`simple_pendulum.py <../../../examples/simple_pendulum.py>`.

First, we import PyMbs functions and classes from ``pymbs.input``:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 10

Then we set up our initial reference frame, which will also serve as an item
to *collect* all the elements we add to our model. The three item list gives
the direction of the gravity vector, i.e. gravity will point in negative
Z-direction in this example:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 13

Now we list our parameters, in this case the mass, length and inertia of the
pendulum. Each gets a symbol, and you can use floating-point numbers or
expressions to set their values:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 16-18

We add a body to the system, representing the pendulum. It has a mass ``m`` and
an inertia ``I``:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 21

Sometimes it is convenient or even necessary to be able to refer to the current
position and orientation of a point on a body. To do this, you can add
additional coordinate systems to a body. Here, we would like on at the top of
the pendulum, ``link``, where the pendulum will be suspended from. You can
specify the point at which the coordinate system is located and its
orientation. Here, we only set the location to :math:`\frac12 l`, i.e. the end
of the pendulum:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 24

To limit the degree of freedom of our pendulum, we now add a joint to our
model.  The pendulum is supposed to swing around top end (``link``), so we
connect the ``world`` with ``pend.link``, and supply ``Ry`` to the ``dofList``.
``Ry`` means *rotation* around y-axis:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 27

Before we take a look at the behaviour of our system, we need to define
a visualization for the pendulum. A box will do nicely here:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 30

Now we can generate the equations of motion for our system:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 33

Finally we can take a look at our system and check how it behaves:

.. literalinclude:: ../../../examples/simple_pendulum.py
    :lines: 36

This is what it should look like:

.. only:: html

    .. image:: simple_pendulum.gif

.. only:: latex

    .. image:: simple_pendulum.jpg
        :scale: 50 %

In the end, the complete source looks like this:

.. literalinclude:: ../../../examples/simple_pendulum.py
