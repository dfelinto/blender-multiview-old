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

#include "../common/GlutKeyboardManager.h"

class MyGlutKeyHandler : public GlutKeyboardHandler
{
public :
	static
		MyGlutKeyHandler *
	New(
	) {
		MEM_SmartPtr<MyGlutKeyHandler> output = new MyGlutKeyHandler();

		if (output == NULL
		) {
			return NULL;
		}
		return output.Release();
		
	}	

		void
	HandleKeyboard(
		unsigned char key,
		int x,
		int y
	){

		switch (key) {

			case 27 :

				exit(0);
		}	
	}
		
	~MyGlutKeyHandler(
	) 
	{
	};

private :

	MyGlutKeyHandler(
	)
	{
	}

};
#endif

