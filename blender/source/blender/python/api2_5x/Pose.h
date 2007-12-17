/* 
 * $Id: 
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

#ifndef EXPP_POSE_H
#define EXPP_POSE_H

#include <Python.h>
#include "DNA_action_types.h"
#include "DNA_object_types.h"
#include "Object.h"

//-------------------TYPE CHECKS---------------------------------
#define BPyPose_Check(v)			PyObject_TypeCheck(v, &BPyPose_Type)
#define BPyPoseBone_Check(v)		PyObject_TypeCheck(v, &BPyPoseBone_Type)
#define BPyPoseBoneDict_Check(v)	PyObject_TypeCheck(v, &BPyPoseBoneDict_Type)
//-------------------TYPEOBJECT----------------------------------
extern PyTypeObject BPyPose_Type;
extern PyTypeObject BPyPoseBone_Type;
extern PyTypeObject BPyPoseBoneDict_Type;
//-------------------STRUCT DEFINITION----------------------------
typedef struct {
	PyObject_HEAD 
	PyObject *bonesMap;
	ListBase *bones;  
} BPyPoseBoneDictObject;

typedef struct {
	PyObject_HEAD
	BPyObject *bpyob;
	BPyPoseBoneDictObject *Bones; 
} BPyPoseObject;

typedef struct {
	PyObject_HEAD
	bPoseChannel *posechannel;
	
} BPyPoseBoneObject;

//-------------------VISIBLE PROTOTYPES-------------------------
void Pose_Init(void);

PyObject *PoseType_Init(void);
PyObject *PoseBoneType_Init(void);
PyObject *PoseBoneDictType_Init(void);

PyObject *Pose_CreatePyObject(BPyObject *bpyob);
PyObject *PyPoseBone_FromPosechannel(bPoseChannel *pchan);
Object *Object_FromPoseChannel(bPoseChannel *curr_pchan);
#endif
