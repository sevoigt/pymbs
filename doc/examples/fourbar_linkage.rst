Four Bar Linkage
----------------

We will simulate a four bar linkage and introduce the command for handling
kinematic loops like this. If you want to try it first, or look at the complete
source code, see
:download:`FourBarLinkage.py <../../Examples/FourBarLinkage.py>`.

As usual, we start with importing PyMbs, creating a model system and adding
various parameters. Here, they are the lengths, masses and interia of the
bars to be connected:

.. literalinclude:: ../../Examples/FourBarLinkage.py
    :lines: 26-33
    :append: ...

Now we create our additional three bars (``world`` will act as the first bar)
and add the coordinate systems :math:`A` through :math:`D` at the corners:

.. literalinclude:: ../../Examples/FourBarLinkage.py
    :lines: 46-62

Then we can connect three of them using normal joints:

.. literalinclude:: ../../Examples/FourBarLinkage.py
    :lines: 65-67

However, the last one (here its :math:`C`) has to be connected using the
command ``addLoop.FourBar`` to correctly handle the kinematic loop:

.. literalinclude:: ../../Examples/FourBarLinkage.py
    :lines: 68

With the ``posture`` parameter, either ``1`` or ``-1``, you can choose which
solution is being used. Usually, this switches between the bars intersecting or
not.

In the end, we add a line of each of the bars for visualisation, generate our
equations of motion and start the GUI:

.. literalinclude:: ../../Examples/FourBarLinkage.py
    :lines: 71-

The result should look roughly like this:

.. only:: html

    .. image:: FourBarLinkage.gif

.. only:: latex

    .. image:: FourBarLinkage.jpg
        :scale: 50 %

In the end, the complete source looks like this:

.. literalinclude:: ../../Examples/FourBarLinkage.py
