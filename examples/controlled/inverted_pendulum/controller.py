import numpy as np

def controlForce(t, y, sensors):
    k = [ 25.4841998, 83.5330999,  20.38735984, 18.19367992]
    y0 = np.r_[0, np.pi, 0 ,0]
    F = np.dot(y-y0, k)
    return np.r_[F, 0, 0]
