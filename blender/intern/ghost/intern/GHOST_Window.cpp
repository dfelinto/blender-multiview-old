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
 * @date	May 10, 2001
 */

#include "GHOST_Window.h"


GHOST_Window::GHOST_Window(
	const STR_String& /*title*/,
	GHOST_TInt32 /*left*/, GHOST_TInt32 /*top*/, GHOST_TUns32 width, GHOST_TUns32 height,
	GHOST_TWindowState state,
	GHOST_TDrawingContextType type,
	const bool stereoVisual)
:
	m_drawingContextType(type),
	m_cursorVisible(true),
	m_cursorShape(GHOST_kStandardCursorDefault),
	m_stereoVisual(stereoVisual)
{
    m_fullScreen = state == GHOST_kWindowStateFullScreen;
    if (m_fullScreen) {
        m_fullScreenWidth = width;
        m_fullScreenHeight = height;
    }
}


GHOST_Window::~GHOST_Window()
{
}


GHOST_TSuccess GHOST_Window::setDrawingContextType(GHOST_TDrawingContextType type)
{
	GHOST_TSuccess success = GHOST_kSuccess;
	if (type != m_drawingContextType) {
		success = removeDrawingContext();
		if (success) {
			success = installDrawingContext(type);
			m_drawingContextType = type;
		}
		else {
			m_drawingContextType = GHOST_kDrawingContextTypeNone;
		}
	}
	return success;
}

GHOST_TSuccess GHOST_Window::setCursorVisibility(bool visible)
{
	if (setWindowCursorVisibility(visible)) {
		m_cursorVisible = visible;
		return GHOST_kSuccess;
	}
	else {
		return GHOST_kFailure;
	}
}

GHOST_TSuccess GHOST_Window::setCursorShape(GHOST_TStandardCursor cursorShape)
{
	if (setWindowCursorShape(cursorShape)) {
		m_cursorShape = cursorShape;
		return GHOST_kSuccess;
	}
	else {
		return GHOST_kFailure;
	}
}

GHOST_TSuccess GHOST_Window::setCustomCursorShape(GHOST_TUns8 bitmap[16][2], GHOST_TUns8 mask[16][2], int hotX, int hotY)
{
	if (setWindowCustomCursorShape(bitmap, mask, hotX, hotY)) {
		m_cursorShape = GHOST_kStandardCursorCustom;
		return GHOST_kSuccess;
	}
	else {
		return GHOST_kFailure;
	}
}

