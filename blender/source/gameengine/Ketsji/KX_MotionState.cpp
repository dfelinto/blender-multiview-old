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
#include "KX_MotionState.h"
#include "SG_Spatial.h"

KX_MotionState::KX_MotionState(SG_Spatial* node) : m_node(node)
{

}

KX_MotionState::~KX_MotionState()
{
}

void	KX_MotionState::getWorldPosition(float& posX,float& posY,float& posZ)
{
	MT_Point3 pos = m_node->GetWorldPosition();
	posX = pos[0];
	posY = pos[1];
	posZ = pos[2];
}

void	KX_MotionState::getWorldScaling(float& scaleX,float& scaleY,float& scaleZ)
{
	MT_Vector3 scale = m_node->GetWorldScaling();
	scaleX = scale[0];
	scaleY = scale[1];
	scaleZ = scale[2];
}

void	KX_MotionState::getWorldOrientation(float& quatIma0,float& quatIma1,float& quatIma2,float& quatReal)
{
	MT_Quaternion orn = m_node->GetWorldOrientation().getRotation();
	quatIma0 = orn[0];
	quatIma1 = orn[1];
	quatIma2 = orn[2];
	quatReal = orn[3];
}
	
void	KX_MotionState::setWorldPosition(float posX,float posY,float posZ)
{
	m_node->SetLocalPosition(MT_Point3(posX,posY,posZ));

}

void	KX_MotionState::setWorldOrientation(float quatIma0,float quatIma1,float quatIma2,float quatReal)
{
	MT_Quaternion orn;
	orn[0] = quatIma0;
	orn[1] = quatIma1;
	orn[2] = quatIma2;
	orn[3] = quatReal;

	m_node->SetLocalOrientation(orn);
}

void	KX_MotionState::calculateWorldTransformations()
{
	m_node->ComputeWorldTransforms(NULL);
}

 
