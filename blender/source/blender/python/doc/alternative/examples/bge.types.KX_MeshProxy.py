import bge.logic

co = bge.logic.getCurrentController()
obj = co.owner

m_i = 0
mesh = obj.getMesh(m_i) # There can be more than one mesh...
while mesh != None:
	for mat in range(mesh.getNumMaterials()):
		for v_index in range(mesh.getVertexArrayLength(mat)):
			vertex = mesh.getVertex(mat, v_index)
			# Do something with vertex here...
			# ... eg: colour the vertex red.
			vertex.colour = [1.0, 0.0, 0.0, 1.0]
	m_i += 1
	mesh = obj.getMesh(m_i)
 
