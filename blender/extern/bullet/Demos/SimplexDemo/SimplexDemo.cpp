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

/*
	SimplexDemo demonstrated the working of the subdistance algorithm as used in GJK.
	It draws the simplex, and calculates the closest vector from simplex to the origin
*/

#include "GL_Simplex1to4.h"
#include "SimdQuaternion.h"
#include "SimdTransform.h"
#include "GL_ShapeDrawer.h"
#include <GL/glut.h>
#include "GlutStuff.h"



#include "NarrowPhaseCollision/VoronoiSimplexSolver.h"

VoronoiSimplexSolver	simplexSolver;



float yaw=0.f,pitch=0.f,roll=0.f;
const int maxNumObjects = 4;
const int numObjects = 1;
int screenWidth = 640.f;
int screenHeight = 480.f;
/// simplex contains the vertices, and some extra code to draw and debug
GL_Simplex1to4	simplex;


PolyhedralConvexShape*	shapePtr[maxNumObjects];


///
///
///
int main(int argc,char** argv)
{

	simplex.SetSimplexSolver(&simplexSolver);

	simplex.AddVertex(SimdPoint3(-2,0,-2));
	simplex.AddVertex(SimdPoint3(2,0,-2));
	simplex.AddVertex(SimdPoint3(0,0,2));
	simplex.AddVertex(SimdPoint3(0,2,0));

	shapePtr[0] = &simplex;

	SimdTransform tr;
	tr.setIdentity();

	return glutmain(argc, argv,screenWidth,screenHeight,"SimplexDemo");
}

//to be implemented by the demo

void clientMoveAndDisplay()
{
	
	clientDisplay();
}



void clientDisplay(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glDisable(GL_LIGHTING);

	GL_ShapeDrawer::DrawCoordSystem();

	float m[16];
	int i;

	for (i=0;i<numObjects;i++)
	{
		SimdTransform transA;
		transA.setIdentity();
		SimdVector3	dpos(0.f,5.f,0.f);
		transA.setOrigin( dpos );
		SimdQuaternion orn;
		orn.setEuler(yaw,pitch,roll);
		transA.setRotation(orn);
		transA.getOpenGLMatrix( m );

		/// draw the simplex
		GL_ShapeDrawer::DrawOpenGL(m,shapePtr[i],SimdVector3(1,1,1));

		/// calculate closest point from simplex to the origin, and draw this vector
		simplex.CalcClosest(m);

	}
	pitch += 0.005f;
	yaw += 0.01f;

	glFlush();
    glutSwapBuffers();
}
