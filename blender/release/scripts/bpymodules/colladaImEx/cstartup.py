# --------------------------------------------------------------------------
# Illusoft Collada 1.4 plugin for Blender version 0.2.45
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


__version__ = '0.2.45'

# Show the wait cursor in blender
Blender.Window.WaitCursor(1)

# indicates if the user can choose a file to import
useDefaultFile = True

# the name of the dir which contains the subclasses 
##subClassesDir = 'colladaImEx'
# the location of the scripts directory for Blender
##scriptsDir = os.path.join(Blender.Get('scriptsdir'), subClassesDir)

# the location of the test file
##defaultFileUrl = '\\examples\\test.xml'
##defaultFileUrl = '\\examples\\mushroom140.dae'
##defaultFileUrl = scriptsDir + '\\examples\\msphere.dae'
##defaultFileUrl = scriptsDir + '\\examples\\test_out.xml'
##defaultFileUrl = '\\examples\\mcube.dae'
##defaultFileUrl = '\\examples\\mcube_simple.dae'
##defaultFileUrl = '\\examples\\mcube_meter.dae'
##defaultFileUrl = '\\examples\\seymour140.dae'
##defaultFileUrl = scriptsDir + '\\examples\\physics\\friction.dae'
##defaultFileUrl = scriptsDir + '\\examples\\mushroom140.dae'

# the location of the export test file
##defaultExportUrl = scriptsDir + '\\examples\\test_out.xml'
defaultFileUrl = ''
defaultExportUrl = ''


# append the path for the collada utils
##if scriptsDir not in sys.path:
###    sys.path.append(scriptsDir)
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

def Main(doImp, scriptsLoc):
    global debug, __version__, doImport, scriptsLocation, defaultFilename

    doImport = doImp
    if scriptsLoc == "":
        scriptsLocation = Blender.Get('scriptsdir')+Blender.sys.sep+'bpymodules'+Blender.sys.sep+'colladaImEx'+Blender.sys.sep
    else:
        scriptsLocation = scriptsLoc
    
    
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
        defFilename = Blender.sys.dirname(Blender.sys.progname)+"\\"
        colladaReg = Blender.Registry.GetKey('collada',True)
            
        if not (colladaReg is None) and 'path' in colladaReg and Blender.sys.exists(colladaReg['path']):
            defFilename = colladaReg['path']
        elif not (doImport):
            defFilename += 'untitled.dae'
        
        defaultFilename = defFilename
        
        Blender.Draw.Register(Gui, Event, ButtonEvent)  # registering the 3 callbacks
        ##if not doImport:
        ##    defFilename = colladaReg['path']
            ##defFilename = Blender.Get('filename').rsplit('.',1)[0]+'.dae'
        ##print Blender.Get('filename')
        ##Blender.Window.FileSelector(FileSelected,msg,defFilename)
    
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
    global doImport, fileButton
    
    if fileName != '':
        # check if file exists
##        if Blender.sys.exists(fileName) == 1 and not doImport:
##            overwrite = Blender.Draw.PupMenu( "File Already Exists, Overwrite?%t|Yes%x1|No%x0" )
##            if overwrite == 0:
##                return False
        if Blender.sys.exists(fileName) != 1 and doImport:
            cutils.Debug.Debug('File(%s) does not exist' % (fileName),'ERROR')
            return False
        
##        if doImport:
##            # Check if the file has a valid extension .DAE or .XML
##            extension = fileName.rsplit('.',1)[1].lower()
##            if extension != 'xml' and extension != 'dae':
##                cutils.Debug.Debug('File(%s) is not a .dae or .xml file' % (fileName),'ERROR')
        fileButton.val = fileName
        ##transl = translator.Translator(doImport,__version__,debug,fileName)           
    else:
        cutils.Debug.Debug('ERROR: filename is empty','ERROR')

toggle = 0
fileButton = None
toggleTriangles = None
togglePolygons = None
toggleExportSelection = None
toggleClearScene = None
toggleNewScene = None
toggleBakeMatrix = None
toggleLookAt = None


def Gui():
    global toggleLookAt, toggleBakeMatrix, toggleNewScene, toggleClearScene, toggleTriangles, togglePolygons, toggleExportSelection, scriptsLocation, doImport, defaultFilename, fileButton    
        
    Blender.BGL.glClearColor(0.898,0.910,0.808,1) # Set BG Color1
    Blender.BGL.glClear(Blender.BGL.GL_COLOR_BUFFER_BIT)
    Blender.BGL.glColor3f(0.835,0.848,0.745) # BG Color 2
    size = Blender.Window.GetAreaSize()
    Blender.BGL.glRectd(40,0,200,size[1])
    try:
        logoImage = Blender.Image.Load(scriptsLocation + 'logo.png')
        Blender.BGL.glEnable(Blender.BGL.GL_BLEND ) # Only needed for alpha blending images with background.
        Blender.BGL.glBlendFunc(Blender.BGL.GL_SRC_ALPHA, Blender.BGL.GL_ONE_MINUS_SRC_ALPHA)     
        Blender.Draw.Image(logoImage, 45, size[1]-30)     
        Blender.BGL.glDisable(Blender.BGL.GL_BLEND)        
    except IOError: # image not found
        Blender.BGL.glColor3i(0.255,0.255,0.2)
        Blender.BGL.glRasterPos2i(45, size[1]-30)
        Blender.Draw.Text("Collada 1.4.0 Plugin for Blender", "large") 
        
    Blender.BGL.glColor3f(0.255,0.255,0.2)
    Blender.BGL.glRasterPos2i(45, size[1]-40)
    Blender.Draw.Text("Version: %s"%(__version__),"small")
    
    # Write donation text
    donateText1 = "If this plugin is valuable to you or your company, please consider a donation at"
    donateText2 = "http://colladablender.illusoft.com to support this plugin. Thanks a lot!"
    Blender.BGL.glRasterPos2i(45, size[1]-60)
    Blender.Draw.Text(donateText1, "small")    
    Blender.BGL.glRasterPos2i(45, size[1]-70)
    Blender.Draw.Text(donateText2, "small") 
    
    # Write import / export text
    Blender.BGL.glColor3f(0.9,0.08,0.08)
    Blender.BGL.glRasterPos2i(45, size[1]-95)
    if doImport:
        importExportText = "Import"
    else:
        importExportText = "Export"        
    Blender.Draw.Text(importExportText, "normal")
    
    Blender.BGL.glColor3f(0.255,0.255,0.2)
    
    # Create File path input    
    yval = size[1]-130
    Blender.BGL.glRasterPos2i(45, yval)
    if fileButton is None or fileButton.val == '':
        fileName = defaultFilename
    else:
        fileName = fileButton.val
    Blender.Draw.Text('%s file:'%(importExportText),"normal")
    maxWidth = 400
    if size[0] - (105 + 35) > maxWidth:
        fileWidth = maxWidth
    else:
        fileWidth = size[0] - (105 + 35)
    fileButton = Blender.Draw.String('', 5, 105, yval-5, fileWidth, 20, fileName, 255) 
    Blender.Draw.PushButton('...', 2, 105 + fileWidth, yval-5, 30, 20, 'browse file')
    
    Blender.Draw.PushButton("Cancel", 3, 45, 10, 55, 20, "Cancel")
    Blender.Draw.PushButton(importExportText + ' and Close', 4, 45+55+35, 10, 100, 20, importExportText + ' and close this screen')
    
    # Create Export Options:    
    if not doImport:
        yval = yval - 50
        # Create Triangle / Polygons Options    
        if not (toggleTriangles is None):
            toggleTrianglesVal = toggleTriangles.val
        else:
            toggleTrianglesVal = 0
        
        if not (togglePolygons is None):
            togglePolygonsVal = togglePolygons.val
        else:
            togglePolygonsVal = 0
            
        toggleTriangles = Blender.Draw.Toggle('Triangles',6,45, yval, 60, 20, toggleTrianglesVal, 'Export all geometry as triangles')
        togglePolygons = Blender.Draw.Toggle('Polygons',7,45+60 + 30, yval, 60, 20, togglePolygonsVal, 'Export all geometry as polygons')
        
        yval = yval - 40
        # Create Export Selection Option
        if not (toggleExportSelection is None):
            toggleExportSelectionVal = toggleExportSelection.val
        else:
            toggleExportSelectionVal = 0
            
        toggleExportSelection = Blender.Draw.Toggle('Only Export Selection',8,45, yval, 150, 20, toggleExportSelectionVal, 'Only export selected objects')
        
        yval = yval - 40
        # Create Export Selection Option
        if not (toggleBakeMatrix is None):
            toggleBakeMatrixVal = toggleBakeMatrix.val
        else:
            toggleBakeMatrixVal = 0
            
        toggleBakeMatrix = Blender.Draw.Toggle('Bake Matrices',11,45, yval, 150, 20, toggleBakeMatrixVal, 'Put all transformations in a single matrix')
        
        yval = yval - 40
        # Create Export Selection Option
        if not (toggleLookAt is None):
            toggleLookAtVal = toggleLookAt.val
        else:
            toggleLookAtVal = 0
            
        ##toggleLookAt = Blender.Draw.Toggle('Camera as Lookat',12,45, yval, 150, 20, toggleLookAtVal, 'Export the transformation of camera\'s as lookat')
        
        Blender.Draw.PushButton(importExportText, 12, 45+55+35+100+35, 10, 55, 20, importExportText)
    else:
        yval = yval - 50
        # Create Import To new Scene Options
        if not (toggleNewScene is None):
            toggleNewSceneVal = toggleNewScene.val
        else:
            toggleNewSceneVal = 0
        
        if not (toggleClearScene is None):
            toggleClearSceneVal = toggleClearScene.val
        else:
            toggleClearSceneVal = 0
            
        toggleNewScene = Blender.Draw.Toggle('New Scene',9,40, yval, 75, 20, toggleNewSceneVal, 'Import file into a new Scene')
        toggleClearScene = Blender.Draw.Toggle('Clear Scene',10,40+75 + 10, yval, 75, 20, toggleClearSceneVal, 'Clear everything on the current scene')
        
        
       
def Event(evt, val):
    pass
        
def ButtonEvent(evt):
    global toggleLookAt, toggleBakeMatrix, toggleExportSelection,toggleNewScene, toggleClearScene, toggleTriangles, togglePolygons, doImport, defaultFilename, fileSelectorShown, fileButton
        
    if evt == 1:
        toggle = 1 - toggle
        Blender.Draw.Redraw(1)
    elif evt == 2: # browse file
        browseText = ''
        if doImport:
            browseText = 'Import .dae'
        else:
            browseText = 'Export .dae'
        Blender.Window.FileSelector(FileSelected,browseText,defaultFilename)
        Blender.Draw.Redraw(1)
    elif evt == 3:
        Blender.Draw.Exit()
    elif evt == 4 or evt == 12: # Ok, time to export/import
        #keep track of the time to execute this script
        startTime = Blender.sys.time()
        fileName = fileButton.val
        exists = Blender.sys.exists(fileName)
        if exists == 1 and not doImport:
            overwrite = Blender.Draw.PupMenu( "File Already Exists, Overwrite?%t|Yes%x1|No%x0" )
            if overwrite == 0:
                return False
        elif exists != 1 and doImport:
            Blender.Draw.PupMenu("File does not exist: %t|"+fileName)
            cutils.Debug.Debug('File(%s) does not exist' % (fileName),'ERROR')
            return False
        elif not Blender.sys.exists(Blender.sys.dirname(fileName)):
            Blender.Draw.PupMenu("Path is not valid: %t|"+Blender.sys.dirname(fileName))
            cutils.Debug.Debug('Path is not valid: %s' % (Blender.sys.dirname(fileName)),'ERROR')
            return False
        
        if toggleTriangles is None:
            useTriangles = False
        else:
            useTriangles = bool(toggleTriangles.val)
            
        if togglePolygons is None:
            usePolygons = False
        else:
            usePolygons = bool(togglePolygons.val)
            
        if toggleBakeMatrix is None:
            bakeMatrices = False
        else:
            bakeMatrices = bool(toggleBakeMatrix.val)
            
        if toggleExportSelection is None:
            exportSelection = False
        else:
            exportSelection = bool(toggleExportSelection.val)
            
        if toggleNewScene is None:
            newScene = False
        else:
            newScene = bool(toggleNewScene.val)
        
        if toggleClearScene is None:
            clearScene = False
        else:
            clearScene = bool(toggleClearScene.val)
            
        if toggleLookAt is None:
            lookAt = False
        else:
            lookAt = bool(toggleLookAt.val)
        d = {}
        d['path'] = fileName
        Blender.Registry.SetKey('collada',d, True)
        
        if doImport:
            importExportText = "Import"
        else:
            importExportText = "Export"        
        
        try:
            transl = translator.Translator(doImport,__version__,debug,fileName, useTriangles, usePolygons, bakeMatrices, exportSelection, newScene, clearScene, lookAt)
            # Redraw al 3D windows.
            Blender.Window.RedrawAll()    
            
            # calculate the elapsed time
            endTime = Blender.sys.time()
            elapsedTime = endTime - startTime
            Blender.Draw.PupMenu(importExportText + " Successfull %t")
        except:
            endTime = Blender.sys.time()
            elapsedTime = endTime - startTime
            Blender.Draw.PupMenu(importExportText + "ing failed%t | Check the console for more info")
            raise # throw the exception
        
        cutils.Debug.Debug('FINISHED - time elapsed: %.1f'%(elapsedTime),'FEEDBACK')
        
        # Hide the wait cursor in blender
        Blender.Window.WaitCursor(0)
        if evt == 4:
            Blender.Draw.Exit()
    elif evt == 6: # Toggle Triangles
        if toggleTriangles.val:
            togglePolygons.val = 0
        Blender.Draw.Redraw(1)
    elif evt == 7: # Toggle Polygons
        if togglePolygons.val:
            toggleTriangles.val = 0
        Blender.Draw.Redraw(1)
    elif evt == 9: # Toggle Create new Scene
        if toggleNewScene.val:
            toggleClearScene.val = 0
        Blender.Draw.Redraw(1)
    elif evt == 10: # Toggle Clear current Scene
        if toggleClearScene.val:
            toggleNewScene.val = 0
        Blender.Draw.Redraw(1)