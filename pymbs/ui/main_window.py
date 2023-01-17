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

# -*- coding: utf-8 -*-


from PyQt5 import QtCore, QtGui
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


