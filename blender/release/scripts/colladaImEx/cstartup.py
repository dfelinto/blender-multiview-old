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

debug = False
_ERROR = False

import sys
import os
try:
    import Blender
except NameError:
    print "Error! Could not find Blender modules!"
    _ERROR = True


__version__ = '0.2'

# Show the wait cursor in blender
Blender.Window.WaitCursor(1)

# indicates if the user can choose a file to import
useDefaultFile = True

# the name of the dir which contains the subclasses 
subClassesDir = 'colladaImEx'
# the location of the scripts directory for Blender
scriptsDir = os.path.join(Blender.Get('scriptsdir'), subClassesDir)

# the location of the test file
##defaultFileUrl = '\\examples\\test.xml'
##defaultFileUrl = '\\examples\\mushroom140.dae'
defaultFileUrl = scriptsDir + '\\examples\\msphere.dae'
defaultFileUrl = scriptsDir + '\\examples\\test_out.xml'
##defaultFileUrl = '\\examples\\mcube.dae'
##defaultFileUrl = '\\examples\\mcube_simple.dae'
##defaultFileUrl = '\\examples\\mcube_meter.dae'
##defaultFileUrl = '\\examples\\seymour140.dae'
defaultFileUrl = scriptsDir + '\\examples\\physics\\friction.dae'
defaultFileUrl = scriptsDir + '\\examples\\mushroom140.dae'

# the location of the export test file
defaultExportUrl = scriptsDir + '\\examples\\test_out.xml'


# append the path for the collada utils
if scriptsDir not in sys.path:
    sys.path.append(scriptsDir)
try:
    import cutils
except NameError:
    print "\nError! Could not find Collada Utils (cutils) module!"
    _ERROR = True

try:
    import xmlUtils
except NameError:
    print "\nError! Could not find XML module!"
    _ERROR = True
    
try:
    import math
except NameError:
    print "Error! Could not find math module"
    _ERROR = True
    
if _ERROR:
    from sys import version_info
    version = '%s.%s' % version_info[0:2]
    print """
This script requires the xml module that is part of a
default standalone Python install.

To run the collada importer and exporter you need to have
Python version %s installed in your system. It can be downloaded from:

http://www.python.org

Notes:
- The minor (third) version number doesn't matter, you can have either
Python %s.1 or %s.2 or higher.
- If you do have Python %s installed and still can't run the scripts, then
make sure Blender's Python interpreter is finding the standalone modules
(run 'System Information' from Blender's Help -> System menu).
""" % (version, version, version, version)
    Blender.Draw.PupMenu("Error: missing module(s), please check console")

try:
    import collada
except NameError:
    print "Error! Could not find Collada(collada) module"
    _ERROR = True
    
try:
    import translator
except NameError:
    print "Error! Could not find Collada Translator (translator) module"
    _ERROR = True

# A List with al the modules (in the scriptsdir) to be reloaded
modules = [cutils, xmlUtils, collada, translator]

def Main(doImp):
    global debug, __version__, doImport

    doImport = doImp
    
    if not ReloadModules():
        print 'cannot reload all modules'
        return False
    # Clear the console
    cutils.ClearConsole()
    
    # set the debuglevel
    if debug:
        cutils.Debug.SetLevel('DEBUG')
    else:
        cutils.Debug.SetLevel('FEEDBACK')
    
    cutils.Debug.Debug('Illusoft Collada 1.4 Plugin v%s started'%(__version__),'FEEDBACK')
    # Create a Collada <-> Blender Translator
    if debug:
        #keep track of the time to execute this script
        startTime = Blender.sys.time()
        
        ##fileurl = scriptsDir
        fileurl = ''
        if doImport:
            fileurl+= defaultFileUrl
        else :
            fileurl += defaultExportUrl
        transl = translator.Translator(doImport,__version__,debug,fileurl)        
        
        ##translator = Translator(False,__version__,debug,scriptsDir+defaultExportUrl)        
        ##translator = Translator(True,__version__,debug,scriptsDir+defaultExportUrl)
        # Redraw al 3D windows.
        Blender.Window.RedrawAll()    
        
        # calculate the elapsed time
        endTime = Blender.sys.time()
        elapsedTime = endTime - startTime
        cutils.Debug.Debug('FINISHED - time elapsed: %.1f'%(elapsedTime),'FEEDBACK')
        
        # Hide the wait cursor in blender
        Blender.Window.WaitCursor(0)
    else:
        msg = ''
        if doImport:
            msg = 'Import .dae'
        else:
            msg = 'Export .dae'
        defFilename = Blender.sys.dirname(Blender.sys.progname)+"\\"
        colladaReg = Blender.Registry.GetKey('collada',True)
            
        if not (colladaReg is None) and 'path' in colladaReg and Blender.sys.exists(colladaReg['path']):
            defFilename = colladaReg['path']
        if not doImport:
            defFilename = colladaReg['path']
            ##defFilename = Blender.Get('filename').rsplit('.',1)[0]+'.dae'
        ##print Blender.Get('filename')
        Blender.Window.FileSelector(FileSelected,msg,defFilename)
    
def ReloadModules():    
    # Loop through all the modules and try to reload them
    for module in modules:        
        try:
            reload(module)
        except NameError:
            cutils.Debug.Debug('cannot reload module %s' %(module),'ERROR')
            return False
    return True

def FileSelected(fileName):
    global doImport
    #keep track of the time to execute this script
    startTime = Blender.sys.time()
    if fileName != '':
        # check if file exists
        if Blender.sys.exists(fileName) == 1 and not doImport:
            overwrite = Blender.Draw.PupMenu( "File Already Exists, Overwrite?%t|Yes%x1|No%x0" )
            if overwrite == 0:
                return False
        elif Blender.sys.exists(fileName) != 1 and doImport:
            cutils.Debug.Debug('File(%s) does not exist' % (fileName),'ERROR')
            return False
        
        if doImport:
            # Check if the file has a valid extension .DAE or .XML
            extension = fileName.rsplit('.',1)[1].lower()
            if extension != 'xml' and extension != 'dae':
                cutils.Debug.Debug('File(%s) is not a .dae or .xml file' % (fileName),'ERROR')
        d = {}
        d['path'] = fileName
        Blender.Registry.SetKey('collada',d, True)
        transl = translator.Translator(doImport,__version__,debug,fileName)           
    else:
        cutils.Debug.Debug('ERROR: filename is empty','ERROR')
    
    # Redraw al 3D windows.
    Blender.Window.RedrawAll()    
    
    # calculate the elapsed time
    endTime = Blender.sys.time()
    elapsedTime = endTime - startTime
    cutils.Debug.Debug('FINISHED - time elapsed: %.1f'%(elapsedTime),'FEEDBACK')
    
    # Hide the wait cursor in blender
    Blender.Window.WaitCursor(0)