
class Receiver(object):
	"""docstring for Receiver"""
	def __init__(self):
		super(Receiver, self).__init__()
		self.neighbours = {}
	
	def parse_line(self, identifier, line):
		#print identifier, line
		lineComponents = line.split(";")
		sender = lineComponents[0]
		uptime = lineComponents[1]
		msgKind = lineComponents[2]
		msgType = lineComponents[3]
		#print sender,msgKind,msgType
		if msgKind == "MSG":
			if msgType == "NGB":
				numOfNeigbours = lineComponents[4]
				neighbours = lineComponents[5:]
				#print neighbours
				self.neighbours[sender] = neighbours

	def mergeLinks(self):
		simple = set()
		double = set()
		for node, neighbours in self.neighbours.items():
			for neigh in neighbours:
				link = (node, neigh)
				rev_link = (neigh, node)
				if rev_link not in simple:
					simple.add(link)
				else:
					simple.remove(rev_link)
					double.add(link)
		return (simple, double)

