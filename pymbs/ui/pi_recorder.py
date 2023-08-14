# -*- coding: utf-8 -*-
'''
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert,
                     Jens Frenkel, Sebastian Voigt
'''

'''
Created on 10.08.2009

@author: sebavoig
'''


import PyQt6.QtCore as QtCore

import datetime
import numpy
import time
import os





class PIRecorder(object):
    '''
    This class provides functions to save the state of the mbs and the
    resulting positions and orientations of all visualisation objects to
    files. The data (3-vector for position and 3x3 matrix for orientation)
    is called position input (PI) due to the nomenclature in the simulation
    framework SARTURIS (www.sarturis.de)

    A separate file is generated for every object as well as a file containing
    the state vector.
    '''

    def __init__(self, sensorData):
        '''
        Constructor

        sensorsData - a handle the returns the state vector and the
                      corresponding sensor values as dictionary
        '''

        # handle to get data
        self.getData = sensorData

        # timer object
        self.timer = QtCore.QTimer()
        self.timer.setInterval(40)

        self.timer.timeout.connect(self.addData)

        # set initial state
        self.reset()



    def reset(self):
        '''
        Create emtpy dicitonary to save new data.
        '''

        state, sensors = self.getData()

        # create dicitionary for PI-data (uses name of the sensor)
        self.sensors = {'state':[][:]}

        for i in sensors:
            if i.startswith('r_'):
                self.sensors[i[2:]] = [][:]

        # start time reference
        self.startTime = None



    def addData(self):
        '''
        Add current state and position inputs to recorded data.
        '''

        # get data and time
        state, sensors = self.getData()

        curTime = time.time()-self.startTime

        # save state
        self.sensors['state'].append([curTime]+state)

        # save PI-data
        for i in sensors:

            _curSet = [curTime]
            _name = i[2:]

            if i.startswith('r_'):
                _curSet.extend(sensors[i].flatten().tolist()[0])
                _curSet.extend(sensors['T_%s'%_name].flatten().tolist()[0])

                self.sensors[_name].append(_curSet)



    def toggle(self, checked):
        '''
        Convenience function to toggle recording state.

        checked (bool) - start recording if True, else stop
        '''

        if checked:
            self.start()
        else:
            self.stop()



    def start(self):
        '''
        Begin recording.
        '''

        self.startTime = time.time()
        self.timer.start()



    def stop(self):
        '''
        Stop recording and save data to file
        '''

        # stop actual recording
        self.timer.stop()

        # create directory for pid files
        timestamp = datetime.datetime.now()
        timestr = '%i-%i-%i_%i-%i_%i' % (timestamp.year,
                                         timestamp.month,
                                         timestamp.day,
                                         timestamp.hour,
                                         timestamp.minute,
                                         timestamp.second)

        os.mkdir('pi_record_%s' % timestr)

        # save PI-data as csv (one file per object)
        for i in self.sensors:
            data = numpy.asarray(self.sensors[i])

            # state is not a position input!
            if i== 'state':
                ext = 'txt'
            else:
                ext = 'pid'

            numpy.savetxt('pi_record_%s/%s.%s' % (timestr, i,ext),
                           data, delimiter=';', fmt='%.3f')

        print('position inputs saved')

        # clean up
        self.reset