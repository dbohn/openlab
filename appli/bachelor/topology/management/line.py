
class Receiver(object):
	"""docstring for Receiver"""
	def __init__(self):
		super(Receiver, self).__init__()
		self.neighbours = {}
	
	def parse_line(self, identifier, line):
		lineComponents = line.split(";")
		sender = lineComponents[0]
		uptime = lineComponents[1]
		msgKind = lineComponents[2]
		msgType = lineComponents[3]

		if msgKind == "MSG":
			if msgType == "NGB":
				numOfNeigbours = lineComponents[4]
				neighbours = lineComponents[5:]
				self.neighbours[sender] = neighbours
