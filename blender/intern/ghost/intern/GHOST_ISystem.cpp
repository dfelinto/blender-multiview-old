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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * @author	Maarten Gribnau
 * @date	May 7, 2001
 */

#include "GHOST_ISystem.h"

#ifdef WIN32
#	include "GHOST_SystemWin32.h"
#else
#	ifdef __APPLE__
#		include "GHOST_SystemCarbon.h"
#	else
#		include "GHOST_SystemX11.h"
#	endif
#endif


GHOST_ISystem* GHOST_ISystem::m_system = 0;


GHOST_TSuccess GHOST_ISystem::createSystem()
{
	GHOST_TSuccess success;
	if (!m_system) {
#ifdef WIN32
		m_system = new GHOST_SystemWin32 ();
#else
#	ifdef __APPLE__
        m_system = new GHOST_SystemCarbon ();
#	else 
		m_system = new GHOST_SystemX11 ();
#	endif
#endif 
		success = m_system != 0 ? GHOST_kSuccess : GHOST_kFailure;
	}
	else {
		success = GHOST_kFailure;
	}
	if (success) {
		success = m_system->init();
	}
	return success;
}

GHOST_TSuccess GHOST_ISystem::disposeSystem()
{
	GHOST_TSuccess success;
	if (m_system) {
		delete m_system;
		m_system = 0;
	}
	else {
		success = GHOST_kFailure;
	}
	return success;
}


GHOST_ISystem* GHOST_ISystem::getSystem()
{
	return m_system;
}

