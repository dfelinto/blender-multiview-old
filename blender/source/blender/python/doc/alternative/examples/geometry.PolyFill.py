# This example creates 2 polylines and fills them in with faces,
# then makes a mesh in the current scene::
import geometry
from mathutils import Vector

# Outline of 5 points
polyline1= [Vector((-2.0, 1.0, 1.0)), Vector((-1.0, 2.0, 1.0)),
            Vector((1.0, 2.0, 1.0)), Vector((1.0, -1.0, 1.0)),
            Vector((-1.0, -1.0, 1.0))]
polyline2= [Vector((-1, 1, 1.0)), Vector((0, 1, 1.0)),
            Vector((0, 0, 1.0)), Vector((-1.0, 0.0, 1.0))]
fill= geometry.PolyFill([polyline1, polyline2])

# Make a new mesh and add the truangles into it
me= Blender.Mesh.New()
me.verts.extend(polyline1)
me.verts.extend(polyline2)

# Add the faces, they reference the verts in polyline 1 and 2
me.faces.extend(fill)

scn = Blender.Scene.GetCurrent()
ob = scn.objects.new(me)
Blender.Redraw()
