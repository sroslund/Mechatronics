from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import queue


from ece118 import UsbSerial

widgetName = "Serial I/O"
refreshRate = 20

class serialInput(QPlainTextEdit):
	def __init__(self):
		super().__init__(parent=None)
		return

	def createMimeDataFromSelection(self):
		returnType = QMimeData()
		rawSelection = self.textCursor().selectedText()
		rawSelection = rawSelection.replace("\u2029", "\n")
		rawSelection = rawSelection.replace("\x00", "")
		returnType.setText(rawSelection)
		return returnType

class SerialIO(QWidget):
	def __init__(self, portInstance, parent=None):
		super(SerialIO, self).__init__(parent)

		self.portInstance = portInstance

		self.usedLayout = QVBoxLayout()
		self.setLayout(self.usedLayout)


		inputBox = QHBoxLayout()
		self.usedLayout.addLayout(inputBox)
		self.serialAsciiOutput = QLineEdit()
		inputBox.addWidget(self.serialAsciiOutput)
		self.serialAsciiOutput.returnPressed.connect(self.sendAscii)

		self.newLineAddCheck = QCheckBox(r"Add New Line (\n)")
		inputBox.addWidget(self.newLineAddCheck)

		self.serialInput = serialInput()
		self.serialInput.setReadOnly(True)
		self.usedLayout.addWidget(self.serialInput)

		self.clearButton = QPushButton("Clear")
		self.usedLayout.addWidget(self.clearButton)
		self.clearButton.clicked.connect(self.serialInput.clear)

		self.serialInQueue = queue.Queue()

		self.portInstance.registerQueue(self.serialInQueue)

		self.serialChecker = QTimer()
		self.serialChecker.timeout.connect(self.updateOutput)
		self.serialChecker.start(refreshRate)



	def updateOutput(self):
		newLine = bytes()

		if not self.serialInQueue.empty():
			# print('hi')
			self.serialInput.moveCursor(QTextCursor.End)
			while not self.serialInQueue.empty():
				newLine += self.serialInQueue.get(block=False)
			newLine = newLine.decode('ascii', 'ignore')
			self.serialInput.insertPlainText(newLine)


	def sendAscii(self):
		print(self.serialAsciiOutput.text())
		for x in self.serialAsciiOutput.text():
			self.portInstance.sendBytes(x.encode('ascii', errors='ignore'))
		if self.newLineAddCheck.isChecked():
			self.portInstance.sendBytes('\n'.encode('ascii'))
		return
