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
PyObject *V24_NLA_Init( void );

extern PyTypeObject V24_Action_Type;
extern PyTypeObject V24_ActionStrip_Type;
extern PyTypeObject V24_ActionStrips_Type;

/** Python BPy_NLA structure definition. */
typedef struct {
	PyObject_HEAD 
	bAction * action; /* libdata must be second */
} V24_BPy_Action;

typedef struct {
	PyObject_HEAD 
	bActionStrip * strip;
} V24_BPy_ActionStrip;

typedef struct {
	PyObject_HEAD 
	struct Object * ob;
	struct bActionStrip *iter;
} V24_BPy_ActionStrips;

/* Type checking for EXPP PyTypes */
#define V24_BPy_Action_Check(v)       ((v)->ob_type == &V24_Action_Type)
#define V24_BPy_ActionStrip_Check(v)  ((v)->ob_type == &V24_ActionStrip_Type)
#define V24_BPy_ActionStrips_Check(v) ((v)->ob_type == &V24_ActionStrips_Type)

PyObject *V24_Action_CreatePyObject( struct bAction *action );
bAction *V24_Action_FromPyObject( PyObject * py_obj );

PyObject *V24_ActionStrip_CreatePyObject( struct bActionStrip *strip );
PyObject *V24_ActionStrips_CreatePyObject( struct Object *ob );

#endif
