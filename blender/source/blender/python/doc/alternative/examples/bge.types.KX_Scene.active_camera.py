# Get the depth of an object in the camera view.
import GameLogic

obj = GameLogic.getCurrentController().owner
cam = GameLogic.getCurrentScene().active_camera

# Depth is negative and decreasing further from the camera
depth = obj.position[0]*cam.world_to_camera[2][0] + obj.position[1]*cam.world_to_camera[2][1] + obj.position[2]*cam.world_to_camera[2][2] + cam.world_to_camera[2][3]
 
