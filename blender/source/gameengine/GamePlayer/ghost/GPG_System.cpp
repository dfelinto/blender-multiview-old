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
 * Blender Player system on GHOST.
 */

#include "GPG_System.h"

#include <assert.h>

#include "GHOST_ISystem.h"


GPG_System::GPG_System(GHOST_ISystem* system)
: m_system(system)
{
	assert(m_system);
}


double GPG_System::GetTimeInSeconds()
{
	GHOST_TInt64 millis = (GHOST_TInt64)m_system->getMilliSeconds();
	double time = (double)millis;
	time /= 1000.;
	return time;
}


