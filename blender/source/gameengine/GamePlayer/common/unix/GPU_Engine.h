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

#ifndef __GPU_ENGINE_H
#define __GPU_ENGINE_H


#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#define Object DNA_Object  // tricky stuff !!! but without it it doesn't compile...

#include "GPC_Engine.h"


class GPU_Engine : public GPC_Engine
{
public:
	XtIntervalId m_timerId;
	unsigned long m_timerTimeOutMsecs;

public:
	GPU_Engine(char *customLoadingAnimation,
		int foregroundColor, int backgroundColor, int frameRate);
	virtual ~GPU_Engine();
	bool Initialize(Display *display, Window window, int width, int height);

	void HandleNewWindow(Window window);

private:
	void AddEventHandlers();
};


#endif  // __GPU_ENGINE_H
