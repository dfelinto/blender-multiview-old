#! /usr/bin/env python

#######################
# (c) Jan Walter 2000 #
#######################

# CVS
# $Author$
# $Date$
# $RCSfile$
# $Revision$

import sys
import MySQLdb
import math
import string

usePrimitives = 1

def writeCamera(file, cur, frame):
    cur.execute('SELECT name, data, LocX, LocY, LocZ, RotX, RotY, RotZ ' +
                'FROM Object WHERE type = "Camera" AND frame = %s;' % frame)
    results = cur.fetchall()
    objName = results[0][0]
    camName = results[0][1]
    LocX    = results[0][2]
    LocY    = results[0][3]
    LocZ    = results[0][4]
    RotX    = results[0][5]
    RotY    = results[0][6]
    RotZ    = results[0][7]
    cur.execute('SELECT Lens FROM Camera WHERE name = "%s";' % camName)
    results = cur.fetchall()
    lens = results[0][0]
    file.write('Projection "perspective" "fov" %s\n' %
               (360.0 * math.atan(16.0 / lens) / math.pi))
    file.write("Rotate %s %s %s %s\n" % (180 * RotX / math.pi, 1, 0, 0))
    file.write("Rotate %s %s %s %s\n" % (180 * RotY / math.pi, 0, 1, 0))
    file.write("Rotate %s %s %s %s\n" % (-180 * RotZ / math.pi, 0, 0, 1))
    file.write("Translate %s %s %s\n" % (-LocX, -LocY, LocZ))

def writeHeader(file, cur, frame = None):
    if frame == None:
        file.write("##RenderMan RIB-Structure 1.0\n")
        file.write("version 3.03\n")
        return
    file.write("##RenderMan RIB-Structure 1.0\n")
    file.write("version 3.03\n")
    file.write("Format 300 300 1\n")
    file.write('Display "test.%04d.tif" "file" "rgba"\n' % frame)
    # background color
    cur.execute('SELECT HorR, HorG, HorB FROM World WHERE frame = %s;' % frame)
    results = cur.fetchall()
    if results:
        r, g, b = results[0]
        file.write('Declare "bgcolor" "color"\n')
        file.write('Imager "background" "bgcolor" [%s %s %s]\n' % (r, g, b))

def writeIdentifier(file, name):
    file.write("#" * (len(name) + 4) + "\n")
    file.write("# %s #\n" % name)
    file.write("#" * (len(name) + 4) + "\n")

def writeLights(file, cur, frame):
    cur.execute('SELECT name, data, LocX, LocY, LocZ FROM Object ' +
                'WHERE type = "Lamp" AND frame = %s;' % frame)
    results = cur.fetchall()
    numlights = 0
    for result in results:
        objName = result[0]
        lmpName = result[1]
        LocX    = result[2]
        LocY    = result[3]
        LocZ    = result[4]
        cur.execute('SELECT R, G, B FROM Lamp WHERE name = "%s" ' % lmpName +
                    'AND frame = %s' % frame)
        color = cur.fetchall()
        r, g, b = color[0]
        writeIdentifier(file, objName)
        numlights = numlights + 1
        file.write('LightSource "pointlight" %s ' % numlights +
                   '"from" [%s %s %s] ' % (LocX, LocY, -LocZ) +
                   '"lightcolor" [%s %s %s] ' % (r, g, b) +
                   '"intensity" %s\n' % 50)

def writeMatrix(file, cur, name, frame):
    cur.execute('SELECT ' +
                'mat00, mat01, mat02, mat03, ' +
                'mat10, mat11, mat12, mat13, ' +
                'mat20, mat21, mat22, mat23, ' +
                'mat30, mat31, mat32, mat33 FROM Object ' +
                'WHERE name = "%s" AND frame = %s' % (name, frame))
    results = cur.fetchall()
    file.write("Transform " +
               "[%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s]\n" %
               (+results[0][ 0], +results[0][ 1],
                -results[0][ 2], +results[0][ 3],
                +results[0][ 4], +results[0][ 5],
                -results[0][ 6], +results[0][ 7],
                +results[0][ 8], +results[0][ 9],
                -results[0][10], +results[0][11],
                +results[0][12], +results[0][13],
                -results[0][14], +results[0][15]))

def writeObject(file, cur, name, frame):
    global usePrimitives
    if usePrimitives and name[:len("Cone")] == "Cone":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Translate %s %s %s\n" % (0, 0, -1))
        file.write("Cone %s %s %s\n" % (2, math.sqrt(2), 360))
        file.write("Disk %s %s %s\n" % (0, math.sqrt(2), 360))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("Cube")] == "Cube":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   (-1, -1, -1,  1, -1, -1, -1, -1,  1,  1, -1,  1))
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   (-1, -1,  1, -1,  1,  1, -1, -1, -1, -1,  1, -1))
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   ( 1,  1, -1, -1,  1, -1,  1,  1,  1, -1,  1,  1))
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   ( 1, -1, -1,  1,  1, -1,  1, -1,  1,  1,  1,  1))
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   ( 1, -1,  1,  1,  1,  1, -1, -1,  1, -1,  1,  1))
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   (-1,  1, -1,  1,  1, -1, -1, -1, -1,  1, -1, -1))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("Cylinder")] == "Cylinder":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Disk %s %s %s\n" % (-1, math.sqrt(2), 360))
        file.write("Cylinder %s %s %s %s\n" % (math.sqrt(2), -1, 1, 360))
        file.write("Disk %s %s %s\n" % (1, math.sqrt(2), 360))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("Plane")] == "Plane":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write('Patch "bilinear" "P" ' +
                   "[%s %s %s %s %s %s %s %s %s %s %s %s]\n" %
                   (-1, 1, 0, 1, 1, 0, -1, -1, 0, 1, -1, 0))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("Sphere")] == "Sphere":
        radius = math.sqrt(2)
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Sphere %s %s %s %s\n" % (radius, -radius, radius, 360))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("SurfDonut")] == "SurfDonut":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Torus %s %s %s %s %s\n" % (0.75, 0.25, 0, 360, 360))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("SurfSphere")] == "SurfSphere":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Sphere %s %s %s %s\n" % (1, -1, 1, 360))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("SurfTube")] == "SurfTube":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Cylinder %s %s %s %s\n" % (1, -1, 1, 360))
        file.write("AttributeEnd\n")
    elif usePrimitives and name[:len("Tube")] == "Tube":
        writeIdentifier(file, name)
        writeTexture(file, cur, name, frame)
        file.write("AttributeBegin\n")
        writeMatrix(file, cur, name, frame)
        file.write("Cylinder %s %s %s %s\n" % (math.sqrt(2), -1, 1, 360))
        file.write("AttributeEnd\n")
    else:
        # if all this doesn't help try to export a mesh
        cur.execute('SELECT type, data FROM Object ' +
                    'WHERE name = "%s" AND frame = %s;' % (name, frame))
        results = cur.fetchall()
        if results[0][0] == "NMesh":
            writeIdentifier(file, name)
            writeTexture(file, cur, name, frame)
            file.write("AttributeBegin\n")
            writeMatrix(file, cur, name, frame)
            meshName = results[0][1]
            cur.execute('SELECT idx1, idx2, idx3, idx4, smooth FROM NMFace ' +
                        'WHERE name = "%s" AND frame = %s;' %
                        (meshName, frame))
            faces = cur.fetchall()
            cur.execute('SELECT x, y, z FROM NMVert ' +
                        'WHERE name = "%s" AND frame = %s ORDER BY idx;' %
                        (meshName, frame))
            vertices = cur.fetchall()
            cur.execute('SELECT nx, ny, nz FROM NMVert ' +
                        'WHERE name = "%s" AND frame = %s ORDER BY idx;' %
                        (meshName, frame))
            normals = cur.fetchall()
            for face in faces:
                # first triangle
                if face[4]:
                    file.write('Polygon "P" [ ')
                    for i in xrange(3):
                        file.write("%s %s %s " % (vertices[face[i]][0],
                                                  vertices[face[i]][1],
                                                  vertices[face[i]][2]))
                    file.write('] "N" [ ')
                    for i in xrange(3):
                        file.write("%s %s %s " % (normals[face[i]][0],
                                                  normals[face[i]][1],
                                                  normals[face[i]][2]))
                    file.write(']\n')
                else:
                    file.write('Polygon "P" [ ')
                    for i in xrange(3):
                        file.write("%s %s %s " % (vertices[face[i]][0],
                                                  vertices[face[i]][1],
                                                  vertices[face[i]][2]))
                    file.write(']\n')
                if face[3] != -1:
                    # second triangle
                    if face[4]:
                        file.write('Polygon "P" [ ')
                        for i in [0, 2, 3]:
                            file.write("%s %s %s " % (vertices[face[i]][0],
                                                      vertices[face[i]][1],
                                                      vertices[face[i]][2]))
                        file.write('] "N" [ ')
                        for i in [0, 2, 3]:
                            file.write("%s %s %s " % (normals[face[i]][0],
                                                      normals[face[i]][1],
                                                      normals[face[i]][2]))
                        file.write(']\n')
                    else:
                        file.write('Polygon "P" [ ')
                        for i in [0, 2, 3]:
                            file.write("%s %s %s " % (vertices[face[i]][0],
                                                      vertices[face[i]][1],
                                                      vertices[face[i]][2]))
                        file.write(']\n')
            file.write("AttributeEnd\n")
        elif results[0][0] == "Curve":
            print "write NURBS surface ..."
            cur.execute('SELECT pntsu, pntsv, orderu, orderv, ' +
                        'closedu, closedv FROM Curve ' +
                        'WHERE name = "%s" AND frame = %s;' % (name, frame))
            results = cur.fetchall()
            if results:
                nucvs   = results[0][0]
                nvcvs   = results[0][1]
                uorder  = results[0][2]
                vorder  = results[0][3]
                closedu = results[0][4]
                closedv = results[0][5]
                newnucvs = nucvs
                newnvcvs = nvcvs
                if not vorder:
                    # don't handle curves right now ...
                    return
                # knots
                cur.execute('SELECT value FROM Knot ' +
                            'WHERE name = "%s" AND param = "u" ' % name +
                            'AND frame = %s ORDER BY idx;' % frame)
                results = cur.fetchall()
                uknots = []
                for result in results:
                    uknots.append(result[0])
                cur.execute('SELECT value FROM Knot ' +
                            'WHERE name = "%s" AND param = "v" ' % name +
                            'AND frame = %s ORDER BY idx;' % frame)
                results = cur.fetchall()
                vknots = []
                for result in results:
                    vknots.append(result[0])
                # control points
                cur.execute('SELECT x, y, z, w FROM Ctrlpt ' +
                            'WHERE name = "%s" AND frame = %s ORDER BY idx;' %
                            (name, frame))
                results = cur.fetchall()
                ctrlpts = []
                for result in results:
                    x = result[0]
                    y = result[1]
                    z = result[2]
                    w = result[3]
                    ctrlpts.append([x, y, z, w])
                # closedu
                if closedu:
                    newctrlpts = []
                    for v in xrange(nvcvs):
                        for u in xrange(nucvs):
                            newctrlpts.append(ctrlpts[v*nucvs + u])
                        for i in xrange(uorder-1):
                            newctrlpts.append(ctrlpts[v*nucvs + i])
                    newnucvs = nucvs + uorder - 1
                    ctrlpts = newctrlpts
                # closedv
                if closedv:
                    newctrlpts = ctrlpts + ctrlpts[:newnucvs*(vorder-1)]
                    newnvcvs = nvcvs + vorder - 1
                    ctrlpts = newctrlpts
                # umin, umax, vmin, and vmax
                umin = uknots[uorder-1]
                umax = uknots[newnucvs]
                vmin = vknots[vorder-1]
                vmax = vknots[newnvcvs]
                # write to file
                writeIdentifier(file, name)
                writeTexture(file, cur, name, frame)
                file.write("AttributeBegin\n")
                writeMatrix(file, cur, name, frame)
                file.write("NuPatch %s %s [ " % (newnucvs, uorder))
                for uknot in uknots:
                    file.write("%s " % uknot)
                file.write("] %s %s %s %s [ " % (umin, umax, newnvcvs, vorder))
                for vknot in vknots:
                    file.write("%s " % vknot)
                file.write('] %s %s "Pw" [ ' % (vmin, vmax))
                for ctrlpt in ctrlpts:
                    x, y, z, w = ctrlpt
                    file.write("%s %s %s %s " % (x*w, y*w, z*w, w))
                file.write("]\n")
                file.write("AttributeEnd\n")

def writeScene(file, cur, frame):
    names = []
    povnames = []
    # get all object names without lamps and cameras
    cur.execute('SELECT name FROM Object WHERE type <> "Camera" ' +
                'AND type <> "Lamp" AND frame = %s;' % frame)
    results = cur.fetchall()
    for result in results:
        names.append(result[0])
    # write objects
    for name in names:
        writeObject(file, cur, name, frame)

def writeTexture(file, cur, name, frame):
    r, g, b = 1, 1, 1
    # name of object should not be used
    # use name of associated data instead
    cur.execute('SELECT data FROM Object ' +
                'WHERE name = "%s" AND frame = %s;' % (name, frame))
    results = cur.fetchall()
    if results:
        dataname = results[0][0]
        cur.execute('SELECT material FROM MatObj ' +
                    'WHERE object = "%s" AND frame = %s;' % (dataname, frame))
        results = cur.fetchall()
        if results:
            material = results[0][0]
            cur.execute('SELECT R, G, B FROM Material ' +
                        'WHERE name = "%s" AND frame = %s;' % (material, frame))
            results = cur.fetchall()
            if results:
                r, g, b = results[0]
    file.write('Surface "plastic"\n')
    file.write("Color [%s %s %s]\n" % (r, g, b))

if __name__ == "__main__":
    # open database "Blender"
    if sys.platform == "win32":
        dbconn = MySQLdb.connect(user = "root")
    else:
        dbconn = MySQLdb.connect(user = "root",
                                 unix_socket = "/tmp/mysql.sock")
    cur = dbconn.cursor()
    cur.execute("USE Blender;")
    # get start and end frame
    cur.execute("select min(frame) from Object;")
    results = cur.fetchall()
    staframe = results[0][0]
    cur.execute("select max(frame) from Object;")
    results = cur.fetchall()
    endframe = results[0][0]
    # use database
    filename = "test.rib"
    file = open(filename, "w")
    writeHeader(file, cur)
    for i in xrange(staframe, endframe + 1):
        print "write frame %s ..." % i
        file.write("FrameBegin %s\n" % (i-1,))
        file.write("Format 300 300 1\n")
        file.write('Display "test%04d.tif" "file" "rgba"\n' % (i-1,))
        # background color
        cur.execute('SELECT HorR, HorG, HorB FROM World WHERE frame = %s;' % i)
        results = cur.fetchall()
        if results:
            r, g, b = results[0]
            file.write('Declare "bgcolor" "color"\n')
            file.write('Imager "background" "bgcolor" [%s %s %s]\n' %
                       (r, g, b))
        writeCamera(file, cur, i)
        file.write("WorldBegin\n")
        file.write('Attribute "light" "shadows" "on"\n')
        writeLights(file, cur, i)
        writeScene(file, cur, i)
        file.write("WorldEnd\n")
        file.write("FrameEnd\n")
    file.close()
    # close database
    dbconn.close()
