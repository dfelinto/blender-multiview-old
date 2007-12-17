/* 
 * $Id: Material.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * Contributor(s): Willian P. Germano, Michel Selten, Alex Mole,
 * Alexander Szakaly, Campbell Barton, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Material.h" /*This must come first*/

#include "DNA_space_types.h"
#include "DNA_material_types.h"
#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_material.h"
#include "BKE_texture.h"
#include "BKE_idprop.h"
#include "BKE_utildefines.h" /* for CLAMP */
#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"
#include "BSE_editipo.h"
#include "BIF_space.h"
#include "mydevice.h"
#include "MTex.h"
#include "MTexSeq.h"
#include "Texture.h"
#include "Ipo.h"
#include "Group.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "IDProp.h"
#include "bpy_list.h"
#include "bpy_float.h"
#include "Const.h"
/* closure values for getColorComponent()/setColorComponent() */

#define IPOKEY_RGB          0
#define IPOKEY_ALPHA        1 
#define IPOKEY_HALOSIZE     2 
#define IPOKEY_MODE         3
#define IPOKEY_ALLCOLOR     10
#define IPOKEY_ALLMIRROR    14
#define IPOKEY_OFS          12
#define IPOKEY_SIZE         13
#define IPOKEY_ALLMAPPING   11


enum mat_float_consts {
	EXPP_MAT_COMP_R = 0,
	EXPP_MAT_COMP_G,
	EXPP_MAT_COMP_B,
	EXPP_MAT_COMP_SPECR,
	EXPP_MAT_COMP_SPECG,
	EXPP_MAT_COMP_SPECB,
	EXPP_MAT_COMP_MIRR,
	EXPP_MAT_COMP_MIRG,
	EXPP_MAT_COMP_MIRB,
	EXPP_MAT_COMP_SSSR,
	EXPP_MAT_COMP_SSSG,
	EXPP_MAT_COMP_SSSB,
	EXPP_MAT_ATTR_ADD,
	EXPP_MAT_ATTR_ALPHA,
	EXPP_MAT_ATTR_SHADOWALPHA,
	EXPP_MAT_ATTR_AMB,
	EXPP_MAT_ATTR_EMIT,
	EXPP_MAT_ATTR_REF,
	EXPP_MAT_ATTR_SPEC,
	EXPP_MAT_ATTR_SPECTRA,
	EXPP_MAT_ATTR_ROUGHNESS,
	EXPP_MAT_ATTR_SPECSIZE,
	EXPP_MAT_ATTR_DIFFUSESIZE,
	EXPP_MAT_ATTR_SPECSMOOTH,
	EXPP_MAT_ATTR_DIFFUSESMOOTH,
	EXPP_MAT_ATTR_DIFFUSE_DARKNESS,
	EXPP_MAT_ATTR_REFRACINDEX,
	EXPP_MAT_ATTR_RMS,
	EXPP_MAT_ATTR_HALOSIZE,
	EXPP_MAT_ATTR_FLARESIZE,
	EXPP_MAT_ATTR_FLAREBOOST,
	EXPP_MAT_ATTR_SUBSIZE,	
	EXPP_MAT_ATTR_RAYMIRR,
	EXPP_MAT_ATTR_FRESNELMIRR,
	EXPP_MAT_ATTR_FRESNELMIRRFAC,
	EXPP_MAT_ATTR_FILTER,
	EXPP_MAT_ATTR_TRANSLUCENCY,
	EXPP_MAT_ATTR_ZOFFS,
	EXPP_MAT_ATTR_IOR,
	EXPP_MAT_ATTR_FRESNELTRANS,
	EXPP_MAT_ATTR_FRESNELTRANSFAC,
	EXPP_MAT_ATTR_SPECTRANS,
	EXPP_MAT_ATTR_MIRRTRANSADD,
	EXPP_MAT_ATTR_RB_FRICTION,
	EXPP_MAT_ATTR_RB_RESTITUTION,
	EXPP_MAT_ATTR_SSS_SCALE,
	EXPP_MAT_ATTR_SSS_RADIUS_RED,
	EXPP_MAT_ATTR_SSS_RADIUS_GREEN,
	EXPP_MAT_ATTR_SSS_RADIUS_BLUE,
	EXPP_MAT_ATTR_SSS_IOR,
	EXPP_MAT_ATTR_SSS_ERROR,
	EXPP_MAT_ATTR_SSS_COLBLEND,
	EXPP_MAT_ATTR_SSS_TEXSCATTER,
	EXPP_MAT_ATTR_SSS_FRONT,
	EXPP_MAT_ATTR_SSS_BACK,
};

/* int/short/char */
enum mat_int_consts {
	EXPP_MAT_ATTR_TRANSDEPTH = 0,
	EXPP_MAT_ATTR_MIRRDEPTH,
	EXPP_MAT_ATTR_HARD,
	EXPP_MAT_ATTR_HALOSEED,
	EXPP_MAT_ATTR_NFLARES,
	EXPP_MAT_ATTR_FLARESEED,
	EXPP_MAT_ATTR_NSTARS,
	EXPP_MAT_ATTR_NLINES,
	EXPP_MAT_ATTR_NRINGS,
	EXPP_MAT_ATTR_SPEC_SHADER,
	EXPP_MAT_ATTR_DIFF_SHADER,
};

/*****************************************************************************/
/* PythonTypeObject constant declarations                                    */
/*****************************************************************************/

/* 
 * structure of "tuples" of constant's string name and int value
 *
 * For example, these two structures will define the constant category
 * "bpy.class.Object.DrawTypes" the constant 
 * "bpy.class.Object.DrawTypes.BOUNDBOX" and others.
 */

static constIdents specShaderIdents[] = {
	{"COOKTORR",	{(int)MA_SPEC_COOKTORR}},
	{"PHONG",		{(int)MA_SPEC_PHONG}},
	{"BLINN",		{(int)MA_SPEC_BLINN}},
	{"TOON",		{(int)MA_SPEC_TOON}},
	{"WARDISO",		{(int)MA_SPEC_WARDISO}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition specShader = {
	EXPP_CONST_INT, "SpecularShader",
		sizeof(specShaderIdents)/sizeof(constIdents), specShaderIdents
};

static constIdents diffShaderIdents[] = {
	{"LAMBERT",		{(int)MA_DIFF_LAMBERT}},
	{"ORENNAYAR",	{(int)MA_DIFF_ORENNAYAR}},
	{"TOON",		{(int)MA_DIFF_TOON}},
	{"MINNAERT",	{(int)MA_DIFF_MINNAERT}},
	{"WARDISO",		{(int)MA_SPEC_WARDISO}},
};
static constDefinition diffShader = {
	EXPP_CONST_INT, "DiffuseShader",
		sizeof(diffShaderIdents)/sizeof(constIdents), diffShaderIdents
};

static constIdents IpoKeyTypesIdents[] = {
	{"RGB",			{(int)IPOKEY_RGB}},
	{"ALPHA",		{(int)IPOKEY_ALPHA}},
	{"HALOSIZE",	{(int)IPOKEY_HALOSIZE}},
	{"MODE",		{(int)IPOKEY_MODE}},
	{"ALLCOLOR",	{(int)IPOKEY_ALLCOLOR}},
	{"ALLMIRROR",	{(int)IPOKEY_ALLMIRROR}},
	{"OFS",			{(int)IPOKEY_OFS}},
	{"SIZE",		{(int)IPOKEY_SIZE}},
	{"ALLMAPPING",	{(int)IPOKEY_ALLMAPPING}},
};
static constDefinition IpoKey = {
	EXPP_CONST_INT, "IpoKeyTypes",
		sizeof(IpoKeyTypesIdents)/sizeof(constIdents), IpoKeyTypesIdents
};

PyObject *MaterialType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyMaterial_Type.tp_dict == NULL ) {
		BPyMaterial_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyMaterial_Type.tp_dict, &IpoKey );
		PyConstCategory_AddObjectToDict( BPyMaterial_Type.tp_dict, &diffShader );
		PyConstCategory_AddObjectToDict( BPyMaterial_Type.tp_dict, &specShader );
		PyType_Ready( &BPyMaterial_Type ) ;
		BPyMaterial_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyMaterial_Type ;
}

/***************************/
/*** The Material PyType ***/
/***************************/

static PyObject *Material_getSpecShader( BPyMaterialObject * self );
static int Material_setSpecShader( BPyMaterialObject * self, PyObject *value  );
static PyObject *Material_getDiffShader( BPyMaterialObject * self );
static int Material_setDiffShader( BPyMaterialObject * self, PyObject *value  );
static int Material_setIpo( BPyMaterialObject * self, PyObject * value );
static int Material_setLightGroup( BPyMaterialObject * self, PyObject * value );
static int Material_setSssEnable( BPyMaterialObject * self, PyObject * value );

static int Material_setRGBCol( BPyMaterialObject * self, PyObject * value );
static int Material_setSpecCol( BPyMaterialObject * self, PyObject * value );
static int Material_setMirCol( BPyMaterialObject * self, PyObject * value );
static int Material_setSssCol( BPyMaterialObject * self, PyObject * value );

/*static int Material_setSeptex( BPyMaterialObject * self, PyObject * value );
  static PyObject *Material_getSeptex( BPyMaterialObject * self );*/

/*****************************************************************************/
/* Python BPyMaterialObject methods declarations: */
/*****************************************************************************/

static PyObject *Material_getMode(BPyMaterialObject *self, void *flag);
static int Material_setMode(BPyMaterialObject *self, PyObject *value, void *flag);

static PyObject *Material_getIpo( BPyMaterialObject * self );
static PyObject *Material_getLightGroup( BPyMaterialObject * self );
static PyObject *Material_getRGBCol( BPyMaterialObject * self );
static PyObject *Material_getSpecCol( BPyMaterialObject * self );
static PyObject *Material_getMirCol( BPyMaterialObject * self );
static PyObject *Material_getSssCol( BPyMaterialObject * self );

static PyObject *Material_getSssEnable( BPyMaterialObject * self );

static PyObject *Material_insertKey( BPyMaterialObject * self, PyObject * args );
static PyObject *Material_copy( BPyMaterialObject * self );

static PyObject *getFloatAttr( BPyMaterialObject *self, void *type );
static int setFloatAttrClamp( BPyMaterialObject *self, PyObject *value, void *type );
static PyObject *getIntAttr( BPyMaterialObject *self, void *type );
static int setIntAttrClamp( BPyMaterialObject *self, PyObject *value, void *type );

/*****************************************************************************/
/* Python BPyMaterialObject methods table: */
/*****************************************************************************/
static PyMethodDef BPyMaterial_methods[] = {
	/* name, method, flags, doc */
	{"insertKey", ( PyCFunction ) Material_insertKey, METH_VARARGS,
	 "(Material Ipo Constant) - Insert IPO Key at current frame"},
	{"__copy__", ( PyCFunction ) Material_copy, METH_NOARGS,
	 "() - Return a copy of the material."},
	{"copy", ( PyCFunction ) Material_copy, METH_NOARGS,
	 "() - Return a copy of the material."},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/

static PyGetSetDef BPyMaterial_getset[] = {
	GENERIC_LIB_GETSETATTR_SCRIPTLINK,
	GENERIC_LIB_GETSETATTR_TEXTURE,
	/* custom types */
	{"ipo",
	 (getter)Material_getIpo, (setter)Material_setIpo,
	 "Material Ipo data",
	 NULL},
	{"colorbandDiffuse",
	 (getter)BPyList_get, (setter)BPyList_set,
	 "Set the light group for this material",
	 (void *) BPY_LIST_TYPE_COLORBAND},
	{"colorbandSpecular",
	 (getter)BPyList_get, (setter)BPyList_set,
	 "Set the light group for this material",
	 (void *) BPY_LIST_TYPE_COLORBAND_SPEC},
	{"enableSSS",
	 (getter)Material_getSssEnable, (setter)Material_setSssEnable,
	 "if true, SSS will be rendered for this material",
	 NULL},
	{"lightGroup",
	 (getter)Material_getLightGroup, (setter)Material_setLightGroup,
	 "Set the light group for this material",
	 NULL},
	 
	 /* consts */
	{"diffuseShader",
	 (getter)Material_getDiffShader, (setter)Material_setDiffShader,
	 "Diffuse shader type",
	 NULL},
	{"specularShader",
	 (getter)Material_getSpecShader, (setter)Material_setSpecShader,
	 "Diffuse shader type",
	 NULL},
	 
	 /* colors */
	{"rgbCol",
	 (getter)Material_getRGBCol, (setter)Material_setRGBCol,
	 "Diffuse RGB color triplet",
	 NULL},
	{"specCol",
	 (getter)Material_getSpecCol, (setter)Material_setSpecCol,
	 "Specular RGB color triplet",
	 NULL},
	{"mirCol",
	 (getter)Material_getMirCol, (setter)Material_setMirCol,
	 "Mirror RGB color triplet",
	 NULL},
	{"sssCol",
	 (getter)Material_getSssCol, (setter)Material_setSssCol,
	 "Sss RGB color triplet",
	 NULL},

	 /* mode - TODO - add docstrings */
	{"enableTraceable",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_TRACEBLE},
	{"enableShadow",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_SHADOW},
	{"enableShadeless",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_SHLESS},
	{"enableWire",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_WIRE},
	{"enableVColLight",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_VERTEXCOL},
	{"enableHalo",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALO},
 	{"enableZTransp",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_ZTRA},
 	{"enableVColPaint",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_VERTEXCOLP},	
	{"enableZInvert",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_ZINV},
 	{"enableHaloRings",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALO_RINGS},
 	{"enableEnv",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_ENV},
 	{"enableHaloLines",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALO_LINES},
 	{"enableOnlyShadow",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_ONLYSHADOW},
 	{"enableXAlpha",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALO_XALPHA},
 	{"enableStar",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_STAR},	
	{"enableTexFace",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_FACETEXTURE},
 	{"enableHaloTex",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALOTEX},
 	{"enableHaloPuno",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALOPUNO},
 	{"enableOnlyCast",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_ONLYCAST},
 	{"enableNoMist",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_NOMIST},
 	{"enableHaloShade",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALO_SHADE},
  	{"enableHaloFlare",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_HALO_FLARE},
  	{"enableRadio",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_RADIO},
  	{"enableRayTransp",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_RAYTRANSP},
  	{"enableRayMirror",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_RAYMIRROR},
  	{"enableShadowTransp",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_SHADOW_TRA},
  	{"enableRampColor",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_RAMP_COL},
  	{"enableRampSpec",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_RAMP_SPEC},
  	{"enableRayBias",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_RAYBIAS},
  	{"enableFullOsa",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_FULL_OSA}, 	
	{"enableTangentStress",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_TANGENT_STR},
  	{"enableShadowBuff",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_SHADBUF},
  	{"enableTangentV",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_TANGENT_V},
  	{"enableNMapTS",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_NORMAP_TANG},
   	{"enableGroupExclusive",
	 (getter)Material_getMode, (setter)Material_setMode,
	 "",
	 (void *)MA_GROUP_NOLAY}, 	
	 
	/* int/short/char */
	{"transDepth",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Amount of refractions for raytrace",
	 (void *)EXPP_MAT_ATTR_TRANSDEPTH},
	{"hard",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Specularity hardness",
	 (void *)EXPP_MAT_ATTR_HARD},
	{"flareSeed",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Offset in the flare seed table",
	 (void *)EXPP_MAT_ATTR_FLARESEED},
	{"haloSeed",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Randomizes halo ring dimension and line location",
	 (void *)EXPP_MAT_ATTR_HALOSEED},
	{"nFlares",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of subflares with halo",
	 (void *)EXPP_MAT_ATTR_NFLARES},
	{"nLines",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of star-shaped lines with halo",
	 (void *)EXPP_MAT_ATTR_NLINES},
	{"nRings",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of rings with halo",
	 (void *)EXPP_MAT_ATTR_NRINGS},
	{"nStars",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of star points with halo",
	 (void *)EXPP_MAT_ATTR_NSTARS},
	 
	 /* floats */
	{"add",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Strength of the add effect",
	 (void *)EXPP_MAT_ATTR_ADD},
	{"alpha",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Alpha setting",
	 (void *)EXPP_MAT_ATTR_ALPHA},
	{"shadAlpha",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Shadow Alpha setting",
	 (void *)EXPP_MAT_ATTR_SHADOWALPHA},
	{"amb",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of global ambient color material receives",
	 (void *)EXPP_MAT_ATTR_AMB},
	{"diffuseDarkness",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's diffuse darkness (\"Minnaert\" diffuse shader only)",
	 (void *)EXPP_MAT_ATTR_DIFFUSE_DARKNESS},
	{"diffuseSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's diffuse area size (\"Toon\" diffuse shader only)",
	 (void *)EXPP_MAT_ATTR_DIFFUSESIZE},
	{"diffuseSmooth",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's diffuse area smoothing (\"Toon\" diffuse shader only)",
	 (void *)EXPP_MAT_ATTR_DIFFUSESMOOTH},
	{"emit",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of light the material emits",
	 (void *)EXPP_MAT_ATTR_EMIT},
	{"filter",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of filtering when transparent raytrace is enabled",
	 (void *)EXPP_MAT_ATTR_FILTER},
	{"flareBoost",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Flare's extra strength",
	 (void *)EXPP_MAT_ATTR_FLAREBOOST},
	{"flareSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Ratio of flare size to halo size",
	 (void *)EXPP_MAT_ATTR_FLARESIZE},
	{"fresnelDepth",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Power of Fresnel for mirror reflection",
	 (void *)EXPP_MAT_ATTR_FRESNELMIRR},
	{"fresnelDepthFac",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Blending factor for Fresnel mirror",
	 (void *)EXPP_MAT_ATTR_FRESNELMIRRFAC},
	{"fresnelTrans",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Power of Fresnel for transparency",
	 (void *)EXPP_MAT_ATTR_FRESNELTRANS},
	{"fresnelTransFac",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Blending factor for Fresnel transparency",
	 (void *)EXPP_MAT_ATTR_FRESNELTRANSFAC},
	{"rbFriction",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Rigid Body Friction coefficient",
	 (void *)EXPP_MAT_ATTR_RB_FRICTION},
	{"rbRestitution",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Rigid Body Restitution coefficient",
	 (void *)EXPP_MAT_ATTR_RB_RESTITUTION},
	{"haloSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Dimension of the halo",
	 (void *) EXPP_MAT_ATTR_HALOSIZE},
	{"IOR",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Angular index of refraction for raytrace",
	 (void *) EXPP_MAT_ATTR_IOR },
	{"mirR",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mirror color red component",
	 (void *) EXPP_MAT_COMP_MIRR },
	{"mirG",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mirror color green component",
	 (void *) EXPP_MAT_COMP_MIRG },
	{"mirB",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mirror color blue component",
	 (void *) EXPP_MAT_COMP_MIRB },
	{"sssR",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "SSS color red component",
	 (void *) EXPP_MAT_COMP_SSSR},
	{"sssG",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "SSS color green component",
	 (void *) EXPP_MAT_COMP_SSSG},
	{"sssB",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "SSS color blue component",
	 (void *) EXPP_MAT_COMP_SSSB },
	{"rayMirr",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mirror reflection amount for raytrace",
	 (void *)EXPP_MAT_ATTR_RAYMIRR},
	{"rayMirrDepth",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of raytrace inter-reflections",
	 (void *)EXPP_MAT_ATTR_MIRRDEPTH},
	{"ref",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of reflections (for shader)",
	 (void *)EXPP_MAT_ATTR_REF},
	{"refracIndex",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's Index of Refraction (applies to the \"Blinn\" Specular Shader only",
	 (void *)EXPP_MAT_ATTR_REFRACINDEX},
	{"rms",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's surface slope standard deviation (\"WardIso\" specular shader only)",
	 (void *)EXPP_MAT_ATTR_RMS},
	{"roughness",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's roughness (\"Oren Nayar\" diffuse shader only)",
	 (void *)EXPP_MAT_ATTR_ROUGHNESS},
	{"spec",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Degree of specularity",
	 (void *)EXPP_MAT_ATTR_SPEC},
	{"specR",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Specular color red component",
	 (void *) EXPP_MAT_COMP_SPECR },
	{"specG",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Specular color green component",
	 (void *) EXPP_MAT_COMP_SPECG },
	{"specB",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Specular color blue component",
	 (void *) EXPP_MAT_COMP_SPECB },
	{"specTransp",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Makes specular areas opaque on transparent materials",
	 (void *) EXPP_MAT_ATTR_SPECTRANS },
	{"specSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Material's specular area size (\"Toon\" specular shader only)",
	 (void *)EXPP_MAT_ATTR_SPECSIZE},
	{"specSmooth",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Sets the smoothness of specular toon area",
	 (void *)EXPP_MAT_ATTR_SPECSMOOTH},
	{"subSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Dimension of subflares, dots and circles",
	 (void *)EXPP_MAT_ATTR_SUBSIZE},
	{"translucency",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of diffuse shading of the back side",
	 (void *) EXPP_MAT_ATTR_TRANSLUCENCY },
	{"zOffset",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Artificial offset in the Z buffer (for Ztransp option)",
	 (void *) EXPP_MAT_ATTR_ZOFFS },
	{"R",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Diffuse color red component",
	 (void *) EXPP_MAT_COMP_R },
	{"G",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Diffuse color green component",
	 (void *) EXPP_MAT_COMP_G },
	{"B",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Diffuse color blue component",
	 (void *) EXPP_MAT_COMP_B },
	 
	/* SSS settings */
	{"sssScale",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "object scale for sss",
	 (void *)EXPP_MAT_ATTR_SSS_SCALE },
	{"sssRadiusRed",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mean red scattering path length",
	 (void *)EXPP_MAT_ATTR_SSS_RADIUS_RED},
	{"sssRadiusGreen",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mean red scattering path length",
	 (void *)EXPP_MAT_ATTR_SSS_RADIUS_GREEN},
	{"sssRadiusBlue",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Mean red scattering path length",
	 (void *)EXPP_MAT_ATTR_SSS_RADIUS_BLUE},
	{"sssIOR",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "index of refraction",
	 (void *)EXPP_MAT_ATTR_SSS_IOR },
	{"sssError",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Error",
	 (void *)EXPP_MAT_ATTR_SSS_ERROR},
	{"sssColorBlend",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Blend factor for SSS Colors",
	 (void *)EXPP_MAT_ATTR_SSS_COLBLEND},
	{"sssTextureScatter",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Texture scattering factor",
	 (void *)EXPP_MAT_ATTR_SSS_TEXSCATTER},
	{"sssFont",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Front scattering weight",
	 (void *)EXPP_MAT_ATTR_SSS_FRONT},
	{"sssBack",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Back scattering weight",
	 (void *)EXPP_MAT_ATTR_SSS_BACK},
	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *Material_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Material";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_material( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new lamp data" );
	
	id->us = 0;
	return Material_CreatePyObject((Material *)id);
}

/*****************************************************************************/
/* Python BPyMaterial_Type structure definition:                                */
/*****************************************************************************/
PyTypeObject BPyMaterial_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Material",         /* char *tp_name; */
	sizeof( BPyMaterialObject ),     /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,						/* cmpfunc tp_compare; */
	NULL,						/* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,						/* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	NULL,                       /* getattrofunc tp_getattro; */
	NULL,                       /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT,         /* long tp_flags; */

	NULL,                       /*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,                       /* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,                       /* inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	NULL,                       /* richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,                          /* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	NULL,                       /* getiterfunc tp_iter; */
	NULL,                       /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyMaterial_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyMaterial_getset,			/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Material_new,	/* newfunc tp_new; */
	/*  Low-level free-memory routine */
	NULL,                       /* freefunc tp_free;  */
	/* For PyObject_IS_GC */
	NULL,                       /* inquiry tp_is_gc;  */
	NULL,                       /* PyObject *tp_bases; */
	/* method resolution order */
	NULL,                       /* PyObject *tp_mro;  */
	NULL,                       /* PyObject *tp_cache; */
	NULL,                       /* PyObject *tp_subclasses; */
	NULL,                       /* PyObject *tp_weaklist; */
	NULL
};

/*****************************************************************************/
/* Function:	Material_CreatePyObject		*/
/* Description: Create a new BPyMaterialObject from an  existing */
/*		 Blender material structure.	 */
/*****************************************************************************/
PyObject *Material_CreatePyObject( Material *mat )
{
	BPyMaterialObject *pymat;
	
	if (!mat)
		Py_RETURN_NONE;
	
	pymat = ( BPyMaterialObject * ) PyObject_NEW( BPyMaterialObject,
						 &BPyMaterial_Type );

	if( !pymat )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyMaterialObject object" );

	pymat->material = mat;
	
	/* lazy init */
	pymat->col =	NULL;
	pymat->amb =	NULL;
	pymat->spec =	NULL;
	pymat->mir =	NULL;
	pymat->sss =	NULL;
	
	return ( PyObject * ) pymat;
}

static PyObject *Material_getSpecShader( BPyMaterialObject * self )
{
	return PyConst_NewInt( &specShader, self->material->spec_shader );	
}

static int Material_setSpecShader( BPyMaterialObject * self, PyObject *value  )
{
	constValue *c = Const_FromPyObject( &specShader, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected SpecularShader constant or string" );
	
	self->material->spec_shader = c->i;
	return 0;
}

static PyObject *Material_getDiffShader( BPyMaterialObject * self )
{
	return PyConst_NewInt( &diffShader, self->material->diff_shader );	
}

static int Material_setDiffShader( BPyMaterialObject * self, PyObject *value  )
{
	constValue *c = Const_FromPyObject( &diffShader, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected SpecularShader constant or string" );
	
	self->material->diff_shader = c->i;
	return 0;
}

static PyObject *Material_getLightGroup( BPyMaterialObject * self )
{
	return Group_CreatePyObject( self->material->group );
}

static int Material_setLightGroup( BPyMaterialObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->material->group, NULL, 1, ID_GR, 0);
}

static PyObject *Material_getMode(BPyMaterialObject *self, void *flag)
{
	if (self->material->flag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int Material_setMode(BPyMaterialObject *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->material->flag |= (int)flag;
	else
		self->material->flag &= ~(int)flag;
	return 0;
}

static PyObject *Material_getIpo( BPyMaterialObject * self )
{
	return Ipo_CreatePyObject( self->material->ipo );
}

static int Material_setIpo( BPyMaterialObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->material->ipo, 0, 1, ID_IP, ID_MA);
}

static PyObject *Material_getRGBCol( BPyMaterialObject * self )
{
	if (self->col)
		return EXPP_incr_ret((PyObject *)self->col);
	
	self->col = (BPyColorObject *)Color_CreatePyObject(NULL, 3, BPY_COLOR_MAT_DIFF, 0, (PyObject *)self);
	return (PyObject *)self->col;
}

/*
static PyObject *Material_getAmbCol(BPyMaterialObject *self)
{
	return rgbTuple_getCol(self->amb);
}
*/
static PyObject *Material_getSpecCol( BPyMaterialObject * self )
{
	if (self->spec)
		return EXPP_incr_ret((PyObject *)self->spec);
	
	self->spec = (BPyColorObject *)Color_CreatePyObject(NULL, 3, BPY_COLOR_MAT_SPEC, 0, (PyObject *)self);
	return (PyObject *)self->spec;
}

static PyObject *Material_getMirCol( BPyMaterialObject * self )
{
	if (self->mir)
		return EXPP_incr_ret((PyObject *)self->mir);
	
	self->mir = (BPyColorObject *)Color_CreatePyObject(NULL, 3, BPY_COLOR_MAT_MIR, 0, (PyObject *)self);
	return (PyObject *)self->mir;
}

static PyObject *Material_getSssCol( BPyMaterialObject * self )
{
	if (self->sss)
		return EXPP_incr_ret((PyObject *)self->sss);
	
	self->sss = (BPyColorObject *)Color_CreatePyObject(NULL, 3, BPY_COLOR_MAT_SSS, 0, (PyObject *)self);
	return (PyObject *)self->sss;
}

/* SSS */
static PyObject* Material_getSssEnable( BPyMaterialObject * self )
{
	return EXPP_getBitfield( &self->material->sss_flag, MA_DIFF_SSS, 'h' );
}

/* 
 *  Material_insertKey( key )
 *   inserts Material IPO key at current frame
 */

static PyObject *Material_insertKey( BPyMaterialObject * self, PyObject * args )
{
    int key = 0, map;
    
	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int argument" ) ); 
    				
	map = texchannel_to_adrcode(self->material->texact);
	
	if(key==IPOKEY_RGB || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_COL_R, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_COL_G, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_COL_B, 0);
	}
	if(key==IPOKEY_ALPHA || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_ALPHA, 0);
	}
	if(key==IPOKEY_HALOSIZE || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_HASIZE, 0);
	}
	if(key==IPOKEY_MODE || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_MODE, 0);
	}
	if(key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC_R, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC_G, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC_B, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_REF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_EMIT, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_AMB, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_HARD, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_MODE, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_TRANSLU, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_ADD, 0);
	}
	if(key==IPOKEY_ALLMIRROR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_RAYM, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESMIR, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESMIRI, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESTRA, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESTRAI, 0);
	}
	if(key==IPOKEY_OFS || key==IPOKEY_ALLMAPPING) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_OFS_X, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_OFS_Y, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_OFS_Z, 0);
	}
	if(key==IPOKEY_SIZE || key==IPOKEY_ALLMAPPING) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_SIZE_X, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_SIZE_Y, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_SIZE_Z, 0);
	}
	if(key==IPOKEY_ALLMAPPING) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_R, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_G, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_B, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_DVAR, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_COLF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_NORF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_VARF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_DISP, 0);
	}

	allspace(REMAKEIPO, 0);
	EXPP_allqueue(REDRAWIPO, 0);
	EXPP_allqueue(REDRAWVIEW3D, 0);
	EXPP_allqueue(REDRAWACTION, 0);
	EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}


/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( BPyMaterialObject *self, void *type )
{
	float param;
	Material *mat= self->material;
	int ipocurve= -1; /* means no ipo */
	
	switch( (int)type ) {
	case EXPP_MAT_ATTR_ADD:
		param = mat->add;
		ipocurve = MA_ADD;
		break;
	case EXPP_MAT_ATTR_ALPHA:
		param = mat->alpha;
		ipocurve = MA_ALPHA;
		break;
	case EXPP_MAT_ATTR_SHADOWALPHA:
		param = mat->shad_alpha;
		break;
	case EXPP_MAT_ATTR_AMB:
		param = mat->amb;
		ipocurve = MA_AMB;
		break;
	case EXPP_MAT_ATTR_EMIT:
		param = mat->emit;
		ipocurve = MA_EMIT;
		break;
	case EXPP_MAT_ATTR_REF:
		param = mat->ref;
		ipocurve = MA_REF;
		break;
	case EXPP_MAT_ATTR_SPEC:
		param = mat->spec;
		ipocurve = MA_SPEC;
		break;
	case EXPP_MAT_ATTR_SPECTRA: 
		param = mat->spectra;
		ipocurve = MA_SPTR;
		break;
	case EXPP_MAT_ATTR_ROUGHNESS: 
		param = mat->roughness;
		break;
	case EXPP_MAT_ATTR_SPECSIZE: 
		param = mat->param[2];
		break;
	case EXPP_MAT_ATTR_DIFFUSESIZE: 
		param = mat->param[0];
		break;
	case EXPP_MAT_ATTR_SPECSMOOTH: 
		param = mat->param[3];
		break;
	case EXPP_MAT_ATTR_DIFFUSESMOOTH: 
		param = mat->param[1];
		break;
	case EXPP_MAT_ATTR_DIFFUSE_DARKNESS: 
		param = mat->darkness;
		break;
	case EXPP_MAT_ATTR_REFRACINDEX: 
		param = mat->refrac;
		break;
	case EXPP_MAT_ATTR_RMS: 
		param = mat->rms;
		break;
	
	case EXPP_MAT_ATTR_HALOSIZE:
		param = mat->hasize;
		ipocurve = MA_HASIZE;
		break;
	case EXPP_MAT_ATTR_FLARESIZE: 
		param = mat->flaresize;
		break;
	case EXPP_MAT_ATTR_FLAREBOOST:
		param = mat->flareboost;
		break;
	case EXPP_MAT_ATTR_SUBSIZE:
		param = mat->subsize;
		break;
		
	case EXPP_MAT_ATTR_RAYMIRR:
		param = mat->ray_mirror;
		ipocurve = MA_RAYM;
		break;
	case EXPP_MAT_ATTR_FRESNELMIRR: 
		param = mat->fresnel_mir;
		ipocurve = MA_FRESMIR;
		break;
	case EXPP_MAT_ATTR_FRESNELMIRRFAC: 
		param = mat->fresnel_mir_i;
		ipocurve = MA_FRESMIRI;
		break;
	case EXPP_MAT_ATTR_FILTER: 
		param = mat->filter;
		break;
	case EXPP_MAT_ATTR_TRANSLUCENCY: 
		param = mat->translucency;
		ipocurve = MA_TRANSLU;
		break;
	case EXPP_MAT_ATTR_ZOFFS:
		param = mat->zoffs;
		break;
	case EXPP_MAT_ATTR_IOR: 
		param = mat->ang;
		break;
	case EXPP_MAT_ATTR_FRESNELTRANS: 
		param = mat->fresnel_tra;
		ipocurve = MA_FRESTRA;
		break;
	case EXPP_MAT_ATTR_FRESNELTRANSFAC: 
		param = mat->fresnel_tra_i;
		ipocurve = MA_FRESTRAI;
		break;
	case EXPP_MAT_ATTR_SPECTRANS: 
		param = mat->spectra;
		break;
	case EXPP_MAT_ATTR_RB_FRICTION: 
		param = mat->friction;
		break;
	case EXPP_MAT_ATTR_RB_RESTITUTION: 
		param = mat->reflect;
		break;
	case EXPP_MAT_ATTR_SSS_SCALE: 
		param = mat->sss_scale;
		break;
	case EXPP_MAT_ATTR_SSS_RADIUS_RED: 
		param = mat->sss_radius[0];
		break;
	case EXPP_MAT_ATTR_SSS_RADIUS_GREEN: 
		param = mat->sss_radius[1];
		break;
	case EXPP_MAT_ATTR_SSS_RADIUS_BLUE: 
		param = mat->sss_radius[2];
		break;
	case EXPP_MAT_ATTR_SSS_IOR: 
		param = mat->sss_ior;
		break;
	case EXPP_MAT_ATTR_SSS_ERROR: 
		param = mat->sss_error;
		break;
	case EXPP_MAT_ATTR_SSS_COLBLEND: 
		param = mat->sss_colfac;
		break;
	case EXPP_MAT_ATTR_SSS_TEXSCATTER: 
		param = mat->sss_texfac;
		break;
	case EXPP_MAT_ATTR_SSS_FRONT: 
		param = mat->sss_front;
		break;
	case EXPP_MAT_ATTR_SSS_BACK: 
		param = mat->sss_back;
		break;

	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getFloatAttr" );
	}
	
	if (ipocurve != -1) {
		return BPyFloat_CreatePyObject( param, (PyObject *)self, ipocurve );
	} else {
		return PyFloat_FromDouble( param );
	}
}

/*
 * set floating point attributes which require clamping
 */

static int setFloatAttrClamp( BPyMaterialObject *self, PyObject *value, void *type )
{
	float *param;
	Material *mat = self->material;
	float min, max;
	
	switch( (int)type ) {
	
	/* Color First */
	case EXPP_MAT_COMP_R:
		min = 0.0f; max = 1.0f;
		param = &mat->r;
		break;
	case EXPP_MAT_COMP_G:
		min = 0.0f; max = 1.0f;
		param = &mat->g;
		break;
	case EXPP_MAT_COMP_B:
		min = 0.0f; max = 1.0f;
		param = &mat->b;
		break;
	case EXPP_MAT_COMP_SPECR:
		min = 0.0f; max = 1.0f;
		param = &mat->specr;
		break;
	case EXPP_MAT_COMP_SPECG:
		min = 0.0f; max = 1.0f;
		param = &mat->specg;
		break;
	case EXPP_MAT_COMP_SPECB:
		min = 0.0f; max = 1.0f;
		param = &mat->specb;
		break;
	case EXPP_MAT_COMP_MIRR:
		min = 0.0f; max = 1.0f;
		param = &mat->mirr;
		break;
	case EXPP_MAT_COMP_MIRG:
		min = 0.0f; max = 1.0f;
		param = &mat->mirg;
		break;
	case EXPP_MAT_COMP_MIRB:
		min = 0.0f; max = 1.0f;
		param = &mat->mirb;
		break;
	case EXPP_MAT_COMP_SSSR:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_col[0];
		break;
	case EXPP_MAT_COMP_SSSG:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_col[1];
		break;
	case EXPP_MAT_COMP_SSSB:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_col[2];
		break;
	
	/* Other floating point values */
	case EXPP_MAT_ATTR_ADD:
		min = 0.0f; max = 1.0f;
		param = &mat->add;
		break;
	case EXPP_MAT_ATTR_ALPHA:
		min = 0.0f; max = 1.0f;
		param = &mat->alpha;
		break;
	case EXPP_MAT_ATTR_SHADOWALPHA:
		min = 0.0f; max = 1.0f;
		param = &mat->shad_alpha;
		break;
	case EXPP_MAT_ATTR_AMB:
		min = 0.0f; max = 1.0f;
		param = &mat->amb;
		break;
	case EXPP_MAT_ATTR_EMIT:
		min = 0.0f; max = 1.0f;
		param = &mat->emit;
		break;
	case EXPP_MAT_ATTR_REF:
		min = 0.0f; max = 1.0f;
		param = &mat->ref;
		break;
	case EXPP_MAT_ATTR_SPEC:
		min = 0.0f; max = 1.0f;
		param = &mat->spec;
		break;
	case EXPP_MAT_ATTR_SPECTRA:
		min = 0.0f; max = 1.0f;
		param = &mat->spectra;
		break;
		
	case EXPP_MAT_ATTR_ROUGHNESS:
		min = 0.0f; max = 3.140f;
		param = &mat->roughness;
		break;
	case EXPP_MAT_ATTR_SPECSIZE:
		min = 0.0f; max = 1.530f;
		param = &mat->param[2];
		break;
	case EXPP_MAT_ATTR_DIFFUSESIZE:
		min = 0.0f; max = 3.140f;
		param = &mat->param[0];
		break;
	case EXPP_MAT_ATTR_SPECSMOOTH:
		min = 0.0f; max = 1.0f;
		param = &mat->param[3];
		break;
	case EXPP_MAT_ATTR_DIFFUSESMOOTH:
		min = 0.0f; max = 1.0f;
		param = &mat->param[1];
		break;
	case EXPP_MAT_ATTR_DIFFUSE_DARKNESS:
		min = 0.0f; max = 2.0f;
		param = &mat->darkness;
		break;
	case EXPP_MAT_ATTR_REFRACINDEX:
		min = 1.0f; max = 10.0f;
		param = &mat->refrac;
		break;
	case EXPP_MAT_ATTR_RMS:
		min = 0.0f; max = 0.4f;
		param = &mat->rms;
		break;
	
	case EXPP_MAT_ATTR_HALOSIZE:
		min = 0.0f; max = 100.0f;
		param = &mat->hasize;
		break;
	case EXPP_MAT_ATTR_FLARESIZE:
		min = 0.1f; max = 25.0f;
		param = &mat->flaresize;
		break;
	case EXPP_MAT_ATTR_FLAREBOOST:
		min = 0.1f; max = 10.0f;
		param = &mat->flareboost;
		break;
	case EXPP_MAT_ATTR_SUBSIZE:
		min = 0.1f; max = 25.0f;
		param = &mat->subsize;
		break;
	case EXPP_MAT_ATTR_RAYMIRR:
		min = 0.0f; max = 1.0f;
		param = &mat->ray_mirror;
		break;
	case EXPP_MAT_ATTR_FRESNELMIRR:
		min = 0.0f; max = 5.0f;
		param = &mat->fresnel_mir;
		break;
	case EXPP_MAT_ATTR_FRESNELMIRRFAC:
		min = 1.0f; max = 5.0f;
		param = &mat->fresnel_mir_i;
		break;
	case EXPP_MAT_ATTR_FILTER:
		min = 0.0f; max = 1.0f;
		param = &mat->filter;
		break;
	case EXPP_MAT_ATTR_TRANSLUCENCY:
		min = 0.0f; max = 1.0f;
		param = &mat->translucency;
		break;
	case EXPP_MAT_ATTR_ZOFFS:
		min = 0.0f; max = 10.0f;
		param = &mat->zoffs;
		break;
	case EXPP_MAT_ATTR_IOR:
		min = 1.0f; max = 3.0f;
		param = &mat->ang;
		break;
	case EXPP_MAT_ATTR_FRESNELTRANS:
		min = 0.0f; max = 10.0f;
		param = &mat->fresnel_tra;
		break;
	case EXPP_MAT_ATTR_FRESNELTRANSFAC:
		min = 0.0f; max = 5.0f;
		param = &mat->fresnel_tra_i;
		break;
	case EXPP_MAT_ATTR_SPECTRANS:
		min = 0.0f; max = 1.0f;
		param = &mat->spectra;
		break;
	case EXPP_MAT_ATTR_RB_FRICTION:
		min = 0.0f; max = 100.0f;
		param = &mat->friction;
		break;
	case EXPP_MAT_ATTR_RB_RESTITUTION:
		min = 0.0f; max = 1.0f;
		param = &mat->reflect;
		break;
	case EXPP_MAT_ATTR_SSS_SCALE:
		min = 0.001f; max = 1000.0f;
		param = &mat->sss_scale;
		break;
	case EXPP_MAT_ATTR_SSS_RADIUS_RED:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_radius[0];
		break;
	case EXPP_MAT_ATTR_SSS_RADIUS_GREEN:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_radius[1];
		break;
	case EXPP_MAT_ATTR_SSS_RADIUS_BLUE:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_radius[2];
		break;
	case EXPP_MAT_ATTR_SSS_IOR:
		min = 0.0f; max = 2.0f;
		param = &mat->sss_ior;
		break;
	case EXPP_MAT_ATTR_SSS_ERROR:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_error;
		break;
		
	case EXPP_MAT_ATTR_SSS_COLBLEND:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_colfac;
		break;
	case EXPP_MAT_ATTR_SSS_TEXSCATTER:
		min = 0.0f; max = 1.0f;
		param = &mat->sss_texfac;
		break;
		
	case EXPP_MAT_ATTR_SSS_FRONT:
		min = 0.0f; max = 2.0f;
		param = &mat->sss_front;
		break;
	case EXPP_MAT_ATTR_SSS_BACK:
		min = 0.0f; max = 10.0f;
		param = &mat->sss_back;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	return EXPP_setFloatClamped( value, param, min, max );
	
}


/*
 * get integer attributes
 */
static PyObject *getIntAttr( BPyMaterialObject *self, void *type )
{
	int param; 
	Material *mat = self->material;

	switch( (int)type ) {
	
	case EXPP_MAT_ATTR_MIRRDEPTH:
		param = (int)mat->ray_depth;
		break;
	case EXPP_MAT_ATTR_TRANSDEPTH:
		param = (int)mat->ray_depth_tra;
		break;
	case EXPP_MAT_ATTR_NFLARES:
		param = mat->flarec;
		break;
	case EXPP_MAT_ATTR_NSTARS:
		param = mat->starc;
		break;	
	case EXPP_MAT_ATTR_NLINES:
		param = (int)mat->linec;
		break;
	case EXPP_MAT_ATTR_NRINGS:
		param = (int)mat->ringc;
		break;
	case EXPP_MAT_ATTR_HARD:
		param = (int)mat->har;
		break;
	case EXPP_MAT_ATTR_HALOSEED:
		param = (int)mat->seed1;
		break;
	case EXPP_MAT_ATTR_FLARESEED:
		param = (int)mat->seed2;
		break;
	case EXPP_MAT_ATTR_DIFF_SHADER:
		param = (int)mat->diff_shader;
		break;
	case EXPP_MAT_ATTR_SPEC_SHADER:
		param = (int)mat->spec_shader;
		break;		
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getIntAttr" );
	}

	return PyInt_FromLong( param );
}


/*
 * set integer attributes which require clamping
 */

static int setIntAttrClamp( BPyMaterialObject *self, PyObject *value, void *type )
{
	void *param;
	Material *mat = self->material;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_MAT_ATTR_MIRRDEPTH:
		min = 0; max = 10;
		size = 'h';
		param = (void *)&mat->ray_depth;
		break;
	case EXPP_MAT_ATTR_TRANSDEPTH:
		min = 0; max = 10;
		size = 'h';
		param = (void *)&mat->ray_depth_tra;
		break;
	case EXPP_MAT_ATTR_NFLARES:
		min = 0; max = 10;
		size = 'h';
		param = (void *)&mat->flarec;
		break;
	case EXPP_MAT_ATTR_NSTARS:
		min = 1; max = 32;
		size = 'h';
		param = (void *)&mat->starc;
		break;	
	case EXPP_MAT_ATTR_NLINES:
		min = 0; max = 250;
		size = 'h';
		param = (void *)&mat->linec;
		break;
	case EXPP_MAT_ATTR_NRINGS:
		min = 0; max = 24;
		size = 'h';
		param = (void *)&mat->ringc;
		break;
	case EXPP_MAT_ATTR_HARD:
		min = 1; max = 255;
		size = 'h';
		param = (void *)&mat->har;
		break;
	case EXPP_MAT_ATTR_HALOSEED:
		/* todo - 127 when halo modes enabled */
		min = 1; max = 255;
		size = 'h';
		param = (void *)&mat->seed1;
		break;
	case EXPP_MAT_ATTR_FLARESEED:
		min = 0; max = 255;
		size = 'h';
		param = (void *)&mat->seed2;
		break;	
	case EXPP_MAT_ATTR_SPEC_SHADER:
		min = MA_SPEC_COOKTORR;
		max = MA_SPEC_WARDISO;
		size = 'h';
		param = (void *)&mat->seed2;
		break;
	case EXPP_MAT_ATTR_DIFF_SHADER:
		min = MA_DIFF_LAMBERT;
		max = MA_DIFF_MINNAERT;
		size = 'h';
		param = (void *)&mat->seed2;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}
	return EXPP_setIValueClamped( value, param, min, max, size );
}

static int Material_setRGBCol( BPyMaterialObject * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->material->r, &self->material->g, &self->material->b);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->material->r, 0.0f, 1.0f);
	CLAMP(self->material->g, 0.0f, 1.0f);
	CLAMP(self->material->b, 0.0f, 1.0f);
	return 0;
}

static int Material_setSpecCol( BPyMaterialObject * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->material->specr, &self->material->specg, &self->material->specb);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->material->specr, 0.0f, 1.0f);
	CLAMP(self->material->specg, 0.0f, 1.0f);
	CLAMP(self->material->specb, 0.0f, 1.0f);
	return 0;
}

static int Material_setMirCol( BPyMaterialObject * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->material->mirr, &self->material->mirg, &self->material->mirb);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->material->mirr, 0.0f, 1.0f);
	CLAMP(self->material->mirg, 0.0f, 1.0f);
	CLAMP(self->material->mirb, 0.0f, 1.0f);
	return 0;
}

static int Material_setSssCol( BPyMaterialObject * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->material->sss_col[0], &self->material->sss_col[1], &self->material->sss_col[2]);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->material->sss_col[0], 0.0f, 1.0f);
	CLAMP(self->material->sss_col[1], 0.0f, 1.0f);
	CLAMP(self->material->sss_col[2], 0.0f, 1.0f);
	return 0;
}

/* SSS */
static int Material_setSssEnable( BPyMaterialObject * self, PyObject * value )
{
	return EXPP_setBitfield( value, &self->material->sss_flag, MA_DIFF_SSS, 'h' );
}

/* mat.__copy__ */
static PyObject *Material_copy( BPyMaterialObject * self )
{
	BPyMaterialObject *pymat; /* for Material Data object wrapper in Python */
	Material *blmat; /* for actual Material Data we create in Blender */
	
	blmat = copy_material( self->material );	/* first copy the Material Data in Blender */

	if( blmat )		/* now create the wrapper obj in Python */
		pymat = ( BPyMaterialObject * ) Material_CreatePyObject( blmat );
	else
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"couldn't create Material Data in Blender" ) );

	blmat->id.us = 0;	/* was incref'ed by add_material() above */

	if( pymat == NULL )
		return ( EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create Material Data object" ) );

	return ( PyObject * ) pymat;
}

/** expands pointer array of length 'oldsize' to length 'newsize'.
	* A pointer to the (void *) array must be passed as first argument 
	* The array pointer content can be NULL, in this case a new array of length
	* 'newsize' is created.
	*/

static int expandPtrArray( void **p, int oldsize, int newsize )
{
	void *newarray;

	if( newsize < oldsize ) {
		return 0;
	}
	newarray = MEM_callocN( sizeof( void * ) * newsize, "PtrArray" );
	if( *p ) {
		memcpy( newarray, *p, sizeof( void * ) * oldsize );
		MEM_freeN( *p );
	}
	*p = newarray;
	return 1;
}

int EXPP_synchronizeMaterialLists( Object * object )
{
	Material ***p_dataMaterials = give_matarar( object );
	short *nmaterials = give_totcolp( object );
	int result = 0;

	if( object->totcol > *nmaterials ) {
		/* More object mats than data mats */
		result = expandPtrArray( ( void * ) p_dataMaterials,
					 *nmaterials, object->totcol );
		*nmaterials = object->totcol;
	} else {
		if( object->totcol < *nmaterials ) {
			/* More data mats than object mats */
			result = expandPtrArray( ( void * ) &object->mat,
						 object->totcol, *nmaterials );
			object->totcol = (char)*nmaterials;
		}
	}			/* else no synchronization needed, they are of equal length */

	return result;		/* 1 if changed, 0 otherwise */
}

void EXPP_incr_mats_us( Material ** matlist, int len )
{
	int i;
	Material *mat;

	if( len <= 0 )
		return;

	for( i = 0; i < len; i++ ) {
		mat = matlist[i];
		if( mat )
			mat->id.us++;
	}

	return;
}
