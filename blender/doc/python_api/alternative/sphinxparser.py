# -*- coding: utf-8 -*-

import sys
import re
import datatypes

class SphinxBlock:
	def __init__(self):
		self.ident = 0
		self.title = ''
		self.text = ''
		self.children = []
		self.parent = None

class SphinxParser:
	@staticmethod
	def get_ident(line):
		line.rstrip()
		for i in range(len(line)):
			if not line[i].isspace():
				return i
	
	@classmethod
	def parse_tree(cls, lines):
		tree = SphinxBlock()
		tree.parent = tree
		current = tree
		for line in lines:
			ident = cls.get_ident(line)
			if line.strip():
				if ident <= current.ident:
					while ident < current.ident:
						current = current.parent
					if ident == current.ident:
						current = current.parent
					else:
						raise IndentationError(line)
				if line.lstrip().startswith('..'):
					next = SphinxBlock()
					next.ident = ident
					next.parent = current
					next.title = line.lstrip()[2:].strip()
					mo = re.match(r"([^:]+)::(.*)", next.title)
					if not mo:
						raise SyntaxError("Invalid Sphinx Syntax: " + next.title)
					next.title = mo.group(1)
					next.text = mo.group(2).strip() + '\n'
					current.children.append(next)
					current = next
				else:
					current.text += line[current.ident:] + '\n'
			elif not current.text.endswith('\n\n'):
				current.text += '\n'
		cls.format_block(tree)
		return tree
	
	@staticmethod
	def dedent(text):
		lines = text.splitlines()
		minindent = 100
		for i in range(1, len(lines)):
			if lines[i].strip():
				indent = len(lines[i]) - len(lines[i].lstrip())
				if indent < minindent:
					minindent = indent
		for i in range(1, len(lines)):
			lines[i] = lines[i][minindent:]
		return '\n'.join(lines)
	
	@classmethod
	def format_block(cls, block):
		block.text = cls.dedent(block.text).strip()
		
		for b in block.children:
			cls.format_block(b)
	
	@classmethod
	def max_depth(cls, block):
		if block.children:
			m = 0
			for c in block.children:
				d = cls.max_depth(c)
				if d > m:
					m = d
			return m + 1
		return 0
	
	@classmethod
	def parse(cls, filename, name, short_desc):
		file = open(filename)
		tree = cls.parse_tree(file)
		
		m = datatypes.Module(name, short_desc)
		cls.parse_module(m, tree)
		return m
	
	@classmethod
	def parse_module(cls, m, tree):
		m.description = tree.text
		
		children = []
		
		for block in tree.children:
			if block.title == 'function':
				f = cls.function(block, m)
				m.functions[f.name] = f
			elif block.title == 'data':
				d = cls.data(block, m)
				m.dates[d.name] = d
			elif block.title == 'class':
				c = cls.parse_class(block, m)
				m.classes[c.name] = c
			elif block.title == 'module':
				continue
			else:
				children.append(block)
		
		tree.children = children
		cls.extract_inner_doc(tree, m)
	
	@classmethod
	def function(cls, tree, parent):
		f = datatypes.Function(None, parent)
		f.description = tree.text
		cls.parse_function(f)
		cls.extract_inner_doc(tree, f)
		return f
	
	@classmethod
	def data(cls, tree, parent):
		lines = tree.text.splitlines()
		name = lines[0]
		lines = lines[1:]
		d = datatypes.Data(name, parent)
		d.description = '\n'.join(lines)
		cls.extract_inner_doc(tree, d)
		return d
	
	@classmethod
	def attribute(cls, tree, parent):
		lines = tree.text.splitlines()
		name = lines[0]
		lines = lines[1:]
		a = datatypes.Attribute(name, parent)
		a.description = '\n'.join(lines)
		cls.extract_inner_doc(tree, a)
		return a
	
	@classmethod
	def parse_class(cls, tree, module):
		lines = tree.text.splitlines()
		name = lines[0]
		lines = lines[1:]
		
		mo = re.match(r"([^(]+)(\((.+)\))?", name.strip())
		if not mo:
			raise SyntaxError(name)
		name = mo.group(1)
		base = mo.group(3)
		
		if name in module.classes:
			c = module.classes[name]
		else:
			module.classes[name] = c = datatypes.Class(name, module)
		
		if base:
			if base in module.classes:
				c.base = module.classes[base]
			else:
				module.classes[base] = c.base = datatypes.Class(base, module)
		
		c.description = '\n'.join(lines)
		
		children = []
		
		for block in tree.children:
			if block.title == 'method':
				m = cls.function(block, c)
				c.methods[m.name] = m
			elif block.title == 'attribute':
				a = cls.attribute(block, c)
				c.attributes[a.name] = a
			elif block.title == 'data':
				a = cls.attribute(block, c)
				a.readonly = True
				c.attributes[a.name] = a
			else:
				children.append(block)
		
		tree.children = children
		cls.extract_inner_doc(tree, c)
		
		return c
	
	@classmethod
	def extract_inner_doc(cls, tree, obj):
		for block in tree.children:
			if block.title == "warning":
				obj.warnings.append(block.text)
			elif block.title == "note":
				obj.notes.append(block.text)
			elif block.title == "seealso":
				obj.seealsos.append(block.text)
			elif block.title == "deprecated":
				obj.deprecateds.append(block.text)
			else:
				raise SyntaxError("Unknown inner doc block: " + block.title)
	
	@classmethod
	def parse_inner_doc(cls, obj):
		tree = cls.parse_tree(obj.description.splitlines())
		cls.format_block(tree)
		
		if tree.text == "" and len(tree.children) == 1:
			tree = tree.children[0]
		
		if cls.max_depth(tree) > 1:
			raise IndentationError(tree.title)
		
		obj.description = tree.text
		
		cls.extract_inner_doc(tree, obj)
	
	@staticmethod
	def split_arguments(arguments):
		args = arguments.split(',')
		origlen = 0
		while origlen != len(args):
			origlen = len(args)
			i = 0
			while i < len(args):
				mo = re.search(r"[(){}[\]]", args[i])
				if mo and not "=" in args[i]:
					args[i-1] += "," + args.pop(i)
				else:
					i += 1
		return args
	
	@classmethod
	def parse_function(cls, obj):
		lines = obj.description.splitlines()
		lines.reverse()
		
		line = lines.pop()
		
		# find the signature
		mo = re.match(r"([^(]+)\((.*)\)$", line)
		if not mo or 'Undocumented' in line:
			print('Warning: function {0} is undocumented!'.format(obj.fullname()))
			return
		if not obj.name:
			obj.name = mo.group(1).strip()
		elif mo.group(1).strip() != obj.name:
			print("Warning: Signature function name '{0}' doesn't fit to real function name '{1}'.".format(mo.group(1).strip(), obj.fullname()))
		signature = line
		
		# find the arguments
		args = {}
		if mo.group(2).strip():
			arguments = cls.split_arguments(mo.group(2))
			for argument in arguments:
				default = None
				if "=" in argument:
					mo = re.match(r"([^=]+)=(.+)$", argument)
					if not mo:
						raise SyntaxError("Invalid default parameter: " + signature)
					argument = mo.group(1)
					default = mo.group(2).strip()
				arg = datatypes.Argument(argument.strip(), default)
				obj.arguments.append(arg)
				args[arg.name] = arg
		
		# read the description
		obj.description = ""
		
		while lines:
			line = lines.pop()
			if line.strip().startswith(":"):
				lines.append(line)
				break
			obj.description += line + '\n'
		
		obj.description = cls.dedent(obj.description)
		
		# parse info fields
		rets = {}
		last = None
		while lines:
			line = lines.pop()
			mo = re.match(r":([^:]+):(.*)$", line.strip())
			if not mo:
				if last:
					setattr(last[0], last[1], getattr(last[0], last[1]) + '\n' + line)
					continue
				else:
					raise SyntaxError("Not an info field: " + line)
			key = mo.group(1).strip()
			value = mo.group(2).strip()
			if key.startswith('arg'):
				key = key[3:].strip()
				if not key in args:
					raise SyntaxError("Argument not found in signature: " + line)
				args[key].description = value
				last = (args[key], 'description')
			elif key.startswith('type'):
				key = key[4:].strip()
				if not key in args:
					raise SyntaxError("Argument not found in signature: " + line)
				args[key]._type = value
				last = (args[key], '_type')
			elif key.startswith('return'):
				key = key[6:].strip()
				if not key:
					key = 'default'
				if not key in rets:
					rets[key] = datatypes.ReturnValue(key)
					obj.return_values.append(rets[key])
				rets[key].description = value
				last = (rets[key], 'description')
			elif key.startswith('rtype'):
				key = key[5:].strip()
				if not key:
					key = 'default'
				if not key in rets:
					rets[key] = datatypes.ReturnValue(key)
					obj.return_values.append(rets[key])
				rets[key]._type = value
				last = (rets[key], '_type')
			else:
				raise SyntaxError("Unknown info field: " + line)
		
		# print undocumented warnings
		for arg in obj.arguments:
			if arg.name != "self" and arg.description == "Undocumented" or arg._type == "Undocumented":
				print("Warning: Function {0} has an undocumented argument: {1}.".format(obj.fullname(), arg.name))
		
		for ret in obj.return_values:
			if ret.description == "Undocumented" or ret._type == "Undocumented":
				print("Warning: Function {0} has an undocumented return value: {1}.".format(obj.fullname(), ret.name))
