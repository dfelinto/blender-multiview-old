import GameLogic
import OpenGL
from OpenGL.GL import *
from OpenGL.GLU import *
import glew
from glew import *

glewInit()

vertex_shader = """

void main(void)
{
	gl_Position = ftransform();
}
"""

fragment_shader ="""

void main(void)
{
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
"""

class MyMaterial:
	def __init__(self):
		self.pass_no = 0
		# Create a shader
		self.m_program = glCreateProgramObjectARB()
		# Compile the vertex shader
		self.shader(GL_VERTEX_SHADER_ARB, (vertex_shader))
		# Compile the fragment shader
		self.shader(GL_FRAGMENT_SHADER_ARB, (fragment_shader))
		# Link the shaders together
		self.link()
	
	def PrintInfoLog(self, tag, object):
		"""
		PrintInfoLog prints the GLSL compiler log
		"""
		print "Tag:    def PrintGLError(self, tag = ""):"
		
	def PrintGLError(self, tag = ""):
		"""
		Prints the current GL error status
		"""
		if len(tag):
			print tag
		err = glGetError()
		if err != GL_NO_ERROR:
			print "GL Error: %s\\n"%(gluErrorString(err))

	def shader(self, type, shaders):
		"""
		shader compiles a GLSL shader and attaches it to the current
		program.
		
		type should be either GL_VERTEX_SHADER_ARB or GL_FRAGMENT_SHADER_ARB
		shaders should be a sequence of shader source to compile.
		"""
		# Create a shader object
		shader_object = glCreateShaderObjectARB(type)

		# Add the source code
		glShaderSourceARB(shader_object, len(shaders), shaders)
		
		# Compile the shader
		glCompileShaderARB(shader_object)
		
		# Print the compiler log
		self.PrintInfoLog("vertex shader", shader_object)
		
		# Check if compiled, and attach if it did
		compiled = glGetObjectParameterivARB(shader_object, GL_OBJECT_COMPILE_STATUS_ARB)
		if compiled:
			glAttachObjectARB(self.m_program, shader_object)
			
		# Delete the object (glAttachObjectARB makes a copy)
		glDeleteObjectARB(shader_object)
		
		# print the gl error log
		self.PrintGLError()
	
	def link(self):
		"""
		Links the shaders together.
		"""
		# clear error indicator
		glGetError()
		
		glLinkProgramARB(self.m_program)

		self.PrintInfoLog("link", self.m_program)
		
		linked = glGetObjectParameterivARB(self.m_program, GL_OBJECT_LINK_STATUS_ARB)
		if not linked:
			print "Shader failed to link"
			return

		glValidateProgramARB(self.m_program)
		valid = glGetObjectParameterivARB(self.m_program, GL_OBJECT_VALIDATE_STATUS_ARB)
		if not valid:
			print "Shader failed to validate"
			return
	
	def activate(self, rasty, cachingInfo, mat):
		self.pass_no+=1
		if (self.pass_no == 1):
			glDisable(GL_COLOR_MATERIAL)
			glUseProgramObjectARB(self.m_program)
			return True
		
		glEnable(GL_COLOR_MATERIAL)
		glUseProgramObjectARB(0)
		self.pass_no = 0   
		return False

obj = GameLogic.getCurrentController().owner

mesh = obj.meshes[0]

for mat in mesh.materials:
	mat.setCustomMaterial(MyMaterial())
	print mat.texture
