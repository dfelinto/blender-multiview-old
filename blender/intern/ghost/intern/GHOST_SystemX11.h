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

#ifndef _GHOST_SYSTEM_X11_H_
#define _GHOST_SYSTEM_X11_H_

#include "GHOST_System.h"
#include "../GHOST_Types.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

class GHOST_WindowX11;

/**
 * X11 Implementation of GHOST_System class.
 * @see GHOST_System.
 * @author	Laurence Bourn
 * @date	October 26, 2001
 */

class GHOST_SystemX11 : public GHOST_System {
public:

	/**
	 * Constructor
	 * this class should only be instanciated by GHOST_ISystem.
	 */

	GHOST_SystemX11(
	);
	
		GHOST_TSuccess 
	init(
	);


	/**
	 * @section Interface Inherited from GHOST_ISystem 
	 */

	/**
	 * Returns the system time.
	 * Returns the number of milliseconds since the start of the system process.
	 * @return The number of milliseconds.
	 */
		GHOST_TUns64 
	getMilliSeconds(
	) const;
	

	/**
	 * Returns the number of displays on this system.
	 * @return The number of displays.
	 */
		GHOST_TUns8 
	getNumDisplays(
	) const;

	/**
	 * Returns the dimensions of the main display on this system.
	 * @return The dimension of the main display.
	 */
		void 
	getMainDisplayDimensions(
		GHOST_TUns32& width,
		GHOST_TUns32& height
	) const;

	/**
	 * Create a new window.
	 * The new window is added to the list of windows managed. 
	 * Never explicitly delete the window, use disposeWindow() instead.
	 * @param	title	The name of the window (displayed in the title bar of the window if the OS supports it).
	 * @param	left		The coordinate of the left edge of the window.
	 * @param	top		The coordinate of the top edge of the window.
	 * @param	width		The width the window.
	 * @param	height		The height the window.
	 * @param	state		The state of the window when opened.
	 * @param	type		The type of drawing context installed in this window.
	 * @param       stereoVisual    Create a stereo visual for quad buffered stereo.
	 * @return	The new window (or 0 if creation failed).
	 */
		GHOST_IWindow* 
	createWindow(
		const STR_String& title,
		GHOST_TInt32 left,
		GHOST_TInt32 top,
		GHOST_TUns32 width,
		GHOST_TUns32 height,
		GHOST_TWindowState state,
		GHOST_TDrawingContextType type,
		const bool stereoVisual
	);
	 
	/**
	 * @section Interface Inherited from GHOST_ISystem 
	 */

	/**
	 * Retrieves events from the system and stores them in the queue.
	 * @param waitForEvent Flag to wait for an event (or return immediately).
	 * @return Indication of the presence of events.
	 */
		bool 
	processEvents(
		bool waitForEvent
	);

	/**
	 * @section Interface Inherited from GHOST_System 
	 */
		GHOST_TSuccess 
	getCursorPosition(
		GHOST_TInt32& x,
		GHOST_TInt32& y
	) const;
	
		GHOST_TSuccess 
	setCursorPosition(
		GHOST_TInt32 x,
		GHOST_TInt32 y
	) const;

	/**
	 * Returns the state of all modifier keys.
	 * @param keys	The state of all modifier keys (true == pressed).
	 * @return		Indication of success.
	 */
		GHOST_TSuccess 
	getModifierKeys(
		GHOST_ModifierKeys& keys
	) const ;

	/**
	 * Returns the state of the mouse buttons (ouside the message queue).
	 * @param buttons	The state of the buttons.
	 * @return			Indication of success.
	 */
		GHOST_TSuccess 
	getButtons(
		GHOST_Buttons& buttons
	) const;

	/**
	 * @section
	 * Flag a window as dirty. This will
	 * generate a GHOST window update event on a call to processEvents() 
	 */

		void
	addDirtyWindow(
		GHOST_WindowX11 * bad_wind
	);
  
 
	/**
	 * return a pointer to the X11 display structure
	 */

		Display *
	getXDisplay(
	) {
		return m_display;
	}	

	
private :

	Display * m_display;

	/**
	 * Atom used to detect window close events
	 */
	Atom m_delete_window_atom;

	/// The vector of windows that need to be updated.
	std::vector<GHOST_WindowX11 *> m_dirty_windows;

	/// Start time at initialization.
	GHOST_TUns64 m_start_time;

	/// A vector of keyboard key masks
	char *m_keyboard_vector;

	/**
	 * Return the ghost window associated with the
	 * X11 window xwind
	 */

		GHOST_WindowX11 * 
	findGhostWindow(
		Window xwind
	) const ;

		void
	processEvent(
		XEvent *xe
 	);

		bool
	generateWindowExposeEvents(
 	);
 
		GHOST_TKey
	convertXKey(
		unsigned int key
	);

};

#endif




