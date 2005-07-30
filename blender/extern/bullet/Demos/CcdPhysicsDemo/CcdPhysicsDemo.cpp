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

#include "CcdPhysicsEnvironment.h"
#include "CcdPhysicsController.h"
#include "MyMotionState.h"
//#include "GL_LineSegmentShape.h"
#include "CollisionShapes/BoxShape.h"
#include "CollisionShapes/Simplex1to4Shape.h"
#include "Dynamics/RigidBody.h"
#include "ConstraintSolver/SimpleConstraintSolver.h"
#include "ConstraintSolver/OdeConstraintSolver.h"
#include "CollisionDispatch/ToiContactDispatcher.h"
#include "BroadphaseCollision/SimpleBroadphase.h"
#include "IDebugDraw.h"
#include "GLDebugDrawer.h"

#include "PHY_Pro.h"


#include <GL/glut.h>
#include "GL_ShapeDrawer.h"

#include "GlutStuff.h"
	

const int numObjects = 22;

const int maxNumObjects = 100;
MyMotionState ms[maxNumObjects];
CcdPhysicsController* physObjects[maxNumObjects] = {0,0,0,0};
int	shapeIndex[maxNumObjects];
CcdPhysicsEnvironment* physicsEnvironmentPtr = 0;


//GL_LineSegmentShape shapeE(SimdPoint3(-50,0,0),
//						   SimdPoint3(50,0,0));
PolyhedralConvexShape* shapePtr[4] = 
{
	new BoxShape (SimdVector3(100,10,100)),
	new BoxShape (SimdVector3(2,2,2)),
	new BU_Simplex1to4(SimdPoint3(-2,-2,-2),SimdPoint3(2,-2,-2),SimdPoint3(-2,2,-2),SimdPoint3(0,0,2)),

	new BoxShape (SimdVector3(1,3,1))

};

int main(int argc,char** argv)
{
	GLDebugDrawer	debugDrawer;

	//ConstraintSolver* solver = new SimpleConstraintSolver;
	ConstraintSolver* solver = new OdeConstraintSolver;

	ToiContactDispatcher* dispatcher = new	ToiContactDispatcher(solver);
		
	BroadphaseInterface* broadphase = new SimpleBroadphase();


	physicsEnvironmentPtr = new CcdPhysicsEnvironment(dispatcher,broadphase);
	

	physicsEnvironmentPtr->setGravity(0,-10,0);
	PHY_ShapeProps shapeProps;
	
	shapeProps.m_do_anisotropic = false;
	shapeProps.m_do_fh = false;
	shapeProps.m_do_rot_fh = false;
	shapeProps.m_friction_scaling[0] = 1.;
	shapeProps.m_friction_scaling[1] = 1.;
	shapeProps.m_friction_scaling[2] = 1.;

	shapeProps.m_inertia = 1.f;
	shapeProps.m_lin_drag = 0.95999998f;
	shapeProps.m_ang_drag = 0.89999998f;
	shapeProps.m_mass = 1.0f;
	
	PHY_MaterialProps materialProps;
	materialProps.m_friction = 10.5f;
	materialProps.m_restitution = 0.0f;

	CcdConstructionInfo ccdObjectCi;
	ccdObjectCi.m_friction = 10.5f;

	ccdObjectCi.m_linearDamping = shapeProps.m_lin_drag;
	ccdObjectCi.m_angularDamping = shapeProps.m_ang_drag;

	SimdTransform tr;
	tr.setIdentity();

	int i;
	for (i=0;i<numObjects;i++)
	{
		if (i>0)
			shapeIndex[i] = 1;
		else
			shapeIndex[i] = 0;
	}
	for (i=0;i<numObjects;i++)
	{
		shapeProps.m_shape = shapePtr[shapeIndex[i]];

		bool isDyna = i>0;
		
		if (0)//i==1)
		{
			SimdQuaternion orn(0,0,0.1*SIMD_HALF_PI);
			ms[i].setWorldOrientation(orn.x(),orn.y(),orn.z(),orn[3]);
		}
		
		ms[i].setWorldPosition(-15+(i*5) % 30,i*15-10,0);

		ccdObjectCi.m_MotionState = &ms[i];
		ccdObjectCi.m_gravity = SimdVector3(0,0,0);
		ccdObjectCi.m_localInertiaTensor =SimdVector3(0,0,0);
		if (!isDyna)
		{
			shapeProps.m_mass = 0.f;
			ccdObjectCi.m_mass = shapeProps.m_mass;
		}
		else
		{
			shapeProps.m_mass = 1.f;
			ccdObjectCi.m_mass = shapeProps.m_mass;
		}

		
		SimdVector3 localInertia;
		shapePtr[shapeIndex[i]]->CalculateLocalInertia(shapeProps.m_mass,localInertia);
		ccdObjectCi.m_localInertiaTensor = localInertia;

		ccdObjectCi.m_collisionShape = shapePtr[shapeIndex[i]];

		ccdObjectCi.m_broadphaseHandle = 0;

		physObjects[i]= new CcdPhysicsController( ccdObjectCi);
		physicsEnvironmentPtr->addCcdPhysicsController( physObjects[i]);

		if (i==1)
		{
			//physObjects[i]->SetAngularVelocity(0,0,-2,true);
		}

		physicsEnvironmentPtr->setDebugDrawer(&debugDrawer);
		debugDrawer.SetDebugMode(1);

		
	}
	return glutmain(argc, argv,640,480,"GJK Demo");
}

//to be implemented by the demo

void renderme()
{
	float m[16];
	int i;

	for (i=0;i<numObjects;i++)
	{
		SimdTransform transA;
		transA.setIdentity();
		
		float pos[3];
		float rot[4];

		ms[i].getWorldPosition(pos[0],pos[1],pos[2]);
		ms[i].getWorldOrientation(rot[0],rot[1],rot[2],rot[3]);

		SimdQuaternion q(rot[0],rot[1],rot[2],rot[3]);
		transA.setRotation(q);

		SimdPoint3 dpos;
		dpos.setValue(pos[0],pos[1],pos[2]);

		transA.setOrigin( dpos );
		transA.getOpenGLMatrix( m );
		
		SimdVector3 wireColor(0.f,0.f,1.f); //wants deactivation

		///color differently for active, sleeping, wantsdeactivation states
		if (physObjects[i]->GetRigidBody()->GetActivationState() == 1) //active
		{
			wireColor = SimdVector3 (1.f,0.f,0.f);
		}
		if (physObjects[i]->GetRigidBody()->GetActivationState() == 2) //ISLAND_SLEEPING
		{
			wireColor = SimdVector3 (0.f,1.f,0.f);
		}

		char	extraDebug[128];
		sprintf(extraDebug,"islId, Body=%i , %i",physObjects[i]->GetRigidBody()->m_islandTag1,physObjects[i]->GetRigidBody()->m_debugBodyId);
		shapePtr[shapeIndex[i]]->SetExtraDebugInfo(extraDebug);
		GL_ShapeDrawer::DrawOpenGL(m,shapePtr[shapeIndex[i]],wireColor);
	}

}
void clientMoveAndDisplay()
{
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	float deltaTime = 1.f/60.f;

	physicsEnvironmentPtr->proceedDeltaTime(0.f,deltaTime);
	
	renderme();

    glFlush();
    glutSwapBuffers();

}




void clientDisplay(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	renderme();

    glFlush();
    glutSwapBuffers();
}


