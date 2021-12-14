import numpy as np

# TODO: dummy controller; to be replaced by real algorithm
def trolley_force(t, y, sensors):
    F = np.array([[-0.1, 0, 0]])
    return np.transpose(F)

def rope_force(t, y, sensors):
    F = np.array([[0, 0, 10.8125]])
    return np.transpose(F)
