from bgl import *
xval, yval= 100, 40
# Get the scale of the view matrix
viewMatrix = Buffer(GL_FLOAT, 16)
glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix)
f = 1/viewMatrix[0]
glRasterPos2f(xval*f, yval*f) # Instead of the usual glRasterPos2i(xval, yval)
