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

#ifndef NAN_INCLUDED_GlutKeyboardManager

#define NAN_INCLUDED_GlutKeyboardManager

#include "MEM_NonCopyable.h"
#include "MEM_SmartPtr.h"
#include "GHOST_Types.h"

// inherit from GlutKeyboardHandler and installl the drawer in the singleton
// class GlutKeyboardManager.

class GlutKeyboardHandler : public MEM_NonCopyable {
public :

	virtual 
		void
	HandleKeyboard(
		GHOST_TKey key,
		int x,
		int y
	)= 0;

	virtual 
	~GlutKeyboardHandler(
	){};		
};

class GlutKeyboardManager : public MEM_NonCopyable{

public :

	static
		GlutKeyboardManager *
	Instance(
	);

	static
		void
	HandleKeyboard(
		GHOST_TKey key,
		int x,
		int y
	);

		void
	InstallHandler(
		GlutKeyboardHandler *
	);

		void
	ReleaseHandler(
	);

	~GlutKeyboardManager(
	);

private :

	GlutKeyboardManager (
	) :
		m_handler (0)
	{
	};
	
	GlutKeyboardHandler * m_handler;

	static MEM_SmartPtr<GlutKeyboardManager> m_s_instance;
};	


#endif