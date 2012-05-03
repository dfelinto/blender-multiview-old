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

glToken = re.compile("(?:(?:gl|glu|glut|glew|glx|wgl)[A-Z][a-zA-Z0-9]*)|(?:(?:GL_|GLU_|GLUT_|GLEW_|GLX_|WGL_)[A-Z0-9_]*[A-Z0-9])")

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
        
        core = "OpenGL 1.1, GLU, GLUT, GLEW, or Unknown"

        for token in tokens:
            summaryTokens.add(token)

            if token in database:
                extensions.add(database[token])
                summaryExtensions.add(database[token])
            else:
                extensions.add(core)
        
        extensionsTokens = {}

        for token in tokens:
            for extension in extensions:
                if extension == core or (token in database and extension == database[token]):
                    if not extension in extensionsTokens:
                        extensionsTokens[extension] = []

                    extensionsTokens[extension].append(token)
                    

        report[filename] = (extensionsTokens, extensions, tokens)

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
            database[token] = basename


            
listFiles("./core", printDirectory, addDatabaseEntries, isSvn, isDummy)
listFiles("./extensions", printDirectory, addDatabaseEntries, isSvn, isDummy)

listFiles("../../..", printDirectory, addReportEntry, isSvn, isNotGLUserFile)

out = open("./report.txt", "w")

out.write("Summary:\n\n")
out.write("\tFiles that use OpenGL: " + str(summaryFileCount) + "\n")

reportList = report.keys()
reportList.sort()

for filename in reportList:
    out.write("\t\t" + filename + "\n")
    
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
        out.write("\t\t" + token + " (" + database[token] + ")\n")
    else:
        out.write("\t\t" + token + "\n")

out.write("\n")
out.write("Detailed Report:\n")
out.write("\n")

for filename in reportList:
    entry = report[filename]

    out.write(filename + ":\n")
    
    extensionsTokens = entry[0]
    extensions = entry[1]
    tokens = entry[2]

    extensionsTokensList = extensionsTokens.keys()
    extensionsTokensList.sort()
    
    for extension in extensionsTokensList:
        extensionTokens = extensionsTokens[extension]
        
        out.write("\t" + extension + "\n")

        extensionTokens.sort()

        for extensionToken in extensionTokens:
            out.write("\t\t" + extensionToken + "\n")
            
        out.write("\n")
        
out.close()
