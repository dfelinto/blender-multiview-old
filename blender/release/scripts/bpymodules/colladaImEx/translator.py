# --------------------------------------------------------------------------
# Illusoft Collada 1.4 plugin for Blender version 0.3.90
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
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
	
	def __init__(self, isImporter, version, debugM, fileName,_useTriangles, _usePolygons, _bakeMatrices, _exportSelection, _createNewScene, _clearScene, _lookAt, _exportPhysics, _exportCurrentScene, _exportRelativePaths, _useUV):
		global __version__, debugMode, usePhysics, useTriangles, usePolygons, bakeMatrices, exportSelection, createNewScene, clearScene, lookAt, replaceNames, exportPhysics, exportCurrentScene, useRelativePaths, useUV
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
		exportPhysics = _exportPhysics
		exportCurrentScene = _exportCurrentScene
		useRelativePaths = _exportRelativePaths
		useUV = _useUV
		
		replaceNames = clearScene
		
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
	
	# Keep track of the layers on import
	layers = None
	
	cameraLibrary = None
##	  geometryLibrary = None
	controllersLibrary = None
	animationsLibrary = None
##	  materialLibrary = None
	texturesLibrary = None
	lampsLibrary = None
	colladaDocument = None
	scenesLibrary = None
	fps = 25
	
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
				return self.CreateID(tempName+typeName, typeName)
	
	def IncrementString(self, name):
		tempName = name;
		if name.rfind('.') >= 0:
			while tempName[-1:].isdigit():
				tempName =	tempName[:-1]
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
		self.animationsLibrary = AnimationsLibrary(self)
		self.controllersLibrary = ControllersLibrary(self)
		
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
		
		# Keep track of the 20 layers
		self.layers = [None for x in range(20)]
		
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
		self.animationsLibrary.SetDaeLibrary(self.colladaDocument.animationsLibrary)
		self.controllersLibrary.SetDaeLibrary(self.colladaDocument.controllersLibrary)
		
		# Parse the COLLADA file
		self.colladaDocument.LoadDocumentFromFile(fileName)
		
		if self.colladaDocument.asset.upAxis == collada.DaeSyntax.Y_UP:
			self.tMat[0][0] = 0
			self.tMat[1][1] = 0
			self.tMat[2][2] = 0
			self.tMat[0][1] = 1
			self.tMat[1][2] = 1
			self.tMat[2][0] = 1
			self.axiss = ["Y", "Z", "X"]
			
		self.axiss = ["X", "Y", "Z"]
			
		self.progressStep = self.progressField/(self.colladaDocument.GetItemCount()+1)
		
		# Read the COLLADA stucture and build the scene in Blender.
		Blender.Window.DrawProgressBar(0.4, 'Translate Collada 2 Blender')
		self.sceneGraph.LoadFromCollada(self.colladaDocument.scene)
		
		self.Progress()
	
	def CalcVector(self, vector):
		if self.colladaDocument.asset.upAxis == collada.DaeSyntax.Y_UP:
			return Vector(vector[2], vector[0], vector[1])
		else:
			return vector
	
	def Export(self, fileName):
		global __version__, filename
		filename = fileName
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
			if not exportCurrentScene or self.currentBScene == bScene:
				self.fps = bScene.getRenderingContext( ).framesPerSec()
				daeInstanceVisualScene = collada.DaeVisualSceneInstance()
				daeInstancePhysicsScene = collada.DaePhysicsSceneInstance()
				daeVisualScene = self.colladaDocument.visualScenesLibrary.FindObject(bScene.name)
				if daeVisualScene is None:
					sceneGraph = SceneGraph(self)
					scenesList = sceneGraph.SaveToDae(bScene)
					daeVisualScene = scenesList[0]
					if exportPhysics:
						daePhysicsScene = scenesList[1]

				daeInstanceVisualScene.object = daeVisualScene
				
				#self.colladaDocument.visualScenesLibrary.AddItem(daeIntanceVisualScene)
				daeScene.iVisualScenes.append(daeInstanceVisualScene)
				if exportPhysics:
					if not (daePhysicsScene is None):
						daeInstancePhysicsScene.object = daePhysicsScene
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
			if ob != None:
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
			
		# Create all the animations
			
		# Update the current Scene.
		self.document.currentBScene.update(1)
	
	def SaveToDae(self, bScene):
		global exportSelection, exportPhysics
		daeVisualScene = collada.DaeVisualScene()
		daeVisualScene.id = daeVisualScene.name = self.document.CreateID(bScene.name,'-Scene')
		
		daePhysicsScene = collada.DaePhysicsScene()
		daePhysicsScene.id = daePhysicsScene.name = self.document.CreateID(bScene.name+'-Physics', '-Scene')
		
		if exportSelection:
			self.rootNodes = Blender.Object.GetSelected()
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
			daeVisualScene.nodes.append(daeNode)
			if exportPhysics:
				daeInstancePhysicsModel = nodeResult[1] 			   
				daePhysicsScene.iPhysicsModels.append(daeInstancePhysicsModel)
			
		self.document.colladaDocument.visualScenesLibrary.AddItem(daeVisualScene)
		if exportPhysics and len(daePhysicsScene.iPhysicsModels) > 0:			
			self.document.colladaDocument.physicsScenesLibrary.AddItem(daePhysicsScene)
		else:
			daePhysicsScene = None

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
			
class Controller(object):
	def __init__(self, document):
		self.document = document
		
	def LoadFromDae(self, daeController, daeControllerInstance):
		# Check if this controller is a SKIN controller
		if not (daeController.skin is None):
			# Create a new GeometryInstance for getting the mesh for this controller.
			daeGeoInstance = collada.DaeGeometryInstance()
			daeGeoInstance.url = daeController.skin.source
			daeGeoInstance.bindMaterials = daeControllerInstance.bindMaterials
			# Get the Blender Mesh
			bMesh = self.document.meshLibrary.FindObject(daeGeoInstance, True)
			
			# todo: create vert groups
			
			
			#return the mesh.
			return bMesh
		else: # It's a morph controller. do nothing.
			return None
		

class Animation(object):
	def __init__(self, document):
		self.document = document
	
	def LoadFromDae(self, daeAnimation, daeNode, bObject):
		# Loop trough all channels
		for channel in daeAnimation.channels:
			ca = channel.target.split("/",1)
			if ca[0] == daeNode.id:				
				for s in daeAnimation.samplers:
					if s.id == channel.source[1:]:
						sampler = s
				if not (s is None):
					if bObject.ipo is None:
						ipo = Blender.Ipo.New("Object",daeAnimation.id)					
						bObject.setIpo(ipo)
					else:
						ipo = bObject.ipo
					type = self.FindType(ca[1], daeNode)
					input = sampler.GetInput("INPUT")
					inputSource = daeAnimation.GetSource(input.source)
					# Check if the input has a TIME parameter and is the only one.
					if not (type is None) and inputSource.techniqueCommon.accessor.HasParam("TIME") and len(inputSource.techniqueCommon.accessor.params) == 1:
						output = sampler.GetInput("OUTPUT")
						outputSource = daeAnimation.GetSource(output.source)
						accessorCount = outputSource.techniqueCommon.accessor.count
						accessorStride = outputSource.techniqueCommon.accessor.stride
						times = [x*self.document.fps for x in inputSource.source.data]
						if type[0] == "translate" or type[0] == "scale" or (type[0] == "rotate" and type[1][1] == "ANGLE"):
							axiss = []
							if len(type[1]) == 1:
								axiss = ["X", "Y", "Z"]
							elif type[0] == "rotate":
								axiss = [type[1][0][-1]]
							elif len(type[1]) == 2:
								if type[1][1] in ["X", "Y", "Z"]:
									axiss = [type[1][1]]
							for axis in axiss:
								if type[0] == "translate":
									cname = "Loc"
								elif type[0] == "scale":
									cname = "Scale"
								else:
									cname = "Rot"
								cname += self.document.axiss[axiss.index(axis)]
								curve = ipo.addCurve(cname)
								for time in times:
									val = outputSource.source.data[times.index(time)*accessorStride+axiss.index(axis)]
									if type[0] == "rotate":
										val /= 10
									curve.addBezier((time,val))						
						
	
	def FindType(self, target, daeNode):
		ta = target.split(".",1)
		for t in daeNode.transforms:
			if t[2] == ta[0]:
				return [t[0],ta]
		return None

	def SaveToDae(self, ipo, targetDaeNode):
		animations = None
		curves = ipo.getCurves()
		if not curves is None:
			animations = dict()
			for curve in curves:
				cName = curve.getName()
				interpolation = curve.getInterpolation()
				if cName.startswith("Loc") or cName.startswith("Rot") or cName.startswith("Scale"):
					if cName.startswith("Loc"):
						n = collada.DaeSyntax.TRANSLATE
					elif cName.startswith("Scale"):
						n = collada.DaeSyntax.SCALE						
					else:
						n = collada.DaeSyntax.ROTATE+cName[-1]
					ani = animations.setdefault(n,{})
					for bp in curve.bezierPoints:
						anit = ani.setdefault(bp.pt[0],{'X':0, 'Y':0, 'Z':0, 'interpolation':interpolation})
						anit[cName[-1]] = bp.pt[1]
						if cName.startswith("Rot"):
							anit[cName[-1]] = anit[cName[-1]]*10 # Multiply the angle times 10 (Blender uses angle/10)					
				else:
					pass
		
		
		# add animations to collada
		for name, animation in animations.iteritems():
			daeAnimation = collada.DaeAnimation()
			daeAnimation.id = daeAnimation.name = self.document.CreateID(targetDaeNode.id+'-'+name,'-Animation')

			daeSourceInput = collada.DaeSource()
			daeSourceInput.id = self.document.CreateID(daeAnimation.id,'-input')
			daeFloatArrayInput = collada.DaeFloatArray()
			daeFloatArrayInput.id = self.document.CreateID(daeSourceInput.id,'-array')
			daeSourceInput.source = daeFloatArrayInput
			daeSourceInput.techniqueCommon = collada.DaeSource.DaeTechniqueCommon()
			accessorInput = collada.DaeAccessor()
			daeSourceInput.techniqueCommon.accessor = accessorInput
			accessorInput.source = daeFloatArrayInput.id
			accessorInput.count = len(animation)
			accessorInput.AddParam('TIME','float')
			
			if name == collada.DaeSyntax.TRANSLATE or name == collada.DaeSyntax.SCALE:
				vals = self.CreateSourceOutput(daeAnimation, len(animation),  "xyz")				
			elif name.startswith(collada.DaeSyntax.ROTATE):
				vals = self.CreateSourceOutput(daeAnimation, len(animation),  "angle")
				
			daeSourceOutput = vals[0]
			outputArray = vals[1]
			daeSourceInterpolation = vals[2]
			interpolationArray = vals[3]
			
			daeAnimation.sources.append(daeSourceInput)
			daeAnimation.sources.append(daeSourceOutput)
			daeAnimation.sources.append(daeSourceInterpolation)
			
			
			for key, value in animation.iteritems():
				daeFloatArrayInput.data.append(key/self.document.fps)
				interpolation = value['interpolation']
				if interpolation == 'Constant':
					cInterpolation = 'STEP'
				elif interpolation == 'Linear':
					cInterpolation = 'LINEAR'
				else:
					cInterpolation = 'BEZIER'
					
				if name == collada.DaeSyntax.TRANSLATE or name == collada.DaeSyntax.SCALE:						
					outputArray.data.append(value['X'])
					outputArray.data.append(value['Y'])
					outputArray.data.append(value['Z'])
					
					interpolationArray.data.append(cInterpolation)		
					interpolationArray.data.append(cInterpolation)
					interpolationArray.data.append(cInterpolation)
				elif name.startswith(collada.DaeSyntax.ROTATE):
					outputArray.data.append(value[name[-1]])
					interpolationArray.data.append(cInterpolation)
					
			if not name.startswith(collada.DaeSyntax.ROTATE) or sum(outputArray.data) != 0:				
				daeSampler = collada.DaeSampler()
				daeSampler.id = self.document.CreateID(daeAnimation.id,"-sampler")
				daeAnimation.samplers.append(daeSampler)
				
				daeInputInput = collada.DaeInput()
				daeInputInput.semantic = 'INPUT'
				daeInputInput.source = daeSourceInput.id
				daeSampler.inputs.append(daeInputInput)
				
				daeInputOutput = collada.DaeInput()
				daeInputOutput.semantic = 'OUTPUT'
				daeInputOutput.source = daeSourceOutput.id
				daeSampler.inputs.append(daeInputOutput)
				
				daeInputInterpolation = collada.DaeInput()
				daeInputInterpolation.semantic = 'INTERPOLATION'
				daeInputInterpolation.source = daeSourceInterpolation.id
				daeSampler.inputs.append(daeInputInterpolation)
				
				daeChannel = collada.DaeChannel()
				daeChannel.source = daeSampler
				
				daeChannel.target = targetDaeNode.id+'/'+name
				if name.startswith(collada.DaeSyntax.ROTATE):
					daeChannel.target += ".ANGLE"
				daeAnimation.channels.append(daeChannel)
				
				self.document.colladaDocument.animationsLibrary.AddItem(daeAnimation)
		
	def CreateSourceOutput(self, daeAnimation, count, type):
		daeSourceOutput = collada.DaeSource()
		daeSourceOutput.id = self.document.CreateID(daeAnimation.id,'-output')
		if type == "xyz" or "angle":
			outputArray = collada.DaeFloatArray()
		else:
			pass
			
		##daeFloatArrayOutput = collada.DaeFloatArray()
		outputArray.id = self.document.CreateID(daeSourceOutput.id,'-array')
		daeSourceOutput.source = outputArray
		daeSourceOutput.techniqueCommon = collada.DaeSource.DaeTechniqueCommon()
		accessorOutput = collada.DaeAccessor()
		daeSourceOutput.techniqueCommon.accessor = accessorOutput
		accessorOutput.source = outputArray.id
		accessorOutput.count = count
		
		
		daeSourceInterpolation = collada.DaeSource()
		daeSourceInterpolation.id = self.document.CreateID(daeAnimation.id,'-interpolation')
		interpolationArray = collada.DaeNameArray()
		interpolationArray.id = self.document.CreateID(daeSourceInterpolation.id,'-array')
		daeSourceInterpolation.source = interpolationArray
		daeSourceInterpolation.techniqueCommon = collada.DaeSource.DaeTechniqueCommon()
		accessorInterpolation = collada.DaeAccessor()
		daeSourceInterpolation.techniqueCommon.accessor = accessorInterpolation
		accessorInterpolation.source = interpolationArray.id
		accessorInterpolation.count = count
		
		if type == "xyz":			
			accessorOutput.AddParam('X','float')
			accessorOutput.AddParam('Y','float')
			accessorOutput.AddParam('Z','float')
			accessorInterpolation.AddParam('X','Name')
			accessorInterpolation.AddParam('Y','Name')
			accessorInterpolation.AddParam('Z','Name')
			
		elif type == "angle":
			accessorOutput.AddParam('ANGLE','float')
			accessorInterpolation.AddParam('ANGLE','Name')
			
		return [daeSourceOutput, outputArray, daeSourceInterpolation, interpolationArray]
		
	
class SceneNode(object):
	
	def __init__(self, document, sceneNode):
		self.id = ''
		self.type = ''
		self.document = document
		self.transformMatrix = None
		self.parentNode = sceneNode
		self.bObject = None
		self.bEditBone = None
		
		self.aap = None
		
	def ObjectFromDae(self,daeNode):
		global replaceNames
		self.document.Progress()
		self.id = daeNode.id
		self.type = daeNode.type
		
		editBone = None
		newObject = None
		dataObject = None
		armature = None
		parentBone = None
		
		if daeNode.IsJoint():
			# it's a Joint, not implemented yet
			if daeNode.parentNode == None or not daeNode.parentNode.IsJoint():				
				newObject = Blender.Object.New('Armature',self.id)
				self.document.currentBScene.link(newObject)
			else:
				armature = self.parentNode.bObject.data
				armature.makeEditable()
				editBone = Blender.Armature.Editbone()
				armature.bones[str(self.id)] = editBone				
				if self.parentNode.id in armature.bones.keys():
					parentBone = armature.bones[str(self.parentNode.id)]
					editBone.parent = parentBone
					editBone.options = Blender.Armature.CONNECTED	
			
				self.bObject = self.parentNode.bObject
		else : #its a Node			  
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
						newObject = Blender.Object.New('Mesh',self.id)
						dataObject = self.document.controllersLibrary.FindObject(daeInstance, True)
					elif isinstance(daeInstance,collada.DaeGeometryInstance):
						newObject = Blender.Object.New('Mesh',self.document.CreateNameForObject(self.id,replaceNames, 'object'))
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
			
			# TODO: Vertex Colors: MAYBE CHANGE THIS LATER update the mesh..
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
				mat = mat*RotationMatrix(data[3] % 360,4,'r',Vector([data[0],data[1],data[2]])* self.document.tMat)
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
				# TODO: LOOKAT Transform: use the correct up-axis
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
				
		self.transformMatrix = mat
		if isinstance(self.parentNode, SceneNode):
				self.transformMatrix *= self.parentNode.transformMatrix
		
##		print ""
##		print self.id
##		print self.transformMatrix
			
		if newObject is None: # We have a bone.
			vecTail = Vector(0,0,0)
			vecHead = Vector(0,0,0)
			if not (parentBone is None):
				vecTail = parentBone.tail
				vecHead = parentBone.tail
			
			vecParentPos = Vector(self.parentNode.transformMatrix[3][0], self.parentNode.transformMatrix[3][1], self.parentNode.transformMatrix[3][2])
			vecChildPos = Vector(self.transformMatrix[3][0], self.transformMatrix[3][1], self.transformMatrix[3][2])
			vecArmaturePos = Vector(self.bObject.loc[0], self.bObject.loc[1], self.bObject.loc[2])
			##vecTail = vecChildPos - vecParentPos
			vecTail = vecChildPos - vecArmaturePos
##			print "child", vecChildPos
##			print "parent", vecParentPos
##			print "armature", vecArmaturePos
##			for i in range(len(daeNode.transforms)):
##				transform = daeNode.transforms[len(daeNode.transforms)-(i+1)]
##				type = transform[0]
##				data = transform[1]
##				if type == collada.DaeSyntax.TRANSLATE:
##					##vecTail = vecTail + self.document.CalcVector(Vector(data))
##					vecTail = vecHead - Vector(self.transformMatrix[3][0], self.transformMatrix[3][1], self.transformMatrix[3][2])
##					print vecTail
##					##print Vector(data), self.document.CalcVector(Vector(data))
			
			editBone.tail = vecTail
			editBone.head = vecHead
			armature.update()
		else: # Just a Blender Object		
			newObject.setMatrix(self.transformMatrix)
			self.bObject = newObject
		childlist = []
		
		
		
		for daeChild in daeNode.nodes:
			childSceneNode = SceneNode(self.document,self)
			object = childSceneNode.ObjectFromDae(daeChild)
			if not(object is None):
				childlist.append(object)
		if not (newObject is None):
			newObject.makeParent(childlist,0,1)
			
			# Check if this node has an animation.
			daeAnimations = self.document.animationsLibrary.GetDaeAnimations(self.id)
			for daeAnimation in daeAnimations:
				a = Animation(self.document)
				a.LoadFromDae(daeAnimation, daeNode, newObject)
				
			# Check if the daeNode has some Layer information.
			if not len(daeNode.layer) == 0:
				layers = self.document.layers
				# Keep track of the blender layers to which this Node belongs.
				myLayers = []
				# Loop through all layers of this node.
				for layer in daeNode.layer:
					# Check if this layer is used before.
					if layer in layers:
						# If so, the layer to add is the index of that layer + 1.
						layerNo = layers.index(layer)+1
					else:
						# else, create a new layer.
						addLayer = True
						# When the layer is a digit, try to use the same digit in Blender.
						if layer.isdigit():
							# If So, check if this digit is between 1 and 20 AND if this layer is not used before
							digit = int(layer)
							if digit >= 1 and digit <= len(layers) and layers[digit-1] is None:
								# Add the new layer to the list.
								layers[digit-1] = layer
								layerNo = digit
								# Set this flag to false, so further checking is skipped.
								addLayer = False
						# If the layer was not a digit, Create a new one.
						if addLayer:
							layerNo = 1
							# Get the first free spot.
							if None in layers:
								index = layers.index(None)							
								layers[index] = layer
								layerNo = index+1
					# When this layerNo is not in myLayers yet, add it to the list.
					if not (layerNo in myLayers):
						myLayers.append(layerNo)
			else:
				# Use the current selected layers.
				myLayers = self.document.currentBScene.layers
			# Set the layers of the new Object.
			newObject.layers = myLayers
		
		# Return the new Object
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
##			if euler.z != 0: daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotzVec])
##			if euler.y != 0: daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotyVec])
##			if euler.x != 0: daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotxVec])
			daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotzVec])
			daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotyVec])
			daeNode.transforms.append([collada.DaeSyntax.ROTATE, rotxVec])
			
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
			bindMaterials = meshNode.GetBindMaterials(bNode.getData(), daeGeometry.uvTextures)
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
		elif type == 'Armature':
			pass
		
		# Check if the object has an IPO curve. if so, export animation.
		if not (bNode.ipo is None):
			ipo = bNode.ipo
			animation = Animation(self.document)
			animation.SaveToDae(ipo, daeNode)
		
		# Export layer information.
		daeNode.layer = bNode.layers
		
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
		global usePhysics, exportPhysics
		if meshID is None or (not(usePhysics is None) and not usePhysics or not exportPhysics):
			return None
		
		# Check if physics is supported.
		if usePhysics is None:
			usePhysics = hasattr(bNode, "rbShapeBoundType")
			if not usePhysics:
				return None
		
		rbFlags = [0 for a in range(16)]
		rbF = bNode.rbFlags
		lastIndex = 0;
		# Get the bit flags.
		while rbF > 0:
			val = rbF >> 1
			if val << 1 == rbF:
				##rbFlags.append(0)
				rbFlags[lastIndex] = 0
			else:
				##rbFlags.append(1)
				rbFlags[lastIndex] = 1
			lastIndex += 1
			rbF = val
		
		daePhysicsModel = collada.DaePhysicsModel();
		daePhysicsModel.id = daePhysicsModel.name = self.document.CreateID(daeNode.id,'-PhysicsModel')
		daeRigidBody = collada.DaeRigidBody();
		daeRigidBody.id = daeRigidBody.name = daeRigidBody.sid = self.document.CreateID(daeNode.id,'-RigidBody')
		daeRigidBodyTechniqueCommon = collada.DaeRigidBody.DaeTechniqueCommon()
		daeRigidBodyTechniqueCommon.dynamic = bool(rbFlags[0])
		daeRigidBodyTechniqueCommon.mass = bNode.rbMass
		# Check the shape of the rigid body.
		if bNode.rbShapeBoundType == 0 and rbFlags[PhysicsNode.actor] and rbFlags[PhysicsNode.bounds]: # Box
			shape = collada.DaeBoxShape()
			shape.halfExtents = list(bNode.rbHalfExtents)
		elif bNode.rbShapeBoundType == 1 and rbFlags[PhysicsNode.actor] and rbFlags[PhysicsNode.bounds]: # Sphere
			shape = collada.DaeSphereShape()
			shape.radius = bNode.rbRadius
		elif bNode.rbShapeBoundType == 2 and rbFlags[PhysicsNode.actor] and rbFlags[PhysicsNode.bounds]: # Cylinder
			shape = collada.DaeCylinderShape()
			shape.radius = [[bNode.rbRadius],[bNode.rbRadius]]
			shape.height = bNode.rbHalfExtents[2]
		elif bNode.rbShapeBoundType == 3 and rbFlags[PhysicsNode.actor] and rbFlags[PhysicsNode.bounds]: # Cone
			shape = collada.DaeTaperedCylinderShape()
			shape.radius1 = [[bNode.rbRadius],[bNode.rbRadius]]
			shape.radius2 = [0 , 0]
			shape.height = bNode.rbHalfExtents[2]
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
			# Set Restitution.
			daePhysicsMaterial.restitution = 0 # TODO: Physics: when this object is a mesh, use the restitution and friction from its material.
			# Set Friction.
			daePhysicsMaterial.staticFriction = 0.5
			daePhysicsMaterial.dynamicFriction = 0.5
			
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
		global replaceNames 	 
		meshID = daeGeometry.id
		meshName = daeGeometry.name
		
		# Create a new meshObject
		bMesh2 = Blender.NMesh.New(self.document.CreateNameForObject(meshID,replaceNames,'mesh'))

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
			if not (daeMesh.FindSource(vPosInput).techniqueCommon is None):

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
					elif isinstance(primitive, collada.DaePolylist): # Polylist
						plist.append(primitive.polygons)
						vertCount = 5
					realVertCount = vertCount
					# Loop through each P (primitive)	 
					for p in plist:
						if vertCount == 4:
							realVertCount = len(p)/maxOffset
						elif vertCount == 5:
							realVertCount = primitive.vcount[0]
						pIndex = 0
						# A list with edges in this face
						faceEdges = []
						# a list to store all the created faces in to add them to the mesh afterwards.
						allFaceVerts = [] 
						# loop through all the values in this 'p'
						while pIndex < len(p):
							# Keep track of the verts in this face
							curFaceVerts2 = []
							uvList	= []
							# for every vertice for this primitive
							for i in range(realVertCount):
								# Check all the inputs and do the right thing
								for input in inputs:								
									inputVal = p[pIndex+(i*maxOffset)+input.offset] 							   
									if input.semantic == "VERTEX":
										vert2 = pVertices[inputVal]
										curFaceVerts2.append(vert2)
									elif input.semantic == "TEXCOORD":
										uvList.append((uvs[inputVal][0],uvs[inputVal][1]))
									elif input.semantic == "NORMAL":
										pass						   
							if vertCount > 2:
								faceCount = 1 + (realVertCount-4) / 2 + (realVertCount-4) % 2							 
								firstIndex = 2
								lastIndex = 1							
								for a in range(faceCount):
									newFirstIndex = (firstIndex + 1) % realVertCount
									newLastIndex = (lastIndex -1) % realVertCount
									fuv = []
									if newFirstIndex != newLastIndex:
										fv = [curFaceVerts2[firstIndex]] + [curFaceVerts2[newFirstIndex]] + [curFaceVerts2[newLastIndex]] +  [curFaceVerts2[lastIndex]]
										if len(uvList) == realVertCount:
											fuv = [uvList[firstIndex]] + [uvList[newFirstIndex]] + [uvList[newLastIndex]] + [uvList[lastIndex]]
									else:
										fv = [curFaceVerts2[firstIndex]] + [curFaceVerts2[newFirstIndex]] + [curFaceVerts2[lastIndex]]
										if len(uvList) == realVertCount:
											fuv = [uvList[firstIndex]] + [uvList[newFirstIndex]] + [uvList[lastIndex]]
									firstIndex = newFirstIndex
									lastIndex = newLastIndex
									# Create a new Face.
									newFace = Blender.NMesh.Face(fv)
									# Set the UV Coordinates
									newFace.uv = fuv
									# Add the new face to the list
									faces.append(newFace)
									# Add the material to this face
									if primitive.material != '':
										if self.materials.has_key(primitive.material):
											# Find the material index.
											matIndex = self.FindMaterial(bMesh2.materials, self.materials[primitive.material].name)
											# Set the material index for the new face.
											newFace.materialIndex = matIndex
											textures = self.materials[primitive.material].getTextures()
											if len(textures) > 0 and not (textures[0] is None):
												texture = textures[0]
												image = texture.tex.getImage()
												if not image is None:
													newFace.image = image
										else:
											print "Warning: Cannot find material:", primitive.material
							else:
								bMesh2.addEdge(curFaceVerts2[0], curFaceVerts2[1])
							# update the index
							pIndex += realVertCount * maxOffset
						bMesh2.faces = faces
		return bMesh2
	
	def SaveToDae(self, bMesh):
		global useTriangles, usePolygons, useUV
		
		uvTextures = dict()
		
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
			if not useUV and bMesh.materials and len(bMesh.materials) > 0:
				matIndex = face.mat
			elif mesh.faceUV and (useUV or bMesh.materials is None or len(bMesh.materials) == 0):
				if not face.image is None:
					matIndex = face.image.name
			
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
			if mesh.faceUV:
				if not face.image is None:
					if not face.image.name in uvTextures:
						uvTextures[face.image.name] = self.document.CreateID(face.image.name, "-Material")
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
			if k != -1:
				if not useUV and not bMesh.materials is None and len(bMesh.materials) > 0 and k >= 0:
					daeTriangles.material = bMesh.materials[k].name
				elif mesh.faceUV and (useUV or bMesh.materials is None or len(bMesh.materials) == 0):
					daeTriangles.material = uvTextures[k]
			daeTriangles.inputs.append(daeInput)
			if mesh.faceUV:
				daeTriangles.inputs.append(daeInputUV)
			daeMesh.primitives.append(daeTriangles)
		
		for k, daePolygons in daePolygonsDict.iteritems():
			if k != -1:
				if not useUV and not bMesh.materials is None and len(bMesh.materials) > 0 and k >= 0:
					daePolygons.material = bMesh.materials[k].name
				elif mesh.faceUV and (useUV or bMesh.materials is None or len(bMesh.materials) == 0):
					daePolygons.material = uvTextures[k]
			daePolygons.inputs.append(daeInput)
			if mesh.faceUV:
				daePolygons.inputs.append(daeInputUV)
			daeMesh.primitives.append(daePolygons)
		
		if daeLines != None:
			daeLines.inputs.append(daeInput)
			daeMesh.primitives.append(daeLines)
		
		daeMesh.sources.append(daeSource)
		daeMesh.sources.append(daeSourceNormals)
		if mesh.faceUV:
			daeMesh.sources.append(daeSourceTextures)
		
		daeGeometry.data = daeMesh
		
		self.document.colladaDocument.geometriesLibrary.AddItem(daeGeometry)
		daeGeometry.uvTextures = uvTextures
		return daeGeometry
	
	def GetBindMaterials(self, bMesh, uvTextures):
		global useUV
		
		bindMaterials = []
		mesh = Blender.Mesh.Get(bMesh.name);
		# now check the materials
		if (not useUV) and bMesh.materials and len(bMesh.materials) > 0:
			daeBindMaterial = collada.DaeFxBindMaterial()
			for bMaterial in bMesh.materials:				 
				instance = collada.DaeFxMaterialInstance()
				daeMaterial = self.document.colladaDocument.materialsLibrary.FindObject(bMaterial.name)
				if daeMaterial is None:
					materialNode = MaterialNode(self.document)
					daeMaterial = materialNode.SaveToDae(bMaterial)
				instance.object = daeMaterial
				daeBindMaterial.techniqueCommon.iMaterials.append(instance)
			# now we have to add this bindmaterial to the intance of this geometry
			bindMaterials.append(daeBindMaterial)
		elif mesh.faceUV and (useUV or bMesh.materials is None or len(bMesh.materials) == 0):
			daeBindMaterial = collada.DaeFxBindMaterial()
			for imageName, imageNameUnique in uvTextures.iteritems():
				image = Blender.Image.Get(imageName)
				instance = collada.DaeFxMaterialInstance()
				daeMaterial = self.document.colladaDocument.materialsLibrary.FindObject(imageNameUnique)
				if daeMaterial is None:
					textureNode = TextureNode(self.document)
					daeMaterial = textureNode.SaveToDae(image)
					daeMaterial.id = daeMaterial.name = imageNameUnique
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
		
		filename = ''
		if Blender.sys.exists(daeImage.initFrom):
			filename = daeImage.initFrom
		elif Blender.sys.exists(self.document.filePath + daeImage.initFrom):
			filename = self.document.filePath + daeImage.initFrom
		
		
		if filename <> '':
			bTexture.setType('Image')			 
			img = Blender.Image.Load(filename)		  
			bTexture.setImage(img)
		else:
			print 'image not found: %s'%(daeImage.initFrom)
			
		return bTexture
	
	def SaveToDae(self, bImage):
		daeMaterial = collada.DaeFxMaterial()
		##daeMaterial.id = daeMaterial.name = self.document.CreateID(bImage.name, '-Material')
		self.document.colladaDocument.materialsLibrary.AddItem(daeMaterial)
		
		instance = collada.DaeFxEffectInstance()
		daeEffect = self.document.colladaDocument.effectsLibrary.FindObject(bImage.name+'-fx')
		
		if daeEffect is None:
			daeEffect = collada.DaeFxEffect()
			daeEffect.id = daeEffect.name = self.document.CreateID(bImage.name , '-fx')
			
			shader = collada.DaeFxShadeLambert()
			daeImage = self.document.colladaDocument.imagesLibrary.FindObject(bImage.name) 				   
			if daeImage is None: # Create the image
				daeImage = collada.DaeImage()
				daeImage.id = daeImage.name = self.document.CreateID(bImage.name,'-image')
				daeImage.initFrom = Blender.sys.expandpath(bImage.filename)
				if useRelativePaths:
					daeImage.initFrom = CreateRelativePath(filename, daeImage.initFrom) 																 
				self.document.colladaDocument.imagesLibrary.AddItem(daeImage)
				daeTexture = collada.DaeFxTexture()
				daeTexture.texture = daeImage.id
				shader.AddValue(collada.DaeFxSyntax.DIFFUSE, daeTexture)
				
			daeEffect.AddShader(shader)
			self.document.colladaDocument.effectsLibrary.AddItem(daeEffect) 		   
		instance.object = daeEffect
		
		daeMaterial.iEffects.append(instance)
		
		return daeMaterial
	
class MaterialNode(object):
	def __init__(self, document):
		self.document = document
		
	def LoadFromDae(self, daeMaterial):
		materialID = daeMaterial.id
		materialName = daeMaterial.name
		
		bMat = Blender.Material.New(materialID)
		for i in daeMaterial.iEffects:
			daeEffect = self.document.colladaDocument.effectsLibrary.FindObject(i.url)		  
			if not (daeEffect.profileCommon is None):
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
						if not (shader.diffuse.texture is None): # Texture
							texture = shader.diffuse.texture.texture
							if not (texture is None):
								bTexture = self.document.texturesLibrary.FindObject(texture, True)
								if not bTexture is None:
									bMat.setTexture(0, bTexture, Blender.Texture.TexCo.UV)
							
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
		global useRelativePaths, filename
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
			
			# hasDiffuse indicates if the material already has a diffuse texture
			hasDiffuse = False
			
			# Check if a texture is used for color
			textures = bMaterial.getTextures()
			for mTex in textures:				 
				# Check if this texture is mapped to Color
				if not mTex is None and mTex.mapto == Blender.Texture.MapTo.COL:
					texture = mTex.tex
					if not texture.image is None and Blender.sys.exists(texture.image.filename):
						daeImage = self.document.colladaDocument.imagesLibrary.FindObject(texture.name) 				   
						if daeImage is None: # Create the image
							daeImage = collada.DaeImage()
							daeImage.id = daeImage.name = self.document.CreateID(texture.name,'-image')
							daeImage.initFrom = Blender.sys.expandpath(texture.image.filename)
							if useRelativePaths:
								daeImage.initFrom = CreateRelativePath(filename, daeImage.initFrom) 																 
							self.document.colladaDocument.imagesLibrary.AddItem(daeImage)
							daeTexture = collada.DaeFxTexture()
							daeTexture.texture = daeImage.id
							hasDiffuse = True
							shader.AddValue(collada.DaeFxSyntax.DIFFUSE, daeTexture)
							break
			
			if not hasDiffuse:
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
			daeTechniqueCommon = collada.DaeOptics.DaeOrthoGraphic()
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
			# Export the falloff Angle.
			daeTechniqueCommon.falloffAngle = bLamp.getSpotSize()
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
			elif isinstance(daeInstance, str):
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
		# TODO: Scene: implement multiple scenes
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
		daeGeometry = None
		if isinstance(daeInstance, collada.DaeInstance):
			daeGeometry = self.daeLibrary.FindObject(daeInstance.url)
		else:
			print daeInstance
			daeGeometry = self.daeLibrary.FindObject(daeInstance)
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
					meshNode.materials[iMaterial.symbol] = Material
		
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
		bTexture.setType('Image')
		
		# Add this texture in this library, under it's real name
		self.objects[imageID] = [bTexture,bTexture.name]
		return bTexture
	
	def SaveToDae(self, id):
		pass
		
class AnimationsLibrary(Library):
	
	def LoadFromDae(self, animationName):
		daeAnimation = self.daeLibrary.FindObject(animationName)
		if animation is None:
			return;
		animationID = animation.id
		animationName = animation.name
		
		animation = Animation(self.document)
		animation.LoadFromDae(daeAnimation)
		##self.objects[animationID] = [animation, animation.name]
		return animation

	def GetDaeAnimations(self, daeNodeId):
		daeAnimations = []
		for daeAnimation in self.daeLibrary.items:
			for channel in daeAnimation.channels:
				ta = channel.target.split("/", 1)
				if ta[0] == daeNodeId:
					daeAnimations.append(daeAnimation)
		return daeAnimations

class ControllersLibrary(Library):
	
	def LoadFromDae(self, daeInstance):
		daeController = self.daeLibrary.FindObject(daeInstance.url)
		if daeController is None:
			Debug.Debug('ControllersLibrary: Object with this TARGET:%s does not exist'%(daeInstance.target),'ERROR')
			return
		controllerID = daeController.id
		controllerName = daeController.name
		
		controller = Controller(self.document)
		bMesh = controller.LoadFromDae(daeController, daeInstance)
		
		# Add this mesh in this library, under it's real name
		self.objects[controllerID] = [bMesh, bMesh.name]
		
		return bMesh