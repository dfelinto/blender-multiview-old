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

#ifndef _GPG_CANVAS_H_
#define _GPG_CANVAS_H_

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include "GPC_Canvas.h"

#include "GHOST_IWindow.h"


class GPG_Canvas : public GPC_Canvas
{
protected:
	/** GHOST window. */
	GHOST_IWindow* m_window;

public:
	GPG_Canvas(GHOST_IWindow* window);
	virtual ~GPG_Canvas(void);

	virtual void Init(void);
	virtual void SetMousePosition(int x, int y);
	virtual void SetMouseState(RAS_MouseState mousestate);
	virtual void SwapBuffers();

	bool BeginDraw() { return true;};
	void EndDraw() {};
};

#endif // _GPG_CANVAS_H_
