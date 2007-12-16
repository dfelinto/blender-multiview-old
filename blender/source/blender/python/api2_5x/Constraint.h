/* 
 * $Id: Constraint.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_CONSTRAINT_H
#define EXPP_CONSTRAINT_H

#include <Python.h>
#include "DNA_object_types.h"
#include "DNA_action_types.h"
#include "DNA_constraint_types.h"
#include "DNA_listBase.h"

/*****************************************************************************/
/* Python BPyModifierObject structure definition:                                 */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD		/* required macro */
	Object *obj;		/* "parent" object */
	bPoseChannel *pchan;/* "parent" pose channel */
	/* if con this is null, the constraint has been removed and we need to
	 * raise an error when its data is accessed */
	bConstraint *con;
} BPyConstraintObject;

extern PyTypeObject BPyConstraint_Type;

#define BPyConstraint_Check(v) PyObject_TypeCheck(v, &BPyConstraint_Type) /* for type checking */
typedef struct {
	PyObject_HEAD		/* required macro */
	Object *obj;		/* "parent" object */
	bPoseChannel *pchan;/* "parent" pose channel */
	bConstraint *iter;
} BPyConstraintSeqObject;

/* 
 *   prototypes
 */

PyObject *ConstraintType_Init( void );
PyObject *ConstraintSeqType_Init( void );
PyObject *Constraint_CreatePyObject( bPoseChannel *pchan, Object *obj,
		bConstraint *con );

PyObject *PoseConstraintSeq_CreatePyObject( bPoseChannel *pchan );
PyObject *ObConstraintSeq_CreatePyObject( Object *obj );

#define Constraint_FromPyObject(pyobj) (( ( BPyConstraintObject * ) pyobj )->con)

#endif				/* EXPP_CONSTRAINT_H */
