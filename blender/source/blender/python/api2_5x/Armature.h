/* 
 * $Id: Armature.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * Contributor(s): Joseph gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_ARMATURE_H
#define EXPP_ARMATURE_H

#include <Python.h>
#include "DNA_armature_types.h"

//-------------------TYPE CHECKS---------------------------------
#define BPyArmature_Check(v) PyObject_TypeCheck(v, &BPyArmature_Type)
#define BPyBonesDict_Check(v) PyObject_TypeCheck(v, &BPyBoneDict_Type)
//-------------------MODULE INIT---------------------------------
PyObject *ArmatureType_Init( void );
PyObject *BoneDictType_Init( void );
//-------------------TYPEOBJECT----------------------------------
extern PyTypeObject BPyArmature_Type;
extern PyTypeObject BPyBoneDict_Type;
//-------------------STRUCT DEFINITION---------------------------
typedef struct {
	PyObject_HEAD 
	PyObject *bonesMap;      //wrapper for bones
	PyObject *editbonesMap; //wrapper for editbones
	ListBase *bones;            //pointer to armature->bonebase
	ListBase editbones;         //allocated list of EditBones 
	short editmode_flag;       //1 = in , 0 = not in
} BPyBoneDictObject;

typedef struct {
	PyObject_HEAD 
	struct bArmature * armature;
	BPyBoneDictObject *Bones;          //BPyBoneDictObject
	PyObject *weaklist;
} BPyArmatureObject;

//-------------------VISIBLE PROTOTYPES-------------------------
PyObject *Armature_CreatePyObject(struct bArmature *armature);
PyObject *Armature_RebuildEditbones(PyObject *pyarmature);
PyObject *Armature_RebuildBones(PyObject *pyarmature);
#define Armature_FromPyObject(py_obj) (((BPyArmatureObject *)py_obj)->armature)

#endif				
