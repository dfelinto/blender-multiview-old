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
from Blender.Mathutils import *

#---Classes---

class Debug(object):
    
    __debugLevels = ['ALL','DEBUG','FEEDBACK','WARNING','ERROR','NONE']
    
    # the current debugLevel
    debugLevel = 'ALL'
    
    # Method: Debug a message
    def Debug(message, level):
        currentLevelIndex = Debug.__debugLevels.index(Debug.debugLevel)
        if str(level).isdigit() and level >= currentLevelIndex:
            print Debug.__debugLevels[level] + ': ' + message 
        else:
            try:
                i = Debug.__debugLevels.index(level)
                if i >= currentLevelIndex :
                    print level + ': ' + str(message)
            except:
                pass
    # make Debug a static method
    Debug = staticmethod(Debug)
    
    # Method: Set the Debug Level
    def SetLevel(level):
        try:
            Debug.__debugLevels.index(level)
            Debug.debugLevel = level
            return True
        except:
            Debug('Debuglevel not available','WARNING')
            return False
    # Make SetLevel a static method
    SetLevel = staticmethod(SetLevel)
    
    
        
    
#---Functions----

angleToRadian = 3.1415926 / 180.0
radianToAngle = 180.0 / 3.1415926

# Convert a string to a float if the value exists
def ToFloat(val):
    if val is None or val == '':
        return None
    else:
        return float(val)

# Convert a string to a int if the value exists
def ToInt(val):
    if val is None or val == '':
        return None
    else:
        return int(val)
    
# Convert a string to a list of 3 floats e.g '1.0 2.0 3.0' -> [1.0, 2.0, 3.0]
def ToFloat3(stringValue):
    if stringValue is None:
        return None
    split = stringValue.split( )
    return [ float( split[ 0 ] ), float( split[ 1 ] ), float( split[ 2 ] ) ]

def ToList(var):
    result = []
    if var is None:
        return result
    
    split = var.split( )
    for i in split:
        result.append(i)
    return result
# Convert a string or list to a list of floats
def ToFloatList(var):
    result = []
    if var is None:
        return result
        
    if type(var) == list:
        for i in var:
            result.append(float(i))
    else:    
        split = var.split( )
        for i in split:
            result.append(float(i))
    return result

def ToIntList(lst):
    result = []
    if lst is None:
        return result
    if type(lst) == list:
        for i in lst:
            result.append(int(i))
    else:
        split = lst.split( )        
        for i in split:
            result.append(int(i))
    return result

def ToBoolList(lst):
    result = []
    if lst is None:
        return result
    for i in lst:
        result.append(bool(i))
    return result

# Convert a string to a list of 4 floats e.g '1.0 2.0 3.0 4.0' -> [1.0, 2.0, 3.0, 4.0]
def ToFloat4(stringValue):
    split = stringValue.split( )
    return [ float( split[ 0 ] ), float( split[ 1 ] ), float( split[ 2 ] ) , float( split[3])]

def ToFloat7(stringValue):
    data = stringValue.split( )
    return [ float(data[0]), float(data[1]), float(data[2]), float(data[3]), float(data[4]), float(data[5]), float(data[6])]

def AddVec3( vector1, vector2 ):
    vector1.x += vector2.x
    vector1.y += vector2.y
    vector1.z += vector2.z

def ToMatrix4( matrixElement ):
    data = matrixElement.split( )
    
    vec1 = [ float(data[0]), float(data[4]), float(data[8]), float(data[12]) ]
    vec2 = [ float(data[1]), float(data[5]), float(data[9]), float(data[13]) ]
    vec3 = [ float(data[2]), float(data[6]), float(data[10]), float(data[14]) ]
    vec4 = [ float(data[3]), float(data[7]), float(data[11]), float(data[15]) ]
    
    return Blender.Mathutils.Matrix( vec1, vec2, vec3, vec4 )

def ToMatrix3(matrixElement):
    data = matrixElement.split( )
    
    vec1 = [ float(data[0]), float(data[3]), float(data[6]) ]
    vec2 = [ float(data[1]), float(data[4]), float(data[7])]
    vec3 = [ float(data[2]), float(data[5]), float(data[8])]
    
    return Blender.Mathutils.Matrix( vec1, vec2, vec3)
    
def GetVector3( element ):
    value = [ float( element[ 0 ] ), float( element[ 1 ] ), float( element[ 2 ] ) ]
    return Blender.Mathutils.Vector( value )

def GetEuler( rotateElement ):
    euler = [ float( rotateElement[ 0 ] ) * float( rotateElement[ 3 ] ) * angleToRadian,
              float( rotateElement[ 1 ] ) * float( rotateElement[ 3 ] ) * angleToRadian,
              float( rotateElement[ 2 ] ) * float( rotateElement[ 3 ] ) * angleToRadian ]
    return Blender.Mathutils.Euler( euler )

def AddEuler(euler1, euler2):
    euler1.x += euler2.x
    euler1.y += euler2.y
    euler1.z += euler2.z
    
# Clear the console
def ClearConsole():
    if sys.platform == 'linux-i386' or sys.platform == 'linux2':
        sysCommand = 'clear'
    elif sys.platform == 'win32' or sys.platform == 'dos' or sys.platform[0:5] == 'ms-dos' :
        sysCommand = 'cls'
    else :
        sysCommand = 'unknown'
            
    if sysCommand != 'unknown':
        os.system(sysCommand)
        
def MatrixToString(mat):
    result = ''
    if mat is None:
        return result
    
    for vec in mat:
        result += '\n\t'
        for i in vec:
            result += str(i)+' '
        
    return result+'\n'
            
def RoundList(lst, nDigits):
    result = []
    for i in lst:
        result.append(round(i, nDigits))
    return result
        

def ListToString(lst):
    val  = ''
    if lst is None:
        return val
    else:
        for i in lst:
            if type(i) == list:
                val += ListToString(i)+'\n'
            else:
                val += str(i)+' '
        return val[:-1]
    
def GetValidFilename(filename):
    filename = Blender.sys.expandpath( filename )
    filename = filename.replace( "//", "/" )    
    filename = filename.replace( Blender.sys.sep, "/" )
    return "file://" + filename