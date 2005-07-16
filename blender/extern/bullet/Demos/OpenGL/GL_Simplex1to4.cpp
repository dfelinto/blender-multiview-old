/*
 * Copyright (c) 2005 Erwin Coumans <www.erwincoumans.com>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erwin Coumans makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 */
#include "GL_Simplex1to4.h"
#include "NarrowPhaseCollision/SimplexSolverInterface.h"
#include "GL_ShapeDrawer.h"
#include <windows.h>
#include <GL/gl.h>
#include "SimdTransform.h"

GL_Simplex1to4::GL_Simplex1to4()
:m_simplexSolver(0)
{
}

///
/// Debugging method CalcClosest calculates the closest point to the origin, using m_simplexSolver
///
void	GL_Simplex1to4::CalcClosest(float* m)
{
	SimdTransform tr;
	tr.setFromOpenGLMatrix(m);
	


			GL_ShapeDrawer::DrawCoordSystem();
			
			if (m_simplexSolver)
			{
				m_simplexSolver->reset();
				bool res;

				SimdVector3 v;
				SimdPoint3 pBuf[4];
				SimdPoint3 qBuf[4];
				SimdPoint3 yBuf[4];


				for (int i=0;i<m_numVertices;i++)
				{
					v =  tr(m_vertices[i]);
					m_simplexSolver->addVertex(v,v,SimdPoint3(0.f,0.f,0.f));
					res = m_simplexSolver->closest(v);
					int res = m_simplexSolver->getSimplex(pBuf, qBuf, yBuf);

				}


				//draw v?
				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				glColor3f(1.f, 0.f, 0.f);
				glVertex3f(0.f, 0.f, 0.f);
				glVertex3f(v.x(),v.y(),v.z());
				glEnd();
				
				glEnable(GL_LIGHTING);


			}

}
