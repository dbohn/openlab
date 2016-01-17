import subprocess
import time

class GraphvizRenderer(object):
	"""This class will render a neighbourhood graph using GraphViz"""
	def __init__(self):
		super(GraphvizRenderer, self).__init__()
		self.filetype = "pdf"
	
	def generateCode(self, graph, names=None):
		"""Generates the GraphViz Source Code"""
		(simple, double) = graph
		source = "digraph g {\n\tconcentrate=true;\n"
		for (node, neighbour) in simple:
			source += "\t\"%s\" -> \"%s\" [color=red];\n" %(self.pick(names, node), self.pick(names, neighbour))
		for (node, neighbour) in double:
			source += "\t\"%s\" -> \"%s\" -> \"%s\" [color=black];\n" %(self.pick(names, node), self.pick(names, neighbour), self.pick(names, node))
		source += "}\n"
		return source

	def compile(self, source):
		"""Compiles the given source code"""
		command = ["dot", "-T", self.filetype, "-o", "neighbourhood-%s.pdf" %(time.strftime("%d-%m-%Y-%H-%M"))]
		pipe = subprocess.Popen(command, stdin=subprocess.PIPE)
		pipe.communicate(input=source)

	def render(self, graph, names=None):
		"""Generates the code and compiles it"""
		code = self.generateCode(graph, names)
		self.compile(code)

	def pick(self, names, node):
		"""Helper method to pick the node name"""
		if names is not None and node in names:
			return names[node]
		return node