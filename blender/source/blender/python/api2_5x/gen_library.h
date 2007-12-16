/* 
 * $Id: gen_library.h 11446 2007-07-31 16:11:32Z campbellbarton $
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
#include "DNA_material_types.h"
#include "DNA_listBase.h"

/* for types that have scriptlinks */
#define GENERIC_LIB_GETSETATTR_SCRIPTLINK \
	{"scriptlinks",\
	 (getter)BPyList_get, (setter)BPyList_set,\
	 "script link sequence",\
	 (void *)BPY_LIST_TYPE_SCRIPTLINK}

#define GENERIC_LIB_GETSETATTR_TEXTURE \
	{"textures",\
	 (getter)MTexSeq_CreatePyObject, (setter)NULL,\
	 "MTex Sequence",\
	 NULL}

/* for types that have materials */
#define GENERIC_LIB_GETSETATTR_MATERIAL \
	{"materials",\
	 (getter)BPyList_get, (setter)BPyList_set,\
	 "material list",\
	 (void *)BPY_LIST_TYPE_MATERIAL}

/* for types that have layers */
#define GENERIC_LIB_GETSETATTR_LAYER \
	{"layers",\
	 (getter)GenericLib_getLayers, (setter)GenericLib_setLayers,"\
@ivar layers: A python set subclass that works like a normal set but keeps the changes in sync with blenders.\n\
@type layers: set subclass",\
	 NULL}


extern PyTypeObject BPyGenericLib_Type;

/* Dummy struct for getting the ID from a libdata BPyObject */
typedef struct {
	PyObject_HEAD		/* required python macro */
	ID *id;
} BPyGenericLibObject;

PyObject *GenericLibType_Init( void  );

char * GenericLib_libname( short type );

PyObject *GenericLib_getLayers( void *self );
int GenericLib_setLayers( void *self, PyObject *value );

int GenericLib_assignData(PyObject *value, void **data, void **ndata, short refcount, short type, short subtype);
short GenericLib_getType(PyObject * pydata);

/* Other ID functions */
PyObject	*GetPyObjectFromID( ID * id );

/*internal use*/
short GenericLib_totcol( ID *id );
Material ***GenericLib_materials( ID *id );
ScriptLink *GenericLib_scriptlinks(ID * id);

/* Scriptlink Tuple utility functions */
/* useually a py chech define but we need a function for this one*/
int BPyScriptLinkTuple_Check(PyObject *value);
PyObject *ScriptLinkTuple_CreatePyObject( BPyGenericLibObject * genlib, int index );
void ScriptLinkTuple_ToScriptLink( ScriptLink * slink, int index, PyObject * pyob );

#endif				/* EXPP_gen_library_h */
