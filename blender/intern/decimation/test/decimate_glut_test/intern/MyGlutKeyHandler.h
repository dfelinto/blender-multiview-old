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

#ifndef NAN_INCLUDED_MyGlutKeyHandler_h

#define NAN_INCLUDED_MyGlutKeyHandler_h

#include "common/GlutKeyboardManager.h"
#include "LOD_decimation.h"

class MyGlutKeyHandler : public GlutKeyboardHandler
{
public :
	static
		MyGlutKeyHandler *
	New(
		LOD_Decimation_InfoPtr info
	) {
		return new MyGlutKeyHandler(info);
	}	

		void
	HandleKeyboard(
		GHOST_TKey key,
		int x,
		int y
	){
		int steps = 0;

		switch (key) {
			case GHOST_kKeyD :

				while (steps < 100 && LOD_CollapseEdge(m_info)) {
					steps ++;
				}

				break;

			case GHOST_kKeyEsc :

				// release all the handlers!				
				exit(0);
		}
		
	}
		
	~MyGlutKeyHandler(
	) {
	};

private :

	MyGlutKeyHandler(
		LOD_Decimation_InfoPtr info
	):
		m_info (info)
	{
	}


	LOD_Decimation_InfoPtr m_info;
};
#endif

