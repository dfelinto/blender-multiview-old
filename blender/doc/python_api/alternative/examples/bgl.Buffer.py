import bgl
myByteBuffer = bgl.Buffer(bgl.GL_BYTE, [32,32])
bgl.glGetPolygonStipple(myByteBuffer)
print(myByteBuffer.dimensions)
print(myByteBuffer.list)
sliceBuffer = myByteBuffer[0:16]
print(sliceBuffer)
