import aud
import time

def crossfade(device, handle, source, target, time, loop = -1):
	device.lock()
	position = handle.position
	handle.stop()
	device.play(source.limit(position,
	            position+time).fadeout(0, time))
	handle = device.play(target.loop(loop).fadein(0, time))
	device.unlock()
	return handle

source = aud.Factory.sine(440)
target = aud.Factory.sine(390)

d = aud.device()

h = d.play(source)
time.sleep(5)
h = crossfade(d, h, source, target, 5)
