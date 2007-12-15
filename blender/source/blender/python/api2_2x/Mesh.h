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

/* Most of this file comes from opy_nmesh.[ch] in the old bpython dir */

#ifndef EXPP_MESH_H
#define EXPP_MESH_H

#include <Python.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "DNA_object_types.h"
#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"
#include "Material.h"
#include "Image.h"

/* EXPP PyType Objects */
extern PyTypeObject V24_Mesh_Type;
extern PyTypeObject V24_MVert_Type;
extern PyTypeObject V24_PVert_Type;
extern PyTypeObject V24_MVertSeq_Type;
extern PyTypeObject V24_MEdge_Type;
extern PyTypeObject V24_MFace_Type;
extern PyTypeObject V24_MCol_Type;

struct V24_BPy_Object;

/* Type checking for EXPP PyTypes */
#define V24_BPy_Mesh_Check(v)       ((v)->ob_type == &V24_Mesh_Type)
#define V24_BPy_MFace_Check(v)      ((v)->ob_type == &V24_MFace_Type)
#define V24_BPy_MEdge_Check(v)      ((v)->ob_type == &V24_MEdge_Type)
#define V24_BPy_MVert_Check(v)      ((v)->ob_type == &V24_MVert_Type)
#define V24_BPy_PVert_Check(v)      ((v)->ob_type == &V24_PVert_Type)
#define V24_BPy_MCol_Check(v)       ((v)->ob_type == &V24_MCol_Type)

/* Typedefs for the new types */

typedef struct {
	PyObject_HEAD		/* required python macro   */
	MCol *color;
} V24_BPy_MCol;			    /* a Mesh color: [r,g,b,a] */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	void * data;		/* points to a Mesh or an MVert */
	int index;
} V24_BPy_MVert;			/* a Mesh vertex */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	Mesh * mesh;
	int iter;
} V24_BPy_MVertSeq;			/* a Mesh vertex sequence */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	Mesh *mesh;			/* points to a Mesh */
	int index;
	short iter;			/* char because it can only ever be between -1 and 2 */
} V24_BPy_MEdge;			/* a Mesh edge */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	Mesh * mesh;
	int iter;
} V24_BPy_MEdgeSeq;			/* a Mesh edge sequence */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	Mesh * mesh;
	int index;
	short iter;			/* char because it can only ever be between -1 and 4 */
} V24_BPy_MFace;			/* a Mesh face */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	Mesh * mesh;
	int iter;
} V24_BPy_MFaceSeq;			/* a Mesh face sequence */

typedef struct {
	PyObject_HEAD		/* required python macro   */
	Mesh *mesh;
	Object *object;
	char new;			/* was mesh created or already existed? */
} V24_BPy_Mesh;

/* PROTOS */

PyObject *V24_Mesh_Init( void );
PyObject *V24_Mesh_CreatePyObject( Mesh * me, Object *obj );
Mesh *V24_Mesh_FromPyObject( PyObject * pyobj, Object *obj );

#endif				/* EXPP_MESH_H */
