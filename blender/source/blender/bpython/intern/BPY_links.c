/** Helper functions to handle links between Object types,
 * Script links */

/*
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

#include "Python.h"
#include "BPY_macros.h"
#include "b_interface.h"

/* GLOBALS */

PyObject *g_blenderdict;

/* PROTOS */
char *event_to_name(short event);
void set_scriptlinks(ID *id, short event);

#ifndef SHAREDMODULE
PyObject *DataBlock_fromData (void *data);


void set_scriptlinks(ID *id, short event)
{
	PyObject *link;

	if (!g_blenderdict) // not initialized yet; this can happen at first file load
	{
		return;
	}	
	if (GET_ID_TYPE(id) == ID_SCE) {
		Py_INCREF(Py_None);
		link = Py_None;
	} else {
		link = DataBlock_fromData(id);
	}	

	if (!link)
	{
		printf ("Internal error, unable to create PyBlock for script link\n");
		printf ("This is a bug; please report to bugs@blender.nl");
		Py_INCREF(Py_False);
		PyDict_SetItemString(g_blenderdict, "bylink", Py_False);
		return;
	} else {
		Py_INCREF(Py_True);
		PyDict_SetItemString(g_blenderdict, "bylink", Py_True);
	}
		
	PyDict_SetItemString(g_blenderdict, "link", link);
	PyDict_SetItemString(g_blenderdict, "event", Py_BuildValue("s", event_to_name(event)));
}

/* this is just a hack-added function to release a script link reference.
 * The scriptlink concept will be redone later */

void release_scriptlinks(ID *id)
{
	PyObject *link;
	if (!g_blenderdict) return; // return if Blender module was not initialized
	link = PyDict_GetItemString(g_blenderdict, "link");
	Py_DECREF(link);
	Py_INCREF(Py_None);
	PyDict_SetItemString(g_blenderdict, "link", Py_None);
}

#endif
