/* 
 * $Id: bpy_types.c 11035 2007-06-24 20:03:55Z campbellbarton $
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.
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

#include "bpy_types.h"

#include "MEM_guardedalloc.h"	/* for MEM_callocN */
#include "DNA_space_types.h"	/* SPACE_VIEW3D, SPACE_SEQ */
#include "DNA_scene_types.h"
#include "DNA_object_types.h" /* LibBlockSeq_new */
#include "DNA_texture_types.h"
#include "DNA_curve_types.h"
#include "DNA_ipo_types.h"
#include "DNA_group_types.h"
#include "DNA_world_types.h"
#include "DNA_vfont_types.h"
#include "DNA_armature_types.h"
#include "DNA_sound_types.h"
#include "DNA_text_types.h"
#include "DNA_action_types.h"
#include "DNA_meta_types.h"

/* Use the add_* from BKE_* */
#include "BKE_global.h"
#include "BKE_utildefines.h" /* clamp */
#include "BKE_armature.h"
#include "BKE_ipo.h"
#include "BKE_image.h"
#include "BKE_main.h"
#include "BKE_library.h"
#include "BKE_lattice.h"
#include "BKE_object.h"
#include "BKE_scene.h"
#include "BKE_sca.h" /*free_text_controllers*/
#include "BKE_font.h"
#include "BKE_mball.h"
#include "BKE_mesh.h"
#include "BKE_curve.h"
#include "BKE_material.h"
#include "BKE_group.h"
#include "BKE_text.h"
#include "BKE_texture.h"
#include "BKE_world.h"

#include "BLI_blenlib.h" /* BLI_countlist */
#include "BIF_drawscene.h"	/* for set_scene */
#include "BIF_screen.h"		/* curarea */
#include "BIF_drawimage.h" /* what image */
#include "BIF_drawtext.h" /* unlink_text */
#include "BIF_editsound.h" /* sound_new_sound */
#include "BIF_editaction.h" /* add_empty_action */

/* python types */
#include "../BPY_extern.h" /* clearing scriptlinks */

#include "gen_utils.h"
#include "gen_library.h" /* generic ID functions */

#include "Bone.h"
#include "Object.h"
#include "Camera.h"
#include "Armature.h"
#include "Lamp.h"
#include "CurveBase.h"
#include "Curve.h"
#include "CurNurb.h"
#include "Mesh.h"
#include "Lattice.h"
#include "Metaball.h"
#include "Text.h"
#include "CurveText.h"
#include "Font.h"
#include "Group.h"
#include "World.h"
#include "Texture.h"
#include "Ipo.h"
#include "Text.h"
#include "Sound.h"
#include "sceneObSeq.h"
#include "NLA.h"
#include "Scene.h"
#include "BezTriple.h"
#include "Effect.h"
#include "Image.h"
#include "Key.h"
#include "Ipo.h"
#include "Ipocurve.h"
#include "MTex.h"
#include "MTexSeq.h"
#include "Modifier.h"
#include "CurveText.h"
#include "Font.h"
#include "Constraint.h"
#include "color.h"
#include "Pose.h"
#include "Const.h"
#include "sceneRender.h"
#include "sceneTimeLine.h"
#include "sceneSequence.h"
#include "bpy_gl.h"
#include "bpy_config.h"
#include "bpy_list.h"
#include "bpy_float.h"
#include "layer_set.h"
#include "bpy_data.h"
#include "charRGBA.h"
#include "Draw.h"
#include "logic.h"
#include "object_softbody.h"
#include "Particle.h"
#include "SurfNurb.h"
#include "windowTheme.h"


#include "vector.h"
#include "matrix.h"
#include "quat.h"
#include "euler.h"

#include "Library.h"


/* used only for texts.active */
#include "BIF_screen.h"
#include "DNA_space_types.h"
#include "DNA_screen_types.h"

/*-----------------------------BPy module Init())-----------------------------*/

PyObject * m_types_init( void )
{
	PyObject *module;

	/* init base types that others subtype but arnt used direct */
	module = Py_InitModule3( "bpy.types", NULL, "The bpy.types submodule" );
	

	/* These are in the order they appier in a search of \nPyTypeObject 
	 * to make it easier to account for all types - please try keep it this order  */
	PyModule_AddObject( module, "BoneDict",			BoneDictType_Init() );
	PyModule_AddObject( module, "Armature",			ArmatureType_Init() );
	PyModule_AddObject( module, "BezTrible",		BezTripleType_Init() );
	PyModule_AddObject( module, "GLBuffer",			GLBufferType_Init() ); /* TODO add type INIT */
	PyModule_AddObject( module, "EditBone",			EditBoneType_Init() );
	PyModule_AddObject( module, "Bone",				BoneType_Init() );
	
	PyModule_AddObject( module, "Config",			ConfigType_Init() ); /* Why would we want to access this? */
	PyModule_AddObject( module, "LibBlockSeq",		LibBlockSeqType_Init() );
	
	PyModule_AddObject( module, "BPyFloat",			BPyFloatType_Init() );
	BPyList_Init();
	PyModule_AddObject( module, "BPyList",			BPyListType_Init() );
	
	PyModule_AddObject( module, "Camera",			CameraType_Init() );
	PyModule_AddObject( module, "CharRGBA",			CharRGBAType_Init() ); /* only used for themes */
	PyModule_AddObject( module, "Color",			ColorType_Init() );
	PyModule_AddObject( module, "Const",			ConstType_Init() );
	PyModule_AddObject( module, "ConstCategory",	ConstCategoryType_Init() );
	PyModule_AddObject( module, "Constraint",		ConstraintType_Init() );
	PyModule_AddObject( module, "CurNurb",			CurNurbType_Init() ); /* TODO - this is a silly name - Cam */
	PyModule_AddObject( module, "Curve",			CurveType_Init() );
	CurveBaseType_Init();													/* no user access */
	PyModule_AddObject( module, "Text3d",			CurveTextType_Init() );
	PyModule_AddObject( module, "Button",			ButtonType_Init() );
	PyModule_AddObject( module, "Effect",			EffectType_Init() );
	PyModule_AddObject( module, "Euler",			EulerType_Init() );
	PyModule_AddObject( module, "Font",				FontType_Init() );
	GenericLibType_Init();													/* no user access */
	PyModule_AddObject( module, "Group",			GroupType_Init() );
	 PyModule_AddObject( module, "GroupSeq",		GroupSeqType_Init() );
	PyModule_AddObject( module, "Image",			ImageType_Init() );
	PyModule_AddObject( module, "Ipo",				IpoType_Init() );
	PyModule_AddObject( module, "IpoCurve",			IpoCurveType_Init() );
	PyModule_AddObject( module, "Key",				KeyType_Init() );
	PyModule_AddObject( module, "KeyBlock",			KeyBlockType_Init() );
	PyModule_AddObject( module, "Lamp",				LampType_Init() );
	PyModule_AddObject( module, "Lattice",			LatticeType_Init() );
	PyModule_AddObject( module, "LVertSeq",			LatticePointSeqType_Init() );
	PyModule_AddObject( module, "LVert",			LatticePointType_Init() );
	
	LayerSet_Init(); /* dont run this more then once */
	PyModule_AddObject( module, "LayerSet",			LayerSetType_Init() );
	PyModule_AddObject( module, "Library",			LibraryType_Init() );
	PyModule_AddObject( module, "LibraryData",		LibraryDataType_Init() );

	PyModule_AddObject( module, "GameProperty",		GamePropertyType_Init() ); /* TODO - rename other parts to GameProperty */
	PyModule_AddObject( module, "Material",			MaterialType_Init() );
	PyModule_AddObject( module, "Matrix",			MatrixType_Init() );
	
	PyModule_AddObject( module, "Mesh",				MeshType_Init() );
	PyModule_AddObject( module, "MVert",			MVertType_Init() );
	PyModule_AddObject( module, "PVert",			PVertType_Init() );
	PyModule_AddObject( module, "MVertSeq",			MVertSeqType_Init() );
	PyModule_AddObject( module, "MEdge",			MEdgeType_Init() );
	PyModule_AddObject( module, "MEdgeSeq",			MEdgeSeqType_Init() );
	PyModule_AddObject( module, "MFace",			MFaceType_Init() );
	PyModule_AddObject( module, "MFaceSeq",			MFaceSeqType_Init() );
	
	PyModule_AddObject( module, "MetaBall",			MetaballType_Init() );
	 PyModule_AddObject( module, "MetaElemSeq",		MetaelemSeqType_Init() );
	 PyModule_AddObject( module, "MetaElem",		MetaelemType_Init() );
	
	PyModule_AddObject( module, "Modifier",			ModifierType_Init() );
	PyModule_AddObject( module, "ModifierSeq",		ModifierSeqType_Init() );
	
	PyModule_AddObject( module, "MTex",				MTexType_Init() );
	PyModule_AddObject( module, "MTexSeq",			MTexSeqType_Init() );
	
	PyModule_AddObject( module, "Action",			ActionType_Init() );
	PyModule_AddObject( module, "ActionStrip",		ActionStripType_Init() );
	PyModule_AddObject( module, "ActionStripSeq",	ActionStripSeqType_Init() );
	
	PyModule_AddObject( module, "SoftBody",			SoftBodyType_Init() );
	PyModule_AddObject( module, "Object",			ObjectType_Init() );
	PyModule_AddObject( module, "Particle",			ParticleType_Init() );
	
	PyModule_AddObject( module, "Pose",				PoseType_Init() );
	PyModule_AddObject( module, "PoseBone",			PoseBoneType_Init() );
	PyModule_AddObject( module, "PoseBoneDict",		PoseBoneDictType_Init() );
	
	PyModule_AddObject( module, "Quat",				QuatType_Init() ); /* should this be called Quaternion? */
	PyModule_AddObject( module, "Scene",			SceneType_Init() );
	 PyModule_AddObject( module, "SceneObSeq",		SceneObSeqType_Init() );
	 PyModule_AddObject( module, "Render",			RenderType_Init() );
	 PyModule_AddObject( module, "TimeLine",		TimeLineType_Init() );
	
	PyModule_AddObject( module, "Sequence",			SequenceType_Init() );
	PyModule_AddObject( module, "SceneSeq",			SceneSeqType_Init() );
	
	PyModule_AddObject( module, "Sound",			SoundType_Init() );
	PyModule_AddObject( module, "SurfNurb",			SurfNurbType_Init() );
	PyModule_AddObject( module, "Text",				TextType_Init() );
	PyModule_AddObject( module, "Texture",			TextureType_Init() );
	PyModule_AddObject( module, "Vector",			VectorType_Init() );
	
	PyModule_AddObject( module, "ThemeUI",			ThemeUIType_Init() );
	PyModule_AddObject( module, "ThemeSpace",		ThemeSpaceType_Init() );
	PyModule_AddObject( module, "Theme",			ThemeType_Init() );
	
	PyModule_AddObject( module, "World",			WorldType_Init() );
	
	/* users never see this type, only its subtypes, just make sure its initialized */
	PyModule_AddObject( module, "SurfNurb",			SurfNurbType_Init() );
	
	return module;
}
