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

#include "KX_ISystem.h"


#ifdef WIN32
#pragma warning (disable :4786)
#include <windows.h>
#endif //WIN32

#include <iostream>
#include <stdio.h>
#include "KX_BlenderInputDevice.h"
#include "KX_BlenderSystem.h"

#include "PIL_time.h"

KX_BlenderSystem::KX_BlenderSystem()
: KX_ISystem()
{
	m_starttime = PIL_check_seconds_timer();
}

double KX_BlenderSystem::GetTimeInSeconds()
{
	return PIL_check_seconds_timer() - m_starttime;
}
