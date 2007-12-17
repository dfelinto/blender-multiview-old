/* 
 * $Id: Modifier.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * Contributor(s): Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_MODIFIER_H
#define EXPP_MODIFIER_H

#include <Python.h>
#include "DNA_object_types.h"
#include "DNA_modifier_types.h"
#include "DNA_listBase.h"

/*****************************************************************************/
/* Python BPyModifierObject and BPyModSeqObject structure definition:        */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD		/* required macro */
	Object *object;
	ModifierData *iter;
} BPyModSeqObject;

typedef struct {
	PyObject_HEAD		/* required macro */
	Object *object;
	/* if md this is null, the modifier has been removed and we need to raise
	an error when its data is accessed */
	ModifierData *md; 
} BPyModifierObject;

extern PyTypeObject BPyModSeq_Type;
extern PyTypeObject BPyModifier_Type;
#define BPyModSeq_Check(v)		PyObject_TypeCheck(v, &BPyModSeq_Type)
#define BPyModifier_Check(v)	PyObject_TypeCheck(v, &BPyModifier_Type)

/* 
 *   prototypes
 */

PyObject *ModifierType_Init( void );
PyObject *ModifierSeqType_Init( void );
PyObject *ModSeq_CreatePyObject( Object *obj, ModifierData *iter );
PyObject *Modifier_CreatePyObject( Object *obj, ModifierData *md );
#define Modifier_FromPyObject(py_obj) (((BPyModifierObject *)py_obj)->md)

#endif				/* EXPP_MODIFIER_H */
