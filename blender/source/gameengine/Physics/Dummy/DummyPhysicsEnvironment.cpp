/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
#include "DummyPhysicsEnvironment.h"
#include "PHY_IMotionState.h"



DummyPhysicsEnvironment::DummyPhysicsEnvironment()
{
	// create physicsengine data
}



DummyPhysicsEnvironment::~DummyPhysicsEnvironment()
{
	//destroy physicsengine data
}

void DummyPhysicsEnvironment::proceed(double timeStep)
{
	//step physics simulation, typically perform
	
	//collision detection 
	//solve constraints
	//integrate solution

}

void DummyPhysicsEnvironment::setGravity(float x,float y,float z)
{
}







int			DummyPhysicsEnvironment::createConstraint(class PHY_IPhysicsController* ctrl,class PHY_IPhysicsController* ctrl2,PHY_ConstraintType type,
		float pivotX,float pivotY,float pivotZ,float axisX,float axisY,float axisZ)
{
	
	int constraintid = 0;
	return constraintid;

}

void		DummyPhysicsEnvironment::removeConstraint(int constraintid)
{
	if (constraintid)
	{
	}
}

PHY_IPhysicsController* DummyPhysicsEnvironment::rayTest(void* ignoreClient,float fromX,float fromY,float fromZ, float toX,float toY,float toZ, 
									float& hitX,float& hitY,float& hitZ,float& normalX,float& normalY,float& normalZ)
{
	//collision detection / raytesting
	return NULL;
}

