/* 
 * $Id: bpy_utils.c 10235 2007-03-10 11:47:24Z campbellbarton $
 *
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
 * Contributor(s): Willian P. Germano, Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/
//struct ID; /*keep me up here */
#include "bpy_utils.h" /*This must come first*/

/* for open, close in Blender_Load */
#include "BDR_editobject.h"	/* exit_editmode() */
#include "BIF_usiblender.h"
#include "BLI_blenlib.h"
#include "BLO_writefile.h"
#include "BKE_blender.h"
#include "BKE_global.h"
#include "BKE_utildefines.h"
#include "BKE_scene.h"
#include "BKE_text.h"
#include "BKE_library.h"
#include "BKE_main.h"
#include "BPI_script.h"
#include "DNA_userdef_types.h"
#include "DNA_packedFile_types.h"
#include "DNA_scene_types.h" /* G.scene-"r.cfra */
#include "EXPP_interface.h" /* for bpy_gethome() */
#include "gen_utils.h"
#include "../BPY_extern.h" /* BPY_txt_do_python_Text */
#include "../BPY_menus.h"	/* to update menus */
#include "Registry.h"
#include "Text.h"

/*****************************************************************************/
/* Python API function prototypes for the utils module.                      */
/*****************************************************************************/
static PyObject *m_utils_expandpath( PyObject *self, PyObject *args);
static PyObject *m_utils_run( PyObject * self, PyObject * args );
static PyObject *m_utils_showHelp( PyObject * self, PyObject * args );
static PyObject *m_utils_updateMenus( PyObject * self);

/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* bpy.utils.__doc__                                                         */
/*****************************************************************************/
static char m_utils_expandpath_doc[] =
"(path) - Expand this Blender internal path to a proper file system path.\n\
(path) - the string path to convert.\n\n\
Note: internally Blender paths can contain two special character sequences:\n\
- '//' (at start) for base path directory (the current .blend's dir path);\n\
- '#' (at ending) for current frame number.\n\n\
This function expands these to their actual content, returning a valid path.\n\
If the special chars are not found in the given path, it is simply returned.";


static char m_utils_run_doc[] =
	"(script) - Run the given Python script.\n\
(script) - the path to a file or the name of an available Blender Text.";

static char m_utils_showHelp_doc[] =
"(script) - Show help for the given Python script.\n\
  This will try to open the 'Scripts Help Browser' script, so to have\n\
any help displayed the passed 'script' must be properly documented\n\
with the expected strings (check API ref docs or any bundled script\n\
for examples).\n\n\
(script) - the filename of a script in the default or user defined\n\
           scripts dir (no need to supply the full path name)."; 

static char m_utils_updateMenus_doc[] =
	"() - Update the menus where scripts are registered.  Only needed for\n\
scripts that save other new scripts in the default or user defined folders.";

/*****************************************************************************/
/* Python method structure definition for bpy.utils module:                  */
/*****************************************************************************/
struct PyMethodDef m_utils_methods[] = {
	{"expandpath", m_utils_expandpath, METH_VARARGS, m_utils_expandpath_doc},
	{"run", m_utils_run, METH_VARARGS, m_utils_run_doc},
	{"showHelp", m_utils_showHelp, METH_VARARGS, m_utils_showHelp_doc},
	{"updateMenus", ( PyCFunction ) m_utils_updateMenus, METH_NOARGS, m_utils_updateMenus_doc},
	{NULL, NULL, 0, NULL}
};

/* Module Functions */
PyObject *m_utils_init( void )
{
	PyObject *submodule;
	submodule = Py_InitModule3( "bpy.utils", m_utils_methods, "utils docs" );
	return submodule;
}

static PyObject *m_utils_expandpath( PyObject * self, PyObject * args )
{
	char *path = NULL;
	char expanded[FILE_MAXDIR + FILE_MAXFILE];

	if (!PyArg_ParseTuple( args, "s", &path))
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"expected string argument" );
	
	BLI_strncpy(expanded, path, FILE_MAXDIR + FILE_MAXFILE);
	BLI_convertstringcode(expanded, G.sce, G.scene->r.cfra);

	return PyString_FromString(expanded);
}


static PyObject *m_utils_showHelp(PyObject *self, PyObject *args)
{
	PyObject *script = NULL;
	char hspath[FILE_MAXDIR + FILE_MAXFILE]; /* path to help_browser.py */
	char *sdir = bpy_gethome(1);
	PyObject *rkeyd = NULL, *arglist = NULL;

	if (!PyArg_ParseTuple(args, "O!", &PyString_Type, &script))
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"expected a script filename as argument");

	/* first we try to find the help_browser script */

	if (sdir) BLI_make_file_string("/", hspath, sdir, "help_browser.py");

	if (!sdir || (!BLI_exists(hspath) && (U.pythondir[0] != '\0'))) {
			char upydir[FILE_MAXDIR];

			BLI_strncpy(upydir, U.pythondir, FILE_MAXDIR);
			BLI_convertstringcode(upydir, G.sce, 0);
			BLI_make_file_string("/", hspath, upydir, "help_browser.py");

			if (!BLI_exists(hspath))
				return EXPP_ReturnPyObjError(PyExc_RuntimeError,
					"can't find script help_browser.py");
	}

	/* now we store the passed script in the registry dict and call the
	 * help_browser to show help info for it */
	rkeyd = PyDict_New();
	if (!rkeyd)
		return EXPP_ReturnPyObjError(PyExc_MemoryError,
			"can't create py dictionary!");

	/* note: don't use EXPP_dict_set_item_str for 'script', which is an
	 * argument to the function we're in and so shouldn't be decref'ed: */
	PyDict_SetItemString(rkeyd, "script", script);

	EXPP_dict_set_item_str(bpy_registryDict, "__help_browser", rkeyd);

	arglist = Py_BuildValue("(s)", hspath);
	m_utils_run(self, arglist);
	Py_DECREF(arglist);

	Py_RETURN_NONE;
}

static PyObject *m_utils_run(PyObject *self, PyObject *args)
{
	char *fname = NULL;
	Text *text = NULL;
	int is_blender_text = 0;
	Script *script = NULL;

	if (!PyArg_ParseTuple(args, "s", &fname))
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"expected a filename or a Blender Text name as argument");

	if (!BLI_exists(fname)) {	/* if there's no such filename ... */
		text = G.main->text.first;	/* try an already existing Blender Text */

		while (text) {
			if (!strcmp(fname, text->id.name + 2)) break;
			text = text->id.next;
		}

		if (!text) {
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"no such file or Blender text");
		}
		else is_blender_text = 1;	/* fn is already a Blender Text */
	}

	else {
		text = add_text(fname);

		if (!text) {
			return EXPP_ReturnPyObjError(PyExc_RuntimeError,
				"couldn't create Blender Text from given file");
		}
	}

	/* (this is messy, check Draw.c's Method_Register and Window.c's file
	 * selector for more info)
	 * - caller script is the one that called this m_utils_Run function;
	 * - called script is the argument to this function: fname;
	 * To mark scripts whose global dicts can't be freed right after
	 * the script execution (or better, 'first pass', since these scripts
	 * leave callbacks for gui or file/image selectors) we flag them.  But to
	 * get a pointer to them we need to check which one is currently
	 * running (if none we're already at a spacescript).  To make sure only
	 * the called script will have the SCRIPT_RUNNING flag on, we unset it
	 * for the caller script here: */
	script = G.main->script.first;
	while (script) {
		if (script->flags & SCRIPT_RUNNING) break;
		script = script->id.next;
	}

	if (script) script->flags &= ~SCRIPT_RUNNING; /* unset */

	BPY_txt_do_python_Text(text); /* call new script */

	if (script) script->flags |= SCRIPT_RUNNING; /* set */

	if (!is_blender_text) free_libblock(&G.main->text, text);

	Py_RETURN_NONE;
}

static PyObject * m_utils_updateMenus( PyObject * self )
{

	BPyMenu_RemoveAllEntries();

	if (BPyMenu_Init(1) == -1)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"invalid scripts dir");

	Py_RETURN_NONE;
}
