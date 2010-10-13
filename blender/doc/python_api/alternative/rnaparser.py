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

import types

import rna_info
import bpy

import datatypes
import pyparser

class RNAParser:
	def __init__(self):
		self.structs, self.funcs, self.ops, self.props = rna_info.BuildRNAInfo()
		self.struct = 'bpy_struct'
	
	def parse(self):
		return self.parseOps(), self.parseTypes(), self.parseData()
	
	def parseOps(self):
		m = datatypes.Module('bpy.ops', 'Operators')
		m.description = 'This module contains all operators.'
		
		for key, op in self.ops.items():
			class_name = op.module_name.capitalize()
			if class_name in m.classes:
				_class = m.classes[class_name]
			else:
				m.classes[class_name] = _class = datatypes.Class(class_name, m)
			
			_class.methods[op.func_name] = f = datatypes.Function(op.func_name, _class)
			if not op.description or op.description == "(undocumented operator)":
				f.description = 'Undocumented!'
			else:
				f.description = op.description
			
			self.parseParameters(f, op)
			
			f.location = op.get_location()
			if f.location == (None, None):
				f.location = None
		
		return m
	
	def parseParameters(self, function, func):
		for prop in func.args:
			arg = datatypes.Argument(prop.identifier)
			function.arguments.append(arg)
			arg._type = prop.get_type_description(as_arg = True, class_fmt = ":class:`%s`")
			if prop.name or prop.description:
				arg.description = ", ".join([val for val in (prop.name, prop.description) if val])
			if prop.default_str:
				arg.default = prop.default_str
	
	def parseTypes(self):
		m = datatypes.Module('bpy.types', 'Types')
		m.description = 'This module contains all blender data types.'
		
		for struct in self.structs.values():
			if "_OT_" in struct.identifier:
				continue
			
			if struct.identifier in m.classes:
				_class = m.classes[struct.identifier]
			else:
				m.classes[struct.identifier] = _class = datatypes.Class(struct.identifier, m)
			_class.description = struct.description
			
			# base class
			basename = getattr(struct.base, "identifier", self.struct)
			if basename in m.classes:
				_class.base = m.classes[basename]
			else:
				m.classes[basename] = _class.base = datatypes.Class(basename, m)
			
			# RNA properties
			for prop in struct.properties:
				_class.attributes[prop.identifier] = attr = datatypes.Attribute(prop.identifier, _class, prop.get_type_description(class_fmt=":class:`%s`"))
				if prop.description:
					attr.description = prop.description
			
			# RNA functions
			for func in struct.functions:
				_class.methods[func.identifier] = f = datatypes.Function(func.identifier, _class)
				f.description = func.description
				self.parseParameters(f, func)
				if func.return_values:
					for retval in func.return_values:
						ret = datatypes.ReturnValue(retval.identifier)
						f.return_values.append(ret)
						ret._type = retval.get_type_description(as_ret=True, class_fmt=":class:`%s`")
						if retval.description:
							ret.description = retval.description
						else:
							ret.description = retval.name
			
			# Python properties
			for name, prop in struct.get_py_properties():
				_class.attributes[name] = a = pyparser.PyParser.parse_attribute(name, prop, _class)
				if not prop.fset:
					a.readonly = True
			
			# Python functions
			for name, func in struct.get_py_functions():
				_class.methods[name] = pyparser.PyParser.parse_function(name, func, _class)
			
			# References
			for ref in struct.references:
				ref_split = ref.split('.')
				if len(ref_split) > 2:
					ref = '{0}.{1}'.format(ref_split[-2], ref_split[-1])
				_class.seealsos.append(ref)
		
		# special structure: bpy_struct
		if self.struct:
			_class = pyparser.PyParser.parse_class(self.struct, bpy.types.Struct.__bases__[0], m)
			if self.struct in m.classes:
				_class2 = m.classes[self.struct]
				_class2.methods = _class.methods
				_class2.attributes = _class.attributes
				_class = _class2
			else:
				m.classes[self.struct] = _class
			_class.description = 'Built-in base class for all classes in bpy.types.'
			_class.notes.append('Note that bpy.types.{0} is not actually available from within blender, it only exists for the purpose of documentation.'.format(self.struct))
		
		return m
		
	def parseData(self):
		m = datatypes.Module('bpy.data', 'Data Access')
		m.description = 'This module is used for all blender/python access.'
		return m
