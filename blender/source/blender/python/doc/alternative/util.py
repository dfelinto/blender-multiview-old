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

def get_version_string(context):
	from bpy import app
	version = app.version_string.split("(")[0]
	if app.build_revision != "Unknown":
		version += " r" + app.build_revision
	return version

def indent(context, str, level, first=True):
	result = str.replace('\n', '\n' + ('   ' * level))
	if first:
		result = '   ' * level + result
	return result

def get_signature(context, function):
	args = []
	for argument in function.arguments:
		if argument.default:
			args.append('{0}={1}'.format(argument.name, argument.default))
		else:
			args.append(argument.name)
	return '{0}({1})'.format(function.name, ', '.join(args))

def sorted_dict_values(context, dictionary):
	return [dictionary[key] for key in sorted(dictionary.keys())]

def get_all_attributes(context, _class):
	attributes = []
	for attr in _class.attributes.values():
		attributes.append((attr, None))
	
	_class = _class.base
	while _class:
		for attr in _class.attributes.values():
			attributes.append((attr, _class.name))
		_class = _class.base
	
	attrs = []
	for a in sorted([attr[0].name for attr in attributes]):
		for attr in attributes:
			if attr[0].name == a:
				attrs.append(attr)
				break
	
	return attrs

def get_all_methods(context, _class):
	methods = []
	for meth in _class.methods.values():
		methods.append((meth, None))
	
	_class = _class.base
	while _class:
		for meth in _class.methods.values():
			methods.append((meth, _class.name))
		_class = _class.base
	
	meths = []
	for m in sorted([meth[0].name for meth in methods]):
		for meth in methods:
			if meth[0].name == m:
				meths.append(meth)
				break
	
	return meths

def get_class_tree(context, _class):
	bases = []
	while _class:
		bases.append(":class:`{0}`".format(_class.name))
		_class = _class.base
	bases.reverse()
	return bases

def get_subclasses(context, _class):
	subclasses = []
	for c in _class.parent.classes.values():
		if c.base == _class:
			subclasses.append(c.name)
	subclasses.sort()
	return subclasses
