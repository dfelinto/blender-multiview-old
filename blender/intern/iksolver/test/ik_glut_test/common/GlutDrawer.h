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

#ifndef NAN_INCLUDED_GlutDrawer

#define NAN_INCLUDED_GlutDrawer

#include "MEM_NonCopyable.h"
#include "MEM_SmartPtr.h"

// So pissed off with Glut callback stuff
// that is impossible to call objects unless they are global

// inherit from GlutDrawer and installl the drawer in the singleton
// class GlutDrawManager.

class GlutDrawer {
public :

	virtual 
		void
	Draw(
	)= 0;

	virtual 
	~GlutDrawer(
	){};		
};

class GlutDrawManager : public MEM_NonCopyable{

public :

	static
		GlutDrawManager *
	Instance(
	);

	// this is the function you should pass to glut

	static
		void
	Draw(
	);

		void
	InstallDrawer(
		GlutDrawer *
	);

		void
	ReleaseDrawer(
	);

	~GlutDrawManager(
	);

private :

	GlutDrawManager (
	) :
		m_drawer (0)
	{
	};
	
	GlutDrawer * m_drawer;

	static MEM_SmartPtr<GlutDrawManager> m_s_instance;
};	


#endif