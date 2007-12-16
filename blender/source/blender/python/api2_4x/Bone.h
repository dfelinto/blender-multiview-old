/* 
 * $Id: Bone.h 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef V24_EXPP_BONE_H
#define V24_EXPP_BONE_H

#include <Python.h>
#include "DNA_armature_types.h"

/*-------------------TYPE CHECKS---------------------------------*/
#define V24_BoneObject_Check(v) ((v)->ob_type == &V24_Bone_Type)
#define V24_EditBoneObject_Check(v) ((v)->ob_type == &V24_EditBone_Type)
/*-------------------TYPEOBJECT----------------------------------*/
extern PyTypeObject V24_EditBone_Type;
extern PyTypeObject V24_Bone_Type;
/*-------------------STRUCT DEFINITION----------------------------*/

typedef struct {
	PyObject_HEAD
	Bone * bone;
} V24_BPy_Bone;

typedef struct {
	PyObject_HEAD
	struct EditBone *editbone;
	struct EditBone *parent;
	char name[32];
	float roll;
	float head[3];
	float tail[3];
	int flag;
	float dist;
	float weight;
	float xwidth;
	float zwidth;
	float ease1;
	float ease2;
	float rad_head;
	float rad_tail;
	short segments;
	short layer;
} V24_BPy_EditBone;
/*-------------------VISIBLE PROTOTYPES-------------------------*/
PyObject *V24_PyBone_FromBone(struct Bone *bone);
struct Bone *V24_PyBone_AsBone(V24_BPy_Bone *py_Bone);
PyObject *V24_PyEditBone_FromBone(Bone *bone);
PyObject *V24_PyEditBone_FromEditBone(struct EditBone *editbone);

#endif
