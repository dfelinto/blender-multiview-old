/* 
 * $Id: bpy.c 11934 2007-09-03 20:57:12Z campbellbarton $
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

#include "BLI_blenlib.h"
#include "BKE_global.h"
#include "BKE_utildefines.h"
#include "BKE_scene.h"
#include "BKE_main.h"

#include "DNA_scene_types.h"

#include "Library.h"

#include "bpy.h"
#include "bpy_data.h"
#include "bpy_config.h"
#include "bpy_types.h"
#include "bpy_utils.h"
#include "bpy_gl.h"
#include "bpy_app.h"
#include "bpy_state.h"
#include "modules.h" /* noise library BGL inits defined */

#include "Draw.h"
#include "Geometry.h"
#include "Mathutils.h"
#include "Registry.h"
#include "Window.h"


/*****************************************************************************/
/* Function:		initBlender		 */
/*****************************************************************************/


void m_bpy_init(void)
{
	PyObject *module, *dict;
	
	/* G.scene should only ever be NULL if blender is executed in 
	background mode, not loading a blend file and executing a python script eg.
	blender -P somescript.py -b
	The if below solves the segfaults that happen when python runs and
	G.scene is NULL */
	if(G.background && G.main->scene.first==NULL) {
		Scene *sce= add_scene("1");
		/*set_scene(sce);*/ /* causes a crash */
		G.scene= sce;
	}
	
	module =		Py_InitModule3("bpy", NULL, "The main bpy module");
	dict =			PyModule_GetDict(module);
	
	/* these modules start with nothin added to them */
	PyDict_SetItemString(dict, "state",			m_state_init());
	PyDict_SetItemString(dict, "app",			m_app_init());
	
	PyModule_AddObject( module, "config",		Config_CreatePyObject() );
	PyDict_SetItemString( dict, "data",			m_data_init());
	PyDict_SetItemString( dict, "libraries",	Library_Init(  ) );
	PyDict_SetItemString( dict, "types",		m_types_init(  ) );
	PyDict_SetItemString( dict, "utils",		m_utils_init(  ) );
	PyDict_SetItemString( dict, "gl",			m_gl_init( ) );
	
	if (module) {
		PyObject *module_math, *dict_math;
		module_math =	Py_InitModule3("bpy.math", NULL, "The bpy math module");
		dict_math =		PyModule_GetDict(module);
		
		PyDict_SetItemString( dict_math, "geometry",	Geometry_Init( ) );
		PyDict_SetItemString( dict_math, "noise",		Noise_Init( ) );
		PyDict_SetItemString( dict_math, "utils",		Mathutils_Init( ) );
		
		/* assign to bpy.math */
		PyDict_SetItemString( dict, "math",			module_math);
	}
	
	/* TODO - NOT SURE THIS SHOULD GO HERE */
	PyDict_SetItemString( dict, "draw",			Draw_Init( ) );
	PyDict_SetItemString( dict, "registry",		Registry_Init( ) );
	PyDict_SetItemString( dict, "window",		Window_Init());
	
}
