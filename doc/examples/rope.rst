Rope
----

Now we'll try to simulate a rope. Here you'll see the advantage of PyMbs in
that it is fully scriptable using Python. If you want to try it first, or look
at the complete source code, see
:download:`Rope.py <../../Examples/Rope.py>`.

We start with importing ``time`` (you'll see why in a moment) and PyMbs, and
set up our initial frame of reference as before:

.. literalinclude:: ../../Examples/Rope.py
    :lines: 25-28

To make our script a bit more flexible, we add the length of the rope, the
rotational damping, the mass density, the radius and the number of segments
used to simulate it as variables. Additionally, we choose which method is to
be used to derive the equations of motion:

.. literalinclude:: ../../Examples/Rope.py
    :lines: 30-37

Now we initialize the lists that will hold our bodies and joints. Also, we
calculate the length, mass and inertia of each rope segment:

.. literalinclude:: ../../Examples/Rope.py
    :lines: 40-46

The interesting part is creating all the bodies and joints. We loop over the
number of segments, create a body and an additional frame at the end for each 
segment. We then connect the first segment to the world and the other segments
to each other. A simple line is used for visualisation. In the end, a joint
load is added to each joint, with its value being calculated from multiplying
the angular velocity (``s[1]``) with the rotational damping ``d``:

.. literalinclude:: ../../Examples/Rope.py
    :lines: 49-65

Method of Equation Generation
"""""""""""""""""""""""""""""

Now we get to the issue of which method to choose for generating the equations
of motion. The choice is between *Explicit*, *Recursive* and *OrderN*. The
explicit generation scheme is comparatively slow and should not be used for
larger systems. On my system, using eight segments as a benchmark, the 
*Explicit* scheme took 12.9 seconds to generate the equations, *OrderN* took 
1.06 seconds and the *Recursive* one 0.05 seconds.

You can test these for yourself using the ``time`` module:

.. literalinclude:: ../../Examples/Rope.py
    :lines: 67-74

Not only is the time to generate the equations different, the complexity
of the equations differs as well. Again for eight segments, the python code
generated that is used for the simulation has 26.6 million characters (!) when
using *Explicit*, but only 172 thousand when using *OrderN* and 34 thousand
for *Recursive*. So in this case you should use the *Recursive* scheme; for
other models *OrderN* might be beneficial.

Now we can take a look at our rope swinging:

.. literalinclude:: ../../Examples/Rope.py
    :lines: 76

This is what it should look like:

.. only:: html

    .. image:: Rope.gif

.. only:: latex

    .. image:: Rope.jpg
        :scale: 50 %

If you hit simulate with more than say six segments, the simulation will be
quite slow, because the time integration has to evaluate a python function
to obtain the accelerations for each time step. However, you can use the
*Compile Model in Fortran* or *Compile Model in C* buttons to automatically
export the equations to Fortran or C, compile them and reimport them for
faster integration. This way, even 20 segments appear smooth on my machine.

In the end, the complete source looks like this:

.. literalinclude:: ../../Examples/Rope.py
