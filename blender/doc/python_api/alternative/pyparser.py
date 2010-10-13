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

import datatypes
import sphinxparser

class PyParser:
	@staticmethod
	def is_external(name):
		return not name.startswith('_')
	
	@staticmethod
	def is_structure_member(name):
		return not name.startswith('_') and not name.startswith('n_')
	
	@staticmethod
	def get_doc(o, obj):
		if obj.__doc__:
			o.description = obj.__doc__
			sphinxparser.SphinxParser.parse_inner_doc(o)
		else:
			o.description = "Undocumented! (`contribute to {0} " \
				"<http://wiki.blender.org/index.php/Dev:2.5/Py/API/Documentation/Contribute" \
				"?action=edit&section=new&preload=Dev:2.5/Py/API/Documentation/Contribute/Howto-message" \
				"&preloadtitle={0}>`_)".format(o.fullname())
			print("Warning: object is undocumented: " + o.fullname())
	
	@classmethod
	def parse(cls, module, name, short_desc):
		m = datatypes.Module(name, short_desc)
		cls.get_doc(m, module)
		
		if type(module) != types.ModuleType:
			cls.parse_structure(m, module)
		else:
			cls.parse_module(m, module)
		
		return m
	
	@classmethod
	def parse_structure(cls, m, module):
		for name, desc in sorted(type(module).__dict__.items()):
			if type(desc) == types.MemberDescriptorType and cls.is_structure_member(name):
				data = datatypes.Data(name, m)
				cls.get_doc(data, desc)
				m.dates[name] = data
	
	@classmethod
	def parse_module(cls, m, module):
		for name in (mem for mem in sorted(dir(module)) if cls.is_external(mem)):
			obj = getattr(module, name)
			
			if inspect.isbuiltin(obj):
				m.functions[name] = cls.parse_builtin(name, obj, m)
			elif inspect.isfunction(obj):
				m.functions[name] = cls.parse_function(name, obj, m)
			elif inspect.isclass(obj):
				if name.islower():
					print('Note: Not documenting lowercase class: {0}.{1}'.format(m.name, name))
				else:
					m.classes[name] = cls.parse_class(name, obj, m)
			elif isinstance(obj, (bool, int, float, str, tuple)):
				m.dates[name] = cls.parse_data(name, obj, m, repr(obj))
			else:
				print('Error: Unknown object type: {0}.{1} -> {2}'.format(m.name, name, repr(obj)))
	
	@classmethod
	def parse_builtin(cls, name, function, parent):
		f = datatypes.Function(name, parent)
		cls.get_doc(f, function)
		sphinxparser.SphinxParser.parse_function(f)
		if cls.is_classmethod(function):
			f.decorator = 'class'
		elif cls.is_staticmethod(function):
			f.decorator = 'static'
		return f
	
	@staticmethod
	def get_function_signature(function):
		spec = inspect.getfullargspec(function)
		args = []
		deflen = 0
		if spec.defaults:
			deflen = len(spec.defaults)
		for i, arg in enumerate(spec.args):
			num = i - len(spec.args) + deflen
			if num >= 0:
				args.append("{0}={1}".format(arg, spec.defaults[num]))
			else:
				args.append(arg)
		if spec.varargs:
			args.append("*{0}".format(spec.varargs))
		if spec.varkw:
			args.append("**{0}".format(spec.varkw))
		for arg in spec.kwonlyargs:
			if spec.kwonlydefaults and arg in spec.kwonlydefaults:
				args.append("{0}={1}".format(arg, spec.kwonlydefaults[arg]))
			else:
				args.append(arg)
		return "{0}({1})".format(function.__name__, ", ".join(args))
	
	@classmethod
	def parse_function(cls, name, function, parent):
		f = datatypes.Function(name, parent)
		cls.get_doc(f, function)
		f.description = "{0}\n{1}".format(cls.get_function_signature(function), f.description)
		sphinxparser.SphinxParser.parse_function(f)
		if f.arguments:
			if f.arguments[0].name == 'cls':
				f.decorator = 'class'
			elif f.arguments[0].name == 'self':
				f.arguments = f.arguments[1:]
			else:
				f.decorator = 'static'
		return f
	
	@classmethod
	def parse_data(cls, name, data, parent, value):
		d = datatypes.Data(name, parent, value)
		if value:
			d.description = 'Constant value: ' + value
		else:
			d.description = 'Undocumented!'
		return d
	
	@classmethod
	def parse_attribute(cls, name, attribute, parent):
		a = datatypes.Attribute(name, parent)
		cls.get_doc(a, attribute)
		return a
	
	@staticmethod
	def is_staticmethod(obj):
		return isinstance(obj, staticmethod)
	
	@staticmethod
	def is_classmethod(obj):
		return isinstance(obj, classmethod) or (isinstance(obj, types.BuiltinFunctionType) and hasattr(obj, '__self__') and obj.__self__ is not None)
	
	@classmethod
	def parse_class(cls, name, _class, module):
		c = datatypes.Class(name, module)
		cls.get_doc(c, _class)
		
		for name in (mem for mem in sorted(dir(_class)) if cls.is_external(mem)):
			obj = getattr(_class, name)
			
			if inspect.isbuiltin(obj) or inspect.ismethoddescriptor(obj):
				c.methods[name] = m = cls.parse_builtin(name, obj, c)
			elif inspect.ismethod(obj):
				c.methods[name] = m = cls.parse_function(name, obj, c)
			elif inspect.isdatadescriptor(obj):
				c.attributes[name] = cls.parse_attribute(name, obj, c)
			else:
				print('Error: Unknown object type: {0}.{1} -> {2}'.format(c.fullname(), name, repr(obj)))
		
		return c
