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

#include "GlutDrawer.h"

#include "MT_assert.h"

MEM_SmartPtr<GlutDrawManager> GlutDrawManager::m_s_instance = MEM_SmartPtr<GlutDrawManager>();

	GlutDrawManager *
GlutDrawManager::
Instance(
){
	if (m_s_instance == NULL) {
		m_s_instance = new GlutDrawManager();
	}

	return m_s_instance;
}


// this is the function you should pass to glut

	void
GlutDrawManager::
Draw(
){
	GlutDrawManager *manager = GlutDrawManager::Instance();

	if (manager->m_drawer != NULL) {
		manager->m_drawer->Draw();
	}
}

	void
GlutDrawManager::
InstallDrawer(
	GlutDrawer * drawer
){

	MT_assert(m_drawer == NULL);
	m_drawer = drawer;
}

	void
GlutDrawManager::
ReleaseDrawer(
){
	m_drawer = NULL;
}


GlutDrawManager::
~GlutDrawManager(
){

	delete(m_drawer);
}









