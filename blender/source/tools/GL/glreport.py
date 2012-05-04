#!/usr/bin/env python
# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

import os
import re

def listFiles(dir, doDirCallback, doFileCallback, doSkipDir, doSkipFile):
    doDirCallback(dir)

    subdirlist = []

    for item in os.listdir(dir):
        fullpath = os.path.join(dir,item)
        if os.path.isfile(fullpath) and not doSkipFile(fullpath):
            doFileCallback(fullpath)
        elif os.path.isdir(fullpath):
            if not doSkipDir(item, fullpath):
                subdirlist.append(fullpath)

    for subdir in subdirlist:
        listFiles(subdir, doDirCallback, doFileCallback, doSkipDir, doSkipFile)

def printDirectory(filename):
    print "Entering", filename, "..."

def printFilename(filename):
    print filename

def isSvn(item, fullpath):
    return item == ".svn"

sourceFileProg = re.compile(".*\\.c$|.*\\.cpp$|.*\\.h$|.*\\.m$|.*\\.mm$")

def isNotSourceFile(filename):
    return not sourceFileProg.match(filename)

def isGlew(filename):
    basename = os.path.basename(filename)
    return basename == "glew.h" or basename == "wglew.h" or basename == "glxew.h" or basename == "glew.c"
    
def isNotGLUserFile(filename):
    return isNotSourceFile(filename) or isGlew(filename)
    
def isDummy(filename):
    return filename == ".dummy"

#glToken = re.compile(r'\b(?:(?:(?:gl|glu|glut|glew|glX|wgl|agl)[A-Z][a-zA-Z0-9]*)|(?:(?:GL_|GLU_|GLUT_|GLEW_|GLX_|WGL_|AGL_)[A-Z0-9_]*[A-Z0-9])|(?:(?:GL_|GLU_|GLUT_|GLEW_|GLX_|WGL_|AGL_)[A-Z0-9]+_[a-z0-9_]*[a-z0-9])|(?:GL[a-z]+))\b')
glToken = re.compile(r'\b(?:(?:(?:gl|glu|glut|glew|glX|wgl|agl)[A-Z][a-zA-Z0-9]*)|(?:(?:GL_|GLU_|GLUT_|GLEW_|GLX_|WGL_|AGL_)[A-Z0-9_]*[A-Z0-9])|(?:(?:GL_|GLU_|GLUT_|GLEW_|GLX_|WGL_|AGL_)[A-Z0-9]+_[a-z0-9_]*[a-z0-9]))\b')

report = {}

summaryFileCount = 0
summaryExtensions = set()
summaryTokens = set()

def addReportEntry(filename):
    print "Scanning", filename, "..."
    
    f = open(filename)
    s = f.read()
    f.close()

    matches = glToken.findall(s)

    if matches:
        global summaryFileCount

        summaryFileCount += 1
        
        tokens = set(matches)
        
        extensions = set()

        unknownTokens = set()
        
        for token in tokens:
            summaryTokens.add(token)

            if token in database:
                extensions.update(database[token])
                summaryExtensions.update(database[token])
            else:
                unknownTokens.add(token)
        
        extensionsTokens = {}

        for token in tokens:
            for extension in extensions:
                if token in database and extension in database[token]:
                    if not extension in extensionsTokens:
                        extensionsTokens[extension] = set()
                    
                    extensionsTokens[extension].add(token)

        report[filename] = (extensionsTokens, extensions, tokens, unknownTokens)



database = {}

def addDatabaseEntries(filename):
    print "Scanning", filename, "..."
    
    f = open(filename)
    s = f.read()
    f.close()

    matches = glToken.findall(s)

    if matches:
        tokens = set(matches)
        basename = os.path.basename(filename)
        
        for token in tokens:
            if not token in database:
                database[token] = set()

            database[token].add(basename)


            
listFiles("./core", printDirectory, addDatabaseEntries, isSvn, isDummy)
listFiles("./extensions", printDirectory, addDatabaseEntries, isSvn, isDummy)
listFiles("./es", printDirectory, addDatabaseEntries, isSvn, isDummy)
listFiles("./platform", printDirectory, addDatabaseEntries, isSvn, isDummy)
listFiles("./classic", printDirectory, addDatabaseEntries, isSvn, isDummy)
#listFiles("./types", printDirectory, addDatabaseEntries, isSvn, isDummy)

# insert a couple of symbols by hand
# The GL[A-Z]+ style symbol would conflict too easily with legit tokens
database["GLDEBUGPROCAMD"] = "GL_AMD_debug_output"
database["GLDEBUGPROCARB"] = "GL_ARB_debug_output"

# OpenCL interop
database["cl_context"] = "GL_ARB_cl_event"
database["cl_event"] = "GL_ARB_cl_event"

prefix = "../../../"

listFiles(prefix, printDirectory, addReportEntry, isSvn, isNotGLUserFile)

out = open("./report.txt", "w")

out.write("Summary:\n\n")
out.write("\tFiles that use OpenGL: " + str(summaryFileCount) + "\n")

reportList = report.keys()
reportList.sort()

for filename in reportList:
    out.write("\t\t" + filename[len(prefix):]  + "\n")
    
out.write("\n")

out.write("\tAll extensions used:\n")

summaryExtensionsList = []
for x in summaryExtensions:
    summaryExtensionsList.append(x)
    
summaryExtensionsList.sort()

for extension in summaryExtensionsList:
    out.write("\t\t" + extension + "\n")

out.write("\n")

out.write("\tAll tokens used (extension in parenthesis):\n")

summaryTokensList = []
for x in summaryTokens:
    summaryTokensList.append(x)
    
summaryTokensList.sort()

for token in summaryTokensList:
    if token in database:
        out.write("\t\t" + token + " (" + ",".join(database[token]) + ")\n")
    else:
        out.write("\t\t" + token + "\n")

out.write("\n")
out.write("Detailed Report:\n")
out.write("\n")

for filename in reportList:
    entry = report[filename]

    out.write(filename[len(prefix):] + ":\n")
    
    extensionsTokens = entry[0]
    extensions = entry[1]
    tokens = entry[2]
    unknownTokens = entry[3]

    extensionsTokensList = extensionsTokens.keys()
    extensionsTokensList.sort()
    
    for extension in extensionsTokensList:
        out.write("\t" + extension + "\n")

        extensionTokens = extensionsTokens[extension]

        extensionTokensList = []

        for x in extensionTokens:
            extensionTokensList.append(x)

        extensionTokensList.sort()

        for extensionToken in extensionTokensList:
            out.write("\t\t" + extensionToken + "\n")
            
        out.write("\n")

    if unknownTokens:
        out.write("\tUnrecognized Tokens\n")

        for unknownToken in unknownTokens:
            out.write("\t\t" + unknownToken + "\n")

        out.write("\n")
        
out.close()
