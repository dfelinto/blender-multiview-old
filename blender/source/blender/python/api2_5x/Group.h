/* 
 * $Id: Group.h 11446 2007-07-31 16:11:32Z campbellbarton $
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
 * Contributor(s): Michel Selten
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_GROUP_H
#define EXPP_GROUP_H

#include <Python.h>
#include "DNA_group_types.h"

/* The Group PyTypeObject defined in Group.c */
extern PyTypeObject BPyGroup_Type;
extern PyTypeObject BPyGroupObSeq_Type;

#define BPyGroup_Check(v) PyObject_TypeCheck(v, &BPyGroup_Type)
#define BPyGroupObSeq_Check(v) PyObject_TypeCheck(v, &BPyGroupObSeq_Type)

/*****************************************************************************/
/* Python BPyGroupObject structure definition.                                  */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD
	Group *group;
} BPyGroupObject;


/* Group object sequence, iterate on the groups object listbase*/
typedef struct {
	PyObject_VAR_HEAD		/* required python macro   */
	BPyGroupObject *bpygroup;	/* link to the python group so we can know if its been removed */
	int index;				/* so we can iterate over the objects */
} BPyGroupObSeqObject;

PyObject * GroupType_Init( void );
PyObject * GroupSeqType_Init( void );
PyObject *Group_CreatePyObject( struct Group *group );
#define Group_FromPyObject(py_obj) (((BPyGroupObject *)py_obj)->group)

#endif				/* EXPP_GROUP_H */
