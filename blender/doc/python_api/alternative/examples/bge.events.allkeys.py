# Do the all keys thing
import bge

co = bge.logic.getCurrentController()
# 'Keyboard' is a keyboard sensor
sensor = co.sensors["Keyboard"]

for key,status in sensor.events:
	# key[0] == bge.keys.keycode, key[1] = status
	if status == bge.logic.KX_INPUT_JUST_ACTIVATED:
		if key == bge.keys.WKEY:
			# Activate Forward!
			pass
		if key == bge.keys.SKEY:
			# Activate Backward!
			pass
		if key == bge.keys.AKEY:
			# Activate Left!
			pass
		if key == bge.keys.DKEY:
			# Activate Right!
			pass
