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
#ifndef KX_SCALARINTERPOLATOR
#define KX_SCALARINTERPOLATOR

#include "MT_Scalar.h"
#include "KX_IInterpolator.h"

class KX_IScalarInterpolator;

class KX_ScalarInterpolator : public KX_IInterpolator {
public:
	KX_ScalarInterpolator(MT_Scalar* target, 
						  KX_IScalarInterpolator *ipo) :
		m_target(target),
		m_ipo(ipo)
		{}
	
	virtual ~KX_ScalarInterpolator() {}
	virtual void Execute(float currentTime) const;
	void		SetNewTarget(MT_Scalar* newtarget)
	{
		m_target=newtarget;
	}
	MT_Scalar*	GetTarget()
	{
		return m_target;
	}
private:
	MT_Scalar*               m_target;
	KX_IScalarInterpolator *m_ipo;
};

#endif
