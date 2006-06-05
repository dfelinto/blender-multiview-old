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

from cutils import *
import collada
import sys
import Blender
from Blender.Mathutils import *
import math
import datetime

class Translator(object):
    isImporter = False
    
    def __init__(self, isImporter, version, debugM, fileName,_useTriangles, _usePolygons, _bakeMatrices, _exportSelection, _createNewScene, _clearScene, _lookAt):
        global __version__, debugMode, usePhysics, useTriangles, usePolygons, bakeMatrices, exportSelection, createNewScene, clearScene, lookAt
        __version__ = version
        debugMode = debugM
        usePhysics = None
        useTriangles = _useTriangles
        usePolygons = _usePolygons
        bakeMatrices = _bakeMatrices
        exportSelection = _exportSelection
        createNewScene = _createNewScene
        clearScene = _clearScene
        lookAt = _lookAt
        
        self.isImporter = isImporter
        self.fileName = ''
        if self.isImporter:
            self.__Import(fileName)
        else:
            self.__Export(fileName)
    
    def __Import(self,fileName=''):
        documentTranslator = DocumentTranslator(fileName)            
        documentTranslator.Import(fileName)
        
    def __Export(self,filename=''):
        documentTranslator = DocumentTranslator(filename)
        documentTranslator.Export(filename)  
                
        
class DocumentTranslator(object):
    isImport = None
    ids = []
    sceneGraph = None
    
    cameraLibrary = None
##    geometryLibrary = None
##    controllerLibrary = None
##    animationLibrary = None
##    materialLibrary = None
    texturesLibrary = None
    lampsLibrary = None
    colladaDocument = None
    scenesLibrary = None
    
    def CreateID(self, name, typeName=None):
        if not (name in self.ids):
            self.ids.append(name)
            return name
        else:
            tempName = name;
            if not(typeName is None) and name.rfind(typeName) >= 0:
                # Check for existing number at the end?
                return self.IncrementString(tempName)
            else:
                return self.CreateID(tempName+typeName)
    
    def IncrementString(self, name):
        tempName = name;
        if name.rfind('.') >= 0:
            while tempName[-1:].isdigit():
                tempName =  tempName[:-1]
            digitStr = name[-(len(name)-len(tempName)):]
            digit = 1
            if len(digitStr) > 0 and len(digitStr) != len(name):
                digit = int(digitStr)+1
            return tempName+str(digit).zfill(3)
        else:
            return name+'.001'
                    
    def __init__(self, fileName):
        self.isImport = False 
        self.scenesLibrary = ScenesLibrary(self)
        self.sceneGraph = SceneGraph(self)       
        self.lampsLibrary = LampsLibrary(self)
        self.colladaDocument = None
        self.texturesLibrary = TexturesLibrary(self)
        self.camerasLibrary = CamerasLibrary(self)
        self.materialsLibrary = MaterialsLibrary(self)
        self.meshLibrary = MeshLibrary(self)
        
        self.filename = None
        self.filePath = ''
        
        
        
        
        self.currentBScene = Blender.Scene.getCurrent()
                
        self.progressCount = 0.4
        self.progressField = (1.0 - self.progressCount)
        self.progressStep = 0.0
        self.progressPartCount = 0.0
        
        self.tMat = Matrix()
    
    def CreateNameForObject(self, name, replace, myType):
        if not replace:
            return name
        
        if myType == 'object':
            try:
                ob = Blender.Object.Get(name)
                ob.name = self.CreateNameForObject(self.IncrementString(ob.name), True, 'object')
            except ValueError:
                pass
        elif myType == 'mesh':                   
            try:
                mesh = Blender.Mesh.Get(name)
                if not mesh is None:
                    mesh.name = self.CreateNameForObject(self.IncrementString(mesh.name), True, 'mesh')
            except ValueError:
                pass
            
        return name
        
    def Import(self, fileName):
        global debugMode, createNewScene
        
        self.filename = fileName
        self.filePath = Blender.sys.dirname(self.filename) + Blender.sys.sep
        self.isImport = True
        Blender.Window.EditMode(0)
        Blender.Window.DrawProgressBar(0.0, 'Starting Import')
        
        if createNewScene:
            self.currentBScene = Blender.Scene.New('Scene')
            self.currentBScene.makeCurrent()
        else:
            self.currentBScene = Blender.Scene.getCurrent()
        
        # Create a new Collada document
        Blender.Window.DrawProgressBar(0.1, 'Get Collada Document')
        self.colladaDocument = collada.DaeDocument(debugMode)
        
        # Setup the libraries
        self.camerasLibrary.SetDaeLibrary(self.colladaDocument.camerasLibrary)
        self.lampsLibrary.SetDaeLibrary(self.colladaDocument.lightsLibrary)
        self.texturesLibrary.SetDaeLibrary(self.colladaDocument.imagesLibrary)
        self.materialsLibrary.SetDaeLibrary(self.colladaDocument.materialsLibrary)
        self.meshLibrary.SetDaeLibrary(self.colladaDocument.geometriesLibrary)
        
        # Parse the COLLADA file
        self.colladaDocument.LoadDocumentFromFile(fileName)
        
        if self.colladaDocument.asset.upAxis == collada.DaeSyntax.Y_UP:
            self.tMat[0][0] = 0
            self.tMat[1][1] = 0
            self.tMat[2][2] = 0
            self.tMat[0][1] = 1
            self.tMat[1][2] = 1
            self.tMat[2][0] = 1
            
        self.progressStep = self.progressField/(self.colladaDocument.GetItemCount()+1)
        
        # Read the COLLADA stucture and build the scene in Blender.
        Blender.Window.DrawProgressBar(0.4, 'Translate Collada 2 Blender')
        self.sceneGraph.LoadFromCollada(self.colladaDocument.scene)
        
        self.Progress()
    
    def Export(self, fileName):
        global __version__
        self.ids = []
        self.isImport = False
        Blender.Window.EditMode(0)
        
        # Create a new Collada document
        self.colladaDocument = collada.DaeDocument(debugMode)
        daeAsset = collada.DaeAsset()
        daeAsset.upAxis = 'Z_UP'
        daeAsset.created = datetime.datetime.today()
        daeAsset.modified = datetime.datetime.today()
        daeAsset.unit = collada.DaeUnit()
        daeAsset.unit.name = 'centimeter'
        daeAsset.unit.meter = '0.01'
        daeContributor = collada.DaeContributor()
        daeContributor.author = 'Illusoft Collada 1.4.0 plugin for Blender - http://colladablender.illusoft.com'
        daeContributor.authoringTool = 'Blender v:%s - Illusoft Collada Exporter v:%s' % (Blender.Get('version'), __version__)        
        daeContributor.sourceData = GetValidFilename(Blender.Get('filename'))
        daeAsset.contributors.append(daeContributor)
        
        self.colladaDocument.asset = daeAsset
        
        
        daeScene = collada.DaeScene()
        
        # Loop throug all scenes
        for bScene in Blender.Scene.Get():
            daeInstanceVisualScene = collada.DaeVisualSceneInstance()
            daeInstancePhysicsScene = collada.DaePhysicsSceneInstance()
            daeVisualScene = self.colladaDocument.visualScenesLibrary.FindObject(bScene.name)
            if daeVisualScene is None:
                sceneGraph = SceneGraph(self)
                scenesList = sceneGraph.SaveToDae(bScene)
                daeVisualScene = scenesList[0]
                daePhysicsScene = scenesList[1]
               
            daeInstanceVisualScene.object = daeVisualScene
            daeInstancePhysicsScene.object = daePhysicsScene
            #self.colladaDocument.visualScenesLibrary.AddItem(daeIntanceVisualScene)
            daeScene.iVisualScenes.append(daeInstanceVisualScene)
            daeScene.iPhysicsScenes.append(daeInstancePhysicsScene)
            
            #self.colladaDocument.visualScenesLibrary.AddItem(sceneGraph.ObjectToDae(bScene))
            #daeScene = collada.DaeScene()
            #daeScene.AddInstance()
        
        
        self.colladaDocument.scene = daeScene
        
        self.colladaDocument.SaveDocumentToFile(fileName)
        
    def Progress(self):
        self.progressPartCount = 0.0
        self.progressCount += self.progressStep
        Blender.Window.DrawProgressBar(self.progressCount, 'Creating Blender Nodes...')
        
    def ProgressPart(self, val, text):
        self.progressPartCount += val
        Blender.Window.DrawProgressBar(self.progressCount+self.progressPartCount*self.progressStep,text)
            
class SceneGraph(object):
    
    def __init__(self, document):
        self.document = document
        self.name = ''        
        
        self.childNodes = dict()
        self.rootNodes = []
        
        self.objectNames = dict()
        # Get the current blender scene
        #self.currentBScene = Blender.Scene.getCurrent()
    
    def LoadFromCollada(self, colladaScene):
        global debugMode, newScene, clearScene
        
        # When in debugMode, delete everything from the scene before proceding
        if debugMode or clearScene:
            print "Delete everything from the scene.."
            bNodes = Blender.Object.Get()
            
            newScene = Blender.Scene.New("new Scene")
            newScene.makeCurrent()
            Blender.Scene.Unlink(self.document.currentBScene)
            self.document.currentBScene = newScene
            
        if colladaScene == None:
            return False
        
        visualScenes = colladaScene.GetVisualScenes()
        if visualScenes is None:
            return        
        # For now, only pick the first available Visual Scene
        visualScene = visualScenes[0]
        if visualScene is None:
            return        
        
        Blender.Window.DrawProgressBar(0.5,'Build the items on the scene')
        # loop trough all nodes        
        for daeNode in visualScene.nodes:
            sceneNode = SceneNode(self.document, self)
            ob = sceneNode.ObjectFromDae(daeNode)
            self.objectNames[daeNode.id] = ob.name
            ##sceneNode.ObjectFromDae(daeNode)
        
        # Now get the physics Scene
        physicsScenes = colladaScene.GetPhysicsScenes()
        if not (physicsScenes is None) and len(physicsScenes) > 0:            
            # For now, only pick the fist available physics Scene
            physicsScene = physicsScenes[0]
            for iPhysicsModel in physicsScene.iPhysicsModels:
                physicsNode = PhysicsNode(self.document)
                physicsNode.LoadFromDae(iPhysicsModel, self.objectNames)
            
            
        # Update the current Scene.
        self.document.currentBScene.update(1)
    
    def SaveToDae(self, bScene):
        global exportSelection
        daeVisualScene = collada.DaeVisualScene()
        daeVisualScene.id = daeVisualScene.name = self.document.CreateID(bScene.name,'-Scene')
        
        daePhysicsScene = collada.DaePhysicsScene()
        daePhysicsScene.id = daePhysicsScene.name = self.document.CreateID(bScene.name+'-Physics', '-Scene')
        
        if exportSelection:
            self.rootNodes = Blender.Object.GetSelected()
            if len(self.rootNodes) == 0:
                print self.document.currentBScene.getActiveObject()
            self.childNodes = []
        else: # now loop trough all nodes in this scene and create a list with root nodes and children
            for node in bScene.getChildren():
                pNode = node.parent
                if pNode is None:
                    self.rootNodes.append(node)
                else:
                    try:
                        self.childNodes[pNode.name].append(node)
                    except:
                        self.childNodes[pNode.name] = [node]
                    
        # Begin with the rootnodes
        for rootNode in self.rootNodes:
            sceneNode = SceneNode(self.document,self)
            nodeResult = sceneNode.SaveToDae(rootNode,self.childNodes)
            daeNode = nodeResult[0]
            daeInstancePhysicsModel = nodeResult[1]
            daeVisualScene.nodes.append(daeNode)
            daePhysicsScene.iPhysicsModels.append(daeInstancePhysicsModel)
            
        self.document.colladaDocument.visualScenesLibrary.AddItem(daeVisualScene)
        self.document.colladaDocument.physicsScenesLibrary.AddItem(daePhysicsScene)
        return (daeVisualScene, daePhysicsScene)
    
class PhysicsNode(object):
    dynamic = 0
    child = 1
    actor = 2
    inertiaLockX = 3
    inertiaLockY = 4
    inertiaLockZ = 5
    doFH = 6
    rotFH = 7
    anisotropicFriction = 8
    ghost = 9
    rigidBody = 10
    bounds = 11
    collisionResponse = 12
    
    def __init__(self, document):
        self.document = document
        
    def LoadFromDae(self, daeInstancePhysicsModel, objectNames):
        global usePhysics
        if not(usePhysics is None) and not usePhysics:
            return
        
        for iRigidBody in daeInstancePhysicsModel.iRigidBodies:
            # Get the real blender name instead of the collada name.
            realObjectName = objectNames[iRigidBody.targetString]
            # Get the Blender object with the specified name.
            bObject = Blender.Object.Get(realObjectName)
            # Check if physics is supported.
            if usePhysics is None:
                usePhysics = hasattr(bObject, "rbShapeBoundType")
                if not usePhysics:
                    return

            # Get the rigid body.
            rigidBody = iRigidBody.body
            # Get the common technique of the rigid body.
            rigidBodyT = rigidBody.techniqueCommon
            # Get the physics material.
            physicsMaterial = rigidBodyT.GetPhysicsMaterial()
            # Get the shapes of the bounding volumes in this rigid body.
            shapes = rigidBodyT.shapes
            
            # The Rigid Body Flags
            rbFlags = 0 + rigidBodyT.dynamic + (1 << self.actor) + (1 << self.rigidBody) + (1 << self.bounds)
            rbShapeBoundType = 0
            # For now only get the first shape
            shape = shapes[0]
            # Check the type of the shape
            if isinstance(shape, collada.DaeGeometryShape):
                ##print shape, shape.iGeometry.object.data
                if isinstance(shape.iGeometry.object.data, collada.DaeMesh):
                    rbShapeBoundType = 4
                else:
                    rbShapeBoundType = 5
            elif isinstance(shape, collada.DaeBoxShape):
                rbShapeBoundType = 0
            elif isinstance(shape, collada.DaeSphereShape):
                rbShapeBoundType = 1
            elif isinstance(shape, collada.DaeCylinderShape):
                rbShapeBoundType = 2
            elif isinstance(shape, collada.DaeTaperedCylinderShape):
                rbShapeBoundType = 3           
                
            bObject.rbFlags = rbFlags
            if not (rigidBodyT.mass is None):
                bObject.rbMass = rigidBodyT.mass
            
            bObject.rbShapeBoundType = rbShapeBoundType
            

class SceneNode(object):
    
    def __init__(self, document, sceneNode):
        self.id = ''
        self.type = ''
        self.document = document
        
    def ObjectFromDae(self,daeNode):
        self.document.Progress()
        self.id = daeNode.id
        self.type = daeNode.type
        
        if daeNode.IsJoint():
            # it's a Joint, not implemented yet
            return
            pass
        else : #its a Node            
            newObject = None
            dataObject = None
            daeInstances = daeNode.GetInstances()
            
            if len(daeInstances) == 0:
                newObject = Blender.Object.New('Empty',self.id) 
            else:
                # if only one instance is in this node, add it.
                if len (daeInstances)== 1:
                    # Get the instance
                    daeInstance = daeInstances[0]
                    # Check which type the instance is
                    if isinstance(daeInstance,collada.DaeAnimationInstance):
                        newObject = Blender.Object.New('Empty',self.id)
                    elif isinstance(daeInstance,collada.DaeCameraInstance):
                        newObject = Blender.Object.New('Camera',self.id)
                        dataObject = self.document.camerasLibrary.FindObject(daeInstance,True)
                    elif isinstance(daeInstance,collada.DaeControllerInstance):
                        newObject = Blender.Object.New('Empty',self.id)
                    elif isinstance(daeInstance,collada.DaeGeometryInstance):
                        newObject = Blender.Object.New('Mesh',self.document.CreateNameForObject(self.id,True, 'object'))
                        dataObject = self.document.meshLibrary.FindObject(daeInstance,True)
                    elif isinstance(daeInstance,collada.DaeLightInstance):
                        newObject = Blender.Object.New('Lamp',self.id)
                        dataObject = self.document.lampsLibrary.FindObject(daeInstance,True)
                    elif isinstance(daeInstance,collada.DaeNodeInstance):
                        newObject = Blender.Object.New('Empty',self.id)
                    elif isinstance(daeInstance,collada.DaeVisualSceneInstance):
                        newObject = Blender.Object.New('Empty',self.id)
                    else:
                        print daeInstance
                        return
                    # Link the new Object to the dataObject
                    if dataObject != None:
                        newObject.link(dataObject)
                else:  # otherwise, create an empty node and attach the different instances to it
                    newObject = Blender.Object.New('Empty',self.id)
                    # finish this
                    
            self.document.currentBScene.link(newObject)
            
            # TODO: MAYBE CHANGE THIS LATER update the mesh..
            if newObject.getType() == 'Mesh':
                newObject.getData().update(1,1,1)
                # Set the vertex colors.
                for f in newObject.getData(mesh=1).faces:
                    for c in f.col:
                        c.r = 255
                        c.g = 255
                        c.b = 255
                        c.a = 255                    
            
            # Do the transformation
            mat = Matrix().resize4x4()
            for i in range(len(daeNode.transforms)):
                transform = daeNode.transforms[len(daeNode.transforms)-(i+1)]
                type = transform[0]
                data = transform[1]
                if type == collada.DaeSyntax.TRANSLATE:
                    mat = mat*TranslationMatrix(Vector(data)* self.document.tMat)
                elif type == collada.DaeSyntax.ROTATE:
                    mat = mat*RotationMatrix(data[3],4,'r',Vector([data[0],data[1],data[2]])* self.document.tMat)
                elif type == collada.DaeSyntax.SCALE:
                    skewVec = Vector(data[0],data[1], data[2])*self.document.tMat
                    mat = mat * Matrix([skewVec.x,0,0,0],[0,skewVec.y,0,0],[0,0,skewVec.z,0],[0,0,0,1])
                elif type == collada.DaeSyntax.SKEW:
                    s = math.tan(data[0]*angleToRadian)
                    rotVec = Vector(data[1],data[2],data[3])*self.document.tMat
                    transVec = Vector(data[4],data[5],data[6])*self.document.tMat
                    fac1 = s*transVec.x
                    fac2 = s*transVec.y
                    fac3 = s*transVec.z                    
                    
                    mat = mat * Matrix([1+fac1*rotVec.x,fac1*rotVec.y,fac1*rotVec.z,0],[fac2*rotVec.x,1+fac2*rotVec.y,fac2*rotVec.z,0],[fac3*rotVec.x,fac3*rotVec.y,1+fac3*rotVec.z,0],[0,0,0,1])
                elif type == collada.DaeSyntax.LOOKAT:
                    # TODO: use the correct up-axis
                    position = Vector([data[0],data[1], data[2]])
                    target = Vector([data[3],data[4], data[5]])
                    up = Vector([data[6],data[7], data[8]]).normalize()
                    front = (position-target).normalize()
                    side = -1*CrossVecs(front, up).normalize()
                    m = Matrix().resize4x4()

                    m[0][0] = side.x
                    m[0][1] = side.y
                    m[0][2] = side.z
                    
                    m[1][0] = up.x
                    m[1][1] = up.y
                    m[1][2] = up.z
                    
                    m[2][0] = front.x
                    m[2][1] = front.y
                    m[2][2] = front.z
                    
                    m[3][0] = position.x
                    m[3][1] = position.y
                    m[3][2] = position.z
                    
                    mat = mat*m
                    
                elif type == collada.DaeSyntax.MATRIX:
                    mat = mat * data
                    
            newObject.setMatrix(mat)
            childlist = []
            for daeChild in daeNode.nodes:
                childSceneNode = SceneNode(self.document,self)
                object = childSceneNode.ObjectFromDae(daeChild)
                if not(object is None):
                    childlist.append(object)
            newObject.makeParent(childlist,0,1)
            
            return newObject
        
    def SaveToDae(self,bNode,childNodes):
        global bakeMatrices, exportSelection
        daeNode = collada.DaeNode()
        daeNode.id = daeNode.name = self.document.CreateID(bNode.name,'-Node')# +'-node'
        
        # Get the transformations
        if bakeMatrices :
            mat = Blender.Mathutils.CopyMat(bNode.matrix).transpose()
            daeNode.transforms.append([collada.DaeSyntax.MATRIX, mat])
        else:
            daeNode.transforms.append([collada.DaeSyntax.TRANSLATE, bNode.getLocation()])
            
            euler = bNode.getEuler()
            rotxVec = [1,0,0,euler.x*radianToAngle]
            rotyVec = [0,1,0,euler.y*radianToAngle]
            rotzVec = [0,0,1,euler.z*radianToAngle]
            
            ##rotVec = [round(euler.x*radianToAngle,3), round(euler.y*radianToAngle,3), round(euler.z*radianToAngle,3)]
            if euler.z != 0: daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotzVec])
            if euler.y != 0: daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotyVec])
            if euler.x != 0: daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotxVec])
            
            daeNode.transforms.append([collada.DaeSyntax.SCALE, bNode.getSize()])
            
        # Get the instance
        type = bNode.getType()
        instance  = None
        meshID = None
        if type == 'Mesh':
            instance = collada.DaeGeometryInstance()
            daeGeometry = self.document.colladaDocument.geometriesLibrary.FindObject(bNode.getData(True))
            meshNode = MeshNode(self.document)
            if daeGeometry is None:                
                daeGeometry = meshNode.SaveToDae(bNode.getData())
            meshID = daeGeometry.id
            bindMaterials = meshNode.GetBindMaterials(bNode.getData())
            instance.object = daeGeometry
            instance.bindMaterials = bindMaterials
            daeNode.iGeometries.append(instance)
        elif type == 'Camera':
            instance = collada.DaeCameraInstance()
            daeCamera = self.document.colladaDocument.camerasLibrary.FindObject(bNode.getData(True))
            if daeCamera is None:
                cameraNode = CameraNode(self.document)
                daeCamera = cameraNode.SaveToDae(bNode.getData())
            instance.object = daeCamera
            daeNode.iCameras.append(instance)
        elif type == 'Lamp':
            instance = collada.DaeLightInstance()
            daeLight = self.document.colladaDocument.lightsLibrary.FindObject(bNode.getData(True))
            if daeLight is None:
                lampNode = LampNode(self.document)
                daeLight = lampNode.SaveToDae(bNode.getData())
            instance.object = daeLight
            daeNode.iLights.append(instance)
        
        if not exportSelection:    
            myChildNodes = []
            if childNodes.has_key(bNode.name):
                myChildNodes = childNodes[bNode.name]        
        
            for bNode in myChildNodes:
                sceneNode = SceneNode(self.document, self)
                daeNode.nodes.append(sceneNode.SaveToDae(bNode,childNodes)[0])
        
        daePhysicsInstance = self.SavePhysicsToDae(bNode, meshID, daeNode)
            
        return (daeNode, daePhysicsInstance)
    
    def SavePhysicsToDae(self, bNode, meshID, daeNode):
        global usePhysics
        if meshID is None or (not(usePhysics is None) and not usePhysics):
            return None
        
        # Check if physics is supported.
        if usePhysics is None:
            usePhysics = hasattr(bNode, "rbShapeBoundType")
            if not usePhysics:
                return None
        
        rbFlags = [0 for a in range(16)]
        rbF = bNode.rbFlags
        lastIndex = 0;
        # Get the bit flags
        while rbF > 0:
            val = rbF >> 1
            if val << 1 == rbF:
                #rbFlags.append(0)
                rbFlags[lastIndex] = 0
            else:
                #rbFlags.append(1)
                rbFlags[lastIndex] = 1
            lastIndex += 1
            rbF = val
        
        if len(rbFlags) > 0:
            if rbFlags[10] and rbFlags[2]: # Check if this node is: actor + rigid body + dynamic                
                daePhysicsModel = collada.DaePhysicsModel();
                daePhysicsModel.id = daePhysicsModel.name = self.document.CreateID(daeNode.id,'-PhysicsModel')
                daeRigidBody = collada.DaeRigidBody();
                daeRigidBody.id = daeRigidBody.name = self.document.CreateID(daeNode.id,'-RigidBody')
                daeRigidBodyTechniqueCommon = collada.DaeRigidBody.DaeTechniqueCommon()
                daeRigidBodyTechniqueCommon.dynamic = bool(rbFlags[0])
                daeRigidBodyTechniqueCommon.mass = bNode.rbMass
                # check the shape of the rigid body.
                if bNode.rbShapeBoundType == 0: # Box
                    shape = collada.DaeBoxShape()
                    shape.halfExtents = list(bNode.rbHalfExtents)
                elif bNode.rbShapeBoundType == 1: # Sphere
                    shape = collada.DaeSphereShape()
                    shape.radius = bNode.rbRadius
                elif bNode.rbShapeBoundType == 2: # Cylinder
                    return None
                elif bNode.rbShapeBoundType == 3: # Cone
                    return None
                else: # Convex hull or # Static Triangle Mesh
                    shape = collada.DaeGeometryShape()
                    iGeometry = collada.DaeGeometryInstance()
                    if bNode.rbShapeBoundType == 5:
                        object = self.document.colladaDocument.geometriesLibrary.FindObject(daeNode.id+'-Convex')
                    else:
                        object = self.document.colladaDocument.geometriesLibrary.FindObject(meshID)
                    if object is None:
                        object = collada.DaeGeometry()
                        object.id = object.name = self.document.CreateID(daeNode.id,'-ConvexGeom')
                        convexMesh = collada.DaeConvexMesh()
                        convexMesh.convexHullOf = meshID
                        object.data = convexMesh                                                    
                        self.document.colladaDocument.geometriesLibrary.AddItem(object)
                    iGeometry.object = object    
                    shape.iGeometry = iGeometry
                
                # Create a physics material.
                daePhysicsMaterial = self.document.colladaDocument.physicsMaterialsLibrary.FindObject(daeNode.id+'-PxMaterial')
                if daePhysicsMaterial is None:
                    daePhysicsMaterial = collada.DaePhysicsMaterial()
                    daePhysicsMaterial.id = daePhysicsMaterial.name = self.document.CreateID(daeNode.id, '-PhysicsMaterial')
                    self.document.colladaDocument.physicsMaterialsLibrary.AddItem(daePhysicsMaterial)
                                
                # Create a physics material instance.
                daePhysicsMaterialInstance = collada.DaePhysicsMaterialInstance()
                # Set the physics material of this instance
                daePhysicsMaterialInstance.object = daePhysicsMaterial
                
                
                # add the shape to the commom technique.
                daeRigidBodyTechniqueCommon.shapes.append(shape)                
                # Set the material of the common technique.
                daeRigidBodyTechniqueCommon.iPhysicsMaterial = daePhysicsMaterialInstance
                # Set the common technique of this rigid body.
                daeRigidBody.techniqueCommon = daeRigidBodyTechniqueCommon
                
                # Add the rigid body to the physics model.
                daePhysicsModel.rigidBodies.append(daeRigidBody)
                # Create a new RigidBody instance
                daeRigidBodyInstance = collada.DaeRigidBodyInstance()
                # Set the rigid body of this instance
                daeRigidBodyInstance.body = daeRigidBody
                # Set the node of this instance
                daeRigidBodyInstance.target = daeNode
                
                # Create a new Physics Model Instance.
                daePhysicsModelInstance = collada.DaePhysicsModelInstance()
                # Set the Physics Model of this instance.
                daePhysicsModelInstance.object = daePhysicsModel
                # add the rigidbody instance to this physics model instance.
                daePhysicsModelInstance.iRigidBodies.append(daeRigidBodyInstance)
                # add the physics model to the library.
                self.document.colladaDocument.physicsModelsLibrary.items.append(daePhysicsModel)
                return daePhysicsModelInstance
                
        return None
    
class MeshNode(object):
    def __init__(self,document):
        self.document = document
        self.materials = []
    
    def FindMaterial(self, bMaterials,name):
        for i in range(len(bMaterials)):
            if bMaterials[i].name == name:
                return i
        return -1
    
    def LoadFromDae(self, daeGeometry):        
        meshID = daeGeometry.id
        meshName = daeGeometry.name
        
        # Create a new meshObject
        bMesh2 = Blender.NMesh.New(self.document.CreateNameForObject(meshID,True,'mesh'))

        if isinstance(daeGeometry.data,collada.DaeMesh): # check if it's a mesh
            materials = []
            for matName, material in self.materials.iteritems():
                materials.append(material)
            bMesh2.materials = materials
            
            faces = []
            
            daeMesh = daeGeometry.data
            daeVertices = daeMesh.vertices
            
            # Get all the sources
            sources = dict()
            for source in daeMesh.sources:
                sources[source.id] = source.vectors

            # Get the Position Input
            vPosInput = daeVertices.FindInput('POSITION')

            # Get the Normal Input
            vNorInput = daeVertices.FindInput('NORMAL')
                        
            
            # Keep track of the Blender Vertices
            pVertices = []
            
            self.document.ProgressPart(0.0,'Create Vertices')
            
            
            
            # Create all the vertices.
            for i in sources[vPosInput.source]:
                vPosVector = Vector(i[0], i[1], i[2]) * self.document.tMat
                pVertices.append(Blender.NMesh.Vert(vPosVector.x, vPosVector.y, vPosVector.z))
            bMesh2.verts = pVertices                
                
            faceVerts = [] # The list of vertices for each face to add
            edgeVerts = [] # The list of vertices for each edge to add
            
            # Now create the primitives
            self.document.ProgressPart(0.5,'Create Primitives')
            daePrimitives = daeMesh.primitives
            for primitive in daePrimitives:
                # Get the UV Input
                vUvInput = primitive.FindInput('TEXCOORD')
                uvs = []
                if not (vUvInput is None):
                    for i in sources[vUvInput.source]:
                        vUvVector = Vector(i[0],i[1])
                        uvs.append(vUvVector)
                inputs = primitive.inputs
                maxOffset = primitive.GetMaxOffset()+1 # The number of values for one vertice
                vertCount = 0 # The number of vertices for one primitive
                realVertCount = 0 # The number of vertices for one primitive
                plist = [] # The list with all primitives
                if isinstance(primitive, collada.DaePolygons): # Polygons
                    plist = primitive.polygons
                    vertCount = 4
                elif isinstance(primitive, collada.DaeTriangles): # Triangles
                    plist.append(primitive.triangles)
                    vertCount = 3
                elif isinstance(primitive, collada.DaeLines): # Lines
                    plist.append(primitive.lines)
                    vertCount = 2
                realVertCount = vertCount
                # Loop through each P (primitive)    
                for p in plist:
                    if vertCount == 4:
                        realVertCount = len(p)/maxOffset
                    pIndex = 0
                    # A list with edges in this face
                    faceEdges = []
                    # a list to store all the created faces in to add them to the mesh afterwards.
                    allFaceVerts = [] 
                    # loop through all the values in this 'p'
                    while pIndex < len(p):
                        # Keep track of the verts in this face
                        curFaceVerts2 = []
                        uvList  = []
                        # for every vertice for this primitive
                        for i in range(realVertCount):
                            # Check all the inputs and do the right thing
                            for input in inputs:                                
                                inputVal = p[pIndex+(i*maxOffset)+input.offset]                                
                                ##print p, pIndex, i, maxOffset, input.offset, pIndex+(i*maxOffset)+input.offset, inputVal
                                if input.semantic == "VERTEX":
                                    vert2 = pVertices[inputVal]
##                                    print vert2.co
                                    curFaceVerts2.append(vert2)
                                elif input.semantic == "TEXCOORD":
                                    uvList.append((uvs[inputVal][0],uvs[inputVal][1]))
                                elif input.semantic == "NORMAL":
                                    pass                           
                        if vertCount > 2:
##                            for a in curFaceVerts2:
##                                print a.co   
                            ##curFaceVerts2.reverse()          
                            # Create a new Face.
                            newFace = Blender.NMesh.Face(curFaceVerts2)
                            
                            ##print newFace.col
                            ##newFace.
##                            print ""
##                            for b in newFace.v:
##                                print b.co
                            # Set the UV Coordinates
                            newFace.uv = uvList
                            # Add the new face to the list
                            faces.append(newFace)                         
                            # Add the material to this face
                            if primitive.material != '':
                                # Find the material index.
                                matIndex = self.FindMaterial(bMesh2.materials, self.materials[primitive.material].name)
                                # Set the material index for the new face.
                                newFace.materialIndex = matIndex
                                textures = self.materials[primitive.material].getTextures()
                                if len(textures) > 0 and not (textures[0] is None):
                                    texture = textures[0]
                                    newFace.image = texture.tex.getImage()
                        else:
                            bMesh2.addEdge(curFaceVerts2[0], curFaceVerts2[1])
                        # update the index
                        pIndex += realVertCount * maxOffset
                    bMesh2.faces = faces
        return bMesh2
    
    def SaveToDae(self, bMesh):
        global useTriangles, usePolygons
        daeGeometry = collada.DaeGeometry()
        daeGeometry.id = daeGeometry.name = self.document.CreateID(bMesh.name,'-Geometry')
        
        daeMesh = collada.DaeMesh()                
        
        # Keep track of the edges in faces
        faceEdges = []
        
        daeSource = collada.DaeSource()
        daeSource.id = self.document.CreateID(daeGeometry.id,'-Position')
        daeFloatArray = collada.DaeFloatArray()
        daeFloatArray.id = self.document.CreateID(daeSource.id,'-array')
        daeSource.source = daeFloatArray
        daeSource.techniqueCommon = collada.DaeSource.DaeTechniqueCommon()
        accessor = collada.DaeAccessor()
        daeSource.techniqueCommon.accessor = accessor
        accessor.source = daeFloatArray.id
        accessor.count = len(bMesh.verts)
        accessor.AddParam('X','float')
        accessor.AddParam('Y','float')
        accessor.AddParam('Z','float')        
        
        daeSourceNormals = collada.DaeSource()
        daeSourceNormals.id = self.document.CreateID(daeGeometry.id,'-Normals')
        daeFloatArrayNormals = collada.DaeFloatArray()
        daeFloatArrayNormals.id = self.document.CreateID(daeSourceNormals.id,'-array')
        daeSourceNormals.source = daeFloatArrayNormals
        daeSourceNormals.techniqueCommon = collada.DaeSource.DaeTechniqueCommon()
        accessorNormals = collada.DaeAccessor()
        daeSourceNormals.techniqueCommon.accessor = accessorNormals
        accessorNormals.source = daeFloatArrayNormals.id
        accessorNormals.count = len(bMesh.verts)
        accessorNormals.AddParam('X','float')
        accessorNormals.AddParam('Y','float')
        accessorNormals.AddParam('Z','float') 
        
        daeSourceTextures = collada.DaeSource()
        daeSourceTextures.id = self.document.CreateID(daeGeometry.id , '-UV')
        daeFloatArrayTextures = collada.DaeFloatArray()
        daeFloatArrayTextures.id = self.document.CreateID(daeSourceTextures.id,'-array')
        daeSourceTextures.source = daeFloatArrayTextures
        daeSourceTextures.techniqueCommon = collada.DaeSource.DaeTechniqueCommon()
        accessorTextures = collada.DaeAccessor()
        daeSourceTextures.techniqueCommon.accessor = accessorTextures
        accessorTextures.source = daeFloatArrayTextures.id
        accessorTextures.AddParam('S','float')
        accessorTextures.AddParam('T','float');
        
        
        daeVertices = collada.DaeVertices()
        daeVertices.id = self.document.CreateID(daeGeometry.id,'-Vertex')
        daeInput = collada.DaeInput()
        daeInput.semantic = 'POSITION'
        daeInput.source = daeSource.id
        daeVertices.inputs.append(daeInput)
        daeInputNormals = collada.DaeInput()
        daeInputNormals.semantic = 'NORMAL'
        daeInputNormals.source = daeSourceNormals.id
        daeVertices.inputs.append(daeInputNormals)
        daeMesh.vertices = daeVertices
        
        for vert in bMesh.verts:
            for co in vert.co:                
                daeFloatArray.data.append(co)
            for no in vert.no:
                daeFloatArrayNormals.data.append(no)
        
        
        daePolygonsDict = dict()
        daeTrianglesDict = dict()
        daeLines = None
        
        mesh = Blender.Mesh.Get(bMesh.name);
        
        # Loop trough all the faces
        for face in mesh.faces:
            matIndex = -1
            if bMesh.materials:
                matIndex = face.mat
            
            vertCount = len(face.verts)
                                    
            # Create a new function which adds vertices to a list.
            def AddVerts(verts,prlist):
                prevVert = None
                lastVert = verts[-1]
                
                for v_index in range(len(verts)):
                    # Get the vertice
                    vert = verts[v_index]
                    # Add the vertice to the end of the list.
                    prlist.append(vert.index)
                    # If the mesh has UV-coords, add these to the textures array.
                    if mesh.faceUV:
                        daeFloatArrayTextures.data.append(face.uv[v_index][0])
                        daeFloatArrayTextures.data.append(face.uv[v_index][1])
                        prlist.append(len(daeFloatArrayTextures.data)/2-1)
                        accessorTextures.count = accessorTextures.count + 1
                    # Add the edge to the faceEdges list.
                    if prevVert != None:
                        faceEdges.append(mesh.findEdges(prevVert, vert))
                    else:
                        faceEdges.append(mesh.findEdges([(vert, lastVert)]))
                    # Update the prevVert vertice.
                    prevVert = vert
                    
            if (vertCount == 3 and not usePolygons) or (useTriangles and vertCount == 4): # triangle                
                # Iff a Triangle Item for the current material not exists, create one.
                daeTrianglesDict.setdefault(matIndex, collada.DaeTriangles())
                if vertCount == 3:
                    # Add al the vertices to the triangle list.
                    AddVerts(face.verts,daeTrianglesDict[matIndex].triangles)
                    # Update the vertice count for the trianglelist.
                    daeTrianglesDict[matIndex].count += 1
                else: # Convert polygon to triangles                    
                    verts1 = face.verts[:3]
                    verts2 = face.verts[2:] + tuple([face.verts[0]])
                    # Add al the vertices to the triangle list.
                    AddVerts(verts1,daeTrianglesDict[matIndex].triangles)
                    AddVerts(verts2, daeTrianglesDict[matIndex].triangles)
                    # Update the vertice count for the trianglelist.
                    daeTrianglesDict[matIndex].count += 2              
            else: # polygon
                # Iff a Polygon Item for the current material not exists, create one.
                daePolygonsDict.setdefault(matIndex, collada.DaePolygons())
                # for each polygon, create a new P element
                pverts = [] 
                # Add al the vertices to the pverts list.
                AddVerts(face.verts,pverts)
                # Add the pverts list to the polygons list.
                daePolygonsDict[matIndex].polygons.append(pverts)
                # Update the vertice count for the polygonlist.
                daePolygonsDict[matIndex].count += 1
        # Loop through all the edges
        for edge in mesh.edges:
            if not edge.index in faceEdges:
                if daeLines == None: daeLines = collada.DaeLines()
                daeLines.count += 1
                daeLines.lines.append(edge.v1.index)
                daeLines.lines.append(edge.v2.index)
        
        daeInput = collada.DaeInput()
        daeInput.semantic = 'VERTEX'
        daeInput.offset = 0
        daeInput.source = daeVertices.id
        
        daeInputUV = collada.DaeInput()
        daeInputUV.semantic = 'TEXCOORD'
        daeInputUV.offset = 1
        daeInputUV.source = daeSourceTextures.id
        
        materialName = ''
            
        for k, daeTriangles in daeTrianglesDict.iteritems():
            if k >= 0:
                daeTriangles.material = bMesh.materials[k].name
            daeTriangles.inputs.append(daeInput)
            if mesh.faceUV:
                daeTriangles.inputs.append(daeInputUV)
            daeMesh.primitives.append(daeTriangles)
        
        for k, daePolygons in daePolygonsDict.iteritems():
            if k >= 0:
                daePolygons.material = bMesh.materials[k].name
            daePolygons.inputs.append(daeInput)
            if mesh.faceUV:
                daePolygons.inputs.append(daeInputUV)
            daeMesh.primitives.append(daePolygons)
        
        if daeLines != None:
            daeLines.inputs.append(daeInput)
            daeMesh.primitives.append(daeLines)
##        if len(bMesh.materials) > 0:
##            materialName = bMesh.materials[0].name
##        if daeTriangles != None:
##            daeTriangles.material = materialName
##            daeTriangles.inputs.append(daeInput)
##            daeMesh.primitives.append(daeTriangles)
##        if daePolygons != None:
##            daePolygons.inputs.append(daeInput)
##            daePolygons.material = materialName
##            daeMesh.primitives.append(daePolygons)
##        if daeLines != None:
##            daeLines.material = materialName
##            daeLines.inputs.append(daeInput)
##            daeMesh.primitives.append(daeLines)
        
        daeMesh.sources.append(daeSource)
        daeMesh.sources.append(daeSourceNormals)
        if mesh.faceUV:
            daeMesh.sources.append(daeSourceTextures)
        
        daeGeometry.data = daeMesh
        
        self.document.colladaDocument.geometriesLibrary.AddItem(daeGeometry)
        return daeGeometry
    
    def GetBindMaterials(self, bMesh):
        bindMaterials = []
        # now check the materials
        if bMesh.materials:
            daeBindMaterial = collada.DaeFxBindMaterial()
            for bMaterial in bMesh.materials:                
                instance = collada.DaeFxMaterialInstance()
                ##bMaterial = bMesh.materials[0]
                daeMaterial = self.document.colladaDocument.materialsLibrary.FindObject(bMaterial.name)
                if daeMaterial is None:
                    materialNode = MaterialNode(self.document)
                    daeMaterial = materialNode.SaveToDae(bMaterial)
                instance.object = daeMaterial
                daeBindMaterial.techniqueCommon.iMaterials.append(instance)
            
            # now we have to add this bindmaterial to the intance of this geometry
            bindMaterials.append(daeBindMaterial)
        return bindMaterials
    
class TextureNode(object):
    def __init__(self, document):
        self.document = document
        
    def LoadFromDae(self, daeImage):
        imageID = daeImage.id
        imageName = daeImage.name
        
        bTexture = Blender.Texture.New(imageID)
        
        if Blender.sys.exists(self.document.filePath + daeImage.init_from):
            bTexture.setType('Image')            
            img = Blender.Image.Load(self.document.filePath + daeImage.init_from)        
            bTexture.setImage(img)        
        return bTexture
    
class MaterialNode(object):
    def __init__(self, document):
        self.document = document
        
    def LoadFromDae(self, daeMaterial):
        materialID = daeMaterial.id
        materialName = daeMaterial.name
        
        bMat = Blender.Material.New(materialID)
        for i in daeMaterial.iEffects:
            daeEffect = self.document.colladaDocument.effectsLibrary.FindObject(i.url)        
            shader = daeEffect.profileCommon.technique.shader
            if shader.transparent:
                tcol = shader.transparent.color.rgba        
                tkey = 1
                if shader.transparency:
                    tkey = shader.transparency.float
                alpha = 1 - tkey * (tcol[0]*0.21 + tcol[1]*0.71 + tcol[2]*0.08)
                bMat.setAlpha(alpha)
            elif shader.transparency:
                alpha = 1 - shader.transparency.float
            if shader.reflective:
                color = shader.reflective.color.rgba
                bMat.setMirCol(color[0], color[1], color[2])
            if shader.reflectivity:
                bMat.setRef(shader.reflectivity.float)
 
            if isinstance(shader,collada.DaeFxShadeLambert) or isinstance(shader, collada.DaeFxShadeBlinn) or isinstance(shader, collada.DaeFxShadePhong):
                bMat.setDiffuseShader(Blender.Material.Shaders.DIFFUSE_LAMBERT)
                if shader.diffuse:
                    ##print shader.diffuse.color.rgba, shader.diffuse.color
                    if not (shader.diffuse.color is None):
                        color = shader.diffuse.color.rgba
                        bMat.setRGBCol(color[0],color[1], color[2])
                    else: # Texture
                        texture = shader.diffuse.texture.texture
                        if not (texture is None):
                            bImage = self.document.texturesLibrary.FindObject(texture, True)
                            if not bImage is None:
                                bMat.setTexture(0, bImage, Blender.Texture.TexCo.UV)
                        
                bMat.setSpec(0)           
            
            if isinstance(shader, collada.DaeFxShadeBlinn) or isinstance(shader, collada.DaeFxShadePhong):
                if not isinstance(shader, collada.DaeFxShadePhong):
                    bMat.setSpecShader(Blender.Material.Shaders.SPEC_BLINN)
                    if shader.indexOfRefraction:
                        shader.indexOfRefraction.float
                        bMat.setRefracIndex(shader.indexOfRefraction.float)
                else:
                    bMat.setSpecShader(Blender.Material.Shaders.SPEC_PHONG)
                if shader.specular:
                    specColor = shader.specular.color.rgba
                    bMat.setSpecCol(specColor[0], specColor[1], specColor[2])
                bMat.setSpec(1)
                if shader.shininess:
                    bMat.setHardness(int(shader.shininess.float) * 4)
            
        return bMat        
        
    def SaveToDae(self, bMaterial):
        daeMaterial = collada.DaeFxMaterial()
        daeMaterial.id = daeMaterial.name = self.document.CreateID(bMaterial.name, '-Material')
        
        instance = collada.DaeFxEffectInstance()
        daeEffect = self.document.colladaDocument.effectsLibrary.FindObject(bMaterial.name+'-fx')
        meshNode = MeshNode(self.document)
        if daeEffect is None:
            daeEffect = collada.DaeFxEffect()
            daeEffect.id = daeEffect.name = self.document.CreateID(bMaterial.name , '-fx')
            shader = None
            if bMaterial.getSpec() > 0.0:
                if bMaterial.getSpecShader() == Blender.Material.Shaders.SPEC_BLINN:
                    shader = collada.DaeFxShadeBlinn()
                    shader.AddValue(collada.DaeFxSyntax.INDEXOFREFRACTION, bMaterial.getRefracIndex())
                else:
                    shader = collada.DaeFxShadePhong()                
                shader.AddValue(collada.DaeFxSyntax.SPECULAR, [col * bMaterial.getSpec() for col in bMaterial.getSpecCol()]+[1])
                shader.AddValue(collada.DaeFxSyntax.SHININESS, bMaterial.getHardness() * 0.25)
                
            else :
                shader = collada.DaeFxShadeLambert()
            shader.AddValue(collada.DaeFxSyntax.DIFFUSE,bMaterial.getRGBCol()+[1])
            shader.AddValue(collada.DaeFxSyntax.TRANSPARENCY, 1 - bMaterial.alpha)
            shader.AddValue(collada.DaeFxSyntax.TRANSPARENT, [1,1,1,1])
            shader.AddValue(collada.DaeFxSyntax.EMISSION, [col * bMaterial.getEmit() for col in bMaterial.getRGBCol()] + [1])
            shader.AddValue(collada.DaeFxSyntax.AMBIENT, [col * bMaterial.getAmb() for col in bMaterial.getRGBCol()] + [1])
            shader.AddValue(collada.DaeFxSyntax.REFLECTIVE, bMaterial.getMirCol() + [1])
            shader.AddValue(collada.DaeFxSyntax.REFLECTIVITY, bMaterial.getRef())
            daeEffect.AddShader(shader)

            self.document.colladaDocument.effectsLibrary.AddItem(daeEffect)            
        instance.object = daeEffect
        
        daeMaterial.iEffects.append(instance)
        
        self.document.colladaDocument.materialsLibrary.AddItem(daeMaterial)
        return daeMaterial
        
class CameraNode(object):
    def __init__(self,document):
        self.document = document
    
    def LoadFromDae(self, daeCamera):
        camID = daeCamera.id
        camName = daeCamera.name
        camType = 'persp'
        clipStart = daeCamera.optics.techniqueCommon.znear
        clipEnd = daeCamera.optics.techniqueCommon.zfar
        
        if daeCamera.optics.techniqueCommon.GetType() == collada.DaeSyntax.ORTHOGRAPHIC:
            camType = 'ortho'
        
        camera = Blender.Camera.New(camType,camID)
        camera.clipStart = clipStart
        camera.clipEnd = clipEnd
        return camera

    
    def SaveToDae(self, bCamera):
        daeCamera = collada.DaeCamera()
        daeCamera.id = daeCamera.name = self.document.CreateID(bCamera.name,'-Camera')
        daeOptics = collada.DaeOptics()
        daeTechniqueCommon = None
        if bCamera.type == 1: # orthographic
            daeTechniqueCommon = collada.DaeOptics.DaeOrthographic()
        else: # perspective
            daeTechniqueCommon = collada.DaeOptics.DaePerspective()
            lens = bCamera.getLens( )
            daeTechniqueCommon.yfov = 2 * ( math.atan( 16.0 / lens ) ) * radianToAngle
            
        daeTechniqueCommon.znear = bCamera.clipStart
        daeTechniqueCommon.zfar = bCamera.clipEnd
        
        daeOptics.techniqueCommon = daeTechniqueCommon
        daeCamera.optics = daeOptics
        
        self.document.colladaDocument.camerasLibrary.AddItem(daeCamera)
        return daeCamera
    
class LampNode(object):
    def __init__(self,document):
        self.document = document
        
    def LoadFromDae(self, daeLight):
        
        lampID = daeLight.id
        lampName = daeLight.name
        
        # Create a new lampObject
        lamp = Blender.Lamp.New('Sun',lampID)
        
        if daeLight.techniqueCommon.GetType() == collada.DaeSyntax.DIRECTIONAL:
            lamp.type = Blender.Lamp.Types.Sun
        elif daeLight.techniqueCommon.GetType() == collada.DaeSyntax.POINT:
            lamp.type = Blender.Lamp.Types.Lamp 
            # Get the attenuation
            constAtt = daeLight.techniqueCommon.constantAttenuation
            linAtt = daeLight.techniqueCommon.linearAttenuation
            # set the attenuation
            lamp.energy = 1-constAtt
            if linAtt > 0.0:
                lamp.dist = (lamp.energy/2)/ linAtt
            else:
                lamp.dist = 5000.0
        elif daeLight.techniqueCommon.GetType() == collada.DaeSyntax.SPOT:
            lamp.type = Blender.Lamp.Types.Spot
            # Get the attenuation
            constAtt = daeLight.techniqueCommon.constantAttenuation
            linAtt = daeLight.techniqueCommon.linearAttenuation
            # set the attenuation
            lamp.energy = 1-constAtt
            if linAtt > 0.0:
                lamp.dist = (0.5 - constAtt)/ linAtt
            else:
                lamp.dist = 5000.0
            lamp.spotSize = daeLight.techniqueCommon.falloffAngle
            lamp.spotBlend = daeLight.techniqueCommon.falloffExponent
        elif daeLight.techniqueCommon.GetType() == collada.DaeSyntax.AMBIENT:
            lamp.type = Blender.Lamp.Types.Hemi
            

        # Set the color
        lamp.col = daeLight.techniqueCommon.color
        
        return lamp
    
    def SaveToDae(self, bLamp):
        daeLight = collada.DaeLight()
        daeLight.id = daeLight.name = self.document.CreateID(bLamp.name,'-Light')
        
        daeTechniqueCommon = None
        if bLamp.type == Blender.Lamp.Types.Hemi: # Ambient
            daeTechniqueCommon = collada.DaeLight.DaeAmbient()
        elif bLamp.type == Blender.Lamp.Types.Lamp: # Point light
            daeTechniqueCommon = collada.DaeLight.DaePoint()
        elif bLamp.type == Blender.Lamp.Types.Spot: # Spot
            daeTechniqueCommon = collada.DaeLight.DaeSpot()
            daeTechniqueCommon.constantAttenuation = 1-bLamp.energy
            daeTechniqueCommon.linearAttenuation = (0.5 - daeTechniqueCommon.constantAttenuation)/bLamp.dist
        elif bLamp.type == Blender.Lamp.Types.Sun: # Directional
            daeTechniqueCommon = collada.DaeLight.DaeDirectional()
        else: # area
            daeTechniqueCommon = collada.DaeOptics.TechniqueCommon()
        
        daeTechniqueCommon.color = bLamp.col    
        daeLight.techniqueCommon = daeTechniqueCommon
        
        self.document.colladaDocument.lightsLibrary.AddItem(daeLight)
        return daeLight
        
class Library(object):
    def __init__(self, document):
        self.objects = dict()
        self.document = document
        self.daeLibrary = None
        
    def SetDaeLibrary(self, daeLibrary):
        self.daeLibrary = daeLibrary
    
    def FindObject(self, daeInstance, fromDae):
        for k in self.objects:
            
            if 'url' in dir(daeInstance) and k == daeInstance.url:
                return self.objects[k][0]
            elif 'target' in dir(daeInstance) and k == daeInstance.target:
                return self.objects[k][0]        
        if fromDae:
            # dataObject not in library, so add it
            return self.LoadFromDae(daeInstance)
        else:
            return self.SaveToDae(daeIntance)
    
    def FindObjectEx(self, bObject):
        for k in self.objects:
            if k == id:
                return self.objects[k][0]
        
        return self.SaveToDae(bObject)
        
        
    def LoadFromDae(self, daeInstance):
        Debug.Debug('Library: Please override this method','WARNING')
        
    def SaveToDae(self, bScene):
        Debug.Debug('Library: Please override this method','WARNING')
            
class ScenesLibrary(Library):
    
    def LoadFromDae(self, daeInstance):        
        daeVisualScene = self.daeLibrary.FindObject(daeInstance.url)
        # TODO: implement multiple scenes
        return None
    
    def SaveToDae(self, id):
        pass
        #print bScene
                
class CamerasLibrary(Library):
    
    def LoadFromDae(self, daeInstance):
        daeCamera = self.daeLibrary.FindObject(daeInstance.url)
        if daeCamera is None:
            Debug.Debug('CamerasLibrary: Object with this ID does not exist','ERROR')
            return
        
        camID = daeCamera.id
        camName = daeCamera.name
        
        cameraNode = CameraNode(self.document)
        camera = cameraNode.LoadFromDae(daeCamera)

        self.objects[camID] = [camera,camera.name]
        return camera
    
    def SaveToDae(self, id):
        pass
        
class LampsLibrary(Library):
    
    def LoadFromDae(self, daeInstance):        
        daeLight = self.daeLibrary.FindObject(daeInstance.url)
        if daeLight is None:
            Debug.Debug('LightsLibrary: Object with this ID does not exist','ERROR')
            return
        lampID = daeLight.id
        lampName = daeLight.name
        
        lampNode = LampNode(self.document)
        lamp = lampNode.LoadFromDae(daeLight)
        
        self.objects[lampID] = [lamp,lamp.name]
        return lamp
    
    def SaveToDae(self, id):
        pass
        
class MeshLibrary(Library):
    
    def LoadFromDae(self, daeInstance):        
        daeGeometry = self.daeLibrary.FindObject(daeInstance.url)
        if daeGeometry is None:
            Debug.Debug('MeshLibrary: Object with this ID does not exist','ERROR')
            return
        meshID = daeGeometry.id
        meshName = daeGeometry.name
        
        meshNode = MeshNode(self.document)
        
        # Get the materials ( only get the first one right now)
        bMaterials = daeInstance.bindMaterials
        meshNode.materials = dict()
        if bMaterials:
            for bMaterial in bMaterials:
                for iMaterial in bMaterial.techniqueCommon.iMaterials:
                    Material = self.document.materialsLibrary.FindObject(iMaterial,True)
                    meshNode.materials[iMaterial.target] = Material
                    ##print 'Mat:',Material
        ##print meshNode.materials
##            bMaterial = bMaterials[0]
##            if bMaterial:
##                iMaterials = bMaterial.techniqueCommon.iMaterials
##                if iMaterials:
##                    if iMaterials[0]:
##                        meshNode.materials = [self.document.materialsLibrary.FindObject(iMaterials[0],True)]
##            
        
        bMesh = meshNode.LoadFromDae(daeGeometry)
 
        # Add this mesh in this library, under it's real name
        self.objects[meshID] = [bMesh,bMesh.name]
        return bMesh
    
    def SaveToDae(self, id):
        pass
        
class MaterialsLibrary(Library):
    
    def LoadFromDae(self, daeInstance):        
        daeMaterial = self.daeLibrary.FindObject(daeInstance.target)
        if daeMaterial is None:
            Debug.Debug('MaterialLibrary: Object with this TARGET:%s does not exist'%(daeInstance.target),'ERROR')
            return
        materialID = daeMaterial.id
        materialName = daeMaterial.name
        
        materialNode = MaterialNode(self.document)
        
        bMaterial = materialNode.LoadFromDae(daeMaterial)
 
        # Add this mesh in this library, under it's real name
        self.objects[materialID] = [bMaterial,bMaterial.name]
        return bMaterial
    
    def SaveToDae(self, id):
        pass

class TexturesLibrary(Library):
    
    def LoadFromDae(self, daeImage):        
        if daeImage is None:
            Debug.Debug('TexturesLibrary: Object with this TARGET:%s does not exist'%(daeImage),'ERROR')
            return
        imageID = daeImage.id
        imageName = daeImage.name
        
        textureNode = TextureNode(self.document)
        bTexture = textureNode.LoadFromDae(daeImage)
 
        # Add this texture in this library, under it's real name
        self.objects[imageID] = [bTexture,bTexture.name]
        return bTexture
    
    def SaveToDae(self, id):
        pass
