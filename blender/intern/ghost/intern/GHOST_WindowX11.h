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

#ifndef _GHOST_WINDOWX11_H_
#define _GHOST_WINDOWX11_H_

#include "GHOST_Window.h"
#include <X11/Xlib.h>
#include <GL/glx.h>

#include <map>

class STR_String;
class GHOST_SystemX11;

/**
 * X11 implementation of GHOST_IWindow.
 * Dimensions are given in screen coordinates that are relative to the upper-left corner of the screen. 
 * @author	Laurence Bourn
 * @date	October 26, 2001
 */

class GHOST_WindowX11 : public GHOST_Window
{
public:
	/**
	 * Constructor.
	 * Creates a new window and opens it.
	 * To check if the window was created properly, use the getValid() method.
	 * @param title		The text shown in the title bar of the window.
	 * @param left		The coordinate of the left edge of the window.
	 * @param top		The coordinate of the top edge of the window.
	 * @param width		The width the window.
	 * @param height	The height the window.
	 * @param state		The state the window is initially opened with.
	 * @param type		The type of drawing context installed in this window.
	 * @param stereoVisual	Stereo visual for quad buffered stereo.
	 */
	GHOST_WindowX11(
		GHOST_SystemX11 *system,
		Display * display,
		const STR_String& title, 
		GHOST_TInt32 left,
		GHOST_TInt32 top,
		GHOST_TUns32 width,	
		GHOST_TUns32 height,
		GHOST_TWindowState state,
		GHOST_TDrawingContextType type = GHOST_kDrawingContextTypeNone,
		const bool stereoVisual = false
	);

		bool 
	getValid(
	) const;

		void 
	setTitle(const STR_String& title);

		void 
	getTitle(
		STR_String& title
	) const;

		void 
	getWindowBounds(
		GHOST_Rect& bounds
	) const;
	
		void 
	getClientBounds(
		GHOST_Rect& bounds
	) const;

		GHOST_TSuccess 
	setClientWidth(
		GHOST_TUns32 width
	);

		GHOST_TSuccess 
	setClientHeight(
		GHOST_TUns32 height
	);

		GHOST_TSuccess 
	setClientSize(
		GHOST_TUns32 width,
		GHOST_TUns32 height
	);

		void 
	screenToClient(
		GHOST_TInt32 inX,
		GHOST_TInt32 inY,
		GHOST_TInt32& outX,
		GHOST_TInt32& outY
	) const;

		void 
	clientToScreen(
		GHOST_TInt32 inX,
		GHOST_TInt32 inY,
		GHOST_TInt32& outX,
		GHOST_TInt32& outY
	) const;
	
		GHOST_TWindowState 
	getState(
	) const ;

		GHOST_TSuccess 
	setState(
		GHOST_TWindowState state
	);
	
		GHOST_TSuccess 
	setOrder(
		GHOST_TWindowOrder order
	);
	
		GHOST_TSuccess 
	swapBuffers(
	);
	
		GHOST_TSuccess 
	activateDrawingContext(
	);
		GHOST_TSuccess 
	invalidate(
	);

	/**
	 * Destructor.
	 * Closes the window and disposes resources allocated.
	 */
	 ~GHOST_WindowX11();

	/**
	 * @section 
	 * X11 system specific calls.
	 */

	/**
	 * The reverse of invalidate! Tells this window
	 * that all events for it have been pushed into
	 * the GHOST event queue.
	 */

		void
	validate(
	);	

	/**	
	 * Return a handle to the x11 window type.
	 */
		Window 
	getXWindow(
	);	

protected:
	/**
	 * Tries to install a rendering context in this window.
	 * @param type	The type of rendering context installed.
	 * @return Indication as to whether installation has succeeded.
	 */
		GHOST_TSuccess 
	installDrawingContext(
		GHOST_TDrawingContextType type
	);

	/**
	 * Removes the current drawing context.
	 * @return Indication as to whether removal has succeeded.
	 */
		GHOST_TSuccess 
	removeDrawingContext(
	);

	/**
	 * Sets the cursor visibility on the window using
	 * native window system calls.
	 */
		GHOST_TSuccess 
	setWindowCursorVisibility(
		bool visible
	);
	
	/**
	 * Sets the cursor shape on the window using
	 * native window system calls.
	 */
		GHOST_TSuccess 
	setWindowCursorShape(
		GHOST_TStandardCursor shape
	);

	/**
	 * Sets the cursor shape on the window using
	 * native window system calls.
	 */
		GHOST_TSuccess
	setWindowCustomCursorShape(
		GHOST_TUns8 bitmap[16][2], 
		GHOST_TUns8 mask[16][2], 
		int hotX, 
		int hotY
	);

private :

	/// Force use of public constructor.
	
	GHOST_WindowX11(
	);

	GHOST_WindowX11(
		const GHOST_WindowX11 &
	);

		Cursor
	getStandardCursor(
		GHOST_TStandardCursor g_cursor
	);
	
		Cursor 
	getEmptyCursor(
	);
	
	GLXContext 	m_context;
	Window 	m_window;
	Display 	*m_display;
	XVisualInfo	*m_visual;

	/** The first created OpenGL context (for sharing display lists) */
	static GLXContext s_firstContext;

	/// A pointer to the typed system class.
	
	GHOST_SystemX11 * m_system;

	bool m_valid_setup;

	/** Used to concatenate calls to invalidate() on this window. */
	bool m_invalid_window;

	/** XCursor structure of an empty (blank) cursor */
	Cursor m_empty_cursor;
	
	/** XCursor structure of the custom cursor */
	Cursor m_custom_cursor;
	
	/** Cache of XC_* ID's to XCursor structures */
	std::map<unsigned int, Cursor> m_standard_cursors;
};


#endif // _GHOST_WINDOWX11_H_
