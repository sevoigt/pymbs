import numpy as np

def controlForce(t, y, sensors):
    b = 0.5
    c = 6
    x = sensors['x']
    v = sensors['v']
    F = b * v + c * x
    F = np.squeeze(np.asarray(F))
    return F
