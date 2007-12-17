/* 
 * $Id: bpy.c 11924 2007-09-02 21:03:59Z campbellbarton $
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

/* for open, close in Blender_Load */

#include "bpy_state.h"
#include "BLI_blenlib.h"
#include "BKE_global.h"
#include "BKE_utildefines.h"
#include "BKE_scene.h"
#include "BKE_main.h"

#include "DNA_scene_types.h"
#include "gen_utils.h"

/*****************************************************************************/
/* Global variables	 */
/*****************************************************************************/
PyObject *g_bpystatedict;

/*****************************************************************************/
/* Function:		initBlender		 */
/*****************************************************************************/

/* too small a module for its own file (for now) */
PyObject* m_state_init(void)
{
	PyObject *module, *dict, *smode, *SpaceHandlers;
	module=	Py_InitModule3("bpy.state", NULL, "The blender state");
	g_bpystatedict = dict =	PyModule_GetDict(module);
	
	SpaceHandlers = Py_BuildValue("{s:i, s:i}",
			"VIEW3D_EVENT",	SPACEHANDLER_VIEW3D_EVENT,
			"VIEW3D_DRAW",	SPACEHANDLER_VIEW3D_DRAW);
	
	if (SpaceHandlers)
		PyModule_AddObject(module, "SpaceHandlers", SpaceHandlers);
	
	if (G.background)
		smode = PyString_FromString("background");
	else
		smode = PyString_FromString("interactive");
	
	PyDict_SetItemString(dict, "bylink", Py_False);
	PyDict_SetItemString(dict, "link", Py_None);
	EXPP_dict_set_item_str(dict, "event", PyString_FromString(""));
	PyDict_SetItemString(dict, "mode", smode);
	
	return module;
}

