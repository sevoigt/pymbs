import os
import sys
import time
import tempfile
import vtk  # vtk widget remains blank without this?!

import numpy as np
from numpy import zeros

from PySide6.QtWidgets import QWidget, QTabWidget, QVBoxLayout, QScrollArea, \
                            QLabel, QHBoxLayout, QCheckBox, QLineEdit, \
                            QPushButton, QSlider, QDialog, QMainWindow, \
                            QFileDialog, QApplication, QSizePolicy
from PySide6.QtGui import QDoubleValidator, QIcon, QPixmap
from PySide6.QtCore import Qt, QSize, QMetaObject, QTimer

from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor
from vtkmodules.util.colors import light_grey
from vtkmodules.vtkRenderingCore import vtkRenderer, vtkActor, vtkPolyDataMapper
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkIOGeometry import vtkSTLReader, vtkOBJReader
from vtkmodules.vtkFiltersSources import vtkCylinderSource, vtkCubeSource, \
                                         vtkArrowSource, vtkLineSource, \
                                         vtkSphereSource, vtkTextSource, \
                                         vtkParametricFunctionSource
from vtkmodules.vtkFiltersGeneral import vtkAxes
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonComputationalGeometry import vtkParametricSpline
from vtkmodules.vtkCommonMath import vtkMatrix4x4


# Note: NavigationToolbar2QTAgg was removed in matplotlib 1.5.0
# Now pymbs imports NavigationToolbar2QT which should be compatible with older
# matplotlib versions as well

from matplotlib.backends.backend_qtagg import NavigationToolbar2QT as \
                                              NavigationToolbar

from matplotlib.colors import ColorConverter

from pymbs.common.graphreps import File, Box, Cylinder, Frame, Sphere, \
                                   Line, Arrow, FlexibleBody
from pymbs.symbolics import VarKind
from pymbs.common.pymbserror import PyMbsError

from pymbs.ui.thread import PyMbsThread
from pymbs.ui.integrator import Integrator
from pymbs.ui.matlab_player import MatlabPlayer
from pymbs.ui.pi_recorder import PIRecorder
from pymbs.ui.matplotlibwidget import MatplotlibWidget
from pymbs.ui.compile import compileF90, compileC

from pymbs import __version__ as pymbs_VERSION

class pymbsMainWindow(object):
    '''
    Creates the main application window with group for sliders on the left and
    vtk-scene widget for interactive 3D-visualisation on the right
    '''

    def setupMainWindow(self, MainWindow, AAFrames=0):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        MainWindow.setMinimumSize(QSize(400, 200))
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        # create tab bar
        self.tabBar = QTabWidget()
        self.tabBar.setFixedWidth(220)

        # record button
        self.recordButton = RecordButton()

        # tab for genCoord sliders
        #-------------------------
        self.tabKintest = QWidget()
        self.kintestLayout = QVBoxLayout(self.tabKintest)

        # reset button
        self.resetButton = ResetButton()
        self.kintestLayout.addWidget(self.resetButton)

        # reset button
        self.showsensorButton = SensorResultButton()
        self.kintestLayout.addWidget(self.showsensorButton)

        # scroll area
        self.kintestScrollArea = QScrollArea()
        self.kintestLayout.addWidget(self.kintestScrollArea)

        # slider group
        self.kintestSliderGroup = QWidget(self.kintestScrollArea)
        self.kintestSliderGroup.setFixedWidth(182)  # TODO: should expand automatically!?
        self.kintestSliderGroupLayout = QVBoxLayout(self.kintestSliderGroup)



        # tab for simulation
        #-------------------
        self.tabSimulation = QWidget()
        self.simulationLayout = QVBoxLayout(self.tabSimulation)

        # buttons for simulate and compile
        self.simulateButton = SimButton()
        self.compileCButton = CompileCButton()

        self.simulationLayout.addWidget(self.simulateButton)
        self.simulationLayout.addWidget(self.compileCButton)

        # scroll area
        self.simulationScrollArea = QScrollArea()
        self.simulationLayout.addWidget(self.simulationScrollArea)

        # slider group
        self.simulationSliderGroup = QWidget(self.simulationScrollArea)
        self.simulationSliderGroup.setFixedWidth(182)
        self.simulationSliderGroupLayout = QVBoxLayout(self.simulationSliderGroup)


        # tab for result player
        #----------------------
        self.tabResults = QWidget()
        self.resultLayout = QVBoxLayout(self.tabResults)

        # buttons
        self.loadResultsButton = LoadResultsButton()
        self.playResultsButton = PlayResultsButton()
        self.resetResultsButton = ResetButton()
        self.scale = NumberLineEdit()

        self.resultLayout.addWidget(self.loadResultsButton)
        self.resultLayout.addWidget(self.playResultsButton)
        self.resultLayout.addWidget(self.resetResultsButton)
        tabScaling = QWidget()
        tabScalingLayout = QHBoxLayout(tabScaling)
        tabScalingLayout.addWidget(QLabel(text='RealTime Scaling:'))
        tabScalingLayout.addWidget(self.scale)
        self.resultLayout.addWidget(tabScaling)
        self.trackNode = QCheckBox('Track Node')
        self.resultLayout.addWidget(self.trackNode)
        #self.resultLayout.addStretch(1.0) # TODO does not work anymore - why?


        # add tabs to tabbar
        self.tabBar.addTab(self.tabKintest, 'KinTest')
        self.tabBar.addTab(self.tabSimulation, 'Simulation')
        self.tabBar.addTab(self.tabResults, 'Results')


        # VTK
        self.VtkScene = QVTKRenderWindowInteractor(self.centralwidget)
        #self.VtkScene._RenderWindow.SetAAFrames(AAFrames)
        self.VtkScene.setObjectName("VtkScene")


        # Layouts
        self.tabBox = QVBoxLayout()
        self.tabBox.addWidget(self.tabBar)
        self.tabBox.addWidget(self.recordButton)

        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.horizontalLayout.addLayout(self.tabBox)
        self.horizontalLayout.addWidget(self.VtkScene)

        MainWindow.setCentralWidget(self.centralwidget)


        QMetaObject.connectSlotsByName(MainWindow)




class NumberLineEdit(QLineEdit):
    def __init__(self, parent=None):
        QLineEdit.__init__(self,parent)
        self.setText('1')
        self.setValidator(QDoubleValidator(self))



class OKButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('OK')



class CloseButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Close')



class ResetButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Reset')



class SensorResultButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Show Sensors')



class SimButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Simulate')
        self.setCheckable(True)



class CompileF90Button(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Compile Model in Fortran')



class CompileCButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Compile Model in C')



class LoadResultsButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Load mat file')



class PlayResultsButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)
        self.setText('Play Results')
        self.setCheckable(True)
        self.setEnabled(False)





class RecordButton(QPushButton):
    def __init__(self, parent=None):
        QPushButton.__init__(self, parent)

        # get absolute path for icons
        self.location = os.path.dirname(__file__)
        self.setCheckable(True)
        self.setIconSize(QSize(14,14))

        self.stopped()

        self.clicked.connect(self.toggle)


    def toggle(self, checked):
        if checked:
            self.recording()
        else:
            self.stopped()


    def stopped(self):
        self.setText('  Record')
        self.setIcon(QIcon(QPixmap(self.location+'/record.png')))


    def recording(self):
        self.setText('  Stop')
        self.setIcon(QIcon(QPixmap(self.location+'/stop.png')))





class LabeledSlider(QWidget):
    '''
    Create a slider with label as description
    '''

    def __init__(self, parent=None):
        QWidget.__init__(self, parent)

        self.layout = QVBoxLayout(self)
        self.layout.setContentsMargins(0,0,0,0)

        self.label = QLabel()
        self.slider = QSlider()

        self.layout.addWidget(self.label)
        self.layout.addWidget(self.slider)

        self.initVal = None
        self.stateVal = None

        self.scale = None


    def setLabelText(self, state):
        self.name = str(state)
        self.label.setText(self.name)


    def setOrientation(self, ori):
        assert( ori in(Qt.Orientation.Horizontal, Qt.Orientation.Vertical) )
        self.slider.setOrientation(ori)

    def setWidth(self, width):
        assert( isinstance(width, int) )
        #self.label.setGeometry(0,0,width,12)
        #self.slider.setGeometry(0,18,width,20)

    def setLimits(self, min, max):
        if (isinstance(min,float)):
            min = int(min)
        if (isinstance(max,float)):
            max = int(max)

        if (min > max):
            tmp = min
            min = max
            max = tmp

        assert( isinstance(min, int) )
        assert( isinstance(max, int) )
        self.slider.setMinimum(min)
        self.slider.setMaximum(max)

    def setInitVal(self, init):
        assert( isinstance(init, (float, int)) )
        if (self.scale is not None):
            initScaled = int(init * self.scale)

        self.initVal = initScaled
        self.stateVal = initScaled
        self.label.setText(self.name + ': ' + str(init))

    def setStateVal(self):
        self.stateVal = self.slider.value()
        self.label.setText(self.name + ': ' + str(self.value))

    @property
    def value(self):
        '''The scaled value (= what we need for calculations)'''
        val = float(self.slider.value())
        if (self.scale is not None):
            val = val / self.scale
        return val


class VtkSceneObject():
    '''
    Create a VTK data source, mapper and actor from graphRep objects
    '''

    def __init__(self, graphRep):
        self.r = list()
        self.T = list()
        self.r = graphRep.r
        self.T = graphRep.T

        if isinstance(graphRep.color, (list, tuple)):
            self.color = graphRep.color
        elif graphRep.color is None:
            self.color = light_grey
        else:
            print(f'Wrong color argument for {graphRep}, ignoring it')
            self.color = light_grey

        self.scale = 1.

        if isinstance(graphRep, File):
            file = graphRep.path_to_file.lower()
            if file.endswith('.stl') or file.endswith('.stlb'):
                part = vtkSTLReader()
                part.SetFileName(graphRep.path_to_file)
                self.scale = float(graphRep.scale)
            elif file.endswith('.obj'):
                part = vtkOBJReader()
                part.SetFileName(graphRep.path_to_file)
                self.scale = float(graphRep.scale)
            else:
                print(f'Cannot handle file format {file[-4:]}')

        elif isinstance(graphRep, Cylinder):
            part = vtkCylinderSource()
            part.SetResolution(graphRep.res)
            part.SetRadius(graphRep.radius)
            part.SetHeight(graphRep.l)

        elif isinstance(graphRep, Box):
            part = vtkCubeSource()
            part.SetXLength(graphRep.lx)
            part.SetYLength(graphRep.ly)
            part.SetZLength(graphRep.lz)

        elif isinstance(graphRep, Sphere):
            part = vtkSphereSource()
            part.SetRadius(graphRep.radius)
            part.SetThetaResolution(graphRep.res)
            part.SetPhiResolution(graphRep.res)

        elif isinstance(graphRep, Frame):
            part = vtkAxes()
            part.SetScaleFactor(graphRep.size)

        elif isinstance(graphRep, Line):
            part = vtkLineSource()
            part.SetPoint1(0,0,0)
            part.SetPoint2([graphRep.length,0,0])

        elif isinstance(graphRep, Arrow):
            part = vtkArrowSource()
            part.InvertOn()
            part.SetTipLength(0.35/graphRep.size)
            self.scale = [graphRep.size,1.0,1.0]

        elif isinstance(graphRep, FlexibleBody):

            points = vtkPoints()
            for point in graphRep.positions:
                points.InsertNextPoint(point)

            spline = vtkParametricSpline()
            spline.SetPoints(points)

            part = vtkParametricFunctionSource()
            part.SetParametricFunction(spline)
            part.Update()

        else:
            part = vtkTextSource()
            part.SetText('GraphRep class not yet implemented')

        part.Update()

        # Compute normals
        normals = vtk.vtkPolyDataNormals()
        #normals.AutoOrientNormalsOn()
        #normals.ComputeCellNormalsOn()
        #normals.SetFeatureAngle(60.0)
        normals.SetInputConnection(part.GetOutputPort())

        # Map geometry to OpenGL data
        mapper = vtkPolyDataMapper()
        mapper.SetInputConnection(normals.GetOutputPort())
        mapper.ScalarVisibilityOff()

        # Create Actor for moving, rotating, scaling, appearance etc.
        self.actor = vtkActor()
        self.actor.SetMapper(mapper)
        self.actor.GetProperty().SetColor(self.color)
        self.actor.GetProperty().SetOpacity(1)
        self.actor.GetProperty().ShadingOn()

        self.actor.GetProperty().SetInterpolationToFlat()   # fast and simple
        #self.actor.GetProperty().SetInterpolationToPhong()
        #self.actor.GetProperty().SetInterpolationToPBR()
        #self.actor.GetProperty().SetInterpolationToGouraud()

        self.actor.SetScale(self.scale)

        # turn of lighting of vtkAxes for better visibility
        if isinstance(part, vtkAxes):
            self.actor.GetProperty().SetLighting(False)


class Sensors(QDialog):
    def __init__(self, parent):
        QDialog.__init__(self, parent)

        self.parent = parent
        self.modelName = parent.modelName

        # window title
        self.setWindowTitle('%s Sensors'%parent.modelName)

        # set window icon
        #self.setWindowIcon(QIcon(QPixmap('../gui/icons/pynacolada.png')))

        # standard ok/cancel buttons
        self.okButton = OKButton(self)
        self.cancelButton = CloseButton(self)

        self.standardButtonLayout = QHBoxLayout()
        self.standardButtonLayout.addStretch()
        self.standardButtonLayout.addWidget(self.okButton)
        self.standardButtonLayout.addWidget(self.cancelButton)

        # make it resizeable
        self.setSizeGripEnabled(True)

        # connect buttons
        self.cancelButton.clicked.connect(self.reject)
        self.okButton.clicked.connect(self.accept)

        # Layout fuer Dialogfenster
        self.vbox = QVBoxLayout(self)

        rma = QWidget()
        self.hbox = QHBoxLayout(rma)

        ''' Sensoren '''

        self.doUpdate = False

        sensorensa = QScrollArea()
        sensorensa.setFixedWidth(200)
        sensoren = QWidget(sensorensa)
        #sensoren.setFixedWidth(174)
        sensorenbox = QVBoxLayout(sensoren)

        self.sensorenlist = self.parent.graph.getVariables(VarKind.Sensor)

        self.sensoren = dict()
        for s in self.sensorenlist:

            sname = str(s)

            checkbox = QCheckBox(sname,sensoren)
            sensorenbox.addWidget(checkbox)

            self.sensoren[sname] = checkbox

            checkbox.clicked.connect(self.checkboxclicked)

        sensorensa.setWidget(sensoren)
        sensorensa.setWidgetResizable(True)

        self.hbox.addWidget(sensorensa)

        ''' Diagramm '''
        drawing = QWidget()
        drawing.setMinimumHeight(400)
        drawing.setMinimumWidth(400)
        self.drawingbox = QVBoxLayout(drawing)
        self.mpl = MatplotlibWidget(parent,hold=True)
        # Shink current axis by 20%
        box = self.mpl.axes.get_position()
        self.mpl.axes.set_position([box.x0, box.y0, box.width * 0.7, box.height])

        self.ntb = NavigationToolbar(self.mpl, drawing)


        self.drawingbox.addWidget(self.mpl)
        self.drawingbox.addWidget(self.ntb)
        self.hbox.addWidget(drawing)


        self.vbox.addWidget(rma)
        # Ok- und Cancel-Button
        self.vbox.addLayout(self.standardButtonLayout)

        ''' Timer fuer Update '''

        self.timer = QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.draw)
        self.timer.start()

    def checkboxclicked(self):
        self.draw()

    def draw(self):

        q = [i.stateVal/i.scale for i in self.parent.sliders]

        q = q[:self.parent.nq]
        for i in range(len(q)):
            q.append(0.0)

        values =  self.parent.mod_sensors(0,q)

        ''' Diagramm zeichnen '''

        self.mpl.axes.clear()
        width = 0.35

        #v = [1,2,3]
        colors = ['#00FFFF','#0000FF','#DC143C','#8B0000','#8B008B','#808080','#008000','#FF00FF','#FF4500','#4169E1','#FFFF00','#00FF00','#9932CC','#FF00FF','#A52A2A']
        colors = ['AliceBlue', 'AntiqueWhite', 'Aqua', 'Aquamarine', 'Azure', \
                  'Beige', 'Bisque', 'Black', 'BlanchedAlmond', 'Blue', 'BlueViolet', \
                  'Brown', 'BurlyWood', 'CadetBlue', 'Chartreuse', 'Chocolate', \
                  'Coral', 'CornflowerBlue', 'Cornsilk', 'Crimson', 'Cyan', 'DarkBlue', \
                  'DarkCyan', 'DarkGoldenRod', 'DarkGray', 'DarkGrey', 'DarkGreen', \
                  'DarkKhaki', 'DarkMagenta', 'DarkOliveGreen', 'Darkorange', 'DarkOrchid', \
                  'DarkRed', 'DarkSalmon', 'DarkSeaGreen', 'DarkSlateBlue', 'DarkSlateGray', \
                  'DarkSlateGrey', 'DarkTurquoise', 'DarkViolet', 'DeepPink', 'DeepSkyBlue', \
                  'DimGray', 'DimGrey', 'DodgerBlue', 'FireBrick', 'FloralWhite', 'ForestGreen', \
                  'Fuchsia', 'Gainsboro', 'GhostWhite', 'Gold', 'GoldenRod', 'Gray', 'Grey', \
                  'Green', 'GreenYellow', 'HoneyDew', 'HotPink', 'IndianRed ', 'Indigo ', \
                  'Ivory', 'Khaki', 'Lavender', 'LavenderBlush', 'LawnGreen', 'LemonChiffon', \
                  'LightBlue', 'LightCoral', 'LightCyan', 'LightGoldenRodYellow', 'LightGray', \
                  'LightGrey', 'LightGreen', 'LightPink', 'LightSalmon', 'LightSeaGreen', \
                  'LightSkyBlue', 'LightSlateGray', 'LightSlateGrey', 'LightSteelBlue', 'LightYellow', \
                  'Lime', 'LimeGreen', 'Linen', 'Magenta', 'Maroon', 'MediumAquaMarine', 'MediumBlue', \
                  'MediumOrchid', 'MediumPurple', 'MediumSeaGreen', 'MediumSlateBlue', 'MediumSpringGreen', \
                  'MediumTurquoise', 'MediumVioletRed', 'MidnightBlue', 'MintCream', 'MistyRose', 'Moccasin', \
                  'NavajoWhite', 'Navy', 'OldLace', 'Olive', 'OliveDrab', 'Orange', 'OrangeRed', 'Orchid', \
                  'PaleGoldenRod', 'PaleGreen', 'PaleTurquoise', 'PaleVioletRed', 'PapayaWhip',
                  'PeachPuff', 'Peru', 'Pink', 'Plum', 'PowderBlue', 'Purple', 'Red', 'RosyBrown', 'RoyalBlue', \
                  'SaddleBrown', 'Salmon', 'SandyBrown', 'SeaGreen', 'SeaShell', 'Sienna', 'Silver', 'SkyBlue', \
                  'SlateBlue', 'SlateGray', 'SlateGrey', 'Snow', 'SpringGreen', 'SteelBlue', 'Tan', 'Teal', \
                  'Thistle', 'Tomato', 'Turquoise', 'Violet', 'Wheat', 'White', 'WhiteSmoke', 'Yellow', \
                  'YellowGreen' ]

        v = list()
        shapes = list()
        xleg = list()
        xtext = list()
        for s in self.sensorenlist:
            k = str(s)
            if (self.sensoren[k].isChecked () ):
                i = values[k]
                if (isinstance(i,(int,float))):
                    v.append(i)
                    shapes.append(1)
                    xtext.append('')
                    xleg.append('%s\n%.2f'%(k,i))
                else:
                    shape = i.shape
                    shapes.append(shape[0]*shape[1])
                    value = 0.0
                    for m in range(shape[0]):
                        for n in range(shape[1]):
                            v.append(i[m,n])
                            value += i[m,n]**2
                            xtext.append('%s,%s'%(m,n))
                    value = value**(0.5)
                    xleg.append('%s\n%.2f'%(k,value))

        ind = np.arange(len(v))
        k = 0
        p = 0
        c = 0
        sindx = 0
        cv = ColorConverter()
        rects1 = list()
        rects2 = list()

        if (len(v)==2):
            pass

        for i in range(len(v)):
            ind[i]=k

            col = colors[c]
            rects = self.mpl.axes.bar(k, v[i],color=cv.to_rgb(col))
            rects2.append(rects)

            k = k+1
            p = p+1
            if (p==shapes[sindx]):
                p = 0
                k = k+1
                c = c+1
                sindx = sindx+1

            if (p == 0):
                rects1.append(rects)


        xmax = int(k-1)
        if (len(v)==0):
            v = [0,1]
            xmax = 1
        ymin = min(v)
        ymax = max(v)
        if (ymin > 0):
            ymin = 0
        if (ymax < 0):
            ymax = 0

        self.mpl.axes.axis([0, xmax, ymin*1.05,ymax*1.15])
        self.mpl.axes.set_xlabel('Sensoren')
        self.mpl.axes.set_ylabel('F,T [N,Nm]')
        self.mpl.axes.grid()
        self.mpl.axes.legend(rects1,xleg,bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)

        for i in range(len(rects2)):
            rect=rects2[i][0]
            text = xtext[i]
            height = rect.get_height()
            if (v[i] < 0):
                height = 0
            self.mpl.axes.text(rect.get_x()+rect.get_width()/2., 1.05*height, text,ha='center', va='bottom')

        self.mpl.draw()





class Gui(QMainWindow, pymbsMainWindow):
    '''
    Create the user interface with sliders according to genCoords
    '''

    def __init__(self, grList, graph, modelName, state, options, **kwargs):
        QMainWindow.__init__(self)

        if not grList:
            raise PyMbsError('Cannot show system. There are no visualisation '\
                             'objects defined. Call <addVisualisation.xyz()> '\
                             'of your MbsSystem-instance to add 3d objects '\
                             'representing the bodies of your mbs.')


        # check and set user's gui options
        assert type(options) is dict

        self.options = {'qFlexScaling':20,
                        'qTransRange' :[-3, 3],
                        'qRotRange'   :[-3.14, 3.14]}

        for key, val in options.items():

            if key not in self.options:
                print('Warning: unrecognized key "%s" in options dict' % key)
                continue

            if key == 'qFlexScaling':
                assert isinstance(val, (int, float))
                assert val >= 1

            elif key in ('qTransRange', 'qRotRange'):
                assert isinstance(val, (list, tuple))
                assert len(val) == 2
                assert val[0] < val[1]

                if abs(val[0] > 10) or abs(val[1] > 10):
                   print('Info: Slider range is quite large. ' \
                         'Did you use SI units [m, rad]?.')

        self.options.update(options)


        # default values
        self.AAFrames = 0
        self.cammpos = None
        self.focuspos = None

        self.hasFlexibleBodies = False

        # timer to update opengl scene periodically
        self.updateSceneTimer = QTimer()
        self.updateSceneTimer.setInterval(16)
        self.updateSceneTimer.timeout.connect(self.updateScene)

        # Parse kwargs
        for key, value in list(kwargs.items()):
            assert( isinstance(key, str) )

            # Try Keys
            if (key.lower() == 'aaframes'):
                assert( isinstance(value, int) )
                self.AAFrames = value
            elif (key.lower() == 'savetofile'):
                assert( isinstance(value, str))
                self.saveGraphinfo(value,grList)
            else:
                print('GUI: Unrecognized Key: "%s"'%key)

        # Set up main window and reset button
        self.setupMainWindow(self, self.AAFrames)
        self.modelName = modelName
        self.setWindowTitle('pymbs %s - %s' % (pymbs_VERSION,modelName))

        # Obtain state and inital values
        self.graph = graph
        self.state = state
        # Object to draw sensor values with matplotlib
        self.sensorsWindow = None

        # get number of generalised coords
        q = self.graph.getVariable('q')
        q_shape = q.shape()
        if q_shape:
            self.nq = q_shape[0]
        else:
            self.nq = 1
        self.qScaling = [1]*self.nq

        # get inital values and start timer to update scene, track q changes
        initVals = self.graph.getinitVal(q)
        self.q = initVals
        self.qChanged = True
        self.updateSceneTimer.start()

        # Create sliders for genCoords
        self.sliders = []

        for i in range(self.nq):
            slider = LabeledSlider()
            qName = str(state.q[i])

            slider.setLabelText(qName)
            slider.setOrientation(Qt.Orientation.Horizontal)

            # set slider range depending on joint type
            if ('_Tx' in qName) or ('_Ty' in qName) or ('_Tz' in qName):
                lower, upper = self.options['qTransRange']
                slider.setLimits(int(lower*1000), int(upper*1000))
                slider.scale = 1000.

            elif ('_Rx' in qName) or ('_Ry' in qName) or ('_Rz' in qName):
                lower, upper = self.options['qRotRange']
                slider.setLimits(int(lower*100), int(upper*100))
                slider.scale = 100.

            elif 'flexible' in qName:
                slider.setLimits(-100,100)
                slider.scale = 100.
                self.hasFlexibleBodies = True

            else:
                print('Info: could not determine dof of joint "%s"' % i)
                slider.setLimits(-500,500)
                slider.scale = 100.

            init = float(str(initVals[i]))
            slider.setInitVal(init)
            slider.slider.setValue(int(init))  # TODO was float, requires int now

            self.kintestSliderGroupLayout.addWidget(slider)
            self.sliders.append(slider)

        # finalize scroll area layout
        #self.kintestSliderGroupLayout.addStretch(1.0) # TODO requires int
        self.kintestScrollArea.setWidget(self.kintestSliderGroup)


        # create a slider to scale flexible bodies state
        if self.hasFlexibleBodies:

            initVal = self.options['qFlexScaling']
            self.qScalingSlider = LabeledSlider(self.tabSimulation)
            self.qScalingSlider.setOrientation(Qt.Orientation.Horizontal)
            self.qScalingSlider.name = 'q_flexible_scale'
            self.qScalingSlider.setLimits(10, max(2*10*initVal,1000))
            self.qScalingSlider.scale = 10.
            self.qScalingSlider.setInitVal(initVal)
            self.qScalingSlider.slider.setValue(initVal*10)
            self.simulationLayout.insertWidget(3, self.qScalingSlider)


        # create sliders for inputs
        self.inputs = self.graph.getVariables(VarKind.Input)

        self.inputSliders = []
        for i in range(len(self.inputs)):
            shape = self.graph.getShape(self.inputs[i])
            if self.graph.getShape(self.inputs[i]) != tuple() :
                continue # TODO Nur Workaround, VektorenInputs müssen noch implementiert werden!
            slider = LabeledSlider()

            slider.setLabelText(str(self.inputs[i]))
            slider.setOrientation(Qt.Orientation.Horizontal)
            slider.setLimits(-500,500)
            slider.scale = 100.
            init = int(str(self.graph.getinitVal(self.inputs[i])))
            slider.setInitVal(init)
            slider.slider.setValue(init)

            self.simulationSliderGroupLayout.addWidget(slider)
            self.inputSliders.append(slider)

        # finalize scroll area layout
        #self.simulationSliderGroupLayout.addStretch(1.0)  # TODO requires int
        self.simulationScrollArea.setWidget(self.simulationSliderGroup)


        # import generated files
        # insert tempDir in first place, so that it is searched first
        # we had problems if there was a *_visual in the current directory
        sys.path.insert(0, tempfile.gettempdir())

        # import generated der_state_file
        self.mod_der_state = None
        self.mod_der_state_setInputs = None
        try:
            der_state_file = __import__('%s_der_state'%self.modelName, globals(), locals(), [])
            self.mod_der_state = getattr(der_state_file,'%s_der_state'%self.modelName)
            if hasattr(der_state_file,'%s_setInputs'%self.modelName):
                self.mod_der_state_setInputs = getattr(der_state_file,'%s_setInputs'%self.modelName)
        except:
            print("state derivative file '%s_der_state.py' or needed function inside this file not found! Something went wrong during code generation.\n%s" % (self.modelName,sys.exc_info()), file=sys.stderr)

        # import generated sensors_visual_file
        self.mod_sensors_visual = None
        try:
            sensors_visual_file = __import__('%s_visual'%self.modelName, globals(), locals(), [])
            self.mod_sensors_visual = getattr(sensors_visual_file,'%s_visual'%self.modelName)
        except:
            print("sensors file '%s_visual.py' or function '%s_visual' inside this file not found! Something went wrong during code generation.\n%s" % (self.modelName,self.modelName,sys.exc_info()), file=sys.stderr)


        # import generated sensors_file if model contains sensors
        self.mod_sensors = None
        self.mod_sensors_input = None
        self.sensorState = None
        self.sensorValues = []

        # filenames for dynamic libraries if compileModelC is called
        self.dynamicLibraries = None

        self.sensors =self.graph.getVariables(VarKind.Sensor)

        if self.sensors:
            try:
                sensors_file = __import__('%s_sensors'%self.modelName, globals(), locals(), [])
                self.mod_sensors = getattr(sensors_file,'%s_sensors'%self.modelName)
                if (hasattr(sensors_file,'%s_setInputs'%self.modelName)):
                    self.mod_sensors_input = getattr(sensors_file,'%s_setInputs'%self.modelName)
                self.showsensorButton.setDisabled(False) #Eigentlich unnötig, aber falls vorher deaktiviert
            except ImportError as err:
                print("sensors file '%s_sensors.py' or needed function inside this file not found! Maybe it has not been generated? (%s)" % (self.modelName, err), file=sys.stderr)
                print("Disabling Show Sensors Button!")
                self.showsensorButton.setDisabled(True)

        else:
             print('Info: model does not contain any sensors')
             self.showsensorButton.setDisabled(True)


        # Set Up Empty Integrator
        self.integrator = None

        # Create VtkScene objects
        self.vtkObjects = []
        for i in grList:
            self.vtkObjects.append(VtkSceneObject(i))

        # Connect signals and slots
        self.resetButton.clicked.connect(self.resetSliders)
        self.resetButton.clicked.connect(self.updateKinematics)

        self.showsensorButton.clicked.connect(self.showSensors)

        self.simulateButton.clicked.connect(self.runSimulation)

        self.compileCButton.clicked.connect(self.compileModelC)

        self.loadResultsButton.clicked.connect(self.loadResults)

        self.playResultsButton.clicked.connect(self.startPlayingResults)

        self.resetResultsButton.clicked.connect(self.resetResults)
        self.resetResultsButton.setEnabled(False)


        for i in self.sliders:
            i.slider.valueChanged.connect(i.setStateVal)
            i.slider.valueChanged.connect(self.updateKinematics)

        for i in self.inputSliders:
            i.slider.valueChanged.connect(i.setStateVal)

        # finalise setup for flexible bodies
        if self.hasFlexibleBodies:

            self.qScalingSlider.slider.valueChanged.connect(self.qScalingSlider.setStateVal)
            self.qScalingSlider.slider.valueChanged.connect(self.updateFlexScale)

            self.updateFlexScale()


        # Create simulation thread
        self.SimThread = PyMbsThread(self.simulate, realtime=True)

        # Create result thread
        self.ResultThread = PyMbsThread(self.playResults, realtime=True)


        # create recorder instance and connect required signals and slots
        self.recorder = PIRecorder(self.getSensorData)
        self.recordButton.clicked.connect(self.recorder.toggle)


    def getSensorData(self):
        '''
        Return the current state of the system and the dictionary with the
        corresponding positions and orientations of the visualisation objects.
        '''

        return (self.sensorState, self.sensorValues)


    def compileModelF90(self):
        print('Creating and compiling Fortran90 sources...')
        print("Writing sources to '%s'"%tempfile.gettempdir())

        self.graph.writeCode('f90', self.modelName, tempfile.gettempdir(), pymbs_wrapper=True)

        print("Compiling Fortran Code ...")
        compileF90('%s_der_state' % self.modelName, tempfile.gettempdir())
        compileF90('%s_visual' % self.modelName, tempfile.gettempdir())

        if self.sensors:
           compileF90('%s_sensors' % self.modelName, tempfile.gettempdir())

        self.mod_der_state = None
        self.mod_der_state_setInputs = None
        self.mod_sensors_visual = None

        try:
            der_state_file = __import__('%s_der_state_F90Wrapper' % self.modelName)
            self.mod_der_state = der_state_file.ode_int
            self.mod_der_state_setInputs = der_state_file.setInputs
            sensors_visual_file = __import__('%s_visual_F90Wrapper' % self.modelName)
            self.mod_sensors_visual = sensors_visual_file.graphVisualSensors
               #getattr(sensors_visual_file,'%s_visual'%self.modelName)
            print('Importing generated Fortran module successful')
        except Exception as e:
            print('Compiled Fortran module not found... (%s)' % e, file=sys.stderr)

        if self.sensors:
            # import generated sensors_file
            self.mod_sensors = None
            self.mod_sensors_input = None
            try:
                sensors_file = __import__('%s_sensors_F90Wrapper'%self.modelName, globals(), locals(), [])
                self.mod_sensors = sensors_file.graphSensors
                self.mod_sensors_input = sensors_file.setInputs
                self.showsensorButton.setDisabled(False) #Eigentlich unnötig, aber falls vorher deaktiviert
            except:
                print("Fortran sensors file '%s_sensors_F90Wrapper.py' or needed function inside this file not found! Maybe it has not been generated?\n%s" % (self.modelName,sys.exc_info()), file=sys.stderr)
                print("Disabling Show Sensors Button!")
                self.showsensorButton.setDisabled(True)



    def compileModelC(self):
        self.compileCButton.setEnabled(False)
        print('Creating and compiling C sources...')
        print("Writing sources to '%s'"%tempfile.gettempdir())

        self.dynamicLibraries = []
        self.graph.writeCode('c', self.modelName, tempfile.gettempdir(), pymbs_wrapper=True)
        print("Compiling C Code ...")
        self.dynamicLibraries.append(
            compileC('%s_der_state' % self.modelName, tempfile.gettempdir()))
        self.dynamicLibraries.append(
            compileC('%s_visual' % self.modelName, tempfile.gettempdir()))

        if self.sensors:
            self.dynamicLibraries.append(
                compileC('%s_sensors' % self.modelName, tempfile.gettempdir()))

        try:
            der_state_file = __import__('%s_der_state_CWrapper' % self.modelName)
            self.mod_der_state = der_state_file.ode_int
            self.mod_der_state_setInputs = der_state_file.setInputs
            sensors_visual_file = __import__('%s_visual_CWrapper' % self.modelName)
            self.mod_sensors_visual = sensors_visual_file.graphVisualSensors
            print('Importing generated C module successful')
        except Exception as e:
            print('Compiled C module not found... (%s)' % e, file=sys.stderr)

        if self.sensors:
            # import generated sensors_file
            self.mod_sensors = None
            self.mod_sensors_input = None
            try:
                sensors_file = __import__('%s_sensors_CWrapper'%self.modelName, globals(), locals(), [])
                self.mod_sensors = sensors_file.graphSensors
                self.mod_sensors_input = sensors_file.setInputs
                self.showsensorButton.setDisabled(False) #Eigentlich unnötig, aber falls vorher deaktiviert
            except:
                print("C sensors file '%s_sensors_CWrapper.py' or needed function inside this file not found! Maybe it has not been generated?\n%s" % (self.modelName,sys.exc_info()), file=sys.stderr)
                print("Disabling Show Sensors Button!")
                self.showsensorButton.setDisabled(True)



    def runSimulation(self):
        if (self.mod_der_state is None):
            print("Sorry! Viewing the model is disabled, since no der_state code is available!")
            return

        # Stop Playing of Results
        if (self.playResultsButton.isChecked()):
            self.playResultsButton.setChecked(False)
            self.ResultThread.stop()
            self.resetResults()

        if self.simulateButton.isChecked():
            # Set Up Integrator
            q0 = [i.stateVal/i.scale for i in self.sliders]

            # append initial values for constraints if neccessary
            qd0 = list(zeros(len(q0)))

            '''
            ODE support only
            if self.mu:
                qd0 += self.mu0
            '''

            self.integrator = Integrator(self.mod_der_state, q0, qd0)
            self.SimThread.reinit()
            self.SimThread.start()
        else:
            # Stop Thread
            self.SimThread.stop()



    def simulate(self):
        # Is everything ok?
        if ((self.mod_sensors_visual is None) or (self.mod_der_state is None)):
            print("Sorry! Simulation is disabled, since python code for sensors or der_state has not been imported properly!")
            self.SimThread.stop()
            return 0

        # Perform a step
        if (self.mod_der_state_setInputs != None):
            inputs = {i.name : i.stateVal/i.scale for i in self.inputSliders}
            self.mod_der_state_setInputs(inputs)

        # updateScene called by self.updateSceneTimer
        self.q = self.integrator.step()
        self.qChanged = True

        return self.integrator.t



    def loadResults(self):

        # load results and play them
        fileName = QFileDialog.getOpenFileName(self, 'Open MATLAB Result File', '',
                                               ("MATLAB mat-Files (*.mat)"))
        fileName = str(fileName)
        if (fileName == ''):
            return

        # Create MATLAB Player
        self.ResultPlayer = MatlabPlayer(str(fileName),vtkObjects=self.vtkObjects)

        # Enable Play Button
        self.playResultsButton.setEnabled(True)
        self.resetResultsButton.setEnabled(True)

        # Reset
        self.resetResults()



    def startPlayingResults(self):
        # stop simulation
        if self.simulateButton.isChecked():
            self.simulateButton.setChecked(False)
            self.SimThread.stop()

        # ReInit Thread if already running
        if (self.playResultsButton.isChecked()):
            # Start pymbs Thread
            self.ResultThread.reinit()

            p1 = self.ResultPlayer.get('%s[%s]'%(str(self.vtkObjects[0].r),str(1)))
            p2 = self.ResultPlayer.get('%s[%s]'%(str(self.vtkObjects[0].r),str(2)))
            p3 = self.ResultPlayer.get('%s[%s]'%(str(self.vtkObjects[0].r),str(3)))
            cp = self.ren.GetActiveCamera().GetPosition()
            self.cammpos = [cp[0]-p1,cp[1]-p2,cp[2]-p3]

            fp = self.ren.GetActiveCamera().GetFocalPoint()
            self.focuspos = [fp[0]-p1,fp[1]-p2,fp[2]-p3]

            self.ResultThread.scaling = float(self.scale.text())
            self.ResultThread.start()

        else:
            self.ResultThread.stop()



    def playResults(self):

        if (self.ResultPlayer.isModelica):
            self.updateSceneModelica()
        else:
            # get State Vector
            # updateScene called by self.updateSceneTimer
            self.q = self.ResultPlayer.getState()
            self.qChanged = True

        # next frame
        if (not self.ResultPlayer.nextSample()):
            self.playResultsButton.setChecked(False)
            self.ResultThread.stop()
            self.resetResults()
            return 0

        # return time
        return self.ResultPlayer.getTime()



    def resetResults(self):
        # reset player
        if (self.ResultPlayer is None):
            return

        self.ResultPlayer.reset()

        if (self.ResultPlayer.isModelica):
            self.updateSceneModelica()
        else:
            # get State Vector
            # updateScene called by self.updateSceneTimer
            self.q = self.ResultPlayer.getState()
            self.qChanged = True



    def updateFlexScale(self):
        self.options['qFlexScaling'] = self.qScalingSlider.value

        for i in range(self.nq):
            qName = str(self.state.q[i])
            if 'flexible' in qName:
                self.qScaling[i] = self.options['qFlexScaling']

        self.updateKinematics()



    def updateSceneModelica(self):

        for obj in self.vtkObjects:
            poke = vtkMatrix4x4()

            T = str(obj.T)
            r = str(obj.r)

            # copy values from T
            for i in range(3):
                poke.SetElement(i,3,self.ResultPlayer.get('%s[%s]'%(r,str(i+1))))
                for j in range(3):
                    poke.SetElement(i,j,self.ResultPlayer.get('%s[%s,%s]'%(T,str(i+1),str(j+1))))

            obj.actor.PokeMatrix(poke)
            obj.actor.SetScale(obj.scale)

        if (self.trackNode.	isChecked ()):
            if (self.cammpos is not None):
                p1 = self.ResultPlayer.get('%s[%s]'%(str(self.vtkObjects[0].r),str(1)))
                p2 = self.ResultPlayer.get('%s[%s]'%(str(self.vtkObjects[0].r),str(2)))
                p3 = self.ResultPlayer.get('%s[%s]'%(str(self.vtkObjects[0].r),str(3)))
                newpos = [p1+ self.cammpos[0],p2+ self.cammpos[1],p3+ self.cammpos[2]]
                newfpos = [p1+ self.focuspos[0],p2+ self.focuspos[1],p3+ self.focuspos[2]]
                self.ren.GetActiveCamera().SetPosition(newpos)
                self.ren.GetActiveCamera().SetFocalPoint(newfpos)

                self.ren.ResetCameraClippingRange()

        self.VtkScene.Render()



    def updateScene(self):
        '''
        Calculate Positions and Orientations (Poke Matrix) of all actors of the
        VtkScene. The state self.q is used to get positions and orientations of
        all actors from the model's sensors module. The scene is not rendered
        if the state did not change.
        '''

        if not self.qChanged:
            return

        q = [self.q[i]*self.qScaling[i] for i in range(self.nq)]

        # calculate sensors
        values = self.mod_sensors_visual(q[:self.nq])

        assert isinstance(values, dict)

        for obj in self.vtkObjects:

            if ((len(obj.r) == 1) and (len(obj.T) == 1)):
                poke = vtkMatrix4x4()

                obj_r = str(obj.r[0])
                obj_T = str(obj.T[0])

                # workaround for constant orientation (graph generates T = 1)
                if (isinstance(values[obj_T], int)) and (values[obj_T] == 1):
                    T = np.eye(3)
                else:
                    T = values[obj_T]


                r = values[obj_r]

                # copy values from T
                for i in range(3):
                    poke.SetElement(i,3,r[i])
                    for j in range(3):
                        poke.SetElement(i,j,T[i,j])

                obj.actor.PokeMatrix(poke)
                obj.actor.SetScale(obj.scale)


            else:
                points = vtkPoints()

                for position in obj.r:

                    position_as_string = str(position)

                    pos = values[position_as_string]
                    pos_as_tuple = (pos[0,0],pos[1,0],pos[2,0])

                    points.InsertNextPoint(pos_as_tuple)

                spline = vtkParametricSpline()
                spline.SetPoints(points)

                part = vtkParametricFunctionSource()
                part.SetParametricFunction(spline)
                part.Update()

                mapper = vtkPolyDataMapper()
                mapper.SetInputConnection(part.GetOutputPort())

                # Create Actor for moving, rotating, scaling, appearance etc.

                obj.actor.SetMapper(mapper)

                obj.actor.SetScale(obj.scale)

        self.VtkScene.Render()

        # save state and sensor values as class member for recorder
        self.sensorValues = values
        self.sensorState = q[:self.nq]
        self.qChanged = False



    def updateKinematics(self):
        # Do not allow Sliders during simulation or playback
        if self.simulateButton.isChecked():
            return
        if (self.playResultsButton.isChecked()):
            return

        # Is everything ok?
        if (self.mod_sensors_visual is None):
            print("Sorry! Viewing the model is disabled, since python code for sensors has not been imported properly!")
            self.SimThread.stop()
            return

        # Update Kinematics
        self.q = [i.stateVal/i.scale for i in self.sliders]
        self.qChanged = True



    def resetSliders(self):
        for i in self.sliders:
            i.slider.setValue(i.initVal)
        for i in self.inputSliders:
            i.slider.setValue(i.initVal)



    def saveGraphinfo(self,file,grList):

        f = open( file, 'w' )
        f.write('# -*- coding: utf-8 -*-\n')
        f.write('\'\'\'\n')
        f.write('Created on %s\n'%(time.strftime('%Y-%m-%d')))
        f.write('\n')
        f.write('@author: TU-Dresden pymbs Version %s\n'%pymbs_VERSION)
        f.write('\n')
        f.write('%s\n'%file)
        f.write('\'\'\'\n')
        f.write('\n')
        f.write('# Visualisationsensors\n')
        f.write('data = list()\n')
        for d in grList:
            r = str(d.r)
            T = str(d.T)
            if isinstance(d, File):
                file = d.path_to_file.lower()
                scale = float(d.scale)
                f.write('data.append([\'File\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'Filename\',\'%s\'),(\'scale\',%s)]])\n'%(d.name,r,T,file,scale))
            elif isinstance(d, Cylinder):
                f.write('data.append([\'Cylinder\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'res\',%s),(\'radius\',%s),(\'l\',%s)]])\n'%(d.name,r,T,d.res,d.radius,d.l))
            elif isinstance(d, Box):
                f.write('data.append([\'Box\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'lx\',%s),(\'ly\',%s),(\'lz\',%s)]])\n'%(d.name,r,T,d.lx,d.ly,d.lz))
            elif isinstance(d, Sphere):
                f.write('data.append([\'Sphere\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'radius\',%s),(\'res\',%s)]])\n'%(d.name,r,T,d.radius,d.res))
            elif isinstance(d, Frame):
                f.write('data.append([\'Frame\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'size\',%s)]])\n'%(d.name,r,T,d.size))
            elif isinstance(d, Line):
                f.write('data.append([\'Line\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'L\',%s)]])\n'%(d.name,r,T,d.length))
            elif isinstance(d, Arrow):
                f.write('data.append([\'Arrow\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\'),(\'size\',%s)]])\n'%(d.name,r,T,d.size))
            else:
                f.write('data.append([\'Unkown\',[(\'Name\',\'%s\'),(\'r\',\'%s\'),(\'T\',\'%s\')]])\n'%(d.name,r,T))
        f.write('\n')

        f.close()


    def showSensors(self):

        if (self.sensorsWindow is None):
            self.sensorsWindow = Sensors(self)
            self.sensorsWindow.show()


    def closeEvent(self, event):
        """
        Overload closeEvent to stop simulation, in case it was running
        when the window was closed
        """
        if self.SimThread.is_alive():
            self.SimThread.stop()
            self.SimThread.join()
        event.accept()




def myCallback(renWin, event):
    actor =  renWin.GetPicker().GetActor()

    if actor != None:
        actor.GetProperty().SetOpacity(0.5)

    renWin.Render()




def launchGui(grList, graph, modelname, gravity, state, options, **kwargs):
    app = QApplication(sys.argv)
    gui = Gui(grList, graph, modelname, state, options, **kwargs)
    gui.show()

    # determine normal
    left = [-1,0,0]
    if (gravity[1] != 0):
        up = [0,1,0]
        forwards = [0,0,1]
    else:
        up = [0,0,1]
        forwards = [0,1,0]


    # Set renderer and window
    ren = vtkRenderer()
    renWin = gui.VtkScene.GetRenderWindow()
    renWin.AddRenderer(ren)

    # Add actor to renderer and set background color
    for i in gui.vtkObjects:
        ren.AddActor(i.actor)

    ren.GradientBackgroundOn()
    ren.SetBackground2(26/255,  52/255,  78/255)
    ren.SetBackground(157/255, 150/255, 150/255)

    gui.updateKinematics()

    # SSAO - Screen Space Ambient Occlusion -> no effect? models look worse...
    basicPasses = vtk.vtkRenderStepsPass()
    sceneSize = 10                      # typically the diagonal of the bounding box in [m]

    ssao = vtk.vtkSSAOPass()
    ssao.SetRadius(0.1 * sceneSize)     # comparison radius
    ssao.SetBias(0.001 * sceneSize)     # comparison bias
    ssao.SetKernelSize(128)             # number of samples used
    ssao.BlurOff()                       # do not blur occlusion
    ssao.SetDelegatePass(basicPasses)

    #ren.SetPass(ssao)
    #ren.UseSSAOOn()    # messes up background, would require two renderes for fore- and background

    # change camera
    ren.GetActiveCamera().SetViewUp(up)
    ren.GetActiveCamera().SetFocalPoint([0,0,0])
    ren.GetActiveCamera().SetPosition(forwards)

    ren.ResetCamera()

    gui.ren = ren

    # Setup interaction with renderer window (mouse control etc.)
    iren = gui.VtkScene
    iren.SetInteractorStyle(vtkInteractorStyleTrackballCamera())
    iren.SetRenderWindow(renWin)
    iren.AddObserver('EndPickEvent', myCallback)
    iren.Initialize()

    app.exec()
