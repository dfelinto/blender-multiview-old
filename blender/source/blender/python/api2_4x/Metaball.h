/* 
 * $Id: Metaball.h 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Jacques Guignot
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#ifndef V24_EXPP_METABALL_H
#define V24_EXPP_METABALL_H

#include <Python.h>
#include "DNA_meta_types.h"


extern PyTypeObject V24_Metaball_Type;

#define V24_BPy_Metaball_Check(v) ((v)->ob_type==&V24_Metaball_Type)


/* Python V24_BPy_Metaball structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	MetaBall * metaball; /* libdata must be second */
} V24_BPy_Metaball;


extern PyTypeObject V24_Metaelem_Type;

#define V24_BPy_Metaelem_Check(v) ((v)->ob_type==&V24_Metaelem_Type)

/* Python V24_BPy_Metaelem structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	MetaElem * metaelem;
} V24_BPy_Metaelem;

extern PyTypeObject V24_MetaElemSeq_Type;

#define V24_BPy_MetaElemSeq_Check(v) ((v)->ob_type==&V24_MetaElemSeq_Type)

/* Python V24_BPy_MetaElemSeq structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	V24_BPy_Metaball *bpymetaball; /* link to the python group so we can know if its been removed */
	MetaElem * iter; /* so we can iterate over the objects */
} V24_BPy_MetaElemSeq;

/*
 * prototypes
 */

PyObject *V24_Metaball_Init( void );
PyObject *V24_Metaball_CreatePyObject( MetaBall * mball );
MetaBall *V24_Metaball_FromPyObject( PyObject * py_obj );

#endif				/* EXPP_METABALL_H */
