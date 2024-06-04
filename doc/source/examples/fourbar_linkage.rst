Four Bar Linkage
----------------

We will simulate a four bar linkage and introduce the command for handling
kinematic loops like this. The system will move in the x-z-plane, thus the
revolute joints all rotate around the y-axis of the world. If you want to
try it first, or look at the complete source code, see
:download:`fourbar_linkage.py <../../../examples/fourbar_linkage.py>`.

As usual, we start with importing PyMbs, creating a model system and adding
various parameters. Here, they are the lengths, masses and inertias of the
bars to be connected:

.. literalinclude:: ../../../examples/fourbar_linkage.py
    :lines: 9-14
    :append: ...

Now we create our additional three bars (``world`` will act as the first bar)
and add the coordinate systems :math:`A` through :math:`D` at the corners:

.. literalinclude:: ../../../examples/fourbar_linkage.py
    :lines: 16-45

Then we can connect three of them using regular revolute joints:

.. literalinclude:: ../../../examples/fourbar_linkage.py
    :lines: 48-50

However, the last one (here its :math:`C`) has to be connected using the
command ``addLoop.FourBar`` to correctly handle the kinematic loop:

.. literalinclude:: ../../../examples/fourbar_linkage.py
    :lines: 52

With the ``posture`` parameter, either ``1`` or ``-1``, you can choose which
solution is being used. Usually, this switches between the bars intersecting or
not.

In the end, we add a line of each of the bars for visualisation, generate our
equations of motion and start the GUI:

.. literalinclude:: ../../../examples/fourbar_linkage.py
    :lines: 54-

The result should look roughly like this:

.. only:: html

    .. image:: fourbar_linkage.gif

.. only:: latex

    .. image:: fourbar_linkage.jpg
        :scale: 50 %

In the end, the complete source looks like this:

.. literalinclude:: ../../../examples/fourbar_linkage.py
