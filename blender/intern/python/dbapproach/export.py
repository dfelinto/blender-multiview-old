#######################
# (c) Jan Walter 2000 #
#######################

# CVS
# $Author$
# $Date$
# $RCSfile$
# $Revision$

import Blender
from socket import *

useSocket = 1

class BlenderExport:
    def __init__(self, port = None):
        if port:
            self.s = socket(AF_INET, SOCK_STREAM)
            self.s.connect("localhost", port)
        else:
            self.s = None

    def send(self, frame, data):
        if self.s:
            self.s.send(("%s:" % frame) + data)
            self.s.recv(1024)
        else:
            print "frame:", frame
            print data

    def quit(self):
        if self.s:
            self.s.send("bye bye ...")
            self.s.recv(1024)
            self.s.close()

if useSocket:
    export = BlenderExport(8000)
else:
    export = BlenderExport()
# try new variables (staframe, endframe)
try:
    staframe = Blender.Get("staframe")
    endframe = Blender.Get("endframe")
    useframes = 1
except AttributeError:
    staframe = endframe = Blender.Get("curframe")
    useframes = 0
# for each frame ...
for frame in xrange(staframe, endframe + 1):
    if useframes:
        Blender.Set("curframe", frame)
    # cameras
    cameras = Blender.Camera.Get()
    for camera in cameras:
        data = "Camera:"
        data = data + "name = %s" % camera.name
        data = data + ", block_type = %s" % camera.block_type
        properties = camera.properties
        for property in properties:
            exec("newdata = camera.%s" % property)
            data = data + ", %s = %s" % (property, newdata)
        export.send(frame, data)
    # ipos (not exported yet)
    # lamps
    lamps = Blender.Lamp.Get()
    for lamp in lamps:
        data = "Lamp:"
        data = data + "name = %s" % lamp.name
        data = data + ", block_type = %s" % lamp.block_type
        properties = lamp.properties
        for property in properties:
            exec("newdata = lamp.%s" % property)
            data = data + ", %s = %s" % (property, newdata)
        export.send(frame, data)
    # materials
    materials = Blender.Material.Get()
    for material in materials:
        data = "Material:"
        data = data + "name = %s" % material.name
        data = data + ", block_type = %s" % material.block_type
        properties = material.properties
        for property in properties:
            exec("newdata = material.%s" % property)
            data = data + ", %s = %s" % (property, newdata)
        export.send(frame, data)
    # objects
    objects = Blender.Object.Get()
    for object in objects:
        data = "Object:"
        data = data + "name = %s" % object.name
        data = data + ", block_type = %s" % object.block_type
        properties = object.properties
        # problem with mat !!!
        del properties[properties.index("mat")]
        # special handling of parent, track, data, type and Layer
        if object.parent:
            parent = object.parent.name
        else:
            parent = "None"
        if object.track:
            track = object.track.name
        else:
            track = "None"
        try:
            objdata = object.data.name
        except:
            objdata = "None"
            type    = "None"
        else:
            try:
                type = object.data.block_type
            except:
                type = "None"
        data = data + ", parent = %s" % parent
        data = data + ", track = %s"  % track
        data = data + ", data = %s"   % objdata
        data = data + ", type = %s"   % type
        del properties[properties.index("parent")]
        del properties[properties.index("track")]
        del properties[properties.index("data")]
        del properties[properties.index("Layer")]
        # exclude redundant information
        del properties[properties.index("loc")]
        del properties[properties.index("dloc")]
        del properties[properties.index("rot")]
        del properties[properties.index("drot")]
        del properties[properties.index("size")]
        del properties[properties.index("dsize")]
        for property in properties:
            exec("newdata = object.%s" % property)
            data = data + ", %s = %s" % (property, newdata)
        # now append mat data !!!
        data = data + (", mat = %s %s %s %s " %
                       (object.mat[0][0], object.mat[0][1],
                        object.mat[0][2], object.mat[0][3]) +
                       "%s %s %s %s " %
                       (object.mat[1][0], object.mat[1][1],
                        object.mat[1][2], object.mat[1][3]) +
                       "%s %s %s %s " %
                       (object.mat[2][0], object.mat[2][1],
                        object.mat[2][2], object.mat[2][3]) +
                       "%s %s %s %s" %
                       (object.mat[3][0], object.mat[3][1],
                        object.mat[3][2], object.mat[3][3]))
        export.send(frame, data)
        # nmesh
        if type == "NMesh":
            nmesh = Blender.NMesh.GetRaw(objdata)
            # name
            # verts
            for vertex in nmesh.verts:
                data = "NMVert:"
                data = data + "name = %s" % nmesh.name
                data = data + ", idx = %s" % vertex.index
                data = data + ", x = %s" % vertex.co[0]
                data = data + ", y = %s" % vertex.co[1]
                data = data + ", z = %s" % vertex.co[2]
                data = data + ", u = %s" % vertex.uvco[0]
                data = data + ", v = %s" % vertex.uvco[1]
                data = data + ", nx = %s" % vertex.no[0]
                data = data + ", ny = %s" % vertex.no[1]
                data = data + ", nz = %s" % vertex.no[2]
                export.send(frame, data)
            # faces
            for face in nmesh.faces:
                data = "NMFace:"
                data = data + "name = %s" % nmesh.name
                indices = [-1, -1, -1, -1]
                index = 0
                for vertex in face.v:
                    indices[index] = nmesh.verts.index(vertex)
                    index = index + 1
                data = data + ", idx1 = %s" % indices[0]
                data = data + ", idx2 = %s" % indices[1]
                data = data + ", idx3 = %s" % indices[2]
                data = data + ", idx4 = %s" % indices[3]
                data = data + ", smooth = %s" % face.smooth
                export.send(frame, data)
            # mats
            for material in nmesh.mats:
                data = "MatObj:"
                data = data + "material = %s" % material
                data = data + ", object = %s" % nmesh.name
                export.send(frame, data)
            # has_col
            # has_uvco
            data = "NMesh:"
            data = data + "name = %s" % nmesh.name
            data = data + ", numVerts = %s" % len(nmesh.verts)
            data = data + ", numFaces = %s" % len(nmesh.faces)
            data = data + ", numMats = %s" % len(nmesh.mats)
            data = data + ", has_col = %s" % nmesh.has_col
            data = data + ", has_uvco = %s" % nmesh.has_uvco
            export.send(frame, data)
        elif type == "Curve": # maybe a curve or surface
            try:
                nurbs = Blender.NURBS.GetRaw(object.name)
            except:
                print """can't export "%s" yet ...""" % object.name
            else:
                # mats
                for material in nurbs.mats:
                    data = "MatObj:"
                    data = data + "material = %s" % material
                    data = data + ", object = %s" % nurbs.name
                    export.send(frame, data)
                # ctrlpts
                for ctrlpt in nurbs.ctrlpts:
                    data = "Ctrlpt:"
                    data = data + "name = %s" % nurbs.name
                    data = data + ", idx = %s" % ctrlpt.index
                    data = data + ", x = %s" % ctrlpt.vec[0]
                    data = data + ", y = %s" % ctrlpt.vec[1]
                    data = data + ", z = %s" % ctrlpt.vec[2]
                    data = data + ", w = %s" % ctrlpt.vec[3]
                    export.send(frame, data)
                # knots
                if nurbs.knotsu:
                    for i in xrange(len(nurbs.knotsu)):
                        data = "Knot:"
                        data = data + "name = %s" % nurbs.name
                        data = data + ", idx = %s" % i
                        data = data + ", param = u"
                        data = data + ", value = %s" % nurbs.knotsu[i]
                        export.send(frame, data)
                if nurbs.knotsv:
                    for i in xrange(len(nurbs.knotsv)):
                        data = "Knot:"
                        data = data + "name = %s" % nurbs.name
                        data = data + ", idx = %s" % i
                        data = data + ", param = v"
                        data = data + ", value = %s" % nurbs.knotsv[i]
                        export.send(frame, data)
                data = "Curve:"
                data = data + "name = %s" % nurbs.name
                data = data + ", pntsu = %s" % nurbs.pntsu
                data = data + ", pntsv = %s" % nurbs.pntsv
                data = data + ", orderu = %s" % nurbs.orderu
                data = data + ", orderv = %s" % nurbs.orderv
                data = data + ", closedu = %s" % (nurbs.flagu & 1)
                data = data + ", closedv = %s" % (nurbs.flagv & 1)
                if nurbs.knotsu:
                    data = data + ", numknotsu = %s" % len(nurbs.knotsu)
                else:
                    data = data + ", numknotsu = %s" % 0
                if nurbs.knotsv:
                    data = data + ", numknotsv = %s" % len(nurbs.knotsv)
                else:
                    data = data + ", numknotsv = %s" % 0
                export.send(frame, data)
    # worlds
    worlds = Blender.World.Get()
    for world in worlds:
        data = "World:"
        data = data + "name = %s" % world.name
        data = data + ", block_type = %s" % world.block_type
        properties = world.properties
        for property in properties:
            exec("newdata = world.%s" % property)
            data = data + ", %s = %s" % (property, newdata)
        export.send(frame, data)
export.quit()
