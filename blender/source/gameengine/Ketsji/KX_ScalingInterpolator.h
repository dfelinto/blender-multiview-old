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
#ifndef KX_SCALINGINTERPOLATOR
#define KX_SCALINGINTERPOLATOR

#include "KX_IInterpolator.h"

class MT_Vector3;
class KX_IScalarInterpolator;

class KX_ScalingInterpolator : public KX_IInterpolator {
public:
	KX_ScalingInterpolator(MT_Vector3& target,
						   KX_IScalarInterpolator *ipos[])
		: m_target(target)
	{
		m_ipos[0] = ipos[0]; 
		m_ipos[1] = ipos[1]; 
		m_ipos[2] = ipos[2];
	}
	
	virtual void Execute(float currentTime) const;

private:
	MT_Vector3&              m_target;
	KX_IScalarInterpolator *m_ipos[3];
};

#endif



