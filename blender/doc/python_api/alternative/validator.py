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
import pyparser

class Validator:
	@staticmethod
	def is_external(name):
		return not name.startswith('_')
	
	@staticmethod
	def is_structure_member(name):
		return not name.startswith('_') and not name.startswith('n_')
	
	@classmethod
	def check(cls, module, doc):
		if type(module) != types.ModuleType:
			cls.check_structure(doc, module)
		else:
			cls.check_module(doc, module)
	
	@staticmethod
	def doc_missing(parent, name):
		print('Error: {0}.{1} missing in documentation.'.format(parent.fullname(), name))
	
	@staticmethod
	def mem_missing(parent, name):
		print("Error: {0}.{1} is in documentation but doesn't exist anymore.".format(parent.fullname(), name))
	
	@staticmethod
	def module_members(m):
		return [o for o in m.classes.keys()] + [o for o in m.functions.keys()] + [o for o in m.dates.keys()]
	
	@staticmethod
	def class_members(c):
		return [o for o in c.methods.keys()] + [o for o in c.attributes.keys()]
	
	@classmethod
	def check_structure(cls, m, module):
		for name, desc in sorted(type(module).__dict__.items()):
			if type(desc) == types.MemberDescriptorType and cls.is_structure_member(name):
				if not name in m.dates:
					cls.doc_missing(m, name)
		members = cls.module_members(m)
		for o in members:
			if not hasattr(module, o):
				cls.mem_missing(m, o)
	
	@classmethod
	def check_module(cls, m, module):
		members = cls.module_members(m)
		
		for name in (mem for mem in dir(module) if cls.is_external(mem)):
			obj = getattr(module, name)
			
			if name not in members:
				cls.doc_missing(m, name)
			# elif inspect.isfunction(obj): TODO: could check arguments here ?!
			elif inspect.isclass(obj):
				cls.check_class(m.classes[name], obj)
				
		for o in members:
			if not hasattr(module, o):
				cls.mem_missing(m, o)
	
	@classmethod
	def check_class(cls, c, _class):
		members = cls.class_members(c)
		
		for name in (mem for mem in dir(_class) if cls.is_external(mem)):
			obj = getattr(_class, name)
			
			if name not in members:
				cls.doc_missing(c, name)
		
		for o in members:
			if not hasattr(_class, o):
				cls.mem_missing(c, o)
