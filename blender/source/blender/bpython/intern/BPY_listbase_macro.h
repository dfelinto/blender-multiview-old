/** 
	$Id$
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
	These are macros to create python lists from base lists.
*/



/** example: DEFFUNC_GETLIST(text) defines a method for getting a list
 * of text blocks */

// Example: _GETLIST(name) -> get_namelist
#define _GETLIST(x) get_##x##list 

// Function definition:
// DEFFUNC_GETLIST_MAIN(name) -> get_namelist(PyObject *self, PyObject *args)
#define DEFFUNC_GETLIST_MAIN(x) \
	PyObject *_GETLIST(x)(PyObject *self, PyObject *args)	\
	{														\
		ID *id;												\
		PyObject *list;										\
		list = PyList_New(0);								\
		id = G.main->##x##.first;  							\
		while (id)											\
		{													\
			PyList_Append(list, PyString_FromString(id->name+2)); \
			id = id->next; \
		} \
		return list; \
	} \

// call the above function
#define GETLISTFUNC(x) _GETLIST(x)
// Prototype for the above function
#define GETLISTPROTO(x) PyObject *_GETLIST(x)(PyObject *, PyObject *)
