import aud
d = aud.device()
s = aud.Factory('sound.ogg')

# this:
f = s.pingpong()
# is the same as this:
f = s.join(s.reverse())

h = d.play(f)
