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

#ifndef LOD_GhostTestApp_h

#define LOD_GhostTestApp_h

#include "GHOST_IEventConsumer.h"
#include "MT_Vector3.h"
#include <vector>

class GHOST_IWindow;
class GHOST_ISystem;


class LOD_GhostTestApp :
public GHOST_IEventConsumer
{
public :
	// Construct an instance of the application;

	LOD_GhostTestApp(
	);

	// initialize the applicaton

		bool
	InitApp(
	);

	// Run the application untill internal return.
		void
	Run(
	);
	
	~LOD_GhostTestApp(
	);
	
private :

		void
	UpdateFrame(
	);
	
	// inherited from GHOST_IEventConsumer
	// maps events to GlutXXXHandlers()

		bool 
	processEvent(
		GHOST_IEvent* event
	);

	GHOST_IWindow *m_window;
	GHOST_ISystem *m_system;

	bool m_finish_me_off;
};

#endif