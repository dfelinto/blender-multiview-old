/* 
 * $Id: Scene.h 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * Contributor(s): Willian P. Germano
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_SCENE_H
#define EXPP_SCENE_H

#include <Python.h>
#include "DNA_scene_types.h"

/* checks for the scene being removed */
#define SCENE_DEL_CHECK_PY(bpy_scene) if (!(bpy_scene->scene)) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "Scene has been removed" ) )
#define SCENE_DEL_CHECK_INT(bpy_scene) if (!(bpy_scene->scene)) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "Scene has been removed" ) )

/* The Scene PyType Object defined in Scene.c */
extern PyTypeObject BPyScene_Type;

#define BPyScene_Check(v) PyObject_TypeCheck(v, &BPyScene_Type)

/*---------------------------Python BPySceneObject structure definition----------*/
typedef struct {
	PyObject_HEAD 
	Scene * scene; /* libdata must be second */
	
	/* Since there is only ever once scene PyObject per Scene
	 * we can attach data here and invalidate that data on removal
	 * we cant do object seq because there can be many of them.
	 * these will have to check up with the bpyscene to make sure its not removed */
	
	PyObject * cursor; /* do this because a vector cant reference back to a scene so far */
	
} BPySceneObject;
/*---------------------------Python BPySceneObject visible prototypes-----------*/
/* Python BPyScene_Type helper functions needed by Blender (the Init function) and Object modules. */

PyObject *SceneType_Init( void );
PyObject *Scene_CreatePyObject( Scene * scene );
#define Scene_FromPyObject(py_obj) (( ( BPySceneObject * ) py_obj )->scene)

#endif				/* EXPP_SCENE_H */
