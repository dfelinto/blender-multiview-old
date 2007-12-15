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
 * Contributor(s): Jacques Guignot, Stephen Swaney
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_CURVE_H
#define EXPP_CURVE_H

#include <Python.h>
#include "DNA_curve_types.h"

extern PyTypeObject V24_Curve_Type;

#define V24_BPy_Curve_Check(v) ((v)->ob_type==&V24_Curve_Type)

/* Python V24_BPy_Curve structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	Curve * curve;		/* libdata must be second */
	/* pointer for iterator:  does not point to owned memory */
	Nurb *iter_pointer;
} V24_BPy_Curve;


/*
 *  protoypes
 */

PyObject *V24_Curve_Init( void );
PyObject *V24_Curve_CreatePyObject( struct Curve * curve );
struct Curve *Curve_FromPyObject( PyObject * py_obj );
PyObject *V24_Curve_update( V24_BPy_Curve * self );

PyObject *V24_Curve_getName( V24_BPy_Curve * self );
PyObject *V24_Curve_setName( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getMode( V24_BPy_Curve * self );
PyObject *V24_Curve_setMode( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getBevresol( V24_BPy_Curve * self );
PyObject *V24_Curve_setBevresol( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getResolu( V24_BPy_Curve * self );
PyObject *V24_Curve_setResolu( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getResolv( V24_BPy_Curve * self );
PyObject *V24_Curve_setResolv( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getExt1( V24_BPy_Curve * self );
PyObject *V24_Curve_setExt1( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getExt2( V24_BPy_Curve * self );
PyObject *V24_Curve_setExt2( V24_BPy_Curve * self, PyObject * args );
PyObject *V24_Curve_getWidth( V24_BPy_Curve * self );
PyObject *V24_Curve_setWidth( V24_BPy_Curve * self, PyObject * args );

#endif				/* EXPP_CURVE_H */
