import GameLogic

# get the scene
scene = GameLogic.getCurrentScene()

# print all the objects in the scene
for obj in scene.objects:
	print obj.name

# get an object named 'Cube'
obj = scene.objects["Cube"]

# get the first object in the scene.
obj = scene.objects[0]
 
