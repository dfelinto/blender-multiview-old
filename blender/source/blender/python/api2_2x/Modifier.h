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
/* Python V24_BPy_Modifier and V24_BPy_ModSeq structure definition:                  */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD		/* required macro */
	Object *object;
	ModifierData *iter;
} V24_BPy_ModSeq;

typedef struct {
	PyObject_HEAD		/* required macro */
	Object *object;
	/* if md this is null, the modifier has been removed and we need to raise
	an error when its data is accessed */
	ModifierData *md; 
} V24_BPy_Modifier;

extern PyTypeObject V24_ModSeq_Type;
extern PyTypeObject V24_Modifier_Type;
#define BPy_ModSeq_Check(v)  ((v)->ob_type == &V24_ModSeq_Type)
#define BPy_Modifier_Check(v)  ((v)->ob_type == &V24_Modifier_Type)

/* 
 *   prototypes
 */

PyObject *V24_Modifier_Init( void );
PyObject *V24_ModSeq_CreatePyObject( Object *obj, ModifierData *iter );
PyObject *V24_Modifier_CreatePyObject( Object *obj, ModifierData *md );
ModifierData *V24_Modifier_FromPyObject( PyObject * py_obj );

#endif				/* EXPP_MODIFIER_H */
