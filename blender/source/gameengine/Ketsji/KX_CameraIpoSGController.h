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
#ifndef KX_CAMERAIPOSGCONTROLLER_H
#define KX_CAMERAIPOSGCONTROLLER_H

#include "SG_Controller.h"
#include "SG_Spatial.h"

#include "KX_IInterpolator.h"

struct RAS_CameraData;

class KX_CameraIpoSGController : public SG_Controller
{
public:
	MT_Scalar           m_lens;
	MT_Scalar           m_clipstart;
	MT_Scalar           m_clipend;

private:
	T_InterpolatorList	m_interpolators;
	unsigned short  	m_modify_lens 	 : 1;
	unsigned short	    m_modify_clipstart       : 1;
	unsigned short		m_modify_clipend    	 : 1;
	bool				m_modified;

	double		        m_ipotime;
public:
	KX_CameraIpoSGController() : m_ipotime(0.0),
								m_modify_lens(false),
								m_modify_clipstart(false),
								m_modify_clipend(false),
								m_modified(true)
		{}

	~KX_CameraIpoSGController();
	SG_Controller*	GetReplica(class SG_Node* destnode);
	bool Update(double time);

		void
	SetOption(
		int option,
		int value
	);

	void SetSimulatedTime(double time) {
		m_ipotime = time;
		m_modified = true;
	}
	void	SetModifyLens(bool modify) {	
		m_modify_lens = modify;
	}
	void	SetModifyClipEnd(bool modify) {	
		m_modify_clipend = modify;
	}
	void	SetModifyClipStart(bool modify) {	
		m_modify_clipstart = modify;
	}
	void	AddInterpolator(KX_IInterpolator* interp);
};

#endif // KX_CAMERAIPOSGCONTROLLER_H
