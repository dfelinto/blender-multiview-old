/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
/* opy_nmesh.c */

#include "DNA_mesh_types.h"

#define NMesh_Check(v)       ((v)->ob_type == &NMesh_Type)
#define NMFace_Check(v)      ((v)->ob_type == &NMFace_Type)
#define NMVert_Check(v)      ((v)->ob_type == &NMVert_Type)
#define NMCol_Check(v)       ((v)->ob_type == &NMCol_Type)

typedef struct _NMCol {
	PyObject_HEAD

	unsigned char r, g, b, a;
} NMCol;

struct PyBlock;

typedef struct _NMFace {
	PyObject_HEAD
	
	PyObject *v;
	PyObject *uv;
	PyObject *col;
	short mode;
	short flag;
	unsigned char transp;
	DataBlock *tpage; /* Image */
	char mat_nr, smooth;
} NMFace;

typedef struct _NMesh {
	PyObject_HEAD
	Mesh *mesh;
	PyObject *name;
	PyObject *materials;
	PyObject *verts;
	PyObject *faces;
	int sel_face; /* XXX remove */
	char flags;
#define NMESH_HASMCOL	1<<0
#define NMESH_HASVERTUV	1<<1
#define NMESH_HASFACEUV	1<<2

} NMesh;

typedef struct _NMVert {
	PyObject_VAR_HEAD

	float co[3];
	float no[3];
	float uvco[3];
	
	int index;
} NMVert;


/* PROTOS */

PyObject *newNMesh(Mesh *oldmesh);
Mesh *Mesh_fromNMesh(NMesh *nmesh);
PyObject *NMesh_assignMaterials_toObject(NMesh *nmesh, Object *ob);
Material **nmesh_updateMaterials(NMesh *nmesh);
