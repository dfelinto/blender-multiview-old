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

#include "BL_ArmatureObject.h"
#include "BL_ActionActuator.h"
#include "BLI_blenlib.h"
#include "BKE_action.h"
#include "BKE_armature.h"
#include "GEN_Map.h"
#include "GEN_HashedPtr.h"
#include "MEM_guardedalloc.h"
#include "DNA_action_types.h"

CValue* BL_ArmatureObject::GetReplica()
{
	BL_ArmatureObject* replica = new BL_ArmatureObject(*this);
	
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);

	ProcessReplica(replica);
	return replica;
}

void BL_ArmatureObject::ProcessReplica(BL_ArmatureObject *replica)
{
	KX_GameObject::ProcessReplica(replica);

}

BL_ArmatureObject::~BL_ArmatureObject()
{
	if (m_mrdPose){
		clear_pose(m_mrdPose);
		MEM_freeN(m_mrdPose);
	}
}

void BL_ArmatureObject::ApplyPose()
{
	if (m_pose){
		apply_pose_armature(m_armature, m_pose, 1);
		if (!m_mrdPose)
			copy_pose (&m_mrdPose, m_pose, 0);
		else
			get_pose_from_pose(&m_mrdPose, m_pose);
	}
}

void BL_ArmatureObject::SetPose(bPose *pose)
{
	m_pose = pose;
}

bool BL_ArmatureObject::SetActiveAction(BL_ActionActuator *act, short priority, double curtime)
{
	if (curtime != m_lastframe){
		m_activePriority = 9999;
		m_lastframe= curtime;
		m_activeAct = NULL;
	}

	if (priority<=m_activePriority)
	{
		if (m_activeAct && (m_activeAct!=act))
			m_activeAct->SetBlendTime(0.0);	/* Reset the blend timer */
		m_activeAct = act;
		m_activePriority = priority;
		m_lastframe = curtime;
	
		return true;
	}
	else{
		act->SetBlendTime(0.0);
		return false;
	}
	
}

BL_ActionActuator * BL_ArmatureObject::GetActiveAction()
{
	return m_activeAct;
}

void BL_ArmatureObject::GetPose(bPose **pose)
{
	/* If the caller supplies a null pose, create a new one. */
	/* Otherwise, copy the armature's pose channels into the caller-supplied pose */
	if (!*pose)
		copy_pose(pose, m_pose, 0);
	else
		get_pose_from_pose(pose, m_pose);

}

void BL_ArmatureObject::GetMRDPose(bPose **pose)
{
	/* If the caller supplies a null pose, create a new one. */
	/* Otherwise, copy the armature's pose channels into the caller-supplied pose */

	if (!m_mrdPose){
		copy_pose (&m_mrdPose, m_pose, 0);
	}

	if (!*pose)
		copy_pose(pose, m_mrdPose, 0);
	else
		get_pose_from_pose(pose, m_mrdPose);

}

short BL_ArmatureObject::GetActivePriority()
{
	return m_activePriority;
}

double BL_ArmatureObject::GetLastFrame()
{
	return m_lastframe;
}
