Mass-Spring-Damper System
-------------------------

Another commonly used introductory system is the mass-spring-damper system.
A mass connected to a spring and a damper is displaced and then oscillates
in the absence of other forces. If you want to try it first, or look
at the complete source code,
see :download:`MassSpringDamper.py <../../Examples/MassSpringDamper.py>`.

As in the previous example, we import PyMbs, set up an intial reference frame
and add a few parameters:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 27-29,31-33

Now we add the mass to the system and add a joint. ``Tz`` means that the mass
can only undergo *translation* in z-direction. None if this is really new, but
we add another parameter to the joint, ``startVals=1``, which means the mass
will have an initial displacement of one:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 36-37

This is where it gets interesting. We want to add a force to the system
to simulate the influence of a spring and damper on the mass. First, we need
know the displacement and velocity of the mass. Therefore, we add a distance
sensor:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 40

The ``Distance`` sensor measures the scalar distance between two coordinate
systems, here the systems of ``movingBody`` and ``world``. Additionally, it
measures the velocity. You can access the distance with ``l[0]`` and the
velocity with ``l[1]``. The force for a spring is :math:`F_s = - c x`, with
:math:`c` being the spring constant, and for a damper with a damping
coefficient of :math:`d` the force is :math:`F_d = - d \dot{x}`. Therefore,
we add an expression that includes both:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 41

So far, this only calculates the value of the force, but it isn't applied to
the system. To change this, we add a ``PtPForce`` between the body and the
world:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 42

As in the previous example, we need to add visualisations so that we 
can see the movement of our system:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 45-46

In the end, we generate the equations of motion for our system and start the
representation:

.. literalinclude:: ../../Examples/MassSpringDamper.py
    :lines: 49-50 

This is what it should look like:

.. only:: html

    .. image:: MassSpringDamper.gif

.. only:: latex

    .. image:: MassSpringDamper.jpg
        :scale: 50 %

In the end, the complete source looks like this:

.. literalinclude:: ../../Examples/MassSpringDamper.py
