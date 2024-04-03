"""
This module contains the main window class for the pymbs ui/app
"""

from PyQt6 import QtCore, QtGui
from vtk.qt5.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor


class PyMbsMainWindow(object):
    def setupMainWindow(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(769, 568)
        MainWindow.setMinimumSize(QtCore.QSize(400, 200))
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.horizontalLayout = QtGui.QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.SliderGroup = QtGui.QGroupBox(self.centralwidget)
        self.SliderGroup.setMinimumSize(QtCore.QSize(150, 0))
        self.SliderGroup.setMaximumSize(QtCore.QSize(150, 16777215))
        self.SliderGroup.setObjectName("SliderGroup")
        self.horizontalLayout.addWidget(self.SliderGroup)
        self.VtkScene = QVTKRenderWindowInteractor(self.centralwidget)
        self.VtkScene.setObjectName("VtkScene")
        self.horizontalLayout.addWidget(self.VtkScene)
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "PyMbs Kintest", None, QtGui.QApplication.UnicodeUTF8))
        self.SliderGroup.setTitle(QtGui.QApplication.translate("MainWindow", "GenCoords", None, QtGui.QApplication.UnicodeUTF8))


