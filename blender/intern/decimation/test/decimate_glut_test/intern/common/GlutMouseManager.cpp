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

#include "GlutMouseManager.h"

#include "MT_assert.h"

MEM_SmartPtr<GlutMouseManager> GlutMouseManager::m_s_instance = MEM_SmartPtr<GlutMouseManager>();


	GlutMouseManager *
GlutMouseManager::
Instance(
){
	if (m_s_instance == NULL) {
		m_s_instance = new GlutMouseManager();
	}

	return m_s_instance;
}	

// these are the functions you should pass to GLUT	

	void
GlutMouseManager::
ButtonUp(
	GHOST_IWindow * window,
	GHOST_TButtonMask button_mask,
	int x,
	int y
){
	GlutMouseManager *manager = GlutMouseManager::Instance();

	if (manager->m_handler != NULL) {
		manager->m_handler->ButtonUp(window,button_mask,x,y);
	}
}

	void
GlutMouseManager::
ButtonDown(
	GHOST_IWindow * window,
	GHOST_TButtonMask button_mask,
	int x,
	int y
){
	GlutMouseManager *manager = GlutMouseManager::Instance();

	if (manager->m_handler != NULL) {
		manager->m_handler->ButtonDown(window,button_mask,x,y);
	}
}



	void
GlutMouseManager::
Motion(
	GHOST_IWindow * window,
	int x,
	int y
){
	GlutMouseManager *manager = GlutMouseManager::Instance();

	if (manager->m_handler != NULL) {
		manager->m_handler->Motion(window,x,y);
	}
}

	void
GlutMouseManager::
InstallHandler(
	GlutMouseHandler *handler
){

	MT_assert(m_handler == NULL);
	m_handler = handler;
}

	void
GlutMouseManager::
ReleaseHandler(
){
	m_handler = NULL;
}

GlutMouseManager::
~GlutMouseManager(
){

	delete(m_handler);
}


