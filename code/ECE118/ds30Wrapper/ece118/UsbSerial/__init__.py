import serial
import threading
import queue
import serial.tools.list_ports
import functools
import time

RAWQUEUESIZE = 2048





class UsbSerial(object):
	def __init__(self, Port = None, BaudRate = 115200):


		# store the port info in instance variables
		self.Port = Port
		self.BaudRate = BaudRate

		# set up the dictionary of queues
		self.receivedBytesQueues = list()

		# set up the list for error handlers
		self.errorBackList = list()

		# set up list for outgoing messages
		self.outMessageCallBacks = list()

		# set up queue for outgoing serial as the serial write is blocking
		self.outputQueue = queue.Queue()

		# count packets
		self.packetCountReceiving = 0
		self.packetCountSending = 0

		# connect to the port but first set the activeFlag and create a ref to the port
		self.activeConnection = False
		self.serialPort = None
		self.Connect()

		# need a flag for auto-connect
		self.autoReConnect = False

		# clear the buffer
		if self.activeConnection:
			self.serialPort.reset_input_buffer()
			self.serialPort.reset_output_buffer()

		# set up the threaded input buffer
		self.incomingThread = threading.Thread(target=self.handleIncoming, name='IncomingHandler')
		self.incomingThread.daemon = True
		self.incomingThread.start()

		# set up the threaded output buffer
		self.outputThread = threading.Thread(target=self.handleOutGoing, name='outgoingHandler')
		self.outputThread.daemon = True
		self.outputThread.start()

		return

	@staticmethod
	def listSerialPorts():
		return [potPort.device for potPort in serial.tools.list_ports.comports()]

	def Connect(self):
		# we will handle the no port case here
		if self.Port is None:
			for potPort in serial.tools.list_ports.comports():
				if potPort.device == 'COM1':
					continue
				self.Port = potPort.device
		if self.Port is None:
			self.activeConnection = False
			return False
		else:
			try:
				# open the serial port
				self.serialPort = serial.Serial()
				self.serialPort.baudrate = self.BaudRate
				self.serialPort.port = self.Port
				# self.serialPort.dtr = None
				self.serialPort.open()
				self.activeConnection = True
				return True
			except serial.serialutil.SerialException:
				pass
		return False

	def Disconnect(self):
		if self.activeConnection:
			self.activeConnection = False
			self.serialPort.close()
			self.autoReConnect = False
			return True
		else:
			return False

	def registerQueue(self, inQueue):
		if type(inQueue) is not queue.Queue:
			return False
		else:
			self.receivedBytesQueues.append(inQueue)
		return True

	# def deregisterMessageHandler(self, messageID, functionPointer):
	# 	if type(messageID) is not MessageIDs:
	# 		return False
	# 	if messageID.value in self.callBackDict:
	# 		self.callBackDict[messageID.value].remove(functionPointer)
	# 	return True

	# def registerOutGoingMessageHandler(self, functionPointer):
	# 	"""gets a callback on outgoing messages, this is for all messages, up to application to filter"""
	# 	self.outMessageCallBacks.append(functionPointer)

	def registerErrorHandler(self, functionPointer):
		"""this is for errors in the serial stream, not in the packets"""
		self.errorBackList.append(functionPointer)
		return


	def handleIncoming(self):
		"""This is the threaded instance that actually handles the protocol"""
		# rawQueue = list()
		while True:
			if not self.activeConnection:
				if self.autoReConnect:
					self.Connect()
				time.sleep(.1)
				continue
			try:
				curChar = self.serialPort.read(1)
			except serial.serialutil.SerialException as e:
				self.handleError(e)
				continue
			for curQueue in self.receivedBytesQueues:
				curQueue.put(curChar, block=False)
		return

	def handleOutGoing(self):
		while True:
			if self.activeConnection:
				try:
					newByte = self.outputQueue.get()
					self.serialPort.write(newByte.to_bytes(1, byteorder='big'))
				except serial.serialutil.SerialException as e:
					self.handleError(e)
					pass
			else:
				time.sleep(.1)
		return

	def handleError(self, inException):
		self.activeConnection = False
		# print(inException)
		for fp in self.errorBackList:
			fpThread = threading.Thread(target=functools.partial(fp, (inException)))
			fpThread.daemon = True
			fpThread.start()
		return

	def sendBytes(self, message):
		for x in message:
			self.outputQueue.put(x, block=False)
		return

	def setAutoConnectMode(self, reconnectMode):
		self.autoReConnect = reconnectMode
		return


if __name__ == "__main__":
	import time
	import random
	import queue


	portInstance = UsbSerial()
	print("Port: {} Baudrate: {} Active: {}".format(portInstance.Port, portInstance.BaudRate, portInstance.activeConnection))

	tempQueue = queue.Queue()

	portInstance.registerQueue(tempQueue)

	while True:
		while not tempQueue.empty():
			print(tempQueue.get().decode('ascii'), end='')
		time.sleep(.1)

