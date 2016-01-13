import time

class Receiver(object):
	"""docstring for Receiver"""
	def __init__(self, nodes):
		super(Receiver, self).__init__()
		self.neighbours = {}
		self.infected = set()
		self.nodes = nodes
		self.friendlyNames = {}
		self.rounds = 0

	def infect(self, node, init=False):
		if init:
			self.startTime = time.time()
		self.infected.add(node)
	
	def parse_line(self, identifier, line):
		print identifier, line
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
			elif msgType == "NEW-CACHE":
				initiator = lineComponents[4]
				roundNumber = lineComponents[5]
				value = lineComponents[6]
				self.infect(sender)
				print "Infection of %s in round %d" %(sender, roundNumber)
				if roundNumber > self.rounds:
					self.rounds = roundNumber
				if len(self.infected) >= len(self.neighbours):
					print "Done in %d rounds, taken %d seconds" %(self.rounds, time.time() - self.startTime)

		if sender not in self.friendlyNames:
			self.friendlyNames[sender] = identifier

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

