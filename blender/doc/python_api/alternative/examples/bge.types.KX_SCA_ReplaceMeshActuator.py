# Level-of-detail
# Switch a game object's mesh based on its depth in the camera view.
# +----------+     +-----------+     +-------------------------------------+
# | Always   +-----+ Python    +-----+ Edit Object (Replace Mesh) LOD.Mesh |
# +----------+     +-----------+     +-------------------------------------+
import GameLogic

# List detail meshes here
# Mesh (name, near, far)
# Meshes overlap so that they don't 'pop' when on the edge of the distance.
meshes = ((".Hi", 0.0, -20.0),
		(".Med", -15.0, -50.0),
		(".Lo", -40.0, -100.0)
		)

co = GameLogic.getCurrentController()
obj = co.owner
act = co.actuators["LOD." + obj.name]
cam = GameLogic.getCurrentScene().active_camera

def Depth(pos, plane):
	return pos[0]*plane[0] + pos[1]*plane[1] + pos[2]*plane[2] + plane[3]

# Depth is negative and decreasing further from the camera
depth = Depth(obj.position, cam.world_to_camera[2])

newmesh = None
curmesh = None
# Find the lowest detail mesh for depth
for mesh in meshes:
	if depth < mesh[1] and depth > mesh[2]:
		newmesh = mesh
	if "ME" + obj.name + mesh[0] == act.getMesh():
		curmesh = mesh

if newmesh != None and "ME" + obj.name + newmesh[0] != act.getMesh():
	# The mesh is a different mesh - switch it.
	# Check the current mesh is not a better fit.
	if curmesh == None or curmesh[1] < depth or curmesh[2] > depth:
		act.mesh = obj.getName() + newmesh[0]
		GameLogic.addActiveActuator(act, True)

