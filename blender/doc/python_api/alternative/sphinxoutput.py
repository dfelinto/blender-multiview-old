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

import os
import filecmp
import shutil

from mako.template import Template
from mako.lookup import TemplateLookup
from mako.exceptions import TopLevelLookupException

class SphinxOutput:
	def __init__(self):
		self.lookup = TemplateLookup(directories = ['templates'])
	
	def find_template(self, end, name=''):
		tpl = None
		while name:
			try:
				tpl = self.lookup.get_template('{0}.{1}.rst'.format(name, end))
				break
			except TopLevelLookupException:
				name = name.rpartition('.')[0]
		if not tpl:
			tpl = self.lookup.get_template(end+'.rst')
		return tpl
	
	def write(self, modules):
		outdir = 'sphinx-in'
		tmpdir = 'sphinx-tmp'
		
		# create directories if necessary
		if outdir not in os.listdir('.'):
			os.mkdir(outdir)
		if tmpdir in os.listdir('.'):
			shutil.rmtree(tmpdir)
		os.mkdir(tmpdir)
		
		# write config and contents
		contents_tpl = self.find_template('contents')
		open(os.path.join(tmpdir, 'contents.rst'), 'w').write(contents_tpl.render(modules=modules))
		conf_tpl = self.lookup.get_template('conf.py')
		open(os.path.join(tmpdir, 'conf.py'), 'w').write(conf_tpl.render())
		
		# write modules
		for (name, module) in modules.items():
			module_tpl = self.find_template('Module', name)
			open(os.path.join(tmpdir, name + '.rst'), 'w').write(module_tpl.render(module=module, modules=modules))
			
			# write classes
			for (cname, _class) in module.classes.items():
				cname = name + '.' + cname
				class_tpl = self.find_template('Class', cname)
				open(os.path.join(tmpdir, cname + '.rst'), 'w').write(class_tpl.render(module=module, _class=_class))
		
		# update changed files in outdir
		outdirlist = os.listdir(outdir)
		tmpdirlist = os.listdir(tmpdir)
		
		# delete old files
		for f in outdirlist:
			if f not in tmpdirlist:
				os.remove(os.path.join(outdir, f))
		
		# copy new and changed ones
		for f in tmpdirlist:
			source = os.path.join(tmpdir, f)
			target = os.path.join(outdir, f)
			if not os.path.exists(target) or not filecmp.cmp(source, target):
				shutil.copy(source, target)
		
		# call sphinx
		os.system("sphinx-build sphinx-in sphinx-out")
