import numpy as np

def controlForce(t, y, sensors):
    # sensors are:
    # "x[0]" for trolley position
    # "x[1]" for trolley velocity
    # "alpha0[0]" for angle at the beginning of the rope
    # "alpha0[1]" for anglular velocity at the beginning of the rope
    # "alphaN[0]" for angle at the end of the rope
    # "alphaN[1]" for anglular velocity at the end of the rope
    # "position" for absolute position of end point of the rope
    F = 10.0
    return np.r_[F, 0, 0]
