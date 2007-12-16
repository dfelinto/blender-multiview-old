/* 
 * $Id: Types.c 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Willian P. Germano, Alex Mole, Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Types.h" 
#include "IDProp.h"
/* 
   stuff pasted from the old Types.h
   is only need here now
*/

extern PyTypeObject V24_IDGroup_Type, V24_IDArray_Type;
extern PyTypeObject V24_Action_Type, V24_Armature_Type;
extern PyTypeObject V24_Pose_Type;
extern PyTypeObject V24_BezTriple_Type, V24_Bone_Type, V24_Button_Type;
extern PyTypeObject V24_Camera_Type;
extern PyTypeObject V24_CurNurb_Type, V24_SurfNurb_Type;
extern PyTypeObject V24_Curve_Type;
extern PyTypeObject V24_Effect_Type, V24_Font_Type;
extern PyTypeObject V24_Image_Type, V24_Ipo_Type, V24_IpoCurve_Type;
extern PyTypeObject V24_Lamp_Type, V24_Lattice_Type;
extern PyTypeObject V24_Material_Type, V24_Metaball_Type, V24_MTex_Type;
extern PyTypeObject NMFace_Type, NMEdge_Type, NMVert_Type, NMCol_Type,
	   NMesh_Type;
extern PyTypeObject V24_MFace_Type, V24_MVert_Type, V24_PVert_Type, V24_MEdge_Type, V24_MCol_Type,
	   V24_Mesh_Type;

extern PyTypeObject V24_Object_Type;
extern PyTypeObject V24_Group_Type;
extern PyTypeObject V24_Particle_Type;
extern PyTypeObject V24_Scene_Type, V24_RenderData_Type;
extern PyTypeObject V24_Text_Type, V24_Text3d_Type, V24_Texture_Type;
extern PyTypeObject V24_World_Type;
extern PyTypeObject V24_property_Type;
extern PyTypeObject V24_buffer_Type, constant_Type, euler_Type;
extern PyTypeObject V24_matrix_Type, V24_quaternion_Type, V24_rgbTuple_Type, V24_vector_Type;
extern PyTypeObject V24_point_Type;
extern PyTypeObject V24_Modifier_Type, V24_ModSeq_Type;
extern PyTypeObject V24_EditBone_Type;
extern PyTypeObject V24_ThemeSpace_Type;
extern PyTypeObject V24_ThemeUI_Type;
extern PyTypeObject V24_TimeLine_Type;

char V24_M_Types_doc[] = "The Blender Types module\n\n\
This module is a dictionary of all Blender Python types";

struct PyMethodDef V24_Null_methods[] = { {NULL, NULL, 0, NULL} };



/* The internal types (lowercase first letter, like constant_Type) are only
 * set when some object initializes them.  But unless we do it early, we get
 * some easy and unpredictable (varies with platform, even distro) ways to
 * crash Blender.  Some modules also need this early up, so let's generalize
 * and init all our pytypes here. 
 */

void types_InitAll( void )
{
	V24_Action_Type.ob_type = &PyType_Type;
	V24_Pose_Type.ob_type = &PyType_Type;
	V24_Armature_Type.ob_type = &PyType_Type;
	V24_BezTriple_Type.ob_type = &PyType_Type;
	V24_Bone_Type.ob_type = &PyType_Type;
	V24_Button_Type.ob_type = &PyType_Type;
	V24_Camera_Type.ob_type = &PyType_Type;
	V24_CurNurb_Type.ob_type = &PyType_Type;
	V24_Curve_Type.ob_type = &PyType_Type;
	V24_Effect_Type.ob_type = &PyType_Type;
	V24_Image_Type.ob_type = &PyType_Type;
	V24_Ipo_Type.ob_type = &PyType_Type;
	V24_IpoCurve_Type.ob_type = &PyType_Type;
	V24_Lamp_Type.ob_type = &PyType_Type;
	V24_Lattice_Type.ob_type = &PyType_Type;
	V24_Material_Type.ob_type = &PyType_Type;
	V24_Metaball_Type.ob_type = &PyType_Type;
	V24_MTex_Type.ob_type = &PyType_Type;
	NMCol_Type.ob_type = &PyType_Type;
	NMFace_Type.ob_type = &PyType_Type;
	NMEdge_Type.ob_type = &PyType_Type;
	NMVert_Type.ob_type = &PyType_Type;
	NMesh_Type.ob_type = &PyType_Type;
	V24_MFace_Type.ob_type = &PyType_Type;
   	V24_MVert_Type.ob_type = &PyType_Type;
   	V24_PVert_Type.ob_type = &PyType_Type;
   	V24_MEdge_Type.ob_type = &PyType_Type;
   	V24_MCol_Type.ob_type = &PyType_Type;
   	V24_Mesh_Type.ob_type = &PyType_Type;
	V24_Object_Type.ob_type = &PyType_Type;
	V24_Group_Type.ob_type = &PyType_Type;
	V24_RenderData_Type.ob_type = &PyType_Type;
	V24_Scene_Type.ob_type = &PyType_Type;
	V24_SurfNurb_Type.ob_type = &PyType_Type;
	V24_Text_Type.ob_type = &PyType_Type;
	V24_Text3d_Type.ob_type = &PyType_Type;
	V24_Texture_Type.ob_type = &PyType_Type;
	//V24_TimeLine_Type.ob_type = &PyType_Type;
	V24_World_Type.ob_type = &PyType_Type;
	V24_buffer_Type.ob_type = &PyType_Type;
	constant_Type.ob_type = &PyType_Type;
	euler_Type.ob_type = &PyType_Type;
	V24_matrix_Type.ob_type = &PyType_Type;
	V24_quaternion_Type.ob_type = &PyType_Type;
	PyType_Ready( &V24_rgbTuple_Type );
	V24_vector_Type.ob_type = &PyType_Type;
	V24_property_Type.ob_type = &PyType_Type;
	V24_point_Type.ob_type = &PyType_Type;
	PyType_Ready( &V24_Modifier_Type );
	PyType_Ready( &V24_ModSeq_Type );
	PyType_Ready( &V24_EditBone_Type );
	PyType_Ready( &V24_ThemeSpace_Type );
	PyType_Ready( &V24_ThemeUI_Type );
	V24_IDProp_Init_Types();
}

/*****************************************************************************/
/* Function:	 V24_Types_Init					 	*/
/*****************************************************************************/
PyObject *V24_Types_Init( void )
{
	PyObject *V24_submodule, *dict;

	V24_submodule =
		Py_InitModule3( "Blender.Types", V24_Null_methods, V24_M_Types_doc );

	dict = PyModule_GetDict( V24_submodule );

	/* The Blender Object Type */

	PyDict_SetItemString( dict, "ObjectType",
			      ( PyObject * ) &V24_Object_Type );

	/* Blender Object Data Types */

	PyDict_SetItemString( dict, "GroupType",
			      ( PyObject * ) &V24_Group_Type );

	PyDict_SetItemString( dict, "SceneType", ( PyObject * ) &V24_Scene_Type );
	PyDict_SetItemString( dict, "RenderDataType",
			      ( PyObject * ) &V24_RenderData_Type );

	PyDict_SetItemString( dict, "NMeshType", ( PyObject * ) &NMesh_Type );
	PyDict_SetItemString( dict, "NMFaceType",
			      ( PyObject * ) &NMFace_Type );
	PyDict_SetItemString( dict, "NMVertType",
			      ( PyObject * ) &NMVert_Type );
	PyDict_SetItemString( dict, "NMEdgeType",
			      ( PyObject * ) &NMEdge_Type );
	PyDict_SetItemString( dict, "NMColType", ( PyObject * ) &NMCol_Type );

	PyDict_SetItemString( dict, "MeshType", ( PyObject * ) &V24_Mesh_Type );
	PyDict_SetItemString( dict, "MFaceType",
			      ( PyObject * ) &V24_MFace_Type );
	PyDict_SetItemString( dict, "MEdgeType",
			      ( PyObject * ) &V24_MEdge_Type );
	PyDict_SetItemString( dict, "MVertType",
			      ( PyObject * ) &V24_MVert_Type );
	PyDict_SetItemString( dict, "PVertType",
			      ( PyObject * ) &V24_PVert_Type );
	PyDict_SetItemString( dict, "MColType", ( PyObject * ) &V24_MCol_Type );

	PyDict_SetItemString( dict, "ArmatureType",
			      ( PyObject * ) &V24_Armature_Type );
	PyDict_SetItemString( dict, "BoneType", ( PyObject * ) &V24_Bone_Type );

	PyDict_SetItemString( dict, "V24_CurNurb_Type",
			      ( PyObject * ) &V24_CurNurb_Type );
	PyDict_SetItemString( dict, "V24_SurfNurb_Type",
			      ( PyObject * ) &V24_SurfNurb_Type );
	PyDict_SetItemString( dict, "CurveType", ( PyObject * ) &V24_Curve_Type );

	PyDict_SetItemString( dict, "IpoType", ( PyObject * ) &V24_Ipo_Type );
	PyDict_SetItemString( dict, "MetaballType",
			      ( PyObject * ) &V24_Metaball_Type );

	PyDict_SetItemString( dict, "CameraType",
			      ( PyObject * ) &V24_Camera_Type );
	PyDict_SetItemString( dict, "ImageType", ( PyObject * ) &V24_Image_Type );
	PyDict_SetItemString( dict, "LampType", ( PyObject * ) &V24_Lamp_Type );
	PyDict_SetItemString( dict, "TextType", ( PyObject * ) &V24_Text_Type );
	PyDict_SetItemString( dict, "Text3dType", ( PyObject * ) &V24_Text3d_Type );
	PyDict_SetItemString( dict, "MaterialType",
			      ( PyObject * ) &V24_Material_Type );

	PyDict_SetItemString( dict, "ButtonType",
			      ( PyObject * ) &V24_Button_Type );

	PyDict_SetItemString( dict, "LatticeType",
			      ( PyObject * ) &V24_Lattice_Type );

	PyDict_SetItemString( dict, "TextureType",
			      ( PyObject * ) &V24_Texture_Type );
	PyDict_SetItemString( dict, "MTexType", ( PyObject * ) &V24_MTex_Type );

	/* External helper Types available to the main ones above */

	PyDict_SetItemString( dict, "vectorType",
			      ( PyObject * ) &V24_vector_Type );
	PyDict_SetItemString( dict, "bufferType",
			      ( PyObject * ) &V24_buffer_Type );
	PyDict_SetItemString( dict, "constantType",
			      ( PyObject * ) &constant_Type );
	PyDict_SetItemString( dict, "rgbTupleType",
			      ( PyObject * ) &V24_rgbTuple_Type );
	PyDict_SetItemString( dict, "V24_matrix_Type",
			      ( PyObject * ) &V24_matrix_Type );
	PyDict_SetItemString( dict, "eulerType", ( PyObject * ) &euler_Type );
	PyDict_SetItemString( dict, "quaternionType",
			      ( PyObject * ) &V24_quaternion_Type );
	PyDict_SetItemString( dict, "BezTripleType",
			      ( PyObject * ) &V24_BezTriple_Type );
	PyDict_SetItemString( dict, "ActionType",
			      ( PyObject * ) &V24_Action_Type );
	PyDict_SetItemString( dict, "PoseType",
			      ( PyObject * ) &V24_Pose_Type );
	PyDict_SetItemString( dict, "propertyType",
			      ( PyObject * ) &V24_property_Type );
	PyDict_SetItemString( dict, "pointType",
			      ( PyObject * ) &V24_point_Type );
	PyDict_SetItemString( dict, "ModifierType",
			      ( PyObject * ) &V24_Modifier_Type );
	PyDict_SetItemString( dict, "ModSeqType",
			      ( PyObject * ) &V24_ModSeq_Type );
	PyDict_SetItemString( dict, "EditBoneType",
			      ( PyObject * ) &V24_EditBone_Type);
	PyDict_SetItemString( dict, "ThemeSpaceType",
			      ( PyObject * ) &V24_ThemeSpace_Type);
	PyDict_SetItemString( dict, "V24_ThemeUI_Type",
			      ( PyObject * ) &V24_ThemeUI_Type);
	PyDict_SetItemString( dict, "IDGroupType",
			      ( PyObject * ) &V24_IDGroup_Type);
	PyDict_SetItemString( dict, "IDArrayType",
			      ( PyObject * ) &V24_IDArray_Type);
	return V24_submodule;
}
