/**
 * $Id$
 *
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
#include "RAS_CheckVertexArrays.h"

#ifdef WIN32
#include <windows.h>
#endif // WIN32
#include "GL/gl.h"

#include "STR_String.h"


bool	RAS_SystemSupportsVertexArrays() {

	bool result = false;

	char* ext = (char*) glGetString(GL_EXTENSIONS);
	STR_String extensions;
		
	if (ext)
		extensions = STR_String(ext);

#ifdef WIN32
	if (extensions.Find("GL_EXT_compiled_vertex_array") >= 0)
	{
		result=true;
	}
#endif //WIN32

	return result;

}
