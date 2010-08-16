import GameLogic
co = GameLogic.getCurrentController()
cam = co.owner

# A sphere of radius 4.0 located at [x, y, z] = [1.0, 1.0, 1.0]
if (cam.sphereInsideFrustum([1.0, 1.0, 1.0], 4) != cam.OUTSIDE):
	# Sphere is inside frustum !
	# Do something useful !
	pass
else:
	# Sphere is outside frustum
	pass
 
