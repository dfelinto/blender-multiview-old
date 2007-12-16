/* 
 * $Id: Material.h 10649 2007-05-04 03:23:40Z campbellbarton $
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
 * Contributor(s): Campbell J Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/ 

#ifndef EXPP_BPY_LIST_H
#define EXPP_BPY_LIST_H

#include <Python.h>
#include "DNA_ID.h"
#include "DNA_material_types.h"
#include "gen_library.h"

/*****************************************************************************/
/* Python BPyMaterialObject structure definition:        */
/*****************************************************************************/
typedef struct {
	PyListObject list;
	
	/* this is the BPyGenericLibObject the data uses
	 * when it is NULL, the BPyList is not linked to any data */
	BPyGenericLibObject * genlib;
	
	/* this is a bit annoying to store the type here, since it does not change
	 * but subtyping material list would not work, or require as much code as a list suptype */
	char type;
} BPyListObject;

#define BPY_LIST_TYPE_COLORBAND_SPEC	0
#define BPY_LIST_TYPE_COLORBAND	 		1 /* only have 2 types here so we know which one to access */
#define BPY_LIST_TYPE_MATERIAL			2
#define BPY_LIST_TYPE_SCRIPTLINK		3
#define BPY_LIST_TYPE_RENDERLAYERS 		4

extern PyTypeObject BPyList_Type;	/* The Material PyType Object */

#define BPyList_Check(v) PyObject_TypeCheck(v, &BPyList_Type)

/*****************************************************************************/
/* Module Blender.Material - public functions	 */
/*****************************************************************************/
void BPyList_Init( void );
PyObject *BPyListType_Init( void );
PyObject *BPyList_get( BPyGenericLibObject *genlib, void * type );
int BPyList_set( BPyGenericLibObject *genlib, PyObject * value, void * type );

#endif				/* EXPP_MATERIAL_LIST_H */
