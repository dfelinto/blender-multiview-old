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
 * @date	May 22, 2001
 */

#ifndef _GHOST_SYSTEM_CARBON_H_
#define _GHOST_SYSTEM_CARBON_H_

#ifndef __APPLE__
#error Apple OSX only!
#endif // __APPLE__

#include "GHOST_System.h"
#include "GHOST_Keys.h"

#include <Carbon/Carbon.h>

class GHOST_EventCursor;
class GHOST_EventKey;
class GHOST_EventWindow;

/**
 * OSX/Cocoa Implementation of GHOST_System class.
 * @see GHOST_System.
 * @author	Maarten Gribnau
 * @date	May 21, 2001
 */
class GHOST_SystemCarbon : public GHOST_System {
};

#endif // _GHOST_SYSTEM_CARBON_H_
