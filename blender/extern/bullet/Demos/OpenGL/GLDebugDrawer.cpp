
#include "GLDebugDrawer.h"
#ifdef WIN32 //needed for glut.h
#include <windows.h>
#endif
#include <GL/glut.h>

GLDebugDrawer::GLDebugDrawer()
:m_debugMode(0)
{

}
void	GLDebugDrawer::DrawLine(const SimdVector3& from,const SimdVector3& to,const SimdVector3& color)
{
	if (m_debugMode > 0)
	{
		glBegin(GL_LINES);
		glColor3f(color.getX(), color.getY(), color.getZ());
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
		glEnd();
	}
}

void	GLDebugDrawer::SetDebugMode(int debugMode)
{
	m_debugMode = debugMode;

}
