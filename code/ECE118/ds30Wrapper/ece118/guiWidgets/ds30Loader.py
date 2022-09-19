from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtWidgets import QComboBox
from setuptools.command.test import test

import sys
import enum
import random
import struct
import datetime
import os
import subprocess
import time
import threading
import os

widgetName = "ds30 Loader"

ds30LoaderSearchPaths = [".", r"C:\ece118\ds30 Loader\bin", os.path.join("..", "..", "Utilities", "ds30_Loader", "bin"), os.path.join("..", "ds30 Loader", "bin")]
ds30exeName = 'ds30LoaderConsole.exe'

foundBLString = 'Found PIC32MX320F128H fw ver. 5.0.2'
successfulWriteString = 'Write successfully completed'

defaultArguments = [
	'-d=PIC32MX320F128H',  # need a device
	'-r=115200',  # baud rate
	'-m',  # reset by dtr
	'-b=10',  # hold in reset 10ms
	'-a=500',  # poll time
	'-t=3000',  # timeout time
	'--ht=1000', # need to adjust this to a value that makes sense
	'-b=10',  # hold in reset 10ms
	'-o',  # non interactive mode
]
class ds30Loader(QWidget):
	signal = pyqtSignal(int)
	stringSignal = pyqtSignal(str)
	def __init__(self, portInstance, parent=None):
		super().__init__(parent)

		self.exePath = None
		self.curSetting = QSettings("UCSC_SOE", "ECE118")

		self.portInstance = portInstance
		self.usedLayout = QVBoxLayout()
		self.setLayout(self.usedLayout)
		compress = QHBoxLayout()

		self.pathSelection = QComboBox()
		self.loadFileList()
		self.browseHex = QPushButton("&Browse for Hex")


		self.statusSignal = None

		compress.addWidget(self.pathSelection, 1)
		compress.addWidget(self.browseHex)
		self.browseHex.clicked.connect(self.askForFilePath)


		self.usedLayout.addLayout(compress)

		compress = QHBoxLayout()
		self.usedLayout.addLayout(compress)
		self.burnButton = QPushButton("Burn &Program")
		self.burnButton.clicked.connect(self.startBurn)
		compress.addWidget(self.burnButton)

		self.checkBLButton = QPushButton("Check for Bootloader")
		compress.addWidget(self.checkBLButton)
		self.checkBLButton.clicked.connect(self.startBLCheck)

		compress.addStretch()

		self.consoleOutput = QPlainTextEdit()
		self.consoleOutput.setReadOnly(True)

		self.usedLayout.addWidget(self.consoleOutput)
		self.stringSignal.connect(self.updateString)


		return

	def askForFilePath(self):
		fileSelect = QFileDialog(filter="hex files (*.hex)")
		fileSelect.setFileMode(QFileDialog.ExistingFile)
		fileSelect.setAcceptMode(QFileDialog.AcceptOpen)
		if fileSelect.exec():
			wantedFile = fileSelect.selectedFiles()[0]
			wantedFile = os.path.abspath(wantedFile)
			if self.pathSelection.findText(wantedFile) == -1:
				self.pathSelection.addItem(wantedFile)
			self.pathSelection.setCurrentIndex(self.pathSelection.findText(wantedFile))


	def startBurn(self):
		if not self.findExePath():
			return
		self.statusSignal.emit("Beginning Programming")
		self.saveFileList()
		wantedHex = self.pathSelection.currentText()
		if os.path.exists(wantedHex):
			self.consoleOutput.clear()
			threading.Thread(target=self.burnProgram, args=[wantedHex]).start()
		else:
			self.stringSignal.emit('Hex File Missing')
			self.statusSignal.emit('Hex File Missing, programming aborted')
		return

	def startBLCheck(self):
		if not self.findExePath():
			return
		self.consoleOutput.clear()
		self.statusSignal.emit("Checking for BootLoader")
		threading.Thread(target=self.checkBL).start()
		return

	def checkBL(self):
		self.portInstance.Disconnect()
		argumentList = list()
		argumentList.append(self.exePath)
		argumentList.extend(defaultArguments)
		argumentList.append('--find')
		argumentList.append('-k={}'.format(self.portInstance.Port))  # port from portocol

		self.consoleOutput.appendPlainText(" ".join(argumentList))

		process = subprocess.Popen(' '.join(argumentList), shell=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE,
								   universal_newlines=True)
		cachedOutput = str()
		while True:
			output = process.stdout.read(1)
			cachedOutput += output
			if output == '' and process.poll() is not None:
				break
			else:
				self.stringSignal.emit(output)

		if foundBLString in cachedOutput:
			self.statusSignal.emit('BootLoader Found')
			self.stringSignal.emit('BootLoader Found')
		else:
			self.statusSignal.emit('ERROR: BootLoader not Found')
			self.stringSignal.emit('ERROR: BootLoader not Found')
		self.portInstance.Connect()

		return

	def findExePath(self):
		if self.exePath is None:
			for path in ds30LoaderSearchPaths:
				exePath = os.path.join(path, ds30exeName)
				print(os.path.abspath(exePath))
				if os.path.exists(exePath):
					self.exePath = '"'+exePath+'"'
		if self.exePath is None:
			QMessageBox.critical(self, "Error", "No ds30Loader path found, loading code is not possible")
			return False
		else:
			return True

	def burnProgram(self, wantedHex):

		self.portInstance.Disconnect()

		argumentList = list()
		argumentList.append(self.exePath)
		argumentList.extend(defaultArguments)

		argumentList.append('-p')  # write flash
		argumentList.append('--file="'+wantedHex+'"') # the hex file
		argumentList.append('-k={}'.format(self.portInstance.Port)) # port from portocol

		self.consoleOutput.appendPlainText(" ".join(argumentList))

		process = subprocess.Popen(' '.join(argumentList), shell=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE, universal_newlines=True)

		cachedOutput = str()
		while True:
			output = process.stdout.read(1)
			cachedOutput += output
			if output == '' and process.poll() is not None:
				break
			else:
				self.stringSignal.emit(output)

		if successfulWriteString in cachedOutput:
			self.statusSignal.emit("Programming Complete")
			self.stringSignal.emit("Programming Complete")
		else:
			self.statusSignal.emit("ERROR: Programming Failed")
			self.stringSignal.emit("ERROR: Programming Failed")

		self.portInstance.Connect()
		return

	def saveFileList(self):
		if self.pathSelection.count() > 0:
			self.curSetting.beginGroup("ds30Loader")
			self.curSetting.beginWriteArray("used_files")
			for i in range(self.pathSelection.count()):
				self.curSetting.setArrayIndex(i)
				self.curSetting.setValue("Path", self.pathSelection.itemText(i))
			self.curSetting.endArray()
			self.curSetting.setValue('cur_index', self.pathSelection.currentIndex())
			self.curSetting.endGroup()
			self.curSetting.sync()
			print(self.curSetting.status())
		return

	def loadFileList(self):
		fileList = list()
		self.curSetting.beginGroup("ds30Loader")
		numItems = self.curSetting.beginReadArray("used_files")
		for i in range(numItems):
			self.curSetting.setArrayIndex(i)
			curPath = self.curSetting.value("Path")
			fileList.append(curPath)
			self.pathSelection.addItem(curPath)
		self.curSetting.endArray()
		curIndex = self.curSetting.value('cur_index')
		if curIndex is not None:
			self.pathSelection.setCurrentIndex(curIndex)
		self.curSetting.endGroup()
		return fileList

	def updateString(self, text):
		self.consoleOutput.insertPlainText(text)
		self.consoleOutput.ensureCursorVisible()
		return