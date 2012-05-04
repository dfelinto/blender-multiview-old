OpenGL Tools

OpenGL Report Generator

The script glreport.py will generate a file named report.txt
which contains information about the OpenGL symbols used by
Blender and what OpenGL version and extensions are required.

The contents of "core" and "extensions" was copied from
the GLEW distribution.  The other directories have been
created from other sources such as the OpenGL registry.

Unfortunately the script does not have a way to catch usage
of extensions that do not introduce new tokens.

To-Do:

Create a list of suspect symbols and forbidden extensions
