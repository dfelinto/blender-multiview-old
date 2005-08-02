#ifndef GL_DEBUG_DRAWER_H
#define GL_DEBUG_DRAWER_H

#include "IDebugDraw.h"

class GLDebugDrawer : public IDebugDraw
{
	int m_debugMode;

public:

	GLDebugDrawer();

	virtual void	DrawLine(const SimdVector3& from,const SimdVector3& to,const SimdVector3& color);

	virtual void	SetDebugMode(int debugMode);

};

#endif//GL_DEBUG_DRAWER_H
