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
#ifndef KX_WORLDIPOCONTROLLER_H
#define KX_WORLDIPOCONTROLLER_H

#include "SG_Controller.h"
#include "SG_Spatial.h"

#include "KX_IInterpolator.h"

class KX_WorldIpoController : public SG_Controller
{
public:
	MT_Scalar           m_mist_rgb[3];
	MT_Scalar           m_mist_start;
	MT_Scalar           m_mist_dist;

private:
	T_InterpolatorList	m_interpolators;
	unsigned short	    m_modify_mist_color  : 1;
	unsigned short		m_modify_mist_start	 : 1;
	unsigned short  	m_modify_mist_dist 	 : 1;
	bool				m_modified;

	double		        m_ipotime;

public:
	KX_WorldIpoController() : m_ipotime(0.0),
							  m_modify_mist_color(false),
							  m_modify_mist_start(false),
							  m_modify_mist_dist(false),
							  m_modified(true)
		{}

	virtual ~KX_WorldIpoController();

	virtual	SG_Controller*	GetReplica(class SG_Node* destnode);

	virtual bool Update(double time);
	
	virtual void SetSimulatedTime(double time) {
		m_ipotime = time;
		m_modified = true;
	}

	void	SetModifyMistStart(bool modify) {	
		m_modify_mist_start = modify;
	}

	void	SetModifyMistColor(bool modify) {	
		m_modify_mist_color = modify;
	}

	void	SetModifyMistDist(bool modify) {	
		m_modify_mist_dist = modify;
	}

		void
	SetOption(
		int option,
		int value
	){
		// intentionally empty
	};

	void	AddInterpolator(KX_IInterpolator* interp);
};

#endif // KX_LIGHTIPOSGCONTROLLER_H

