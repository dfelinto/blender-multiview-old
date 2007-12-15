/* 
 * $Id$
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
 * Contributor(s): Michel Selten, Willian P. Germano, Alex Mole, Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_gen_library_h
#define EXPP_gen_library_h

#include <Python.h>

#include "DNA_ID.h"
#include "DNA_listBase.h"

/* ID functions for all libdata */
#define	GENERIC_LIB_GETSETATTR \
	{"name",\
	 (getter)V24_GenericLib_getName, (setter)V24_GenericLib_setName,\
	 "name",\
	 NULL},\
	{"lib",\
	 (getter)V24_GenericLib_getLib, (setter)NULL,\
	 "external library path",\
	 NULL},\
	{"users",\
	 (getter)V24_GenericLib_getUsers, (setter)NULL,\
	 "user count",\
	 NULL},\
	{"fakeUser",\
	 (getter)V24_GenericLib_getFakeUser, (setter)V24_GenericLib_setFakeUser,\
	 "fake user state",\
	 NULL},\
	{"properties",\
	 (getter)V24_GenericLib_getProperties, (setter)NULL,\
	 "properties",\
	 NULL},\
	{"tag",\
	 (getter)V24_GenericLib_getTag, (setter)V24_GenericLib_setTag,\
	 "temproary tag",\
	 NULL}

/* Dummy struct for getting the ID from a libdata BPyObject */
typedef struct {
	PyObject_HEAD		/* required python macro */
	ID *id;
} V24_BPy_GenericLib;

int V24_GenericLib_setName( void *self, PyObject *value );
PyObject *V24_GenericLib_getName( void *self );
PyObject *V24_GenericLib_getFakeUser( void *self );
int V24_GenericLib_setFakeUser( void *self, PyObject *value );
PyObject *V24_GenericLib_getTag( void *self );
int V24_GenericLib_setTag( void *self, PyObject *value );
PyObject *V24_GenericLib_getLib( void *self );
PyObject *V24_GenericLib_getUsers( void *self );
PyObject *V24_GenericLib_getProperties( void *self );

/* use this for oldstyle somedata.getName("name") */
PyObject * V24_GenericLib_setName_with_method( void *self, PyObject *value ); 

int V24_GenericLib_assignData(PyObject *value, void **data, void **ndata, short refcount, short type, short subtype);
short V24_GenericLib_getType(PyObject * pydata);

/* Other ID functions */
ID			*V24_GetIdFromList( ListBase * list, char *name );
PyObject	*V24_GetPyObjectFromID( ID * id );
long V24_GenericLib_hash(V24_BPy_GenericLib * pydata);
#endif				/* EXPP_gen_library_h */
