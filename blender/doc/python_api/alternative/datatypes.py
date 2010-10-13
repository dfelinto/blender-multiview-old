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

class DocObj:
	def __init__(self, name):
		self.name = name
		self.parent = None
		self.description = "Undocumented"
		self.warnings = []
		self.notes = []
		self.seealsos = []
		self.deprecateds = []
	def fullname(self):
		name = self.name
		obj = self.parent
		while obj:
			name = obj.name + "." + name
			obj = obj.parent
		return name

class Module(DocObj):
	def __init__(self, name, short_desc):
		DocObj.__init__(self, name)
		self.classes = {}
		self.dates = {}
		self.functions = {}
		self.short_desc = short_desc

class Class(DocObj):
	def __init__(self, name, parent):
		DocObj.__init__(self, name)
		self.parent = parent
		self.methods = {}
		self.attributes = {}
		self.base = None

class Data(DocObj):
	def __init__(self, name, parent, value = None):
		DocObj.__init__(self, name)
		self.parent = parent
		self.value = value

class Function(DocObj):
	def __init__(self, name, parent):
		DocObj.__init__(self, name)
		self.parent = parent
		self.arguments = []
		self.return_values = []
		self.location = None
		self.decorator = ""

class Attribute(DocObj):
	def __init__(self, name, parent, _type = None):
		DocObj.__init__(self, name)
		self.parent = parent
		self._type = _type
		self.readonly = False

class Argument:
	def __init__(self, name, default = None):
		self.name = name
		self.description = "Undocumented"
		self._type = "Undocumented"
		self.default = default

class ReturnValue:
	def __init__(self, name):
		self.name = name
		self.description = "Undocumented"
		self._type = "Undocumented"
