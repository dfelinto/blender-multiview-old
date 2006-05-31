#!BPY

"""
Name: 'COLLADA 1.4(.dae) ...'
Blender: 241
Group: 'Import'
Tooltip: 'Import scene from COLLADA 1.4 format (.dae)'
"""

__author__ = "Illusoft - Pieter Visser"
__url__ = ("Project homepage, http://colladablender.illusoft.com")
__version__ = "0.2.32"
__email__ = "colladablender@illusoft.com"
__bpydoc__ = """\

Description: Imports a COLLADA 1.4 file into a Blender scene.

Usage: Run the script from the menu or inside Blender. 
"""

# --------------------------------------------------------------------------
# Illusoft Collada 1.4 plugin for Blender version 0.2.32
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

error = False

try:
    import colladaImEx.cstartup
except ImportError:
    ######################## SET PATH TO FOLDER consisting 'colladaImEx' here (if necessary)
    
    # Example:
    
    # scriptsDir = "C:/Temp/"
    
    scriptsDir = ""
    
    #############################################################################
    if scriptsDir == "":
        Blender.Draw.PupMenu("Cannot find folder %t | Please set path in file 'colladaImport14.py'")
        error = True
    else:
        if scriptsDir not in sys.path:
            sys.path.append(scriptsDir)
        try:
            import colladaImEx.cstartup
        except:
            Blender.Draw.PupMenu("Cannot find colladaImEx files %t | Please make sure the path is correct in file 'colladaImport14.py'")
            error = True
            
if not error:        
    reload(colladaImEx.cstartup)
    colladaImEx.cstartup.Main(True)
