/*
 * $Id: Mesh.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
extern PyTypeObject BPyMesh_Type;
extern PyTypeObject BPyMVert_Type;
extern PyTypeObject BPyPVert_Type;
extern PyTypeObject MVertSeq_Type;
extern PyTypeObject BPyMEdge_Type;
extern PyTypeObject BPyMFace_Type;

struct BPyObject;

/* Type checking for EXPP PyTypes */
#define BPyMesh_Check(v)		PyObject_TypeCheck(v, &BPyMesh_Type)
#define BPyMFace_Check(v)		PyObject_TypeCheck(v, &BPyMFace_Type)
#define BPyMEdge_Check(v)		PyObject_TypeCheck(v, &BPyMEdge_Type)
#define BPyMVert_Check(v)		PyObject_TypeCheck(v, &BPyMVert_Type)
#define BPyPVert_Check(v)		PyObject_TypeCheck(v, &BPyPVert_Type)

/* Typedefs for the new types */

typedef struct {
	PyObject_HEAD		/* required python macro   */
	Mesh *mesh;
	Object *object;
	char new;			/* was mesh created or already existed? */
} BPyMeshObject;

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyMeshObject * bpymesh;	/* points to a Python Mesh */
	int index;
} BPyMVertObject;			/* a Mesh vertex */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyMeshObject * bpymesh;	/* points to a Python Mesh */
	int iter;
} BPyMVertSeqObject;			/* a Mesh vertex sequence */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyMeshObject *bpymesh;	/* points to a Python Mesh */
	int index;
	short iter;			/* char because it can only ever be between -1 and 2 */
} BPyMEdgeObject;			/* a Mesh edge */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyMeshObject * bpymesh;	/* points to a Python Mesh */
	int iter;
} BPyMEdgeSeqObject;			/* a Mesh edge sequence */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyMeshObject * bpymesh;	/* points to a Python Mesh */
	int index;
	short iter;			/* char because it can only ever be between -1 and 4 */
} BPyMFaceObject;			/* a Mesh face */

typedef struct {
	PyObject_VAR_HEAD	/* required python macro   */
	BPyMeshObject * bpymesh;	/* points to a Python Mesh */
	int iter;
} BPyMFaceSeqObject;			/* a Mesh face sequence */

/* PROTOS */

PyObject *Mesh_CreatePyObject( Mesh * me, Object *obj );
Mesh *Mesh_FromPyObject( PyObject * pyobj, Object *obj );

PyObject *MeshType_Init( void );
PyObject *MVertType_Init( void );
PyObject *PVertType_Init( void );
PyObject *MVertSeqType_Init( void );
PyObject *MEdgeType_Init( void );
PyObject *MEdgeSeqType_Init( void );
PyObject *MFaceType_Init( void );
PyObject *MFaceSeqType_Init( void );



#endif				/* EXPP_MESH_H */
