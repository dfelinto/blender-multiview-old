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

#ifndef _GPG_SYSTEM_H_
#define _GPG_SYSTEM_H_

#ifdef WIN32
#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
#endif // WIN32

#include "GPC_System.h"

class GHOST_ISystem;

class GPG_System : public GPC_System
{
	GHOST_ISystem* m_system;

public:
	GPG_System(GHOST_ISystem* system);

	virtual double GetTimeInSeconds();
};


#endif // _GPG_SYSTEM_H_
