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

#include "GlutKeyboardManager.h"

#include "MT_assert.h"

MEM_SmartPtr<GlutKeyboardManager> GlutKeyboardManager::m_s_instance = MEM_SmartPtr<GlutKeyboardManager>();

	GlutKeyboardManager *
GlutKeyboardManager::
Instance(
){
	if (m_s_instance == NULL) {
		m_s_instance = new GlutKeyboardManager();
	}

	return m_s_instance;
}


// this is the function you should pass to glut

	void
GlutKeyboardManager::
HandleKeyboard(
	unsigned char key,
	int x,
	int y
){
	GlutKeyboardManager *manager = GlutKeyboardManager::Instance();

	if (manager->m_handler != NULL) {
		manager->m_handler->HandleKeyboard(key,x,y);
	}
}

	void
GlutKeyboardManager::
InstallHandler(
	GlutKeyboardHandler * handler
){

	MT_assert(m_handler == NULL);
	m_handler = handler;
}

	void
GlutKeyboardManager::
ReleaseHandler(
){
	m_handler = NULL;
}


GlutKeyboardManager::
~GlutKeyboardManager(
){

	delete(m_handler);
}