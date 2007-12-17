/* 
 * $Id: Scene.h 11362 2007-07-24 19:01:30Z campbellbarton $
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
 * Contributor(s): Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_SCENEOBSEQ_H
#define EXPP_SCENEOBSEQ_H

#include <Python.h>
#include "Scene.h"
#include "DNA_scene_types.h"

extern PyTypeObject SceneObSeq_Type;

#define BPySceneObSeq_Check(v) PyObject_TypeCheck(v, &SceneObSeq_Type)

/* Scene object sequence, iterate on the scene object listbase 
 * note that this must incref the scene it comes from so its removal check is valid */
typedef struct {
	PyObject_VAR_HEAD /* required python macro   */
	BPySceneObject *bpyscene; /* link to the python scene so we can know if its been removed */
	int index; /* so we can iterate over the objects */
	char mode; /*0:all objects, 1:selected objects, 2:user context*/
} BPySceneObSeqObject;

PyObject *SceneObSeqType_Init( void );
PyObject *Scene_object_new( Scene * scene, PyObject *args );
PyObject *SceneObSeq_CreatePyObject( BPySceneObject *self, int mode);

#endif				/* EXPP_SCENEOBSEQ_H */
