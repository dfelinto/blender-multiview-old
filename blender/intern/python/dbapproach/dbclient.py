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
import string
import MySQLdb
from socket import *

if __name__ == "__main__":
    if sys.platform == "win32":
        dbconn = MySQLdb.connect(user = "root")
    else:
        dbconn = MySQLdb.connect(user = "root",
                                 unix_socket = "/tmp/mysql.sock")
    cur = dbconn.cursor()
    try:
        cur.execute("USE Blender;")
    except:
        import blenderdb
        blenderdb.createBlenderDB()
        cur.execute("USE Blender;")
    s = socket(AF_INET, SOCK_STREAM)
    s.bind("", 8000)
    print "waiting for connection ..."
    s.listen(1)
    conn, addr = s.accept()
    while 1:
        data = conn.recv(1024)
        conn.send("OK")
        if data == "bye bye ...":
            break
        else:
            words = string.split(data, ":")
            frame = string.atoi(words[0])
            type = words[1]
            data = words[2]
            values = ""
            # cameras
            if type == "Camera":
                words = string.split(data, ", ")
                del words[1]  # block_type
                del words[-1] # ipo
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # ctrlpts
            elif type == "Ctrlpt":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # curves
            elif type == "Curve":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # knots
            elif type == "Knot":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # lamps
            elif type == "Lamp":
                words = string.split(data, ", ")
                del words[1]  # block_type
                del words[-1] # ipo
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # nmfaces
            elif type == "NMFace":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                try:
                    cur.execute(command)
                except:
                    print "(failed)"
                else:
                    print "(ok)"
            # nmeshs
            elif type == "NMesh":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command,
                try:
                    cur.execute(command)
                except:
                    print "(failed)"
                else:
                    print "(ok)"
            # nmverts
            elif type == "NMVert":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                try:
                    cur.execute(command)
                except:
                    print "(failed)"
                else:
                    print "(ok)"
            # materials
            elif type == "Material":
                words = string.split(data, ", ")
                del words[1]  # block_type
                del words[-1] # ipo
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # relationship between materials and objects
            elif type == "MatObj":
                words = string.split(data, ", ")
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # objects
            elif type == "Object":
                words = string.split(data, ", ")
                del words[1]  # block_type
                del words[-2] # ipo
                mat = words[-1]
                del words[-1] # mat
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                # add mat values now
                words = string.split(mat)
                words = words[2:] # forget "mat = "
                for word in words:
                    values = values + '"%s", ' % word
                values = values[:-2] # skip last ", "
                # add frame
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
            # worlds
            elif type == "World":
                words = string.split(data, ", ")
                del words[1]  # block_type
                del words[-1] # ipo
                for word in words:
                    name, value = string.split(word, " = ")
                    values = values + '"%s", ' % value
                values = values[:-2] # skip last ", "
                values = ('"%s", ' % frame) + values
                command = "INSERT INTO %s VALUES(%s);" % (type, values)
                print command
                cur.execute(command)
    print "Closing connection ..."
    conn.close()
    dbconn.close()
