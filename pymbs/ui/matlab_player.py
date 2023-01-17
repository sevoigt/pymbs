import scipy.io
#from tkinter.filedialog import askopenfilename
import time
import numpy as np
from copy import deepcopy
#import h5py
from functools import partial
from numpy import array, ndarray

class ModelicaDataItem(object):

    def __init__(self, name, data):

        self.name = name
        self.data = data


    def get(self, index):

        if (not isinstance(self.data,ndarray)):
            self.data = self.data.func(self.data.args[0],self.data.args[1])
        return self.data[index,1]


class MatlabPlayer(object):
    '''
    Class for reading a mat-file containing experimental data that shall be
    visualised. Matlab data is expected to be a (n,1)-vector!
    '''

    def __init__(self, fileName, fps=50, vtkObjects=None):
        self.Y = {}
        self.T = {}
        self.idxT = 0
        self.nT = 0
        self.nq = 0
        self.isModelica = None
        self.vtkObjects = vtkObjects

        if (fps is None):
            self.targetStepWidth = None
        else:
            self.targetStepWidth = 1.0/fps

        self.setResultFile(fileName)


    def setResultFile(self, fileName):
        assert(isinstance(fileName, str))

        if not (fileName.endswith('.mat')) or (fileName.endswith('.hdf5')):
            print('file extension should be .mat or .hdf5, trying to use it anyway')

        self.fileName = fileName

        try:
            print('Opening result file "' + fileName + '" ...')
            channels = scipy.io.loadmat(fileName, struct_as_record=True)
            print('result file has been loaded successfully')
        except NotImplementedError:
            print('mat file seems to have new HDF5 format ...')
            raise NotImplementedError("hdf5-based mat-files not supported")
            channels = h5py.File(fileName)
            channels = dict(channels)
            print('result file has been loaded successfully')
        except:
            raise IOError('could not open file (check path and filetype)')

        # extract T, Y
        if ('T' in channels):
            self.T = channels['T']
            T_shape = self.T.shape

            if ('Y' not in channels):
                raise IOError('No variable Y was found inside mat-file')
            self.Y = channels['Y']
            Y_shape = self.Y.shape

            # check sizes of T and Y
            assert(T_shape[0] == 1)
            assert(T_shape[1] == Y_shape[1])
            self.nT = T_shape[1]-1
            self.nq = Y_shape[0]
        elif 'Aclass' in channels and \
               'description' in channels and \
               'dataInfo' in channels:

            self.isModelica = False
            self.names = self._transformStringArrays(channels['name'])
            self.descr = self._transformStringArrays(channels['description'])

            if len(self.names) != len(channels['name'][0]):
                #Modelica result file...
                self.isModelica = True
                _names = list(channels['name'])
                _descr = list(channels['description'])
                required_len = len(_names[0])
                for i in range(len(_names)):
                    _names[i] += '\x00'*(required_len - len(_names[i]))
                for i in range(len(_descr)):
                    _descr[i] += '\x00'*(required_len - len(_descr[i]))

                self.names = self._transformStringArrays(_names)
                self.descr = self._transformStringArrays(_descr)

            self.mat = channels
            self.data = dict()

            names = list()
            for obj in self.vtkObjects:

                names.append(str(obj.T))
                names.append(str(obj.r))

            self.nameprefix = None

            for i in range(len(self.names)):
                #create Items

                name = str(self.names[i])

                if (self.nameprefix == None):
                    pos = name.find(names[0])
                    if (pos > -1):
                        if (pos > 0):
                            if (name[pos-1]=='.'):
                                if (len(names[0]) + pos == len(name)):
                                    self.nameprefix = name[0:pos]
                                if (name[pos+len(names[0])] == '['):
                                    self.nameprefix = name[0:pos]
                        else:
                            self.nameprefix = name[0:pos]


                #namePath = self.names[i].split('.')
                #namePath = map(str, namePath)
                '''
                row 1 - which table  (1: Paramter, 2: Variables)
                row 2 - index of var in table (variable 'Time' have index 1)
                row 3 - linear interpolation == 0
                row 4 - not defined outside of the defined time range == -1

                '''
                block = self.mat['dataInfo'][0][i]
                column = self.mat['dataInfo'][1][i]
                if block == 0:
                     continue

                data = partial(self._readData, block, column)

                self.data[name] = ModelicaDataItem(name,data)

            self.nT = len(self.mat['data_2'][0]) - 1

        else:
            raise IOError('No variable T was found inside mat-file')


    def getTime(self):
        '''
        get current Time
        '''
        if (self.isModelica is not None):
            return self.data['time'].get(self.idxT)
        else:
            return self.T[0, self.idxT]


    def getState(self):
        '''
        Build an array from the channel dictionary at a specific time
        '''
        return self.Y[:,self.idxT]


    def nextSample(self):
        '''
        Advance in Time
            returns True if there is still Data available, False otherwise
        '''

        # no target fps specified
        if (self.targetStepWidth is None):
            if (self.idxT < self.nT):
                self.idxT += 1
                return True
            else:
                return False

        # targetfps specified
        tOld = self.getTime()
        t = tOld
        while(t < (tOld + self.targetStepWidth)):
            # increase index
            if (self.idxT < self.nT):
                self.idxT += 1
            else:
                return False

            # obtain new time
            t = self.getTime()

        # done
        return True


    def reset(self):
        self.idxT = 0

    def _transformStringArrays(self, a):
        '''
        Create a list containg all names/descriptions from arrays. The '\x00'
        are removed for Open Modelica result files.'''
        _trans = [''.join(s).rstrip() for s in zip(*a)]
        if self.isModelica:
            _trans = [x.replace('\x00','') for x in _trans]
        return _trans

    def _readData(self, block, column):
        '''Return an x,y - array of data for a variable.'''
        x = self.mat['data_%i' % block][0]
        y = self.mat['data_%i' % block][abs(column)-1]
        if column < 0:
            y *= -1
        return array([x, y]).T

    def get(self, symbolname):

        if (self.isModelica):
            key = '%s%s'%(self.nameprefix,symbolname)
            if (key in self.data):
                return self.data[key].get(self.idxT)
            else:
                return 0
        return 0
