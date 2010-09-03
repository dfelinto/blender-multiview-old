# -*- coding: utf-8 -*-
# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

import inspect
import types
import textwrap
import re

import datatypes
import pyparser

class EPyKeyword:
	def __init__(self, title, text):
		self.title = title
		self.text = text

class EPyDocstring:
	@staticmethod
	def replace_inline_markup(line):
		line = re.sub(r"B\{([^}]*)\}", r"**\1**", line)
		line = re.sub(r"I\{([^}]*)\}", r"*\1*", line)
		line = re.sub(r"U\{(http://)*([^}]*)\}", r"http://\2", line)
		line = re.sub(r"L\{([^}]*)\}", r":class:`\1`", line)
		return line
	
	def __init__(self, obj):
		self.description = ''
		self.keywords = []
		
		docstring = textwrap.dedent(obj.__doc__)
		lines = docstring.splitlines()
		lines.reverse()
		
		while lines:
			line = lines.pop()
			
			if line.startswith('@'):
				lines.append(line)
				break
			self.description += line + '\n'
		self.description = self.replace_inline_markup(self.description)
		
		keyword = None
		
		while lines:
			line = lines.pop()
			if line.startswith('@'):
				mo = re.match(r"@([^:]+):(.*)$", line)
				if not mo:
					raise SyntaxError("Malformed keyword line: " + line)
				keyword = EPyKeyword(mo.group(1), mo.group(2).strip())
				self.keywords.append(keyword)
			else:
				keyword.text += ' ' + line.strip()
		
		for keyword in self.keywords:
			keyword.text = self.replace_inline_markup(keyword.text)

class EPyParser:
	@classmethod
	def parse(cls, module, name, short_desc):
		m = datatypes.Module(name, short_desc)
		doc = EPyDocstring(module)
		cls.get_inner_doc(m, doc)
		
		for name in (mem for mem in sorted(dir(module)) if pyparser.PyParser.is_external(mem)):
			obj = getattr(module, name)
			
			if inspect.isfunction(obj):
				m.functions[name] = cls.parse_function(name, obj, m)
			elif inspect.isclass(obj):
				m.classes[name] = cls.parse_class(name, obj, m)
			else:
				print('Error: Unknown object type: {0}.{1} -> {2}'.format(m.name, name, repr(obj)))
		
		return m
	
	@staticmethod
	def get_inner_doc(obj, docstring):
		obj.description = docstring.description
		
		for keyword in docstring.keywords:
			if keyword.title == "note":
				obj.notes.append(keyword.text)
			elif keyword.title == "see":
				obj.seealsos.append(keyword.text)
			elif keyword.title == "attention":
				obj.warnings.append(block.text)
			#elif keyword.title == "deprecated":
			#	obj.deprecateds.append(block.text)
			else:
				raise SyntaxError("Unknown keyword: {0} for {1}".format(keyword.title, obj.fullname()))
	
	@classmethod
	def parse_function(cls, name, function, parent):
		f = datatypes.Function(name, parent)
		
		doc = EPyDocstring(function)
		
		args = {}
		rets = {}
		
		# first get the arguments from the signature
		spec = inspect.getfullargspec(function)
		if spec.defaults:
			deflen = len(spec.defaults)
		else:
			deflen = 0
		
		for i, name in enumerate(spec.args):
			num = i - len(spec.args) + deflen
			if num >= 0:
				default = repr(spec.defaults[num])
			else:
				default = None
			arg = datatypes.Argument(name, default)
			f.arguments.append(arg)
			args[name] = arg
		
		if spec.varargs or spec.varkw or spec.kwonlyargs:
			raise SyntaxError("Varargs, varkw and kwonlyargs are not implemented, please implement!")
		
		# second get the argument docs by checking the keywords of the epydocstring
		keywords = []
		
		for keyword in doc.keywords:
			if keyword.title.startswith('param'):
				for key in keyword.title[5:].split(','):
					key = key.strip()
					if not key in args:
						raise SyntaxError("Argument not found in signature of {0}: {1}".format(f.fullname(), key))
					args[key].description = keyword.text
			elif keyword.title.startswith('type'):
				for key in keyword.title[4:].split(','):
					key = key.strip()
					if not key in args:
						raise SyntaxError("Argument not found in signature of {0}: {1}".format(f.fullname(), key))
					args[key]._type = keyword.text
			elif keyword.title.startswith('return') or keyword.title.startswith('returns'):
				if len(keyword.title) > 7:
					key = keyword.title[7:].strip()
				else:
					key = ""
				if not key:
					key = 'default'
				if not key in rets:
					rets[key] = datatypes.ReturnValue(key)
					f.return_values.append(rets[key])
				rets[key].description = keyword.text
			elif keyword.title.startswith('rtype'):
				key = keyword.title[5:].strip()
				if not key:
					key = 'default'
				if not key in rets:
					rets[key] = datatypes.ReturnValue(key)
					f.return_values.append(rets[key])
				rets[key]._type = keyword.text
			else:
				keywords.append(keyword)
		
		doc.keywords = keywords
		
		# print undocumented warnings
		for arg in f.arguments:
			if arg.description == "Undocumented" or arg._type == "Undocumented":
				print("Warning: Function {0} has an undocumented argument: {1}.".format(f.fullname(), arg.name))
		
		for ret in f.return_values:
			if ret.description == "Undocumented" or ret._type == "Undocumented":
				print("Warning: Function {0} has an undocumented return value: {1}.".format(f.fullname(), ret.name))
		
		cls.get_inner_doc(f, doc)
		return f
	
	@classmethod
	def parse_class(cls, name, _class, module):
		c = datatypes.Class(name, module)
		doc = EPyDocstring(_class)
		
		for name in (mem for mem in sorted(dir(_class)) if pyparser.PyParser.is_external(mem)):
			obj = getattr(_class, name)
			
			if inspect.ismethod(obj) or inspect.isfunction(obj) :
				c.methods[name] = cls.parse_function(name, obj, c)
			else:
				print('Error: Unknown object type: {0}.{1} -> {2}'.format(c.fullname(), name, repr(obj)))
		
		# read instance vars
		keywords = []
		
		for keyword in doc.keywords:
			if keyword.title.startswith('ivar'):
				for name in keyword.title[5:].split(','):
					name = name.strip()
					if name in c.attributes:
						a = c.attributes[name]
					else:
						a = datatypes.Attribute(name, c)
						c.attributes[name] = a
					a.description = keyword.text
			elif keyword.title.startswith('type'):
				for name in keyword.title[5:].split(','):
					name = name.strip()
					if name in c.attributes:
						a = c.attributes[name]
					else:
						a = datatypes.Attribute(name, c)
						c.attributes[name] = a
					a._type = keyword.text
			else:
				keywords.append(keyword)
		
		doc.keywords = keywords
		
		cls.get_inner_doc(c, doc)
		return c
