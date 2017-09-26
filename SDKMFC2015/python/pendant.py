# A template for writing pyqt guis. This template will run without changes.  
import sys
import time, os, atexit, datetime
import numpy as np
from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
import pyqtgraph.ptime as ptime
import pyqtgraph.exporters
import pyqtgraph.exporters.ImageExporter as IE

import robot as rb
import SDK
import calSim as cs

def Cleanup():
  NrkSDK.close()
  pass
  
class complexParam(Parameter):
  def __init__(self, **opts):
    Parameter.__init__(self, **opts)
    self.DH = {}
    self.base = {}
    self.tool = {}
    self.pose = ()
    self.setDH()
    self.setBase()
    self.setTool()
    self.setPose()
  
  def setDH(self):
    self.DH = {}
    for c in self.children():
      if c.name() == 'Robot Params':
        for cc in c.children():
          self.DH[cc.name()] = {cc.param('alpha').name(): cc.param('alpha').value(), 
                  cc.param('A').name(): cc.param('A').value(), cc.param('D').name(): cc.param('D').value()}
          
  def setBase(self):
    for c in self.children():
      if c.name() == 'Extrinsic Params':
        for cc in c.children():
          if cc.name() == 'Base':
            self.base = {cc.param('X').name(): cc.param('X').value(), 
                         cc.param('Y').name(): cc.param('Y').value(), 
                         cc.param('Z').name(): cc.param('Z').value(),
                         cc.param('Rx').name(): cc.param('Rx').value(),
                         cc.param('Ry').name(): cc.param('Ry').value(),
                         cc.param('Rz').name(): cc.param('Rz').value()}
    pass
  
  def setTool(self):
    for c in self.children():
      if c.name() == 'Extrinsic Params':
        for cc in c.children():
          if cc.name() == 'Tool':
            self.tool = {cc.param('X').name(): cc.param('X').value(), 
                         cc.param('Y').name(): cc.param('Y').value(), 
                         cc.param('Z').name(): cc.param('Z').value(),
                         cc.param('Rx').name(): cc.param('Rx').value(),
                         cc.param('Ry').name(): cc.param('Ry').value(),
                         cc.param('Rz').name(): cc.param('Rz').value()}
          
  def setPose(self):
    for c in self.children():
      if c.name() == 'Pose':
        self.pose = (c.param('S').value(), c.param('L').value(), c.param('U').value(), 
                     c.param('R').value(), c.param('B').value(), c.param('T').value(), c.param('F').value())
                     
class Pendant(object):
  def __init__(self, **opts):
    self.NrkSDK = None
    if 'NrkSDK' in opts.keys():
      self.NrkSDK = opts['NrkSDK']
    self.app = None
    if 'QtApp' in opts.keys():
      self.app = opts['QtApp']
    self.pTree = None
    self.parameters = None
    self.win = None
    self.numRobots = 0
    self.activeRobot = 0
    self.setupTree()
    self.setupWindow()
    self.robots = []
    self.robotIDs = []
    self.runApp()
       
  def setupTree(self):
    params = [
        {'name': 'Extrinsic Params', 'type': 'group', 'children': [
            {'name': 'Base', 'type': 'group', 'children':[
              {'name': 'X', 'type': 'float', 'value': 100},
              {'name': 'Y', 'type': 'float', 'value': 100},
              {'name': 'Z', 'type': 'float', 'value': 100},
              {'name': 'Rx', 'type': 'float', 'value': 0},
              {'name': 'Ry', 'type': 'float', 'value': 0},
              {'name': 'Rz', 'type': 'float', 'value': 0},
              ]}
            ]},
        {'name':'Robot Params', 'type':'group', 'children': [
          {'name': 'S', 'type': 'group', 'children':[
                {'name': 'alpha', 'type': 'float', 'value': 0},
                {'name': 'A', 'type': 'float', 'value': 0},
                {'name': 'D', 'type': 'float', 'value': 540}
                ]},
          {'name': 'L', 'type': 'group', 'children':[
              {'name': 'alpha', 'type': 'float', 'value': 90},
              {'name': 'A', 'type': 'float', 'value': 140},
              {'name': 'D', 'type': 'float', 'value': 0}
              ]},
          {'name': 'U', 'type': 'group', 'children':[
              {'name': 'alpha', 'type': 'float', 'value': 0},
              {'name': 'A', 'type': 'float', 'value': 1150},
              {'name': 'D', 'type': 'float', 'value': 0}
              ]},
          {'name': 'R', 'type': 'group', 'children':[
              {'name': 'alpha', 'type': 'float', 'value': 90},
              {'name': 'A', 'type': 'float', 'value': 210},
              {'name': 'D', 'type': 'float', 'value': 1225}
              ]},  
          {'name': 'B', 'type': 'group', 'children':[
              {'name': 'alpha', 'type': 'float', 'value': 90},
              {'name': 'A', 'type': 'float', 'value': 0},
              {'name': 'D', 'type': 'float', 'value': 0}
              ]},
          {'name': 'T', 'type': 'group', 'children':[
              {'name': 'alpha', 'type': 'float', 'value': -90},
              {'name': 'A', 'type': 'float', 'value': 0},
              {'name': 'D', 'type': 'float', 'value': 0}
              ]},
          {'name': 'F', 'type': 'group', 'children':[
              {'name': 'alpha', 'type': 'float', 'value': 0},
              {'name': 'A', 'type': 'float', 'value': 0},
              {'name': 'D', 'type': 'float', 'value': 175}
              ]}
          ]},
        {'name': 'Pose', 'type':'group', 'children':[
            {'name': 'S', 'type': 'float', 'value':0},
            {'name': 'L', 'type': 'float', 'value':90},
            {'name': 'U', 'type': 'float', 'value':0},
            {'name': 'R', 'type': 'float', 'value':0},
            {'name': 'B', 'type': 'float', 'value':90},
            {'name': 'T', 'type': 'float', 'value':0},
            {'name': 'F', 'type': 'float', 'value':0}]},
        {'name': 'Add Robot', 'type': 'action'},
        {'name': 'Update Robot', 'type': 'action'},
        {'name': 'numRobots', 'type': 'int', 'value': 0},
        {'name': 'activeRobot', 'type': 'list', 'values': [0], 'value': 0}
      ]
    self.parameters = complexParam(name = 'params', type = 'group', children = params)
    self.pTree = ParameterTree()
    self.pTree.setParameters(self.parameters, showTop = False)

  def change(self, param, changes):
    for param, change, data in changes:
      path = self.parameters.childPath(param)
      if path is not None:
        childName = '.'.join(path)
      else:
        childName = param.name()
      if childName == 'Add Robot':
        self.addRobot()
      if childName == 'activeRobot':
        self.activeRobot = self.parameters.param('activeRobot').value()
      if 'Robot Params' in childName:
        self.parameters.setDH()
        if self.numRobots > 0:
          if not (self.robots[self.activeRobot].DHparams == self.parameters.DH):
            self.robots[self.activeRobot].DHparams = self.parameters.DH
            self.robots[self.activeRobot].killRobot()
            self.robots[self.activeRobot].genRobot()
      if 'Pose' in childName:
        self.parameters.setPose()
        if self.numRobots > 0:
          self.robots[self.activeRobot].setPose(*self.parameters.pose)
      if 'Extrinsic Params' in childName:
        self.parameters.setBase()
        if self.numRobots > 0:
          self.robots[self.activeRobot].moveBase(**self.parameters.base)
      if childName == 'Update Robot':
        if self.numRobots > 0:
          if not (self.robots[self.activeRobot].currPose == self.parameters.pose):
            self.robots[self.activeRobot].setPose(*self.parameters.pose)
          if not (self.robots[self.activeRobot].base == self.parameters.base):
            self.robots[self.activeRobot].moveBase(**self.parameters.base)
             
    print('  parameter: %s'% childName)
    print('  change:    %s'% change)
    print('  data:      %s'% str(data))
    print('  ----------')
    
  def addRobot(self):
    rCol = 'r%d' % self.numRobots
    rParams = {'base': self.parameters.base, 'DH': self.parameters.DH, 'ID': self.numRobots}
    self.robots.append(rb.robot(rCol, self.NrkSDK, 'SA', *self.parameters.pose, **rParams))
    self.robotIDs.append(self.numRobots)
    with self.parameters.treeChangeBlocker():
      self.parameters.removeChild(self.parameters.param('activeRobot'))
      child = {'name': 'activeRobot', 'type': 'list', 'values': self.robotIDs, 'value': self.activeRobot}
      self.parameters.addChild(child)
      #self.parameters.param('activeRobot').setLimits(self.robotIDs)
      self.numRobots = len(self.robots)
      self.parameters.param('numRobots').setValue(self.numRobots)

  def setupWindow(self):
    self.win = QtGui.QMainWindow()
    glw = QtGui.QWidget()
    layout = QtGui.QGridLayout()
    splitter = QtGui.QSplitter(QtCore.Qt.Horizontal)
    self.win.setCentralWidget(glw)
    glw.setLayout(layout)
    #self.win.setLayout(layout)
    layout.addWidget(splitter)
    splitter.addWidget(self.pTree)
    self.win.resize(800,300)
    self.win.setWindowTitle('Robot Pendant')
    self.win.show()
    self.win.raise_()

  def runApp(self):
    self.parameters.sigTreeStateChanged.connect(self.change)
    
if __name__ == '__main__':
  try:
    app = QtGui.QApplication.instance()
    if app is None:   #Qt cannot be called twice.  Checks if been created.  If so, doesn't create a new one.  Executes existing one on pendant.runApp()
      app = QtGui.QApplication([])
    else:
      print('QApplication instance already exists: %s' % str(app))
    path = "C:\\Users\\mallman\\Documents\\git\\SDKDlls\\SDKMFC2015\\Release"
    dllFile = "SDKMFC2015.dll"
    NrkSDK = SDK.SDKlib(os.path.join(path, dllFile))
    NrkSDK.connToSA()
    pendant = Pendant(NrkSDK = NrkSDK)
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
      if not (app is None): 
        app.exec_()
  finally:
    for r in pendant.robots:
      r.killRobot()
    Cleanup()
  
  
  
  
  
    
  
        

	    


