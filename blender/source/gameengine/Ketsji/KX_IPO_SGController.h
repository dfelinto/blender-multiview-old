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
#ifndef __IPO_SGCONTROLLER_H
#define __IPO_SGCONTROLLER_H

#include "SG_Controller.h"
#include "SG_Spatial.h"

#include "KX_IPOTransform.h"
#include "KX_IInterpolator.h"

class KX_IpoSGController : public SG_Controller
{
	KX_IPOTransform     m_ipo_xform;
	T_InterpolatorList  m_interpolators;
	/* Why not bools? */
	short               m_modify_position	 : 1;
	short               m_modify_orientation : 1;
	short               m_modify_scaling     : 1;

	/** Interpret the ipo as a force rather than a displacement? */
	bool                m_ipo_as_force;

	/** Ipo-as-force acts in local rather than in global coordinates? */
	bool                m_force_ipo_acts_local;

	/** Were settings altered since the last update? */
	bool				m_modified;

	/** Local time of this ipo.*/
	double		        m_ipotime;

	/** A reference to the sm scene an eventually associated physics object is in. */
//	class SM_Scene*     m_sumo_scene;

	/** A reference an eventually associated physics object is in. */
	class SM_Object*     m_sumo_object;

	/** A reference to the original game object. */
	class KX_GameObject* m_game_object;

public:
	KX_IpoSGController();

	virtual ~KX_IpoSGController();

	virtual	SG_Controller*	GetReplica(class SG_Node* destnode);

		void
	SetOption(
		int option,
		int value
	);

	/** Set sumo data. */
	void UpdateSumoReference();
	/** Set reference to the corresponding game object. */
	void SetGameObject(class KX_GameObject*);

	void	SetModifyPosition(bool modifypos) {	
		m_modify_position=modifypos;
	}
	void	SetModifyOrientation(bool modifyorient) {	
		m_modify_orientation=modifyorient;
	}
	void	SetModifyScaling(bool modifyscale) {	
		m_modify_scaling=modifyscale;
	}
	
	KX_IPOTransform& GetIPOTransform()
	{
		return m_ipo_xform;
	}
	void	AddInterpolator(KX_IInterpolator* interp);
	virtual bool Update(double time);
	virtual void	SetSimulatedTime(double time)
	{
		m_ipotime = time;
		m_modified = true;
	}
};
#endif //__IPO_SGCONTROLLER_H
