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

#include "LOD_GhostTestApp.h"

#include "GHOST_ISystem.h"
#include "GHOST_IWindow.h"
#include "common/GlutDrawer.h"
#include "common/GlutKeyboardManager.h"
#include "common/GlutMouseManager.h"

using namespace std;

LOD_GhostTestApp::
LOD_GhostTestApp(
):
	m_window(NULL),
	m_system(NULL),
	m_finish_me_off (false)
{
}

// initialize the applicaton

	bool
LOD_GhostTestApp::
InitApp(
){

	// create a system and window with opengl
	// rendering context.

	GHOST_TSuccess success = GHOST_ISystem::createSystem();
	if (success == GHOST_kFailure) return false;

	m_system = GHOST_ISystem::getSystem();
	if (m_system == NULL) return false;

	m_system->addEventConsumer(this);
	
	m_window = m_system->createWindow(
		"GHOST crud!",
		100,100,640,480,GHOST_kWindowStateNormal,
		GHOST_kDrawingContextTypeOpenGL
	);

	if (
		m_window == NULL
	) {
		m_system = NULL;
		GHOST_ISystem::disposeSystem();
		return false;
	}

	return true;
}

// Run the application untill internal return.
	void
LOD_GhostTestApp::
Run(
){
	if (m_system == NULL) {
		return;
	}

	while (!m_finish_me_off) {
		m_system->processEvents();
		m_system->dispatchEvents();
	};
}

LOD_GhostTestApp::
~LOD_GhostTestApp(
){
	if (m_window) {
		m_system->disposeWindow(m_window);
		m_window = NULL;
		GHOST_ISystem::disposeSystem();
		m_system = NULL;
	}
};


	bool 
LOD_GhostTestApp::
processEvent(
	GHOST_IEvent* event
){

	// map ghost events to the glut schmuk handlers.
	bool handled = false;

	switch(event->getType()) {
		case GHOST_kEventWindowSize:
		case GHOST_kEventWindowActivate:
		case GHOST_kEventWindowUpdate:
			GlutDrawManager::Draw();
			static_cast<GHOST_TEventWindowData *>(event->getData())->window->swapBuffers();

			handled = true;
			break;
		case GHOST_kEventButtonDown:
		{
			int x,y;
			m_system->getCursorPosition(x,y);
	
			int wx,wy;
			m_window->screenToClient(x,y,wx,wy);

			GHOST_TButtonMask button = 
				static_cast<GHOST_TEventButtonData *>(event->getData())->button;
			GlutMouseManager::ButtonDown(m_window,button,wx,wy);
		}
		handled = true;
		break;

		case GHOST_kEventButtonUp:
		{
			int x,y;
			m_system->getCursorPosition(x,y);
	
			int wx,wy;
			m_window->screenToClient(x,y,wx,wy);

			GHOST_TButtonMask button = 
				static_cast<GHOST_TEventButtonData *>(event->getData())->button;
			GlutMouseManager::ButtonUp(m_window,button,wx,wy);
		}
		handled = true;
		break;

		case GHOST_kEventCursorMove:
		{	
			int x,y;
			m_system->getCursorPosition(x,y);
	
			int wx,wy;
			m_window->screenToClient(x,y,wx,wy);

			GlutMouseManager::Motion(m_window,wx,wy);
	
		}	
		handled = true;
		break;

		case GHOST_kEventKeyDown :
		{
			GHOST_TEventKeyData *kd = 
				static_cast<GHOST_TEventKeyData *>(event->getData());

			int x,y;
			m_system->getCursorPosition(x,y);
	
			int wx,wy;
			m_window->screenToClient(x,y,wx,wy);

			GlutKeyboardManager::HandleKeyboard(kd->key,wx,wy);
		}		
		handled = true;
		break;
	
		default :
			break;
	}

	return handled;
}







	
