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

#ifndef NAN_INCLUDED_GlutMouseManager_h

#define NAN_INCLUDED_GlutMouseManager_h

#include "MEM_NonCopyable.h"
#include "MEM_SmartPtr.h"

class GlutMouseHandler {
public :

	virtual		
		void
	Mouse(
		int button,
		int state,
		int x,
		int y
	) = 0;

	virtual
		void
	Motion(
		int x,
		int y
	) = 0;

	virtual 
	~GlutMouseHandler(
	){};		
};

class GlutMouseManager : public MEM_NonCopyable{

public :

	static
		GlutMouseManager *
	Instance(
	);

	// these are the functions you should pass to GLUT	

	static
		void
	Mouse(
		int button,
		int state,
		int x,
		int y
	);

	static
		void
	Motion(
		int x,
		int y
	);

		void
	InstallHandler(
		GlutMouseHandler *
	);

		void
	ReleaseHandler(
	);

	~GlutMouseManager(
	);

private :

	GlutMouseManager (
	) :
		m_handler (0)
	{
	};
	
	GlutMouseHandler * m_handler;

	static MEM_SmartPtr<GlutMouseManager> m_s_instance;
};	


#endif