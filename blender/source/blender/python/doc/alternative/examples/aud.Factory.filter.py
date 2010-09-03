import aud
d = aud.device()
s = aud.Factory("soundfile.ogg")
# this are filter coefficients for a 4th order butterworth lowpass
# with a cut-off frequency of 400 Hz for a 44.1 kHz signal
b = (6.1265e-07, 2.4506e-06, 3.6759e-06, 2.4506e-06, 6.1265e-07)
a = (1.00000,-3.85109, 5.56425,-3.57477, 0.86162)
f = s.filter(b, a)
h = d.play(f)
