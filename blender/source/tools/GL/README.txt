OpenGL Tools

OpenGL Report Generator

The script glreport.py will generate a file named report.txt
which contains information about the OpenGL symbols used by
Blender and what OpenGL version and extensions are required.

The contents of "core" and "extensions" was copied from
the GLEW distribution.

There are some limitations currently, for example the script
assumes that each symbol belongs to just one version or
extension.  It also cannot catch any extensions that do not
introduce new symbols.

To-Do:

Fix the limitations above
List extension strings along with symbols
Add symbols for OpenGL ES
Create a list of suspect symbols and forbidden extensions
