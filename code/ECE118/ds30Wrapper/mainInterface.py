from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import sys
from ece118 import guiWidgets
from ece118 import UsbSerial

from PyQt5.QtWidgets import QWidget





class mainInterface(QMainWindow):
	tempStatusSignal = pyqtSignal(str)
	def __init__(self, parent=None):
		super(mainInterface, self).__init__(parent)

		# self.setMinimumHeight(720)
		# self.setMinimumWidth(1280)
		self.resize(1280, 720)

		self.portInstance = UsbSerial.UsbSerial()
		self.portInstance.setAutoConnectMode(True)
		self.setWindowTitle("ECE118 Main Interface")
		# self.mainWindow = mainWidget(None)
		# self.statusBar().showMessage("Active Connection: {}".format("hi"), 1000)

		self.mainLayout = QVBoxLayout()
		self.mainWidget = QWidget()
		self.mainWidget.setLayout(self.mainLayout)
		self.setCentralWidget(self.mainWidget)


		# top level control layout

		self.connectButton = QPushButton("Connect")
		self.disconnectButton = QPushButton("Disconnect")
		self.burnButton = QPushButton("Burn Hex")
		self.checkForBLButton = QPushButton("Check for boot loader")

		self.barButtons = QHBoxLayout()

		self.barButtons.addWidget(self.connectButton)
		self.barButtons.addWidget(self.disconnectButton)
		self.barButtons.addWidget(self.burnButton)
		self.barButtons.addWidget(self.checkForBLButton)


		self.barButtons.addStretch()

		self.mainLayout.addLayout(self.barButtons)

		self.tabs = QTabWidget()
		self.mainLayout.addWidget(self.tabs)

		self.serialControl = guiWidgets.SerialControl.SerialControl(self.portInstance)

		self.connectButton.clicked.connect(self.serialControl.connect)
		self.disconnectButton.clicked.connect(self.serialControl.disconnect)

		self.tabs.addTab(self.serialControl, guiWidgets.SerialControl.widgetName)
		self.ds30Loader = guiWidgets.ds30Loader.ds30Loader(self.portInstance)
		self.tabs.addTab(self.ds30Loader, guiWidgets.ds30Loader.widgetName)
		self.ds30Loader.statusSignal = self.tempStatusSignal

		self.tabs.addTab(guiWidgets.SerialIO.SerialIO(self.portInstance), guiWidgets.SerialIO.widgetName)

		self.burnButton.clicked.connect(self.ds30Loader.startBurn)
		self.checkForBLButton.clicked.connect(self.ds30Loader.startBLCheck)

		self.tabs.setCurrentIndex(self.tabs.count()-1)

		self.show()

		# we add permanent widgets to the status bar to show a variety of information
		self.serialStatus = QLabel("")
		# self.packetTransmissions = QLabel(" Bob")
		self.statusBar().addPermanentWidget(self.serialStatus)
		# self.statusBar().addPermanentWidget(self.packetTransmissions)

		self.tempStatusSignal.connect(self.updateTempStatus)

		self.updateStatus()

		self.Timer = QTimer()
		self.Timer.timeout.connect(self.updateStatus)
		self.Timer.start(100)
		# self.tempStatusSignal.emit("Hello World")

		# if not self.portInstance.activeConnection:
		# 	QMessageBox.information(self, "Serial Port Status", "No Serial Ports Found")
		return

	def updateStatus(self):
		if self.portInstance.activeConnection:
			self.serialStatus.setText("{} Connected".format(self.portInstance.Port))
			self.connectButton.setDisabled(True)
			self.disconnectButton.setDisabled(False)
			self.checkForBLButton.setDisabled(False)
			self.burnButton.setDisabled(False)

			self.ds30Loader.burnButton.setDisabled(False)
			self.ds30Loader.checkBLButton.setDisabled(False)
		else:
			self.connectButton.setDisabled(False)
			self.disconnectButton.setDisabled(True)
			if self.portInstance.Port is None:
				self.serialStatus.setText("No Ports Found")
				self.checkForBLButton.setDisabled(True)
				self.burnButton.setDisabled(True)

				self.ds30Loader.burnButton.setDisabled(True)
				self.ds30Loader.checkBLButton.setDisabled(True)
			else:
				self.serialStatus.setText("{} Disconnected".format(self.portInstance.Port))


		# self.packetTransmissions.setText("Received: {} Transmitted: {}".format(self.portInstance.packetCountReceiving, self.portInstance.packetCountSending))
		return

	def updateTempStatus(self, newStatus):
		self.statusBar().showMessage(newStatus)


sys._excepthook = sys.excepthook

def my_exception_hook(exctype, value, tracevalue):
	# Print the error and traceback
	import traceback
	with open("LastCrash.txt", 'w') as f:
		# f.write(repr(exctype))
		# f.write('\n')
		# f.write(repr(value))
		# f.write('\n')
		traceback.print_exception(exctype, value, tracevalue, file=f)
		# traceback.print_tb(tracevalue, file=f)
	print(exctype, value, tracevalue)
	# Call the normal Exception hook after
	sys._excepthook(exctype, value, tracevalue)
	sys.exit(0)

# Set the exception hook to our wrapping function
sys.excepthook = my_exception_hook

app = QApplication(sys.argv)
gui = mainInterface()
gui.show()
app.exec_()