#!BPY

"""
Name: 'COLLADA 1.4(.dae) ...'
Blender: 241
Group: 'Import'
Tooltip: 'Import scene from COLLADA 1.4 format (.dae)'
"""

__author__ = "Illusoft - Pieter Visser"
__url__ = ("Project homepage, http://colladablender.illusoft.com")
__version__ = "0.2"
__email__ = "colladablender@illusoft.com"
__bpydoc__ = """\

Description: Imports a COLLADA 1.4 file into a Blender scene.

Usage: Run the script from the menu or inside Blender. 
"""

# --------------------------------------------------------------------------
# Illusoft Collada 1.4 plugin for Blender version 0.2
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
if Blender.Get('scriptsdir'):
    scriptsDir = os.path.join(Blender.Get('scriptsdir'), subClassesDir)
else:
    ######################## SET PATH TO FOLDER 'colladaImEx' here (if necessary)
    
    # Example:
    
    # scriptsDir = "C:/Blender/.blender/scripts/colladaImEx/
    
    scriptsDir = ""
    
    #############################################################################
    
    if scriptsDir == "":
        Blender.Draw.PupMenu("Cannot find folder %t | Please set path in file 'colladaImEx\cstartup.py'")


# append the path for the collada utils
if scriptsDir not in sys.path:
    sys.path.append(scriptsDir)

import cstartup

reload(cstartup)

cstartup.Main(True)
