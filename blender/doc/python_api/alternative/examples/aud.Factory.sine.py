import aud
d = aud.device()
s = aud.Factory.sine(440, d.rate)
h = d.play(s)
