# Set a connected keyboard sensor to accept F1
import bge

co = bge.logic.getCurrentController()
# 'Keyboard' is a keyboard sensor
sensor = co.sensors["Keyboard"]
sensor.key = bge.keys.F1KEY
 
