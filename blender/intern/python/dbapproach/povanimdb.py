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
    cur.execute('SELECT name, data FROM Object WHERE type = "Camera"' +
                'AND frame = %s;' % frame)
    results = cur.fetchall()
    objName = results[0][0]
    camName = results[0][1]
    cur.execute('SELECT Lens FROM Camera WHERE name = "%s";' % camName)
    results = cur.fetchall()
    lens = results[0][0]
    file.write("camera {\n")
    file.write("\tright <1, 0, 0>\n")
    file.write("\tup    <0, 1, 0>\n")
    file.write("\tangle     %s\n" % (360.0 * math.atan(16.0 / lens) / math.pi))
    writeTransform(file, cur, objName, frame)
    file.write("}\n")

def writeHeader(file, cur, frame):
    file.write('#include "finish.inc"\n')
    cur.execute('SELECT HorR, HorG, HorB FROM World WHERE frame = %s;' % frame)
    results = cur.fetchall()
    if results:
        r, g, b = results[0]
        file.write('background { color rgb <%s, %s, %s> }\n' %
                   (r, g, b))

def writeLights(file, cur, frame):
    cur.execute('SELECT name, data FROM Object WHERE type = "Lamp"' +
                'AND frame = %s;' % frame)
    results = cur.fetchall()
    for result in results:
        objName = result[0]
        lmpName = result[1]
        cur.execute('SELECT R, G, B FROM Lamp WHERE name = "%s"' % lmpName +
                    'AND frame = %s' % frame)
        color = cur.fetchall()
        r, g, b = color[0]
        file.write("light_source {\n")
        file.write("\t<0, 0, 0>\n")
        file.write("\tcolor rgb <%s, %s, %s>\n" % (r, g, b))
        writeTransform(file, cur, objName, frame)
        file.write("}\n")

def writeMatrix(file, cur, name, frame):
    cur.execute('SELECT ' +
                'mat00, mat01, mat02, mat03, ' +
                'mat10, mat11, mat12, mat13, ' +
                'mat20, mat21, mat22, mat23, ' +
                'mat30, mat31, mat32, mat33 FROM Object ' +
                'WHERE name = "%s" AND frame = %s' % (name, frame))
    results = cur.fetchall()
    file.write("\tmatrix <%s, %s, %s,\n" % (results[0][0],
                                            results[0][1],
                                            -results[0][2]))
    file.write("\t        %s, %s, %s,\n" % (results[0][4],
                                            results[0][5],
                                            -results[0][6]))
    file.write("\t        %s, %s, %s,\n" % (results[0][8],
                                            results[0][9],
                                            -results[0][10]))
    file.write("\t        %s, %s, %s>\n" % (results[0][12],
                                            results[0][13],
                                            -results[0][14]))

def writeObject(file, cur, name, frame):
    global usePrimitives
    povname = ""
    if usePrimitives and name[:len("Cone")] == "Cone":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = cone {\n" % povname)
        file.write("\t<%s, %s, %s>, %s,\n" % (0, 0, -1, math.sqrt(2)))
        file.write("\t<%s, %s, %s>, %s\n" % (0, 0, 1, 0))
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("Cube")] == "Cube":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = box {\n" % povname)
        file.write("\t<-1, -1, -1>,\n")
        file.write("\t< 1,  1,  1>\n")
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("Cylinder")] == "Cylinder":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = cylinder {\n" % povname)
        file.write("\t<%s, %s, %s>,\n" % (0, 0, -1))
        file.write("\t<%s, %s, %s>, %s\n" % (0, 0, 1, math.sqrt(2)))
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("Plane")] == "Plane":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = polygon {\n" % povname)
        file.write("\t%s,\n" % 5)
        file.write("\t<%s, %s>, <%s, %s>, <%s, %s>, <%s, %s>, <%s, %s>\n" %
                   (-1, -1, -1, +1, +1, +1, +1, -1, -1, -1))
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("Sphere")] == "Sphere":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = sphere {\n" % povname)
        file.write("\t<%s, %s, %s>, %s\n" % (0, 0, 0, math.sqrt(2)))
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("SurfDonut")] == "SurfDonut":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = torus {\n" % povname)
        file.write("\t%s, %s\n" % (0.75, 0.25))
        writeTexture(file, cur, name, frame)
        file.write("\trotate <%s, %s, %s>\n" % (-90, 0, 0))
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("SurfSphere")] == "SurfSphere":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = sphere {\n" % povname)
        file.write("\t<0, 0, 0>, 1\n")
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("SurfTube")] == "SurfTube":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = cylinder {\n" % povname)
        file.write("\t<%s, %s, %s>,\n" % (0, 0, 1))
        file.write("\t<%s, %s, %s>, %s\n" % (0, 0, -1, 1))
        file.write("\topen\n")
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    elif usePrimitives and name[:len("Tube")] == "Tube":
        povname = string.replace(name, ".", "_")
        povname = string.upper(povname)
        file.write("#declare %s = cylinder {\n" % povname)
        file.write("\t<%s, %s, %s>,\n" % (0, 0, 1))
        file.write("\t<%s, %s, %s>, %s\n" % (0, 0, -1, math.sqrt(2)))
        file.write("\topen\n")
        writeTexture(file, cur, name, frame)
        writeMatrix(file, cur, name, frame)
        file.write("}\n")
    else:
        # if all this doesn't help try to export a mesh
        cur.execute('SELECT type, data FROM Object WHERE name = "%s";' % name)
        results = cur.fetchall()
        if results[0][0] == "NMesh":
            povname = string.replace(name, ".", "_")
            povname = string.upper(povname)
            file.write("#declare %s = mesh {\n" % povname)
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
                    file.write("\tsmooth_triangle {\n")
                    file.write("\t\t<%s, %s, %s>, <%s, %s, %s>,\n" %
                               (vertices[face[0]][0],
                                vertices[face[0]][1],
                                vertices[face[0]][2],
                                normals[face[0]][0],
                                normals[face[0]][1],
                                normals[face[0]][2]))
                    file.write("\t\t<%s, %s, %s>, <%s, %s, %s>,\n" %
                               (vertices[face[1]][0],
                                vertices[face[1]][1],
                                vertices[face[1]][2],
                                normals[face[1]][0],
                                normals[face[1]][1],
                                normals[face[1]][2]))
                    file.write("\t\t<%s, %s, %s>, <%s, %s, %s>\n" %
                               (vertices[face[2]][0],
                                vertices[face[2]][1],
                                vertices[face[2]][2],
                                normals[face[2]][0],
                                normals[face[2]][1],
                                normals[face[2]][2]))
                    file.write("\t}\n")
                else:
                    file.write("\ttriangle {\n")
                    file.write("\t\t<%s, %s, %s>,\n" % (vertices[face[0]][0],
                                                        vertices[face[0]][1],
                                                        vertices[face[0]][2]))
                    file.write("\t\t<%s, %s, %s>,\n" % (vertices[face[1]][0],
                                                        vertices[face[1]][1],
                                                        vertices[face[1]][2]))
                    file.write("\t\t<%s, %s, %s>\n" % (vertices[face[2]][0],
                                                       vertices[face[2]][1],
                                                       vertices[face[2]][2]))
                    file.write("\t}\n")
                if face[3] != -1:
                    # second triangle
                    if face[4]:
                        file.write("\tsmooth_triangle {\n")
                        file.write("\t\t<%s, %s, %s>, <%s, %s, %s>,\n" %
                                   (vertices[face[0]][0],
                                    vertices[face[0]][1],
                                    vertices[face[0]][2],
                                    normals[face[0]][0],
                                    normals[face[0]][1],
                                    normals[face[0]][2]))
                        file.write("\t\t<%s, %s, %s>, <%s, %s, %s>,\n" %
                                   (vertices[face[2]][0],
                                    vertices[face[2]][1],
                                    vertices[face[2]][2],
                                    normals[face[2]][0],
                                    normals[face[2]][1],
                                    normals[face[2]][2]))
                        file.write("\t\t<%s, %s, %s>, <%s, %s, %s>\n" %
                                   (vertices[face[3]][0],
                                    vertices[face[3]][1],
                                    vertices[face[3]][2],
                                    normals[face[3]][0],
                                    normals[face[3]][1],
                                    normals[face[3]][2]))
                        file.write("\t}\n")
                    else:
                        file.write("\ttriangle {\n")
                        file.write("\t\t<%s, %s, %s>,\n" %
                                   (vertices[face[0]][0],
                                    vertices[face[0]][1],
                                    vertices[face[0]][2]))
                        file.write("\t\t<%s, %s, %s>,\n" %
                                   (vertices[face[2]][0],
                                    vertices[face[2]][1],
                                    vertices[face[2]][2]))
                        file.write("\t\t<%s, %s, %s>\n" %
                                   (vertices[face[3]][0],
                                    vertices[face[3]][1],
                                    vertices[face[3]][2]))
                        file.write("\t}\n")
            writeTexture(file, cur, name, frame)
            writeMatrix(file, cur, name, frame)
            file.write("}\n")
    return povname

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
        povname = writeObject(file, cur, name, frame)
        povnames.append(povname)
    # make a scene out of all written declarations
    file.write("#declare SCENE = union {\n")
    for povname in povnames:
        if povname != "":
            file.write("\tobject { %s }\n" % povname)
    file.write("}\n")
    file.write("object { SCENE }\n")

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
                        'WHERE name = "%s" AND frame = %s;' %
                        (material, frame))
            results = cur.fetchall()
            if results:
                r, g, b = results[0]
    file.write("\ttexture {\n")
    file.write("\t\tpigment {\n")
    file.write("\t\t\tcolor rgb <%s, %s, %s>\n" % (r, g, b))
    file.write("\t\t}\n")
    file.write("\t\tfinish { Phong_Glossy }\n")
    file.write("\t}\n")


def writeTransform(file, cur, name, frame):
    cur.execute('SELECT SizeX, SizeY, SizeZ FROM Object ' +
                'WHERE name = "%s" AND frame = %s;' % (name, frame))
    results = cur.fetchall()
    file.write("\tscale     <%s, %s, %s>\n" % results[0])
    cur.execute('SELECT RotX, RotY, RotZ FROM Object ' +
                'WHERE name = "%s" AND frame = %s;' % (name, frame))
    results = cur.fetchall()
    file.write("\trotate    <%s, %s, %s>\n" % (- (180.0*results[0][0]/math.pi),
                                               - (180.0*results[0][1]/math.pi),
                                               180.0*results[0][2]/math.pi))
    cur.execute('SELECT LocX, LocY, LocZ FROM Object ' +
                'WHERE name = "%s" AND frame = %s;' % (name, frame))
    results = cur.fetchall()
    file.write("\ttranslate <%s, %s, %s>\n" % (results[0][0],
                                               results[0][1],
                                               -results[0][2]))

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
    for i in xrange(staframe, endframe + 1):
        print "write frame %s ..." % i
        filename = "test%04d.pov" % (i-1,)
        file = open(filename, "w")
        writeHeader(file, cur, i)
        writeCamera(file, cur, i)
        writeLights(file, cur, i)
        writeScene(file, cur, i)
        file.close()
    # close database
    dbconn.close()
