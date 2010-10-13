# Example Uses an L{SCA_MouseSensor}, and two L{KX_ObjectActuator}s to implement MouseLook::
# To use a mouse movement sensor "Mouse" and a 
# motion actuator to mouse look:
import bge.render
import bge.logic

# SCALE sets the speed of motion
SCALE=[1, 0.5]

co = bge.logic.getCurrentController()
obj = co.getOwner()
mouse = co.getSensor("Mouse")
lmotion = co.getActuator("LMove")
wmotion = co.getActuator("WMove")

# Transform the mouse coordinates to see how far the mouse has moved.
def mousePos():
	x = (bge.render.getWindowWidth()/2 - mouse.getXPosition())*SCALE[0]
	y = (bge.render.getWindowHeight()/2 - mouse.getYPosition())*SCALE[1]
	return (x, y)

pos = mousePos()

# Set the amount of motion: X is applied in world coordinates...
lmotion.setTorque(0.0, 0.0, pos[0], False)
# ...Y is applied in local coordinates
wmotion.setTorque(-pos[1], 0.0, 0.0, True)

# Activate both actuators
bge.logic.addActiveActuator(lmotion, True)
bge.logic.addActiveActuator(wmotion, True)

# Centre the mouse
bge.render.setMousePosition(bge.render.getWindowWidth()/2, bge.render.getWindowHeight()/2)
 
