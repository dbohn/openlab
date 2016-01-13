import subprocess
import time


class GraphvizRenderer(object):
	"""docstring for GraphRenderer"""
	def __init__(self):
		super(GraphvizRenderer, self).__init__()
		self.filetype = "pdf"
	
	def generateCode(self, graph):
		(simple, double) = graph
		source = "digraph g {\n\tconcentrate=true;\n"
		for (node, neighbour) in simple:
			source += "\t\"%s\" -> \"%s\" [color=red];\n" %(node, neighbour)
		for (node, neighbour) in double:
			source += "\t\"%s\" -> \"%s\" -> \"%s\" [color=black];\n" %(node, neighbour, node)
		source += "}\n"
		return source

	def compile(self, source):
		command = ["dot", "-T", self.filetype, "-o", "neighbourhood-%s.pdf" %(time.strftime("%d-%m-%Y-%H-%M"))]
		pipe = subprocess.Popen(command, stdin=subprocess.PIPE)
		pipe.communicate(input=source)

	def render(self, graph):
		code = self.generateCode(graph)
		self.compile(code)
