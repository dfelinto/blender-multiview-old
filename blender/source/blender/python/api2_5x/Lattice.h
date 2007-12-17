/* 
 * $Id: Lattice.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_Lattice_H
#define EXPP_Lattice_H

#include <Python.h>
#include "DNA_lattice_types.h"

/* The Group PyTypeObject defined in Lattice.c */
extern PyTypeObject BPyLattice_Type;
extern PyTypeObject BPyLPointSeq_Type;
extern PyTypeObject BPyLPoint_Type;

#define LATTICE_TOTPOINTS(lat) (lat->pntsu * lat->pntsv * lat->pntsv)

#define BPyLattice_Check(v)		PyObject_TypeCheck(v, &BPyLattice_Type)
#define BPyLPointSeq_Check(v)	PyObject_TypeCheck(v, &BPyLPointSeq_Type)
#define BPyLPoint_Check(v)		PyObject_TypeCheck(v, &BPyLPoint_Type)


/*****************************************************************************/
/* Python BPyLatticeObject structure definition:   */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD 
	Lattice * lattice; /* libdata must be second */
} BPyLatticeObject;

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyLatticeObject * bpylat;	/* points to a Python Mesh */
	int index;
} BPyLPointObject;			/* a Mesh vertex */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyLatticeObject * bpylat;	/* points to a Python Mesh */
	int iter;
} BPyLPointSeqObject;			/* a Lattice vertex sequence */

/*
 * prototypes
 */

PyObject *LatticeType_Init( void );
PyObject *LatticePointSeqType_Init( void );
PyObject *LatticePointType_Init( void );

PyObject *Lattice_CreatePyObject( Lattice * lt );
#define Lattice_FromPyObject(py_obj) (((BPyLatticeObject *)py_obj)->lattice)

#endif				/* EXPP_LATTICE_H */
