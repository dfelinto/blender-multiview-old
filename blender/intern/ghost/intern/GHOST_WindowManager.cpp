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
 * @date	May 11, 2001
 */

#include "GHOST_WindowManager.h"

#include <algorithm>

#include "GHOST_Debug.h"
#include "GHOST_Window.h"


GHOST_WindowManager::GHOST_WindowManager()
: m_fullScreenWindow(0), m_activeWindow(0)
{
}


GHOST_WindowManager::~GHOST_WindowManager()
{
}


GHOST_TSuccess GHOST_WindowManager::addWindow(GHOST_IWindow* window)
{
	GHOST_TSuccess success = GHOST_kFailure;
	if (window) {
		if (!getWindowFound(window)) {
			// Store the pointer to the window 
			m_windows.push_back(window);
			success = GHOST_kSuccess;
		}
	}
	return success;
}


GHOST_TSuccess GHOST_WindowManager::removeWindow(const GHOST_IWindow* window)
{
	GHOST_TSuccess success = GHOST_kFailure;
	if (window) {
		if (window == m_fullScreenWindow) {
			endFullScreen();
		}
		else {
			vector<GHOST_IWindow*>::iterator result = find(m_windows.begin(), m_windows.end(), window);
			if (result != m_windows.end()) {
				setWindowInactive(window);
				m_windows.erase(result);
				success = GHOST_kSuccess;
			}
		}
	}
	return success;
}


bool GHOST_WindowManager::getWindowFound(const GHOST_IWindow* window) const
{
	bool found = false;
	if (window) {
        if (getFullScreen() && (window == m_fullScreenWindow)) {
            found = true;
        }
        else {
            vector<GHOST_IWindow*>::const_iterator result = find(m_windows.begin(), m_windows.end(), window);
            if (result != m_windows.end()) {
                found = true;
            }
        }
	}
	return found;
}


bool GHOST_WindowManager::getFullScreen(void) const
{
	return m_fullScreenWindow != 0;
}


GHOST_IWindow* GHOST_WindowManager::getFullScreenWindow(void) const
{
    return m_fullScreenWindow;
}


GHOST_TSuccess GHOST_WindowManager::beginFullScreen(GHOST_IWindow* window,
		bool stereoVisual)
{
	GHOST_TSuccess success = GHOST_kFailure;
	GHOST_ASSERT(window, "GHOST_WindowManager::beginFullScreen(): invalid window");
	GHOST_ASSERT(window->getValid(), "GHOST_WindowManager::beginFullScreen(): invalid window");
	if (!getFullScreen()) {
		m_fullScreenWindow = window;
		setActiveWindow(m_fullScreenWindow);
		success = GHOST_kSuccess;
	}
	return success;
}


GHOST_TSuccess GHOST_WindowManager::endFullScreen(void)
{
	GHOST_TSuccess success = GHOST_kFailure;
	if (getFullScreen()) {
		if (m_fullScreenWindow != 0) {
			//GHOST_PRINT("GHOST_WindowManager::endFullScreen(): deleting full-screen window\n");
			setWindowInactive(m_fullScreenWindow);
			delete m_fullScreenWindow;
			//GHOST_PRINT("GHOST_WindowManager::endFullScreen(): done\n");
			m_fullScreenWindow = 0;
		}
        success = GHOST_kSuccess;
	}
	return success;
}


GHOST_TSuccess GHOST_WindowManager::setActiveWindow(GHOST_IWindow* window)
{
	GHOST_TSuccess success = GHOST_kSuccess;
	if (window != m_activeWindow) {
		if (getWindowFound(window)) {
			m_activeWindow = window;
		}
		else {
			success = GHOST_kFailure;
		}
	}
	return success;
}
	

GHOST_IWindow* GHOST_WindowManager::getActiveWindow(void) const
{
	return m_activeWindow;
}


void GHOST_WindowManager::setWindowInactive(const GHOST_IWindow* window)
{
	if (window == m_activeWindow) {
		m_activeWindow = 0;
	}
}


	std::vector<GHOST_IWindow *> &
GHOST_WindowManager::
getWindows(
){
	return m_windows;
}

