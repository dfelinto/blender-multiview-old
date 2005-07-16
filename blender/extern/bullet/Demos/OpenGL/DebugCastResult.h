#ifndef DEBUG_CAST_RESULT_H
#define DEBUG_CAST_RESULT_H

#include "NarrowPhaseCollision/ConvexCast.h"
#include "SimdTransform.h"
#include "GL_ShapeDrawer.h"
#include <windows.h>
#include <GL/gl.h>
struct DebugCastResult : public ConvexCast::CastResult
{

	SimdTransform	m_fromTrans;
	const PolyhedralConvexShape* m_shape;
	SimdVector3	m_linVel;
	SimdVector3 m_angVel;

	DebugCastResult(const SimdTransform& fromTrans,const PolyhedralConvexShape* shape,
					const SimdVector3& linVel,const SimdVector3& angVel)
	:m_fromTrans(fromTrans),
	m_shape(shape),
	m_linVel(linVel),
	m_angVel(angVel)
	{
	}

	virtual void DrawCoordSystem(const SimdTransform& tr)  
	{
		float m[16];
		tr.getOpenGLMatrix(m);
		glPushMatrix();
		glLoadMatrixf(m);
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(1, 0, 0);
		glColor3f(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 1, 0);
		glColor3f(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 1);
		glEnd();
		glPopMatrix();
	}

	virtual void	DebugDraw(SimdScalar	fraction)
	{
	
		float m[16];
		SimdTransform hitTrans;
		SimdTransformUtil::IntegrateTransform(m_fromTrans,m_linVel,m_angVel,fraction,hitTrans);
		hitTrans.getOpenGLMatrix(m);
		GL_ShapeDrawer::DrawOpenGL(m,m_shape,SimdVector3(1,0,0));
	
	}
};


#endif //DEBUG_CAST_RESULT_H
