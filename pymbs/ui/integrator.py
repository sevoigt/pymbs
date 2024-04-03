"""
This module contains an integrator used for the time stepping simulation in the
pymbs ui/app.
"""
from scipy import integrate


class Integrator(integrate.ode):
    """
    This integrator is adjusted to perform well with typical pymbs models. That
    means the integration method and the tolerances are tuned to deliver
    good simulation speed, stability and result accuracy (although this is
    only verified by looking at the animation of the example models).
    Some models may perform better with different integrator settings thus this
    is a good starting point for experimenting.
    """

    def __init__(self, handle, y0, yd0, dt=0.01, rtol=1e-3, atol=1e-6):
        """
        Provide the initial vlaues and the integrator settings for the time
        stepping simulation.
        """
        integrate.ode.__init__(self, handle)
        self.y0 = list(y0)
        self.yd0 = list(yd0)
        self.init_vals = self.y0 + self.yd0
        self.set_integrator('lsoda', rtol=rtol, atol=atol)
        self.set_initial_value(self.init_vals)
        self.dt = dt
        self.handle = handle


    def set_step_size(self, dt):
        """
        Set the step size for the simulation. This is the 'communication step
        size' and not the fixed time step.
        """
        self.dt = float(dt)


    def reset(self):
        """
        Reset the integrator to restart the simulation
        """
        self.__init__(self.handle, self.y0, self.yd0, self.dt)


    def step(self):
        """
        Perform a simulatin step. This is
        """
        self.integrate(self.t + self.dt)
        #print(str.format('y={0}', self.y)) # for debug
        return self.y
