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

#include "GPG_Canvas.h"

#include <assert.h>

#include "GHOST_ISystem.h"


GPG_Canvas::GPG_Canvas(GHOST_IWindow* window)
: GPC_Canvas(0, 0), m_window(window)
{
	if (m_window)
	{
		GHOST_Rect bnds;
		m_window->getClientBounds(bnds);
		this->Resize(bnds.getWidth(), bnds.getHeight());
	}
}


GPG_Canvas::~GPG_Canvas(void)
{
}


void GPG_Canvas::Init()
{
	if (m_window)
	{
		GHOST_TSuccess success;
		success = m_window->setDrawingContextType(GHOST_kDrawingContextTypeOpenGL);
		assert(success == GHOST_kSuccess);
	}
}

void GPG_Canvas::SetMousePosition(int x, int y)
{
	GHOST_ISystem* system = GHOST_ISystem::getSystem();
	if (system && m_window)
	{
		GHOST_TInt32 gx = (GHOST_TInt32)x;
		GHOST_TInt32 gy = (GHOST_TInt32)y;
		GHOST_TInt32 cx;
		GHOST_TInt32 cy;
		m_window->clientToScreen(gx, gy, cx, cy);
		system->setCursorPosition(cx, cy);
	}
}


void GPG_Canvas::SetMouseState(RAS_MouseState mousestate)
{
	if (m_window)
	{
		switch (mousestate)
		{
		case MOUSE_INVISIBLE:
			m_window->setCursorVisibility(false);
			break;
		case MOUSE_WAIT:
			m_window->setCursorShape(GHOST_kStandardCursorWait);
			m_window->setCursorVisibility(true);
			break;
		case MOUSE_NORMAL:
			m_window->setCursorShape(GHOST_kStandardCursorRightArrow);
			m_window->setCursorVisibility(true);
			break;
		}
	}
}


void GPG_Canvas::SwapBuffers()
{
	if (m_window)
	{
		m_window->swapBuffers();
	}
}
