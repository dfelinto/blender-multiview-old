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

#ifndef BL_ARMATUREOBJECT
#define BL_ARMATUREOBJECT

#include "KX_GameObject.h"

#include "SG_IObject.h"

class BL_ActionActuator;

class BL_ArmatureObject : public KX_GameObject  
{
public:
	double GetLastFrame ();
	short GetActivePriority();
	virtual void ProcessReplica(BL_ArmatureObject *replica);
	class BL_ActionActuator * GetActiveAction();
	BL_ArmatureObject(void* sgReplicationInfo, SG_Callbacks callbacks,
		struct bArmature *arm,
		struct bPose *pose) :
	KX_GameObject(sgReplicationInfo,callbacks),
		m_pose(pose),
		m_mrdPose(NULL),
		m_armature(arm),
		m_activeAct(NULL),
		m_activePriority(999)
	{}

	virtual CValue*		GetReplica();
	virtual				~BL_ArmatureObject();
	void GetMRDPose(bPose **pose);
	void	GetPose(struct bPose **pose);
	void SetPose (struct bPose *pose);
	void ApplyPose();
	bool SetActiveAction(class BL_ActionActuator *act, short priority, double curtime);
	struct bArmature * GetArmature(){return m_armature;};

protected:
	struct bArmature	*m_armature;
	struct bPose		*m_pose;
	struct bPose		*m_mrdPose;
	double	m_lastframe;
	class BL_ActionActuator *m_activeAct;
	short	m_activePriority;
};

#endif

