class PyMaterial:
	def __init__(self):
		self.pass_no = -1

	def activate(self, rasty, cachingInfo, material):
		# Activate the material here.
		#
		# The activate method will be called until it returns False.
		# Every time the activate method returns True the mesh will
		# be rendered.
		#
		# rasty is a CObject for passing to material.updateTexture() 
		#       and material.activate()
		# cachingInfo is a CObject for passing to material.activate()
		# material is the KX_PolygonMaterial instance this material
		#          was added to
		
		# default material properties:
		self.pass_no += 1
		if self.pass_no == 0:
			material.activate(rasty, cachingInfo)
			# Return True to do this pass
			return True
		
		# clean up and return False to finish.
		self.pass_no = -1
		return False

# Create a new Python Material and pass it to the renderer.
mat.setCustomMaterial(PyMaterial())
