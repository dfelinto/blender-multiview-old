/**
 * $Id$
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

#include "KX_OrientationInterpolator.h"


#include "MT_Matrix3x3.h"
#include "KX_IScalarInterpolator.h"

void KX_OrientationInterpolator::Execute(float currentTime) const {
	MT_Vector3 eul(m_ipos[0]->GetValue(currentTime),
				   m_ipos[1]->GetValue(currentTime),
				   m_ipos[2]->GetValue(currentTime));
	MT_Scalar ci = cos(eul[0]); 
	MT_Scalar cj = cos(eul[1]); 
	MT_Scalar ch = cos(eul[2]);
	MT_Scalar si = sin(eul[0]); 
	MT_Scalar sj = sin(eul[1]); 
	MT_Scalar sh = sin(eul[2]);
	MT_Scalar cc = ci*ch; 
	MT_Scalar cs = ci*sh; 
	MT_Scalar sc = si*ch; 
	MT_Scalar ss = si*sh;

	m_target.setValue(cj*ch, sj*sc-cs, sj*cc+ss,
					  cj*sh, sj*ss+cc, sj*cs-sc, 
					    -sj,    cj*si,    cj*ci);
}
