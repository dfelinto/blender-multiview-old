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
#include "SumoPhysicsEnvironment.h"
#include "PHY_imotionstate.h"
#include "SumoPhysicsController.h"
#include "SM_Scene.h"


const MT_Scalar UpperBoundForFuzzicsIntegrator = 0.01;
// At least 100Hz (isn't this CPU hungry ?)


SumoPhysicsEnvironment::SumoPhysicsEnvironment()
{
	// seperate collision scene for events
	m_solidScene = DT_CreateScene();
	m_respTable = DT_CreateRespTable();

	m_sumoScene = new SM_Scene();
	m_sumoScene->setSecondaryRespTable(m_respTable);
	
}



SumoPhysicsEnvironment::~SumoPhysicsEnvironment()
{
	delete m_sumoScene;

	DT_DeleteScene(m_solidScene);
	DT_DeleteRespTable(m_respTable);
}

void SumoPhysicsEnvironment::proceed(double timeStep)
{
	m_sumoScene->proceed(timeStep,UpperBoundForFuzzicsIntegrator);
}

void SumoPhysicsEnvironment::setGravity(float x,float y,float z)
{
	m_sumoScene->setForceField(MT_Vector3(x,y,z));

}





int			SumoPhysicsEnvironment::createConstraint(class PHY_IPhysicsController* ctrl,class PHY_IPhysicsController* ctrl2,PHY_ConstraintType type,
		float pivotX,float pivotY,float pivotZ,float axisX,float axisY,float axisZ)
{

	int constraintid = 0;
	return constraintid;

}

void		SumoPhysicsEnvironment::removeConstraint(int constraintid)
{
	if (constraintid)
	{
	}
}

PHY_IPhysicsController* SumoPhysicsEnvironment::rayTest(void* ignoreClient, float fromX,float fromY,float fromZ, float toX,float toY,float toZ, 
									float& hitX,float& hitY,float& hitZ,float& normalX,float& normalY,float& normalZ)
{
	//collision detection / raytesting
	//m_sumoScene->rayTest(ignoreclient,from,to,result,normal);

	return NULL;
}


