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

#ifdef WIN32 //needed for glut.h
#include <windows.h>
#endif
#include <GL/glut.h>
#include "GL_ShapeDrawer.h"
#include "CollisionShapes/PolyhedralConvexShape.h"
#include "CollisionShapes/TriangleMeshShape.h"

#include "BMF_Api.h"
#include <stdio.h> //printf debugging

void GL_ShapeDrawer::DrawCoordSystem()  {
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
	
}




class GlDrawcallback : public TriangleCallback
{
public:

	virtual void ProcessTriangle(SimdVector3* triangle)
	{
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
		glColor3f(0, 1, 0);
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
		glColor3f(0, 0, 1);
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
		glEnd();

	}

};

void GL_ShapeDrawer::DrawOpenGL(float* m, const CollisionShape* shape, const SimdVector3& color)
{
	glPushMatrix(); 
    glLoadMatrixf(m);

    //DrawCoordSystem();
    
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(color.x(),color.y(), color.z());

	glRasterPos3f(0.0,  0.0,  0.0);

	/// for polyhedral shapes
	if (shape->IsPolyhedral())
	{
		PolyhedralConvexShape* polyshape = (PolyhedralConvexShape*) shape;
		
		if (polyshape->GetExtraDebugInfo())
		{
			BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),polyshape->GetExtraDebugInfo());
		} else
		{
			BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),polyshape->GetName());
		}

		glBegin(GL_LINES);


		int i;
		for (i=0;i<polyshape->GetNumEdges();i++)
		{
			SimdPoint3 a,b;
			polyshape->GetEdge(i,a,b);

			glVertex3f(a.getX(),a.getY(),a.getZ());
			glVertex3f(b.getX(),b.getY(),b.getZ());


		}
		glEnd();

		glColor3f(1.f, 1.f, 1.f);
		for (i=0;i<polyshape->GetNumVertices();i++)
		{
			SimdPoint3 vtx;
			polyshape->GetVertex(i,vtx);
			glRasterPos3f(vtx.x(),  vtx.y(),  vtx.z());
			char buf[12];
			sprintf(buf," %d",i);
			BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
		}

		for (i=0;i<polyshape->GetNumPlanes();i++)
		{
			SimdVector3 normal;
			SimdPoint3 vtx;
			polyshape->GetPlane(normal,vtx,i);
			SimdScalar d = vtx.dot(normal);

			glRasterPos3f(normal.x()*d,  normal.y()*d, normal.z()*d);
			char buf[12];
			sprintf(buf," plane %d",i);
			BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
			
		}
	}
	if (shape->IsConcave())
	{
		TriangleMeshShape* concaveMesh = (TriangleMeshShape*) shape;
		SimdVector3 aabbMax(1e30f,1e30f,1e30f);
		GlDrawcallback drawCallback;

		concaveMesh->ProcessAllTriangles(&drawCallback,-aabbMax,aabbMax);


	}

	//glScaled(2*m_halfExtent[0], 2*m_halfExtent[1], 2*m_halfExtent[2]);
    //glutSolidCube(1.0);
    
	glPopMatrix();
    glPopMatrix();
	
}
