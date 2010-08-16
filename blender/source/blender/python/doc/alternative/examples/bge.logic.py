# To get the controller thats running this python script:
cont = bge.logic.getCurrentController() # bge.logic is automatically imported

# To get the game object this controller is on:
obj = cont.owner

# To get a sensor linked to this controller.
# "sensorname" is the name of the sensor as defined in the Blender interface.
# +---------------------+  +--------+
# | Sensor "sensorname" +--+ Python +
# +---------------------+  +--------+
sens = cont.sensors["sensorname"]

# To get a sequence of all sensors:
sensors = co.sensors

# To get an actuator attached to the controller:
#                          +--------+  +-------------------------+
#                          + Python +--+ Actuator "actuatorname" |
#                          +--------+  +-------------------------+
actuator = co.actuators["actuatorname"]

# Activate an actuator
controller.activate(actuator)

# Get the current scene
scene = bge.logic.getCurrentScene()

# Get the current camera
cam = scene.active_camera
