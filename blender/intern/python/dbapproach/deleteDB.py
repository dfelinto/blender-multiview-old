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

if sys.platform == "win32":
    dbconn = MySQLdb.connect(user = "root")
else:
    dbconn = MySQLdb.connect(user = "root",
                             unix_socket = "/tmp/mysql.sock")
cur = dbconn.cursor()
try:
    cur.execute("USE Blender;")
except:
    print "no database Blender there ..."
else:
    print "deleting Blender database ..."
    cur.execute("DROP DATABASE Blender;")
dbconn.close()
