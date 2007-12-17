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

#ifndef EXPP_BPY_FLOAT_H
#define EXPP_BPY_FLOAT_H

#include <Python.h>
#include "gen_library.h"

/*****************************************************************************/
/* Python BPyMaterialObject structure definition:        */
/*****************************************************************************/
extern PyTypeObject BPyFloat_Type;	/* The Material PyType Object */

typedef struct {
	PyFloatObject pyfloat;
	
	/* this is the BPyGenericLibObject the data uses
	 * when it is NULL, the BPyFloat is not linked to any data */
	BPyGenericLibObject * genlib;
	
	/* the curve this float uses */
	int adrcode;
} BPyFloatObject;

PyObject *BPyFloatType_Init( void );
PyObject *BPyFloat_CreatePyObject( float value, PyObject *self, int adrcode );

#define BPyFloat_Check(v) PyObject_TypeCheck(v, &BPyFloat_Type)	/* for type checking */

#endif				/* EXPP_BPY_FLOAT_H */
