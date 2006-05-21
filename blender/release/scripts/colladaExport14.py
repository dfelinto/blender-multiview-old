#!BPY

"""
Name: 'COLLADA 1.4(.dae) ...'
Blender: 241
Group: 'Export'
Tooltip: 'Export scene from Blender to COLLADA 1.4 format (.dae)'
"""

__author__ = "Illusoft - Pieter Visser"
__url__ = ("Project homepage, http://colladablender.illusoft.com")
__version__ = "0.2.16"
__email__ = "colladablender@illusoft.com"
__bpydoc__ = """\

Description: Exports a Blender scene into a COLLADA 1.4 file.

Usage: Run the script from the menu or inside Blender. 
"""

# --------------------------------------------------------------------------
# Illusoft Collada 1.4 plugin for Blender version 0.2.16
# --------------------------------------------------------------------------
# ***** BEGIN GPL LICENSE BLOCK *****
#
# Copyright (C) 2006: Illusoft - colladablender@illusoft.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ***** END GPL LICENCE BLOCK *****
# --------------------------------------------------------------------------

import sys
import os
import Blender

# the name of the dir which contains the subclasses 
subClassesDir = 'colladaImEx'
# the location of the scripts directory for Blender
scriptsDir = os.path.join(Blender.Get('scriptsdir'), subClassesDir)

# append the path for the collada utils
if scriptsDir not in sys.path:
    sys.path.append(scriptsDir)

import cstartup

reload(cstartup)

cstartup.Main(False)
