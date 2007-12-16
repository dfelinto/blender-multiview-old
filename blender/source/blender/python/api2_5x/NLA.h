/* 
 * $Id: NLA.h 11398 2007-07-28 06:28:33Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_NLA_H
#define EXPP_NLA_H

#include <Python.h>
#include "DNA_action_types.h"
#include "DNA_nla_types.h"

struct Object;

/** NLA module initialization function. */
void NLA_Init( void );
PyObject *ActionType_Init( void );
PyObject *ActionStripType_Init( void );
PyObject *ActionStripSeqType_Init( void );

extern PyTypeObject BPyAction_Type;
extern PyTypeObject BPyActionStrip_Type;
extern PyTypeObject BPyActionStripSeq_Type;

/** Python BPyNLA structure definition. */



typedef struct {
	PyObject_HEAD 
	bAction * action; /* libdata must be second */
} BPyActionObject;

/* TODO - impliment this pytype */
typedef struct {
	PyObject_HEAD 
	bActionChannel * chan;
} BPyActionObjectChan;

typedef struct {
	PyObject_HEAD 
	bActionStrip * strip;
} BPyActionStripObject;

typedef struct {
	PyObject_HEAD 
	struct Object * ob;
	struct bActionStrip *iter;
} BPyActionStripSeqObject;

/* Type checking for EXPP PyTypes */
#define BPyAction_Check(v)			PyObject_TypeCheck(v, &BPyAction_Type)
#define BPyActionStrip_Check(v)		PyObject_TypeCheck(v, &BPyActionStrip_Type)
#define BPyActionStripSeq_Check(v)	PyObject_TypeCheck(v, &BPyActionStripSeq_Type)

PyObject *Action_CreatePyObject( struct bAction *action );
#define Action_FromPyObject(py_obj) (((BPyActionObject *)py_obj)->action)
PyObject *ActionStrip_CreatePyObject( struct bActionStrip *strip );
PyObject *ActionStripSeq_CreatePyObject( struct Object *ob );

#endif
