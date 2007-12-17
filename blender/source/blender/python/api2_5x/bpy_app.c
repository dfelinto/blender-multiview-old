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
#include "bpy_app.h"
#include "BIF_usiblender.h"
#include "BLI_blenlib.h"
#include "BKE_global.h"
#include "BKE_utildefines.h"
#include "BKE_scene.h"
#include "BKE_main.h"
#include "gen_utils.h"
#include "BKE_blender.h" /* for blender version */
#include "EXPP_interface.h" /* bpy_gethome */

#include "DNA_scene_types.h"


/*****************************************************************************/
/* Function:			m_app_quit                                           */
/* Python equivalent:	bpy.app.quit()                                       */
/*****************************************************************************/
static PyObject *m_app_quit( PyObject * self )
{
	BIF_write_autosave(  );	/* save the current data first */
	exit_usiblender(  );	/* renames last autosave to quit.blend */
	Py_RETURN_NONE;
}

static struct PyMethodDef m_app_methods[] = {
	{"quit", (PyCFunction)m_app_quit, METH_O,
		"() - Quit Blender.  The current data is saved as 'quit.blend' before leaving."},
	{NULL, NULL, 0, NULL}
};

PyObject* m_app_init(void)
{
	PyObject *module, *dict;
	
	char *bpyhomedir = bpy_gethome(1), *homedir = BLI_gethome(), dir[FILE_MAXDIR];
	
	/* not much good we can do here :/ */
	if (!bpyhomedir)	bpyhomedir =	"";
	if (!homedir)		homedir =		"";
	
	module =	Py_InitModule3("bpy.app", m_app_methods, "The blender app data");
	dict =			PyModule_GetDict(module);
	
	EXPP_dict_set_item_str(dict, "version", PyFloat_FromDouble(BLENDER_VERSION + (BLENDER_SUBVERSION/10.0f)));
	
	/* home dir */
	EXPP_dict_set_item_str(dict, "homedir", PyString_FromString( homedir ));
	
	/* data dir */
	BLI_make_file_string( "/", dir, bpyhomedir, "bpydata" );
	EXPP_dict_set_item_str(dict, "datadir", PyString_FromString( dir ));
	
	/* scripts dir */
	EXPP_dict_set_item_str(dict, "scriptsdir", PyString_FromString( bpyhomedir ) );
	
	/* icon dir */
	#ifdef WIN32
		BLI_make_file_string( "/", dir, homedir, "icons" );
	#else
		BLI_make_file_string( "/", dir, homedir, ".blender/icons" );
	#endif
	EXPP_dict_set_item_str(dict, "icondir", PyString_FromString( dir ));
	
	return module;
}
