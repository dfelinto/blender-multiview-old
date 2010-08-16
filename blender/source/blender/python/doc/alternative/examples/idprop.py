import bpy
C = bpy.context
C.object["Cube"] = 1,2,3
val = C.object["Cube"]
# val will be an IDArray