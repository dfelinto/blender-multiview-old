/* 
 * $Id: Curve.h 11342 2007-07-23 02:24:07Z campbellbarton $
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
 * Contributor(s): Jacques Guignot, Stephen Swaney
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_CURVEBASE_H
#define EXPP_CURVEBASE_H

#include <Python.h>
#include "DNA_curve_types.h"

extern PyTypeObject BPyCurveBase_Type;

#define BPyCurveBase_Check(v) PyObject_TypeCheck(v, &BPyCurveBase_Type) /* for curve, surf, text3d */

/* Python BPyCurveBaseObject structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	Curve * curve;		/* libdata must be second */
	/* pointer for iterator:  does not point to owned memory */
	//Nurb *iter_pointer;
} BPyCurveBaseObject;


/*
 *  protoypes
 */

PyObject *CurveBase_Init( void );
PyObject *CurveBaseType_Init( void  );
#define CurveBase_FromPyObject(py_obj) (((BPyCurveBaseObject *)py_obj)->curve)

#endif				/* EXPP_CURVEBASE_H */
