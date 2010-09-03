import aud
d = aud.device()

s1 = aud.Factory.sine(440, d.rate)
s2 = aud.Factory.sine(220, d.rate)
# this sine factory has a different sampling rate
s3 = aud.Factory.sine(880, d.rate / 2)

j1 = s1.join(s2)
j2 = s1.join(s3)

# this will work:
h1 = d.play(j1)
# this will fail:
h2 = d.play(j2)
