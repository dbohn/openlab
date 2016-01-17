import time

class Receiver(object):
	"""Parser for received lines"""
	def __init__(self, nodes, debug=False):
		super(Receiver, self).__init__()
		self.neighbours = {}
		self.infected = set()
		self.nodes = nodes
		self.friendlyNames = {}
		self.rounds = 0
		self.startTime = debug
		self.gossips = 0
		self.log = []
		self.finished = False

		self.debug = False

	def infect(self, node, init=False):
		"""Log the node as infected"""
		if init:
			self.startTime = time.time()
		self.infected.add(node)
	
	def parse_line(self, identifier, line):
		"""Parse an incoming line"""
		if self.debug:
			print identifier, line

		lineComponents = line.split(";")
		if len(lineComponents) > 2:
			# Msg format: <sender>;<uptime>;(MSG|ERR);<type>;...
			sender = lineComponents[0]
			uptime = lineComponents[1]
			msgKind = lineComponents[2]
			msgType = lineComponents[3]

			if msgKind == "MSG":
				if msgType == "NGB":
					numOfNeigbours = lineComponents[4]
					neighbours = lineComponents[5:]
					#print neighbours
					self.neighbours[sender] = neighbours
				elif msgType == "GOSSIP":
					self.gossips += 1
				elif msgType == "NEW-CACHE":
					initiator = lineComponents[4]
					roundNumber = int(lineComponents[5])
					value = int(lineComponents[6])
					self.infect(sender)
					self.log.append((sender, roundNumber, time.time() - self.startTime, self.gossips))
					print "Infection of %s in round %d" %(sender, roundNumber)
					if roundNumber > self.rounds:
						self.rounds = roundNumber
					if len(self.infected) >= len(self.neighbours):
						self.finishMeasurement(time.time())

			if sender not in self.friendlyNames:
				self.friendlyNames[sender] = identifier

	def finishMeasurement(self, endTime):
		"""Calculate all the times and log them"""
		self.finished = True
		timeDiff = endTime - self.startTime
		with open("exec_log-%s.log" %(time.strftime("%d-%m-%Y-%H-%M")), "w") as log:
			for line in self.log:
				log.write("%s\t%d\t%d\t%d\n" %(line[0], line[1], line[2], line[3]))
		with open("executions.log", "a") as f:
			f.write("\n%d\t%d\t%d\t%d" %(len(self.nodes), self.rounds, timeDiff, self.gossips))
		print "Done in %d rounds, taken %d seconds, less than %d gossips" %(self.rounds, timeDiff, self.gossips)

	def mergeLinks(self):
		"""Create links from the reported neighbours. Taken from distributed_algorithm example"""
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

