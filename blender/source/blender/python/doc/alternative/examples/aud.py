import aud
device = aud.device()
s = aud.Factory('music.ogg')
c = device.play(s)
buffered = aud.Factory.buffer(s)
c2 = device.play(buffered)
c.stop()
c2.stop() 
