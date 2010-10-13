import GameLogic
co = GameLogic.getCurrentController()
cam = co.owner

# Test point [0.0, 0.0, 0.0]
if (cam.pointInsideFrustum([0.0, 0.0, 0.0])):
	# Point is inside frustum !
	# Do something useful !
	pass
else:
	# Box is outside the frustum !
	pass
 
