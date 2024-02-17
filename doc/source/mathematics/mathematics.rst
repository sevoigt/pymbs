.. _Background:

Background
==========

This chapter is supposed to provide the reader with a brief overview
of the mathematical and physical background.

Deriving Equations of Motion
----------------------------

Format
^^^^^^

PyMbs translates the description based on bodies, joints and load elements
into the following system of differential equations describing the equations of
motion:

  .. math::
	  :label: MbsEquation

	  M\dot{qd}+h &= f + W\lambda \\
	  \dot{q} &= qd \\
	  \Phi(q) &= 0

where :math:`q` is vector of the generalised positions; :math:`q_d` is the
vector of generalised velocities; :math:`\lambda` is the vector of constraint
forces; :math:`M` is the generalised Mass Matrix; :math:`h` is a vector
containing all centrifugal and Coriolis forces; :math:`f` comprises all
internal and external loads; :math:`\Phi\left(q\right)` describes all holonomic
constraints.

Currently, PyMbs features an explicit and a recursive scheme to derive the
equations of motion. The explicit scheme is best used for small systems with up
to four degrees of freedom. It obtains expressions for :math:`M`, :math:`h`,
:math:`f`, :math:`Phi` directly. This code is very compact for small systems
but the expressions become extremely long for larger systems. For that reason a
recursive scheme has been implemented which exploits the kinematic structure of
the system leading to many but simple equations.

Explicit Scheme
^^^^^^^^^^^^^^^

The explicit scheme is based on the Newton-Euler-Algorithm and the symbolic
calculation capabilities of PyMbs through the use of sympy. Since an exhaustive
presentation would go far beyond the scope of this document, only the essential
highlights will be discussed.

Given a rigid body :math:`i` which is able to move freely in a three
dimensional space, following equations describe the trajectory of its centre of
gravity.

  .. math::
		:label: FreeBody

		\underbrace{
		\left[\begin{array}{cc}
		M_{1,i} & 0\\
		0 & M_{2,i}\end{array}\right]}_{M_{i}}
		\left[\begin{array}{c}
		\dot{v}_{i}\\
		\dot{\omega}_{i}\end{array}\right]+\underbrace{\left[\begin{array}{c}
		0\\
		\tilde{\omega}_{i}M_{2,i}\omega_{i}\end{array}\right]}_{h_{i}} &= \underbrace{\left[\begin{array}{c}
		f_{e,i}\\
		m_{e,i}\end{array}\right]}_{f_{i}}\\
		v_{i} &=  \dot{r}_{i}\\
		\omega_{i} &=
		H_{i}\dot{\alpha}_{i}

Where :math:`r_{i}` and :math:`v_{i}` describe the position and the velocity of
the centre of gravity with respect to the inertial frame; :math:`\omega_{i}`
represents its angular velocity with respect to a body-fixed frame;
:math:`\alpha_{i}` describes the bodies orientation using Cardan Angles, for
example. :math:`M_{1,i}` and :math:`M_{2,i}` are sub matrices of the block
diagonal matrix :math:`M_{i}`, given by

  .. math::

		M_{1,i} &= \left[\begin{array}{ccc} m_{i} & 0 & 0\\
		0 & m_{i} & 0\\
		0 & 0 & m_{i}\end{array}\right]	\\
		M_{2,i}=I_{i} &= \left[\begin{array}{ccc}
		I_{xx,i} & I_{xy,i} & I_{xz,i}\\
		I_{xy,i} & I_{yy,i} & I_{yz,i}\\
		I_{xz,i} & I_{yz,i} & I_{zz,i}\end{array}\right]

with :math:`m_{i}` being the mass and :math:`I_{i}` being the symmetric tensor
of inertia. The vector :math:`h_{i}` contains centrifugal and
Coriolis-forces; :math:`f_{i}` comprises all internal and external forces and
torques.

Usually, a single body without any constraints will not be sufficient for a
model of a technical system. Multiple bodies, connected by joints or
governed by constraints are needed. Based on equation :eq:`FreeBody`
one can find a more general description for a system consisting of :math:`N`
bodies.

  .. math::
		:label: MultipleBodies

		\underbrace{\left[\begin{array}{cc}
		M_{1} & 0\\
		0 & M_{2}
		\end{array}\right]}_{M}
		\left[\begin{array}{c}
		\dot{zd}_{1}\\
		\dot{zd}_{2}\end{array}\right]+\underbrace{\left[\begin{array}{c}
		0\\
		\tilde{zd}_{2}M_{2}zd_{2}\end{array}\right]}_{h} &=
		\underbrace{\left[\begin{array}{c} f_{e}\\
		m_{e}\end{array}\right]}_{f}\\
		zd_{1} &= \dot{z}_{1}\\
		zd_{2} &= H\dot{z}_{2}

Although equation :eq:`MultipleBodies` looks very similar to equation
:eq:`FreeBody`, the definitions of the used symbols does differ

  .. math::

		z_{1}=\left[\begin{array}{cccc}
		r_{1}^{T} & r_{2}^{T} & \ldots & r_{N}^{T}\end{array}\right]^{T}

		zd_{2}=\left[\begin{array}{cccc}
		\omega_{1}^{T} & \omega_{2}^{T} & \ldots & \omega_{N}^{T}\end{array}\right]^{T}

		M_{1}=blkdiag\left(M_{1,1},M_{1,2},\ldots,M_{1,N}\right)

:math:`M_{2}`, :math:`f_{e}`, :math:`m_{e}`, :math:`\alpha` and :math:`H` are
generalised in a similar way.

Introducing joints between bodies leads to restrictions of their relative
movements for which algebraic constraints can be found

  .. math::

		\Phi\left(z_{1},z_{2}\right)=0.

These constraints are ensured by introducing corresponding constraint forces
into the equations of motion acting perpendicular to the direction of allowed
displacements. Thus equation :eq:`MultipleBodies` changes to

  .. math::
		:label: MultBodiesConst

		M\left[\begin{array}{c}
		\dot{zd}_{1}\\
		\dot{zd}_{2}\end{array}\right]+h &= f+\left(\frac{\partial\Phi}{\partial
		z}\right)^{T}\lambda\\
		zd_{1} &=   \dot{z_{1}}\\
		zd_{2} &=  H\dot{z_{2}}\\
		\Phi &=
		0

with

	.. math::

		z=\left[\begin{array}{cc}
		z_{1}^{T} & z_{2}^{T}\end{array}\right]^{T}

where :math:`\lambda` are the Lagrange Multipliers representing a vector of
generalised forces.

In many applications, a set of minimal coordinates :math:`q` can be found.
They can be characterised by three essential properties

* There exist some functions :math:`F_{1}` and :math:`F_{2}` such that
  :math:`z_{1} = F_{1}\left(q\right)` and :math:`z_{2} = F_{2}\left(q\right)`
* All elements of :math:`q` are independent, i.e. they form a basis
* All constraints are satisfied by the :math:`q`, i.e.
  :math:`\Phi\left(q\right)=0` :math:`\forall q`

If :math:`q` exists, the following relations hold for :math:`z_{1}`,
:math:`zd_{1}` and its derivatives

  .. math::
		:label: z1_ddot

		z_{1} &= F_{1}\left(q\right) \\
		zd_{1} &= \dot{z}_{1} \\
		&= \frac{\partial F_{1}}{\partial
		q}\dot{q}=J_{T}qd \\
		\dot{zd}_{1} &=
		J_{T}\dot{qd}+\dot{J}_{T}qd

and for :math:`z_{2}`, :math:`zd_{2}` and its derivatives

  .. math::
 		:label: z2_dot

		z_{2} &=
		F_{2}\left(q\right) \\
		zd_{2} &= H\dot{z}_{2} \\
		&= H\frac{\partial F_{2}}{\partial q}\dot{q} \\
		&= J_{R}qd \\
		\dot{zd}_{2} &=
		J_{R}\dot{qd}+\dot{J}_{R}qd

Substituting :math:`\dot{zd}_{1}` and :math:`\dot{zd}_{2}` in equation
:eq:`MultBodiesConst` by the newly found relations, gives

  .. math::
 		:label: Intermediate

		M\underbrace{\left[\begin{array}{c}
		J_{T}\\
		J_{R}\end{array}\right]}_{J}\dot{qd}+M\left[\begin{array}{c}
		\dot{J}_{T}\\
		\dot{J}_{R}\end{array}\right]qd+h=f+\left(\frac{\partial\Phi}{\partial
		z}\right)^{T}\lambda

Multiplication by :math:`J^{T}` from the left transforms the equation into
the space of the minimal coordinates, eliminating all influences of
constraint forces. This is due to the fact that no work is performed by
constraint forces since they always act perpendicular to the allowed
displacements. Thus equation :eq:`Intermediate` becomes

  .. math::
		:label: redEqn

		\underbrace{\left(J^{T}MJ\right)}_{M_{red}}qd+\underbrace{J^{T}\left(M\dot{J}\dot{q}+h\right)}_{h_{red}}
		&= \underbrace{J^{T}f}_{f_{red}}

which can be written using the newly introduced variables

  .. math::
		:label: reducedSystem

		M_{red}\dot{qd}+h_{red}\left(q,qd\right)=f_{red}\left(q,qd\right)

which corresponds to the general equation of motion for multibody systems
:eq:`MbsEquation`.


Recursive Scheme
^^^^^^^^^^^^^^^^

Since the explicit scheme is only suitable for small multibody systems an
explicit scheme has been implemented. This has been done according to [FS]_
where a detailed explanation is given. Therefore only the implemented
equations will be given here.
For each body :math:`i` following information must be provided

*  Mass: :math:`m_{i}`
*  Inertia: :math:`I_{i}`
*  Position of Centre of Gravity: :math:`l_{i}`
*  Distance to all child bodies :math:`h`\ = :math:`p^{z}_{hi}`

The underlying assumption is that every joint has exactly one degree of
freedom. More complex joints can be obtained through a combination of such
simple joints. In general each joint :math:`i` belonging to the body :math:`i`
has two parameters:

*  Axis of Translation: :math:`\Psi_{i}`
*  Axis of Rotation: :math:`\Phi_{i}`

The equations are generated in three steps.  First there is a forward loop,
where all positions and velocities of each body is obtained. Below you find all
equations (taken from [FS]_) with :math:`i` denoting the current body and
:math:`h` denoting its parent body. The index :math:`k` runs over all ancestor
bodies.

  .. math::

		\omega_{i} &= \omega_{h} + qd_{i}\Phi_{i} \\
		{\dot{\omega}^{C}}_{i} &=
		{\dot{\omega}^{c}}_{h}+\tilde{\omega}_{i}\Phi_{i}qd_{i} \\ \beta^{C}_{i} &= \tilde{\omega}_{i}\tilde{\omega}_{i}
		+\tilde{\dot{\omega}}^{C}_{i}\\
		\alpha^{C}_{i} &=
		\alpha^{C}_{h}+\beta^{C}_{h}p^{z}_{hi}+2\tilde{\omega}_{i}\Psi_{i}qd_{i}\\
		O^{M}_{i,k} &= O^{M}_{h,k} + \delta_{k,i}\Psi_{i}\\
		A^{M}_{i,k} &= A^{M}_{h,k} +
		\tilde{O}^{M}_{h,k}p^{z}_{h,i}+\delta_{k,i}\Psi_{i}

All values are initialised with :math:`0` except for :math:`\alpha^{C}_{0}=-g`
with :math:`g` denoting the gravity vector.

Second there is a backward loop in which all joint forces are calculated, with
:math:`r` denoting all direct successors (child bodies) of the body :math:`i`.

  .. math::

		G^{C}_{i} &= m_{i}\left( \alpha^{C}_{i}+\beta^{C}_{i}l_{i}
		\right)-F_{ext,i} \\
		F^{C}_{i} &= \sum{F^{C}_{r}} + G^{C}_{i} \\
		L^{C}_{i} &= \sum(\left(L^{C}_{r}+\tilde{p}^{z}_{i,r}F^{C}_{r}\right)) \\
		G^{M}_{i,k} &= m_{i}\left( A^{M}_{i,k}+\tilde{O}^{M}_{i,k}l_{i}\right) \\
		F^{M}_{i,k} &= \sum{F^{M}_{r,k}}+ G^{M}_{i,k} \\
		L^{M}_{i,k} &= \sum{\left( L^{M}_{r,k} + \tilde{p}^{z}_{i,r}F^{M}_{r,k}
		\right)} + \tilde{l}_{i}G^{M}_{i,k}+I_{i}O^{M}_{i,k}

Third, there is another forward loop in which expressions for the generalised
accelerations are calculated.

  .. math::

		C_{i} &= \Psi_{i}F^{C}_{i} + \Phi_{i}L^{C}_{i} \\
		M_{i,k} = M_{k,i} &= \Psi_{i}F^{M}_{i,k} + \Phi_{i}L^{M}_{i,k}

The so found :math:`C_{i}` and :math:`M_{k,i}` are the elements of the Mass
Matrix :math:`M` and a :math:`C`-Vector which satisfies the following relation

  .. math::

		C = f + W\lambda - h

Thus, the form of equation :eq:`MbsEquation` has been achieved.

.. _Kinematic-Loops:

Explicit Handling of Kinematic Loops
------------------------------------

A unique feature of PyMbs is its ability to deal with certain kinematic loops
in an explicit manner. This consequently leads to a formulation based on
minimal coordinates and thereby to explicit ordinary differential equations
without algebraic constraints. The underlying concept shall be presented in
this section.

Currently, there are four different kinematic loops implemented as shown in
Figure :ref:`fig-kinematicLoops`.

  .. _fig-kinematicLoops:

  .. figure:: img/kinematic_loops.jpg

        Kinematic loops

        Left to right, top: CrankSlider, FourBarTrans. Bottom: Transmission,
        FourBar


Handling kinematic loops is based on the coordinate partitioning method [WH]_.
Given the vectors of generalised positions :math:`q`, a set of independent
coordinates :math:`u` and a set of dependent coordinates :math:`v` are chosen
according to the definitions made in the kinematic loops.

  .. math::

		q = \left[ \begin{array}{c}u\\v\end{array} \right]

First, an explicit relation between the dependent coordinates :math:`v` and
the independent coordinates :math:`u` must be provided for each loop.

  .. math::

		v = H\left(u\right)

Exploiting the (usually much simpler) implicit relation

  .. math::

	h\left(u,v\right)

of the loop :math:`j`, corresponding relations on velocity and acceleration
level may be derived.

  .. math::

		\dot{h} &= h_{u}\dot{u}+h_{v}\dot{v}\\
        \dot{v} &= -h_{v}^{-1}h_{u}\dot{u}\\
        &= B_{vu}\dot{u}


with :math:`B_{vu} = -h_{v}^{-1}h_{u}`. The same can be achieved for finding a
relationship on acceleration level.

  .. math::
		:label: v_ddot

		\ddot{h} &= h_{u}\ddot{u} + h_{v}\ddot{v} +
		\dot{h}_{u}\dot{u} + \dot{h}_{v}\dot{v}\\
		\ddot{v} &= -h_{v}^{-1}\left(h_{u}\ddot{u} + \dot{h}_{u}\dot{u} +
		\dot{h}_{v}\dot{v}\right)\\
		&= B_{vu}\ddot{u} + b^{\prime}

with :math:`b^{\prime} = -h_{v}^{-1}\left( \dot{h}_{u}\dot{u} +
\dot{h}_{v}\dot{v} \right)`.  Thus, the equations the equations of motion can
be reduced even further, so that they only need to be solved for the set of
independent coordinates.  Equation :eq:`MbsEquation` may be rewritten using
equation :eq:`v_ddot`.

  .. math::

		M\left(\underbrace{\left[\begin{array}{c}I\\
		B_{vu}\end{array}\right]}_{J}\ddot{u}+\underbrace{\left[\begin{array}{c}
		0\\
		b^{\prime}\end{array}\right]}_{b}\right)+h &= F+W\lambda\\
		MJ\ddot{u}+Mb+h &= F+W\lambda

Multiplying this equation with :math:`J^{T}` from the left yields

  .. math::
		:label: reducedMbsEqns

		J^{T}MJ\ddot{u}+J^{T}\left(Mb+h\right) &=
		J^{T}F+\underbrace{J^{T}W}_{=0}\lambda\\
		M_{red}\ddot{u}+h_{red} &= F_{red}

with :math:`M_{red}=J^{T}MJ^{T}`; :math:`h_{red}=J^{T}\left(Mb+h\right)`;
:math:`F_{red}=J^{T}F`.  Clearly, equation :eq:`reducedMbsEqns` has the form of
:eq:`MbsEquation` and is therefore compatible.

In order to implement explicit handling for a new kinematic loop, the following
information must be provided:

*  partitioning of joint coordinates into :math:`u` and :math:`v`
*  :math:`H`, such that :math:`v = H\left(v\right)`
*  :math:`B_{vu}`, :math:`b^{\prime}`, such that
   :math:`\dot{v} = B_{vu}\dot{u}` and
   :math:`\ddot{v} = B_{vu}\ddot{u}+b^{\prime}`

Note, that saving :math:`B_{vu}` and :math:`b_{prime}` directly, although they
can be derived from :math:`h\left(u,v\right)=0` automatically, saves time when
assembling a model and leaves room for optimising these expressions towards
brevity.

.. [FS] Fisette, P; Samin, J. C.: Symbolic generation of large multibody system dynamic equations using a new semi-explicit Newton/Euler recursive scheme. In: Archive of applied mechanics Vol. 66, No 3. (1996) pp. 187-199
.. [WH] Wehage, R. A.; Hung, E. I.: Generalized coordinate partitioning for dimension reduction in analysis of constrained dynamic systems. I, Mech. Design 134 (1982) pp. 247-255
