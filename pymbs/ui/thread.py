"""
This module contains a thread class used in the pymbs ui/app to run the
simulation and animation in the background.
"""

import threading
import time

class PyMbsThread(threading.Thread):
    """
    Usage:
    def myFunc():
        print 'doing something'
        time.sleep(1)


    t = PymbsThread(myFunc)
    t.start()	# starts Thread
    t.stop()    # stop Thread
    t.reinit()  # "reset" thread
    t.start()   # start Thread again
    t.stop()
    """

    def __init__(self, function, realtime=False):
        threading.Thread.__init__(self)
        self.execute = False
        self.function = function
        self.realtime = realtime
        self.scaling = 1

        # Start-Time (Model)
        self.model_offset = 0.0
        # Start-Time (Real)
        self.real_offset = 0.0


    def reinit(self):
        if (self.execute):
            self.stop()

        self.__init__(self.function, self.realtime)


    def run(self):
        # Initialise Offsets
        self.real_offset = time.time()
        self.model_offset = self.function()
        t = self.model_offset

        # Debug
        # print "Starting Thread " + str(id(self))

        # Endless Loop
        self.execute = True
        while self.execute:

            # synchronise with real time
            if (self.realtime):
                # Real Elapsed Time
                real = self.scaling*(time.time() - self.real_offset)
                # Model Elapsed Time
                model = t - self.model_offset
                # Difference
                deltaT = model-real
                if (deltaT > 0):
                    time.sleep(deltaT)

            # Execute next step
            t = self.function()

        # Debug
        # print "Finished Thread " + str(id(self))


    def stop(self):
        self.execute = False

        # Debug
        # print "Stopped Thread " + str(id(self))
