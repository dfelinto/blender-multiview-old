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

import sys
import os

sys.path.append('.')

import aud, mathutils, blf, bpy#, bgl, geometry
import pyparser
import rnaparser
import epyparser
import sphinxparser
import sphinxoutput

modules = {}
modules['aud'] = pyparser.PyParser.parse(aud, "aud", "Audio")
modules['mathutils'] = pyparser.PyParser.parse(mathutils, "mathutils", "Math Types & Utilities")
modules['blf'] = pyparser.PyParser.parse(blf, "blf", "Font Drawing")
modules['bpy.utils'] = pyparser.PyParser.parse(bpy.utils, "bpy.utils", "Utilities")
modules['bpy.app'] = pyparser.PyParser.parse(bpy.app, "bpy.app", "Application Data")
modules['bpy.props'] = pyparser.PyParser.parse(bpy.props, "bpy.props", "Property Definitions")
modules['bpy.path'] = pyparser.PyParser.parse(bpy.path, "bpy.path", "Path Utilities")
modules['bpy.ops'], modules['bpy.types'], modules['bpy.data'] = rnaparser.RNAParser().parse()
modules['bgl'] = epyparser.EPyParser.parse(__import__('epy.bgl').bgl, "bgl", "Blender OpenGL Wrapper")
modules['geometry'] = epyparser.EPyParser.parse(__import__('epy.geometry').geometry, "geometry", "Geometry module")
modules['idprop'] = epyparser.EPyParser.parse(__import__('epy.idprop').idprop, "idprop", "ID Prop Types")
modules['VideoTexture'] = epyparser.EPyParser.parse(__import__('epy.VideoTexture').VideoTexture, "VideoTexture", "Game Engine VideoTexture Module")
modules['PhysicsConstraints'] = epyparser.EPyParser.parse(__import__('epy.PhysicsConstraints').PhysicsConstraints, "PhysicsConstraints", "Game Engine PhysicsConstraints Module")
modules['bge.events'] = sphinxparser.SphinxParser.parse(os.path.join('rst', 'bge.events.rst'), "bge.events", "Game Engine bge.events module")
modules['bge.logic'] = sphinxparser.SphinxParser.parse(os.path.join('rst', 'bge.logic.rst'), "bge.logic", "Game Engine bge.logic module")
modules['bge.render'] = sphinxparser.SphinxParser.parse(os.path.join('rst', 'bge.render.rst'), "bge.render", "Game Engine bge.render module")
modules['bge.types'] = sphinxparser.SphinxParser.parse(os.path.join('rst', 'bge.types.rst'), "bge.types", "Game Engine bge.types module")

out = sphinxoutput.SphinxOutput()
out.write(modules)
