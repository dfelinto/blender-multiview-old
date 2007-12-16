/* 
 * $Id: Metaball.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Jacques Guignot
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#ifndef EXPP_METABALL_H
#define EXPP_METABALL_H

#include <Python.h>
#include "DNA_meta_types.h"


extern PyTypeObject BPyMetaball_Type;
extern PyTypeObject BPyMetaelemSeq_Type;

#define BPyMetaball_Check(v)	PyObject_TypeCheck(v, &BPyMetaball_Type)
#define BPyMetaelem_Check(v)	PyObject_TypeCheck(v, &BPyMetaelem_Type)
#define BPyMetaelemSeq_Check(v)	PyObject_TypeCheck(v, &BPyMetaelemSeq_Type)

/* Python BPyMetaballObject structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	MetaBall * metaball; /* libdata must be second */
} BPyMetaballObject;

/* Python BPyMetaelemObject structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	BPyMetaballObject *bpymetaball;
	int index;
} BPyMetaelemObject;

/* Python BPyMetaelemSeqObject structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	BPyMetaballObject *bpymetaball;
	int index; /* so we can iterate over the objects */
} BPyMetaelemSeqObject;

/*
 * prototypes
 */


PyObject *MetaballType_Init( void );
PyObject *MetaelemType_Init( void );
PyObject *MetaelemSeqType_Init( void );

PyObject *Metaball_CreatePyObject( MetaBall * mball );
#define Metaball_FromPyObject(py_obj) (((BPyMetaballObject *)py_obj)->metaball)

#endif				/* EXPP_METABALL_H */
