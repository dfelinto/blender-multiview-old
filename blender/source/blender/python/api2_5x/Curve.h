/* 
 * $Id: Curve.h 11406 2007-07-28 14:53:03Z campbellbarton $
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

#ifndef EXPP_CURVE_H
#define EXPP_CURVE_H

#include <Python.h>
#include "CurveBase.h"
#include "DNA_curve_types.h"

extern PyTypeObject BPyCurve_Type;

#define BPyCurve_Check(v) PyObject_TypeCheck(v, &BPyCurve_Type)

/* Python BPyCurveObject structure definition */
typedef struct {
	BPyCurveBaseObject bpycurvebase;	/* libdata must be second */
	/* pointer for iterator:  does not point to owned memory */
	int index;
} BPyCurveObject;


/*
 *  protoypes
 */

PyObject *CurveType_Init( void );
PyObject *Curve_CreatePyObject( struct Curve * curve );
PyObject *CurveBase_update( BPyCurveObject * self );

#endif				/* EXPP_CURVE_H */
