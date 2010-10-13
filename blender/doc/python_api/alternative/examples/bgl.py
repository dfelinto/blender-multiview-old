import Blender
from bgl import *
from Blender import Draw
R = G = B = 0
A = 1
title = "Testing BGL  + Draw"
instructions = "Use mouse buttons or wheel to change the background color."
quitting = " Press ESC or q to quit."
len1 = Draw.GetStringWidth(title)
len2 = Draw.GetStringWidth(instructions + quitting)

def show_win():
	glClearColor(R,G,B,A)                # define color used to clear buffers 
	glClear(GL_COLOR_BUFFER_BIT)         # use it to clear the color buffer
	glColor3f(0.35,0.18,0.92)            # define default color
	glBegin(GL_POLYGON)                  # begin a vertex data list
	glVertex2i(165, 158)
	glVertex2i(252, 55)
	glVertex2i(104, 128)
	glEnd()
	glColor3f(0.4,0.4,0.4)               # change default color
	glRecti(40, 96, 60+len1, 113)
	glColor3f(1,1,1)
	glRasterPos2i(50,100)                # move cursor to x = 50, y = 100
	Draw.Text(title)                     # draw this text there
	glRasterPos2i(350,40)                # move cursor again
	Draw.Text(instructions + quitting)   # draw another msg
	glBegin(GL_LINE_LOOP)                # begin a vertex-data list
	glVertex2i(46,92)
	glVertex2i(120,92)
	glVertex2i(120,115)
	glVertex2i(46,115)
	glEnd()                              # close this list

def ev(evt, val):                      # event callback for Draw.Register()
	global R,G,B,A                       # ... it handles input events
	if evt == Draw.ESCKEY or evt == Draw.QKEY:
		Draw.Exit()                        # this quits the script
	elif not val: return
	elif evt == Draw.LEFTMOUSE: R = 1 - R
	elif evt == Draw.MIDDLEMOUSE: G = 1 - G
	elif evt == Draw.RIGHTMOUSE: B = 1 - B
	elif evt == Draw.WHEELUPMOUSE:
		R += 0.1
		if R > 1: R = 1
	elif evt == Draw.WHEELDOWNMOUSE:
		R -= 0.1
		if R < 0: R = 0
	else:
		return                             # don't redraw if nothing changed
	Draw.Redraw(1)                       # make changes visible.

Draw.Register(show_win, ev, None)      # start the main loop
