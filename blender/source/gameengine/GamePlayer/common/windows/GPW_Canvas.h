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

#ifndef _GPW_CANVAS_H_
#define _GPW_CANVAS_H_

#ifdef WIN32
#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
#endif

#include <iostream>
#include <windows.h>
#include <gl/gl.h>

#include "GPC_Canvas.h"


class GPW_Canvas : public GPC_Canvas
{
protected:
	/** The window handle. */
	HWND m_hWnd;
	/** Rendering context. */
	HGLRC	m_hRC;
	/** Device Context. */
    HDC		m_hDC;

public:
	/**
	 * Constructor.
	 */
	GPW_Canvas(HWND hWnd, HDC hDC, int width, int height);

	/**
	 * Destructor.
	 */
	virtual ~GPW_Canvas(void);

	virtual void Init(void);

	/**
	 * Moves the cursor to a new location.
	 * @param x The x-coordinate of the new location.
	 * @param x The y-coordinate of the new location.
	 */
	virtual void SetMousePosition(int x, int y);

	/**
	 * Sets the cursor shape and/or visibility.
	 * @param mousestate The new state ofthe cursor.
	 */
	virtual void SetMouseState(RAS_MouseState mousestate);

	bool BeginDraw(void);
	void EndDraw(void);

	virtual void SwapBuffers(void);

	virtual HDC GetHDC(void)
	{
		return m_hDC;
	}

	virtual void SetHDC(HDC hDC)
	{
		if (hDC != m_hDC) {
			m_hDC = hDC;
		}
	}

	virtual HGLRC GetHGLRC(void)
	{
		return m_hRC;
	}

protected:
	/**
	 * Hides the mouse cursor.
	 */
	void HideCursor(void);

	/**
	 * Shows the mouse cursor.
	 */
	void ShowCursor(void);
};



#endif // _GPW_CANVAS_H_
