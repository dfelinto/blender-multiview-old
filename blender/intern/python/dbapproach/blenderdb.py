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

def createBlenderDB():
    if sys.platform == "win32":
        dbconn = MySQLdb.connect(user = "root")
    else:
        dbconn = MySQLdb.connect(user = "root",
                                 unix_socket = "/tmp/mysql.sock")
    cur = dbconn.cursor()
    # database Blender
    try:
        cur.execute("CREATE DATABASE Blender;")
    except:
        print "Database Blender allready exists ..."
    else:
        print "Creating database Blender ..."
    cur.execute("USE Blender;")
    # table Camera
    try:
        cur.execute("CREATE TABLE Camera (frame INT, name VARCHAR(20), " +
                    "Lens DOUBLE, ClSta DOUBLE, ClEnd DOUBLE);")
    except:
        print "Table Camera allready exists ..."
    else:
        print "Creating table Camera ..."
    # table Ctrlpt
    try:
        cur.execute("CREATE TABLE Ctrlpt (frame INT, name VARCHAR(20), " +
                    "idx INT, x DOUBLE, y DOUBLE, z DOUBLE, w DOUBLE);")
    except:
        print "Table Ctrlpt allready exists ..."
    else:
        print "Creating table Ctrlpt ..."
    # table Knot
    try:
        cur.execute("CREATE TABLE Knot (frame INT, name VARCHAR(20), " +
                    "idx INT, param CHAR(1), value DOUBLE);")
    except:
        print "Table Knot allready exists ..."
    else:
        print "Creating table Knot ..."
    # table Curve
    try:
        cur.execute("CREATE TABLE Curve (frame INT, name VARCHAR(20), " +
                    "pntsu INT, pntsv INT, orderu INT, orderv INT, " +
                    "closedu INT, closedv INT, numknotsu INT, numknotsv INT);")
    except:
        print "Table Curve allready exists ..."
    else:
        print "Creating table Curve ..."
    # table Lamp
    try:
        cur.execute("CREATE TABLE Lamp (frame INT, name VARCHAR(20), " +
                    "R DOUBLE, G DOUBLE, B DOUBLE, Energ DOUBLE, " +
                    "Dist DOUBLE, SpoSi DOUBLE, SpoBl DOUBLE, " +
                    "HaInt DOUBLE, Quad1 DOUBLE, Quad2 DOUBLE);")
    except:
        print "Table Lamp allready exists ..."
    else:
        print "Creating table Lamp ..."
    # table Material
    try:
        cur.execute("CREATE TABLE Material (frame INT, name VARCHAR(20), " +
                    "R DOUBLE, G DOUBLE, B DOUBLE, SpecR DOUBLE, " +
                    "SpecG DOUBLE, SpecB DOUBLE, MirR DOUBLE, " +
                    "MirG DOUBLE, MirB DOUBLE, Ref DOUBLE, Alpha DOUBLE, " +
                    "Emit DOUBLE, Amb DOUBLE, Spec DOUBLE, SpTra DOUBLE, " +
                    "HaSize DOUBLE, Mode DOUBLE, Hard DOUBLE);")
    except:
        print "Table Material allready exists ..."
    else:
        print "Creating table Material ..."
    # table MatObj
    try:
        cur.execute("CREATE TABLE MatObj (frame INT, " +
                    "material VARCHAR(20), object VARCHAR(20));")
    except:
        print "Table MatObj allready exists ..."
    else:
        print "Creating table MatObj ..."
    # table NMesh
    try:
        cur.execute("CREATE TABLE NMesh (frame INT NOT NULL, " +
                    "name VARCHAR(20) NOT NULL, " +
                    "numVerts INT, numFaces INT, numMats INT, " +
                    "has_col TINYINT, has_uvco TINYINT, " +
                    "PRIMARY KEY (frame, name));")
    except:
        print "Table NMesh allready exists ..."
    else:
        print "Creating table NMesh ..."
    # table NMFace
    try:
        cur.execute("CREATE TABLE NMFace (frame INT NOT NULL, " +
                    "name VARCHAR(20) NOT NULL, " +
                    "idx1 INT NOT NULL, idx2 INT NOT NULL, " +
                    "idx3 INT NOT NULL, idx4 INT NOT NULL, " +
                    "smooth INT NOT NULL, PRIMARY KEY (frame, name, idx1, " +
                    "idx2, idx3, idx4, smooth));")
    except:
        print "Table NMFace allready exists ..."
    else:
        print "Creating table NMFace ..."
    # table NMVert
    try:
        cur.execute("CREATE TABLE NMVert (frame INT NOT NULL, " +
                    "name VARCHAR(20) NOT NULL, " +
                    "idx INT NOT NULL, x DOUBLE, y DOUBLE, z DOUBLE, " +
                    "u DOUBLE, v DOUBLE, nx DOUBLE, ny DOUBLE, nz DOUBLE, " +
                    "PRIMARY KEY (frame, name, idx));")
    except:
        print "Table NMVert allready exists ..."
    else:
        print "Creating table NMVert ..."
    # table Object
    try:
        cur.execute("CREATE TABLE Object (frame INT, name VARCHAR(20), " +
                    "parent VARCHAR(20), track VARCHAR(20), " +
                    "data VARCHAR(20), type VARCHAR(20), " +
                    "LocX DOUBLE, LocY DOUBLE, LocZ DOUBLE, " +
                    "dLocX DOUBLE, dLocY DOUBLE, dLocZ DOUBLE, " +
                    "RotX DOUBLE, RotY DOUBLE, RotZ DOUBLE, " +
                    "dRotX DOUBLE, dRotY DOUBLE, dRotZ DOUBLE, " +
                    "SizeX DOUBLE, SizeY DOUBLE, SizeZ DOUBLE, " +
                    "dSizeX DOUBLE, dSizeY DOUBLE, dSizeZ DOUBLE, " +
                    "Effx DOUBLE, Effy DOUBLE, Effz DOUBLE, " +
                    "mat00 DOUBLE, mat01 DOUBLE, " +
                    "mat02 DOUBLE, mat03 DOUBLE, " +
                    "mat10 DOUBLE, mat11 DOUBLE, " +
                    "mat12 DOUBLE, mat13 DOUBLE, " +
                    "mat20 DOUBLE, mat21 DOUBLE, " +
                    "mat22 DOUBLE, mat23 DOUBLE, " +
                    "mat30 DOUBLE, mat31 DOUBLE, " +
                    "mat32 DOUBLE, mat33 DOUBLE);")
    except:
        print "Table Object allready exists ..."
    else:
        print "Creating table Object ..."
    # table World
    try:
        cur.execute("CREATE TABLE World (frame INT, name VARCHAR(20), " +
                    "HorR DOUBLE, HorG DOUBLE, HorB DOUBLE, ZenR DOUBLE, " +
                    "ZenG DOUBLE, ZenB DOUBLE, Expos DOUBLE, " +
                    "MisSta DOUBLE, MisDi DOUBLE, MisHi DOUBLE, " +
                    "StarDi DOUBLE, StarSi DOUBLE);")
    except:
        print "Table World allready exists ..."
    else:
        print "Creating table World ..."
    dbconn.close()

if __name__ == "__main__":
    createBlenderDB()
