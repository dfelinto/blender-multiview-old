# Make 500 random boxes, pack them and make a mesh from it
from Blender import Geometry, Scene, Mesh
import random
boxes = []
for i in xrange(500):
	boxes.append( [0,0, random.random()+0.1, random.random()+0.1] )
boxsize = Geometry.BoxPack2D(boxes)
print 'BoxSize', boxsize
me = Mesh.New()
for x in boxes:
	me.verts.extend([(x[0],x[1], 0), (x[0],x[1]+x[3], 0), (x[0]+x[2],x[1]+x[3], 0), (x[0]+x[2],x[1], 0) ])
	v1= me.verts[-1]
	v2= me.verts[-2]
	v3= me.verts[-3]
	v4= me.verts[-4]
	me.faces.extend([(v1,v2,v3,v4)])
scn = Scene.GetCurrent()
scn.objects.new(me)
 
