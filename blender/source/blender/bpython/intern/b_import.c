/* $Id$
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

	Customized Blender import module, Sandbox model (TODO)

	TODO for Sandbox:
		- only allow builtin modules to be imported
		- only allow file read/write from certain directories
		- alternative: override file read/write with popup requester

	main routine: 
	init_ourImport();
	
*/
	


#include "DNA_text_types.h"
#include "Python.h"
#include "import.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_library.h"
#include "BKE_text.h"

#include "BLI_blenlib.h" // mallocs

#include "BPY_macros.h" 
#include "BPY_main.h" 

#include "b_import.h"


/* ---------------------------------------------------------------------------- */

PyObject *importText(char *name)
{
	Text *text;
	char txtname[IDNAME];
	char *buf = NULL;


	// TODO debug for too long names !

	strncpy(txtname, name, IDNAME-4);
	strcat(txtname, ".py"); 
	
	text = (Text*) getGlobal()->main->text.first;
	while(text)
	{
		if (STREQ(getName(text), txtname))
			break;
		text = text->id.next;
	}
	if (!text) {
		return NULL;
	}

	if (!text->compiled) {
		buf = txt_to_buf(text);
		text->compiled = Py_CompileString(buf, getName(text), Py_file_input);
		MEM_freeN(buf);
		if (PyErr_Occurred())
		{
			PyErr_Print();
			BPY_free_compiled_text(text);
			return NULL;
		}	
	}	
	BPY_debug(("import from TextBuffer: %s\n", txtname));
	return PyImport_ExecCodeModule(name, text->compiled);
}

PyObject *blender_import(PyObject *self, PyObject *args)
{
	PyObject *exception, *err, *tb;
	char *name;
	PyObject *globals = NULL, *locals = NULL, *fromlist = NULL;
	PyObject *m;
#ifdef PY_SANDBOXTEST
	PyObject *l, *n;
#endif

	if (!PyArg_ParseTuple(args, "s|OOO:bimport",
	        &name, &globals, &locals, &fromlist))
	    return NULL;

#ifdef PY_SANDBOXTEST
	/* check for builtin modules */
	m = PyImport_AddModule("sys");
	l = PyObject_GetAttrString(m, "builtin_module_names");
	n = PyString_FromString(name);
	
	if (PySequence_Contains(l, n)) {
		Py_DECREF(l);
		m = PyImport_ImportModuleEx(name, globals, locals, fromlist);
		return m;
	} else {
	/* TODO check for external python toolbox modules */

		PyErr_Format(PyExc_ImportError,
			 "Import of external Module %.40s not allowed.\n\
Please disable security in the UserButtons", name);
		Py_DECREF(l);
		return NULL;
	}

#else
	m = PyImport_ImportModuleEx(name, globals, locals, fromlist);
	if (m) 
		return m;
	else 
		PyErr_Fetch(&exception, &err, &tb); // restore exception for probable later use
	
	m = importText(name);
	if (m) { // found module, ignore above exception
		PyErr_Clear();
		Py_XDECREF(exception); Py_XDECREF(err); Py_XDECREF(tb);
		printf("imported from text buffer..\n");
	} else {
		PyErr_Restore(exception, err, tb); 
	}
	return m;
#endif
}
static PyMethodDef bimport[] = {
	{ "blimport", blender_import, METH_VARARGS,
		"our own import"}
};


/*
PyObject *override_method(PyObject *module, char *methname, PyMethodDef *newcfunc)
{
	PyObject *d;
	PyObject *old;
	PyObject *pycfunc;


	d = PyModule_GetDict(module);
	old = PyDict_GetItemString(module, methname);
	if (!old)
		return NULL;

	pycfunc = PyCFunction_New(newcfunc, NULL);
	PyDict_SetItemString(d, methname, pycfunc);

	return old;
}	
*/

	
/* this overrides the built in __import__ function
with our customized importer */
void init_ourImport(void)
{
	PyObject *m, *d;
	PyObject *import = PyCFunction_New(bimport, NULL);

	m = PyImport_AddModule("__builtin__");
	d = PyModule_GetDict(m);
	PyDict_SetItemString(d, "__import__", import);
}	

