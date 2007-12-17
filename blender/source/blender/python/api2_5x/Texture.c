/*  
 *  $Id: Texture.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA    02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Alex Mole, Nathan Letwory, Joilnen B. Leite, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

/*
 * TODO - ->extend
 * 
 * */

#include "Texture.h" /*This must come first*/

#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_idprop.h"
#include "BKE_library.h"
#include "BKE_texture.h"
#include "BKE_utildefines.h"

#include "BLI_blenlib.h"

#include "DNA_object_types.h"
#include "DNA_material_types.h"
#include "DNA_scene_types.h"
#include "DNA_texture_types.h"

#include "MTex.h"
#include "Image.h"
#include "Ipo.h"
#include "IDProp.h"
#include "blendef.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "bpy_list.h"

#include "color.h" /* for Texture_evaluate(vec) */
#include "Material.h" /* for EXPP_Colorband_fromPyList and EXPP_PyList_fromColorband */
#include "RE_shader_ext.h"
#include "Const.h"
#include "color.h"
#include "vector.h"

enum tex_float_consts {
	EXPP_TEX_ATTR_NOISESIZE = 0,
	EXPP_TEX_ATTR_TURBUL,
	EXPP_TEX_ATTR_BRIGHT,
	EXPP_TEX_ATTR_CONTRAST,
	/* access through color only */
	/*EXPP_TEX_ATTR_RFAC,
	EXPP_TEX_ATTR_GFAC,
	EXPP_TEX_ATTR_BFAC,*/
	EXPP_TEX_ATTR_FILTERSIZE,
	EXPP_TEX_ATTR_MG_H,
	EXPP_TEX_ATTR_MG_LACUNARITY,
	EXPP_TEX_ATTR_MG_OCTAVES,
	EXPP_TEX_ATTR_MG_OFFSET,
	EXPP_TEX_ATTR_MG_GAIN,
	EXPP_TEX_ATTR_DIST_AMOUNT,
	EXPP_TEX_ATTR_NS_OUTSCALE,
	EXPP_TEX_ATTR_VN_W1,
	EXPP_TEX_ATTR_VN_W2,
	EXPP_TEX_ATTR_VN_W3,
	EXPP_TEX_ATTR_VN_W4,
	EXPP_TEX_ATTR_VN_MEXP,
	EXPP_TEX_ATTR_CROPXMIN,
	EXPP_TEX_ATTR_CROPYMIN,
	EXPP_TEX_ATTR_CROPXMAX,
	EXPP_TEX_ATTR_CROPYMAX,
	EXPP_TEX_ATTR_CHECKERDIST,
	EXPP_TEX_ATTR_NABLA,
	EXPP_TEX_ATTR_NORFAC,
};

enum tex_int_consts {
	EXPP_TEX_ATTR_NOISEDEPTH = 0,
	EXPP_TEX_ATTR_XREPEAT,
	EXPP_TEX_ATTR_YREPEAT,
	EXPP_TEX_ATTR_IUSER_FRAMES,
	EXPP_TEX_ATTR_IUSER_OFFSET,
	EXPP_TEX_ATTR_IUSER_SFRA,
	EXPP_TEX_ATTR_IUSER_FIE_IMA,
	EXPP_TEX_ATTR_IUSER_CYCL,
};

enum tex_const_consts {
	EXPP_TEX_ATTR_NOISETYPE = 0,
	EXPP_TEX_ATTR_NOISEBASIS,
	EXPP_TEX_ATTR_NOISEBASIS2,
	EXPP_TEX_ATTR_TYPE,
	EXPP_TEX_ATTR_STYPE,
	EXPP_TEX_ATTR_EXTEND,
};

/*****************************************************************************/
/* Blender.Texture constants                                                 */
/*****************************************************************************/

/* max depth is different for magic type textures */

/****************************************************************************/
/* Texture String->Int maps                                                 */
/****************************************************************************/

static constIdents textureTypesIdents[] = {
	{"None",				{(int)NULL}},
	{"Clouds",				{(int)TEX_CLOUDS}},
	{"Wood",				{(int)TEX_WOOD}},
	{"Marble",				{(int)TEX_MARBLE}},
	{"Magic",				{(int)TEX_MAGIC}},
	{"Blend",				{(int)TEX_BLEND}},
	{"Stucci",				{(int)TEX_STUCCI}},
	{"Noise",				{(int)TEX_NOISE}},
	{"Image",				{(int)TEX_IMAGE}},
	{"Plugin",				{(int)TEX_PLUGIN}},
	{"EnvMap",				{(int)TEX_ENVMAP}},
	{"Musgrave",			{(int)TEX_MUSGRAVE}},
	{"Voronoi",				{(int)TEX_VORONOI}},
	{"DistortedNoise",		{(int)TEX_DISTNOISE}},
};
static constDefinition textureTypes = {
	EXPP_CONST_INT, "TextureTypes",
		sizeof(textureTypesIdents)/sizeof(constIdents), textureTypesIdents
};

static constIdents musgraveTypesIdents[] = {
	{"MultiFractal",		{(int)TEX_MFRACTAL}},
	{"HeteroTerrain",		{(int)TEX_RIDGEDMF}},
	{"RidgedMultiFractal",	{(int)TEX_HYBRIDMF}},
	{"fBM",					{(int)TEX_FBM}},
	{"HybridMultiFractal",	{(int)TEX_HTERRAIN}},
};
static constDefinition musgraveTypes = {
	EXPP_CONST_INT, "MusgraveTypes",
		sizeof(musgraveTypesIdents)/sizeof(constIdents), musgraveTypesIdents
};

static constIdents noiseBasisTypesIdents[] = {
	{"BlenderOriginal",		{(int)TEX_BLENDER}},
	{"OriginalPerlin",		{(int)TEX_STDPERLIN}},
	{"ImprovedPerlin",		{(int)TEX_NEWPERLIN}},
	{"VoronoiF1",			{(int)TEX_VORONOI_F1}},
	{"VoronoiF2",			{(int)TEX_VORONOI_F2}},
	{"VoronoiF3",			{(int)TEX_VORONOI_F3}},
	{"VoronoiF4",			{(int)TEX_VORONOI_F4}},
	{"VoronoiF2-F1",		{(int)TEX_VORONOI_F2F1}},
	{"VoronoiCrackle",		{(int)TEX_VORONOI_CRACKLE}},
	{"CellNoise",			{(int)TEX_CELLNOISE}},
};
static constDefinition noiseBasisTypes = {
	EXPP_CONST_INT, "NoiseBasisTypes",
		sizeof(noiseBasisTypesIdents)/sizeof(constIdents), noiseBasisTypesIdents
};

static constIdents displaceVeronoiTypesIdents[] = {
	{"Distance",			{(int)TEX_DISTANCE}},
	{"DistanceSquared",		{(int)TEX_DISTANCE_SQUARED}},
	{"Manhattan",			{(int)TEX_MANHATTAN}},
	{"Chebychev",			{(int)TEX_CHEBYCHEV}},
	{"MinkovskyHalf",		{(int)TEX_MINKOVSKY_HALF}},
	{"MinkovskyFour",		{(int)TEX_MINKOVSKY_FOUR}},
	{"Minkovsky",			{(int)TEX_MINKOVSKY}},
};
static constDefinition displaceVeronoiTypes = {
	EXPP_CONST_INT, "DisplaceVeronoiTypes",
		sizeof(displaceVeronoiTypesIdents)/sizeof(constIdents), displaceVeronoiTypesIdents
};

static constIdents extendTypesIdents[] = {
	{"Extend",				{(int)TEX_EXTEND}},
	{"Clip",				{(int)TEX_CLIP}},
	{"ClipCube",			{(int)TEX_CLIPCUBE}},
	{"Repeat",				{(int)TEX_REPEAT}},
	{"Checker",				{(int)TEX_CHECKER}},
};
static constDefinition extendTypes = {
	EXPP_CONST_INT, "ExtendTypes",
		sizeof(extendTypesIdents)/sizeof(constIdents), extendTypesIdents
};

static constIdents noiseTypesIdents[] = {
	{"Soft",				{(int)TEX_NOISESOFT}},
	{"Hard",				{(int)TEX_NOISEPERL}},
};
static constDefinition noiseTypes = {
	EXPP_CONST_INT, "NoiseTypes",
		sizeof(noiseTypesIdents)/sizeof(constIdents), noiseTypesIdents
};

static constIdents cloudTypesIdents[] = {
	{"Default",				{(int)TEX_DEFAULT}},
	{"Color",				{(int)TEX_COLOR}},
};
static constDefinition cloudTypes = {
	EXPP_CONST_INT, "CloudTypes",
		sizeof(cloudTypesIdents)/sizeof(constIdents), cloudTypesIdents
};

static constIdents woodTypesIdents[] = {
	{"Bands",				{(int)TEX_BAND}},
	{"Rings",				{(int)TEX_RING}},
	{"BandNoise",			{(int)TEX_BANDNOISE}},
	{"RingNoise",			{(int)TEX_RINGNOISE}},
};
static constDefinition woodTypes = {
	EXPP_CONST_INT, "WoodTypes",
		sizeof(woodTypesIdents)/sizeof(constIdents), woodTypesIdents
};

static constIdents marbleTypesIdents[] = {
	{"Soft",				{(int)TEX_SOFT}},
	{"Sharp",				{(int)TEX_SHARP}},
	{"Sharper",				{(int)TEX_SHARPER}},
};
static constDefinition marbleTypes = {
	EXPP_CONST_INT, "MarbleTypes",
		sizeof(marbleTypesIdents)/sizeof(constIdents), marbleTypesIdents
};

static constIdents blendTypesIdents[] = {
	{"Lin",					{(int)TEX_LIN}},
	{"Quad",				{(int)TEX_QUAD}},
	{"Ease",				{(int)TEX_EASE}},
	{"Diag",				{(int)TEX_DIAG}},
	{"Sphere",				{(int)TEX_SPHERE}},
	{"Halo",				{(int)TEX_HALO}},
	{"Rad",					{(int)TEX_RAD}},
};
static constDefinition blendTypes = {
	EXPP_CONST_INT, "BlendTypes",
		sizeof(blendTypesIdents)/sizeof(constIdents), blendTypesIdents
};

static constIdents stucciTypesIdents[] = {
	{"Plastic",				{(int)TEX_PLASTIC}},
	{"WallIn",				{(int)TEX_WALLIN}},
	{"WallOut",				{(int)TEX_WALLOUT}},
};
static constDefinition stucciTypes = {
	EXPP_CONST_INT, "StucciTypes",
		sizeof(stucciTypesIdents)/sizeof(constIdents), stucciTypesIdents
};

static constIdents envmapTypesIdents[] = {
	{"Static",				{(int)ENV_STATIC}},
	{"Anim",				{(int)ENV_ANIM}},
	{"Load",				{(int)ENV_LOAD}},
};
static constDefinition envmapTypes = {
	EXPP_CONST_INT, "EnvmapTypes",
		sizeof(envmapTypesIdents)/sizeof(constIdents), envmapTypesIdents
};

static constIdents voronoiTypesIdents[] = {
	{"Int",					{(int)TEX_INTENSITY}},
	{"Col1",				{(int)TEX_COL1}},
	{"Col2",				{(int)TEX_COL2}},
	{"Col3",				{(int)TEX_COL3}},
};

static constDefinition voronoiTypes = {
	EXPP_CONST_INT, "VoronoiTypes",
		sizeof(voronoiTypesIdents)/sizeof(constIdents), voronoiTypesIdents
};

PyObject *Texture_CreatePyObject( Tex * tex )
{
	BPyTextureObject *pytex;

	if (!tex)
		Py_RETURN_NONE;
	
	pytex = ( BPyTextureObject * ) PyObject_NEW( BPyTextureObject, &BPyTexture_Type );
	if( !pytex )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyTextureObject PyObject" );

	pytex->texture = tex;
	return ( PyObject * ) pytex;
}


/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( BPyTextureObject *self, void *type )
{
	float param;
	
	switch( (int)type ) {
	case EXPP_TEX_ATTR_NOISESIZE:
		param = self->texture->noisesize;
		break;
	case EXPP_TEX_ATTR_TURBUL:
		param = self->texture->turbul;
		break;
	case EXPP_TEX_ATTR_BRIGHT:
		param = self->texture->bright;
		break;
	case EXPP_TEX_ATTR_CONTRAST:
		param = self->texture->contrast;
		break;
	case EXPP_TEX_ATTR_FILTERSIZE:
		param = self->texture->filtersize;
		break;
	case EXPP_TEX_ATTR_MG_H:
		param = self->texture->mg_H;
		break;
	case EXPP_TEX_ATTR_MG_LACUNARITY:
		param = self->texture->mg_lacunarity;
		break;
	case EXPP_TEX_ATTR_MG_OCTAVES:
		param = self->texture->mg_octaves;
		break;
	case EXPP_TEX_ATTR_MG_OFFSET:
		param = self->texture->mg_offset;
		break;
	case EXPP_TEX_ATTR_MG_GAIN:
		param = self->texture->mg_gain;
		break;
	case EXPP_TEX_ATTR_DIST_AMOUNT:
		param = self->texture->dist_amount;
		break;
	case EXPP_TEX_ATTR_NS_OUTSCALE:
		param = self->texture->ns_outscale;
		break;
	case EXPP_TEX_ATTR_VN_W1:
		param = self->texture->vn_w1;
		break;
	case EXPP_TEX_ATTR_VN_W2:
		param = self->texture->vn_w2;
		break;
	case EXPP_TEX_ATTR_VN_W3:
		param = self->texture->vn_w3;
		break;
	case EXPP_TEX_ATTR_VN_W4:
		param = self->texture->vn_w4;
		break;
	case EXPP_TEX_ATTR_VN_MEXP:
		param = self->texture->vn_mexp;
		break;
	case EXPP_TEX_ATTR_CROPXMIN:
		param = self->texture->cropxmin;
		break;
	case EXPP_TEX_ATTR_CROPYMIN:
		param = self->texture->cropymin;
		break;
	case EXPP_TEX_ATTR_CROPXMAX:
		param = self->texture->cropxmax;
		break;
	case EXPP_TEX_ATTR_CROPYMAX:
		param = self->texture->cropymax;
		break;
	case EXPP_TEX_ATTR_CHECKERDIST:
		param = self->texture->checkerdist;
		break;
	case EXPP_TEX_ATTR_NABLA:
		param = self->texture->nabla;
		break;
	case EXPP_TEX_ATTR_NORFAC:
		param = self->texture->norfac;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getFloatAttr" );
	}

	return PyFloat_FromDouble( param );
}

/*
 * set floating point attributes which require clamping
 */

static int setFloatAttrClamp( BPyTextureObject *self, PyObject *value, void *type )
{
	float *param;
	float min, max;
	
	switch( (int)type ) {
	
	/* Color First */
	case EXPP_TEX_ATTR_NOISESIZE:
		min = 0.0001f; max = 2.0f;
		param = &self->texture->noisesize;
		break;
	case EXPP_TEX_ATTR_TURBUL:
		min = 0.0f; max = 200.0f;
		param = &self->texture->turbul;
		break;
	case EXPP_TEX_ATTR_BRIGHT:
		min = 0.0f; max = 2.0f;
		param = &self->texture->bright;
		break;
	case EXPP_TEX_ATTR_CONTRAST:
		min = 0.01f; max = 5.0f;
		param = &self->texture->contrast;
		break;
	case EXPP_TEX_ATTR_FILTERSIZE:
		min = 0.01f; max = 25.0f;
		param = &self->texture->filtersize;
		break;
	case EXPP_TEX_ATTR_MG_H:
		min = 0.0001f; max = 2.0f;
		param = &self->texture->mg_H;
		break;
	case EXPP_TEX_ATTR_MG_LACUNARITY:
		min = 0.0f; max = 6.0f;
		param = &self->texture->mg_lacunarity;
		break;
	case EXPP_TEX_ATTR_MG_OCTAVES:
		min = 0.0f; max = 8.0f;
		param = &self->texture->mg_octaves;
		break;
	case EXPP_TEX_ATTR_MG_OFFSET:
		min = 0.0f; max = 6.0f;
		param = &self->texture->mg_offset;
		break;
	case EXPP_TEX_ATTR_MG_GAIN:
		min = 0.0f; max = 6.0f;
		param = &self->texture->mg_gain;
		break;
	case EXPP_TEX_ATTR_DIST_AMOUNT:
		min = 0.0f; max = 10.0f;
		param = &self->texture->dist_amount;
		break;
	case EXPP_TEX_ATTR_NS_OUTSCALE:
		min = 0.0f; max = 10.0f;
		param = &self->texture->ns_outscale;
		break;
	case EXPP_TEX_ATTR_VN_W1:
		min = -2.0f; max = 2.0f;
		param = &self->texture->vn_w1;
		break;
	case EXPP_TEX_ATTR_VN_W2:
		min = -2.0f; max = 2.0f;
		param = &self->texture->vn_w2;
		break;
	case EXPP_TEX_ATTR_VN_W3:
		min = -2.0f; max = 2.0f;
		param = &self->texture->vn_w3;
		break;
	case EXPP_TEX_ATTR_VN_W4:
		min = -2.0f; max = 2.0f;
		param = &self->texture->vn_w4;
		break;
	case EXPP_TEX_ATTR_VN_MEXP:
		min = 0.01f; max = 10.0f;
		param = &self->texture->vn_mexp;
		break;
	case EXPP_TEX_ATTR_CROPXMIN:
		min = -10.0f; max = 10.0f;
		param = &self->texture->cropxmin;
		break;
	case EXPP_TEX_ATTR_CROPYMIN:
		min = -10.0f; max = 10.0f;
		param = &self->texture->cropymin;
		break;
	case EXPP_TEX_ATTR_CROPXMAX:
		min = -10.0f; max = 10.0f;
		param = &self->texture->cropxmax;
		break;
	case EXPP_TEX_ATTR_CROPYMAX:
		min = -10.0f; max = 10.0f;
		param = &self->texture->cropymax;
		break;
	case EXPP_TEX_ATTR_CHECKERDIST:
		min = 0.0f; max = 0.99f;
		param = &self->texture->checkerdist;
		break;
	case EXPP_TEX_ATTR_NABLA:
		min = 0.001f; max = 0.1f;
		param = &self->texture->nabla;
		break;
	case EXPP_TEX_ATTR_NORFAC:
		min = 0.0f; max = 1.0f;
		param = &self->texture->norfac;
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
static PyObject *getIntAttr( BPyTextureObject *self, void *type )
{
	int param; 

	switch( (int)type ) {
	case EXPP_TEX_ATTR_NOISEDEPTH:
		param = (int)self->texture->noisedepth;
		break;
	case EXPP_TEX_ATTR_XREPEAT:
		param = (int)self->texture->xrepeat;
		break;
	case EXPP_TEX_ATTR_YREPEAT:
		param = (int)self->texture->yrepeat;
		break;
	case EXPP_TEX_ATTR_IUSER_FRAMES:
		param = (int)self->texture->iuser.frames;
		break;
	case EXPP_TEX_ATTR_IUSER_OFFSET:
		param = (int)self->texture->iuser.offset;
		break;
	case EXPP_TEX_ATTR_IUSER_SFRA:
		param = (int)self->texture->iuser.sfra;
		break;
	case EXPP_TEX_ATTR_IUSER_FIE_IMA:
		param = (int)self->texture->iuser.fie_ima;
		break;
	case EXPP_TEX_ATTR_IUSER_CYCL:
		param = (int)self->texture->iuser.cycl;
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

static int setIntAttrClamp( BPyTextureObject *self, PyObject *value, void *type )
{
	void *param;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_TEX_ATTR_NOISESIZE:
		min = 0; max = 6;
		size = 'h';
		param = (void *)&self->texture->noisedepth;
		break;
	case EXPP_TEX_ATTR_XREPEAT:
		min = 1; max = 512;
		size = 'h';
		param = (void *)&self->texture->xrepeat;
		break;
	case EXPP_TEX_ATTR_YREPEAT:
		min = 1; max = 512;
		size = 'h';
		param = (void *)&self->texture->yrepeat;
		break;
	case EXPP_TEX_ATTR_IUSER_FRAMES:
		min = -(int)MAXFRAMEF; max = (int)MAXFRAMEF;
		size = 'i';
		param = (void *)&self->texture->iuser.frames;
		break;
	case EXPP_TEX_ATTR_IUSER_OFFSET:
		min = -(int)MAXFRAMEF; max = (int)MAXFRAMEF;
		size = 'i';
		param = (void *)&self->texture->iuser.offset;
		break;
	case EXPP_TEX_ATTR_IUSER_SFRA:
		min = 1; max = ((int)MAXFRAMEF);
		size = 'i';
		param = (void *)&self->texture->iuser.sfra;
		break;
	case EXPP_TEX_ATTR_IUSER_FIE_IMA:
		min = 1; max = 200;
		size = 'h';
		param = (void *)&self->texture->iuser.fie_ima;
		break;
	case EXPP_TEX_ATTR_IUSER_CYCL: /* bool */
		min = 0; max = 1;
		size = 'h';
		param = (void *)&self->texture->iuser.cycl;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}
	return EXPP_setIValueClamped( value, param, min, max, size );
}

/*
 * get constant attributes
 */

static PyObject *getConstAttr( BPyTextureObject *self, void *type )
{
	constDefinition *constType;
	constValue param;

	switch( (int)type ) {
	case EXPP_TEX_ATTR_NOISETYPE:
		param.i = self->texture->noisetype;
		constType = &noiseTypes;
		break;
	case EXPP_TEX_ATTR_NOISEBASIS:
		param.i = self->texture->noisebasis;
		constType = &noiseBasisTypes;
		break;
	case EXPP_TEX_ATTR_NOISEBASIS2:
		param.i = self->texture->noisebasis2;
		constType = &displaceVeronoiTypes;
		break;
	case EXPP_TEX_ATTR_TYPE:
		param.i = self->texture->type;
		constType = &textureTypes;
		break;
	case EXPP_TEX_ATTR_STYPE:
		param.i = self->texture->stype;
		switch(self->texture->type) {
		case TEX_CLOUDS:
			constType = &cloudTypes;
			break;
		case TEX_WOOD:
			constType = &woodTypes;
			break;
		case TEX_MARBLE:
			constType = &marbleTypes;
			break;
		case TEX_BLEND:
			constType = &blendTypes;
			break;
		case TEX_STUCCI:
			constType = &stucciTypes;
			break;
		case TEX_ENVMAP:
			constType = &extendTypes;
			break;
		case TEX_MUSGRAVE:
			constType = &musgraveTypes;
			break;
		
		/* no stype */
		case TEX_NOISE:
		case TEX_MAGIC:
		case TEX_IMAGE:
		case TEX_PLUGIN:
		case TEX_VORONOI:
		case TEX_DISTNOISE:
		default:
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"this texture type has no stype" );
		}
		break;
	case EXPP_TEX_ATTR_EXTEND:
		param.i = self->texture->extend;
		constType = &extendTypes;
		break;		
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getConstAttr" );
	}
   	return PyConst_NewInt( constType, param.i );
}

/*
 * set constant attributes
 */

static int setConstAttr( BPyTextureObject *self, PyObject *value, void *type )
{
	void *param;
	constDefinition *constType;
	constValue *c;
	
	/* for each constant, assign pointer to its storage and set the type
 	 * of constant class we should match */
	switch( (int)type ) {
	case EXPP_TEX_ATTR_NOISETYPE:
		param = (void *)&self->texture->noisetype;
		constType = &noiseTypes;
		break;
	case EXPP_TEX_ATTR_NOISEBASIS:
		param = (void *)&self->texture->noisebasis;
		constType = &noiseBasisTypes;
		break;
	case EXPP_TEX_ATTR_NOISEBASIS2:
		param = (void *)&self->texture->noisebasis2;
		constType = &displaceVeronoiTypes;
		break;
	case EXPP_TEX_ATTR_TYPE:
		param = (void *)&self->texture->type;
		constType = &textureTypes;
		break;
	case EXPP_TEX_ATTR_STYPE:
		param = (void *)&self->texture->stype;
		switch(self->texture->type) {
		case TEX_CLOUDS:
			constType = &cloudTypes;
			break;
		case TEX_WOOD:
			constType = &woodTypes;
			break;
		case TEX_MARBLE:
			constType = &marbleTypes;
			break;
		case TEX_BLEND:
			constType = &blendTypes;
			break;
		case TEX_STUCCI:
			constType = &stucciTypes;
			break;
		case TEX_ENVMAP:
			constType = &extendTypes;
			break;
		case TEX_MUSGRAVE:
			constType = &musgraveTypes;
			break;
		
		/* no stype */
		case TEX_NOISE:
		case TEX_MAGIC:
		case TEX_IMAGE:
		case TEX_PLUGIN:
		case TEX_VORONOI:
		case TEX_DISTNOISE:
		default:
			return EXPP_ReturnIntError( PyExc_RuntimeError,
				"this texture type has no stype" );
		}
		break;
	case EXPP_TEX_ATTR_EXTEND:
		param = (void *)&self->texture->extend;
		constType = &extendTypes;
		break;		
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in getConstAttr" );
	}
	
	/*
	 * this checks for a constant of the correct type, and if so use the
	 * value
	 */
	/* check that we have a valid constant */
	c = Const_FromPyObject( constType, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected  constant type/string not found" );
	
	*(int *)param = c->i;  
	return 0;
}




/*****************************************************************************/
/* Python BPyTextureObject methods:                                               */
/*****************************************************************************/

static PyObject *Texture_getFlags( BPyTextureObject *self, void * flag )
{
	if (self->texture->flag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Texture_setFlags( BPyTextureObject * self, PyObject * value, void * flag )
{
	if ( PyObject_IsTrue(value) )
		self->texture->flag |= (int)flag;
	else
		self->texture->flag &= ~(int)flag;
	
	/* if Colorband enabled, make sure we allocate memory for it */
	/*
	if ( ( param & TEX_COLORBAND ) && !self->texture->coba )
		self->texture->coba = add_colorband();
	 */
	return 0;
}

static PyObject *Texture_getImage( BPyTextureObject * self )
{
	/* we need this to be an IMAGE texture, and we must have an image */
	if( ( self->texture->type == TEX_IMAGE ||
				self->texture->type == TEX_ENVMAP )
			&& self->texture->ima )
		return Image_CreatePyObject( self->texture->ima );

	Py_RETURN_NONE;
}
static int Texture_setImage( BPyTextureObject * self, PyObject * value )
{
	Image *blimg = NULL;

	if( !BPyImage_Check (value) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					      "expected an Image" );
	blimg = Image_FromPyObject( value );

	if( self->texture->ima ) {
		self->texture->ima->id.us--;
	}

	self->texture->ima = blimg;
	id_us_plus( &blimg->id );

	return 0;
}

static PyObject *Texture_getImageFlags( BPyTextureObject *self, void *flag )
{
	if (self->texture->imaflag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Texture_setImageFlags( BPyTextureObject * self, PyObject * value, void *flag )
{
	if ( PyObject_IsTrue(value) )
		self->texture->imaflag |= (int)flag;
	else
		self->texture->imaflag &= ~(int)flag;
	return 0;
}

static PyObject *Texture_getIUserFlags( BPyTextureObject *self, void *flag )
{
	if( self->texture->iuser.flag & (int)flag )
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Texture_setIUserFlags( BPyTextureObject * self, PyObject * value,
									void *flag )
{
	if( PyObject_IsTrue(value) )
		self->texture->iuser.flag |= (int)flag;
	else
		self->texture->iuser.flag &= ~(int)flag;
	return 0;
}

static PyObject *Texture_getRGBCol( BPyTextureObject *self )
{
	return Color_CreatePyObject(NULL, 3, BPY_COLOR_IMPLICIT, 0, (PyObject *)self);
}

static int Texture_setRGBCol( BPyTextureObject * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->texture->rfac, &self->texture->gfac, &self->texture->bfac);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->texture->rfac, 0.0f, 1.0f);
	CLAMP(self->texture->gfac, 0.0f, 1.0f);
	CLAMP(self->texture->bfac, 0.0f, 1.0f);
	return 0;
}

static PyObject *Texture_getIpo( BPyTextureObject * self )
{
	return Ipo_CreatePyObject( self->texture->ipo );
}
static int Texture_setIpo( BPyTextureObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->texture->ipo, 0, 1, ID_IP, ID_TE);
}

static PyObject *Texture_evaluate( BPyTextureObject * self, PyObject * value )
{
	TexResult texres= {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, NULL};
	float vec[4];
	/* int rgbnor; dont use now */
	
	if (BPyVector_Check(value)) {
		if(((BPyVectorObject *)value)->size < 3)
			return EXPP_ReturnPyObjError(PyExc_TypeError, 
					"expects a 3D vector object or a tuple of 3 numbers");
		
		/* rgbnor = .. we don't need this now */
		multitex_ext(self->texture, ((BPyVectorObject *)value)->vec, NULL, NULL, 1, &texres);
	} else {
		float vec_in[3];
		if (!PyTuple_Check(value) || PyTuple_Size(value) < 3)
			return EXPP_ReturnPyObjError(PyExc_TypeError, 
					"expects a 3D vector object or a tuple of 3 numbers");
		
		vec_in[0] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, 0));
		vec_in[1] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, 1));
		vec_in[2] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, 2));
		if (PyErr_Occurred())
			return EXPP_ReturnPyObjError(PyExc_TypeError, 
					"expects a 3D vector object or a tuple of 3 numbers");
		
		multitex_ext(self->texture, vec_in, NULL, NULL, 1, &texres);
	}
	vec[0] = texres.tr;
	vec[1] = texres.tg;
	vec[2] = texres.tb;
	vec[3] = texres.tin;
	
	return Vector_CreatePyObject( vec, 3, (PyObject *)NULL );
}

static PyObject *Texture_copy( BPyTextureObject * self )
{
	Tex *tex = copy_texture(self->texture );
	tex->id.us = 0;
	return Texture_CreatePyObject(tex);
}


/*****************************************************************************/
/* Python BPyTextureObject methods table:                                         */
/*****************************************************************************/
static PyMethodDef BPyTexture_methods[] = {
	/* name, method, flags, doc */
	{"evaluate", ( PyCFunction ) Texture_evaluate, METH_O,
	 "(vector) - evaluate the texture at this position"},
	{"__copy__", ( PyCFunction ) Texture_copy, METH_NOARGS,
	 "() - return a copy of the the texture"},
	{"copy", ( PyCFunction ) Texture_copy, METH_NOARGS,
	 "() - return a copy of the the texture"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyTexture_Type attributes get/set structure:                         */
/*****************************************************************************/
static PyGetSetDef BPyTexture_getset[] = {
	{"image",
	 (getter)Texture_getImage, (setter)Texture_setImage,
	 "Texture's image object",
	 NULL},
	{"ipo",
	 (getter)Texture_getIpo, (setter)Texture_setIpo,
	 "Texture Ipo data",
	 NULL},
	{"colorband",
	 (getter)BPyList_get, (setter)BPyList_set,
	 "Use of image RGB values for normal mapping enabled ('ImageFlags')",
	 (void *)BPY_LIST_TYPE_COLORBAND},
	{"color",
	 (getter)Texture_getRGBCol, (setter)Texture_setRGBCol,
	 "RGB color tuple",
	 NULL},
	 
	{"noiseType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Noise type (for wood, stucci, marble, clouds textures)",
	 (void *)EXPP_TEX_ATTR_NOISETYPE},
	{"noiseBasis",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Noise type (for wood, stucci, marble, clouds textures)",
	 (void *)EXPP_TEX_ATTR_NOISEBASIS},
	{"noiseBasis2",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_TEX_ATTR_NOISEBASIS2},
	{"type",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Texture's 'Type' mode",
	 (void *)EXPP_TEX_ATTR_TYPE},
	{"stype",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Texture's 'SType' mode",
	 (void *)EXPP_TEX_ATTR_STYPE},
	{"extend",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Texture's 'Extend' mode (for image textures)",
	 (void *)EXPP_TEX_ATTR_EXTEND},
	 
	/* float */
	{"noiseSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Noise size (wood, stucci, marble, clouds, Musgrave, distorted noise, Voronoi)",
	 (void *)EXPP_TEX_ATTR_NOISESIZE},
	{"turbulence",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Turbulence (for magic, wood, stucci, marble textures)",
	 (void *)EXPP_TEX_ATTR_TURBUL},
	{"brightness",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Changes the brightness of a texture's color",
	 (void *)EXPP_TEX_ATTR_BRIGHT},
	{"contrast",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Changes the contrast of a texture's color",
	 (void *)EXPP_TEX_ATTR_CONTRAST},
	{"filterSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The filter size (for image and envmap textures)",
	 (void *)EXPP_TEX_ATTR_FILTERSIZE},
	{"hFracDim",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Highest fractional dimension (for Musgrave textures)",
	 (void *)EXPP_TEX_ATTR_MG_H},
	{"lacunarity",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Gap between succesive frequencies (for Musgrave textures)",
	 (void *)EXPP_TEX_ATTR_MG_LACUNARITY},
	{"octs",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Number of frequencies (for Musgrave textures)",
	 (void *)EXPP_TEX_ATTR_MG_OCTAVES},
	{"fractalOffser",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "fractal offser (Musgrave only)",
	 (void *)EXPP_TEX_ATTR_MG_OFFSET}, 
	{"gain",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "gain multiplier (for Musgrave textures)",
	 (void *)EXPP_TEX_ATTR_MG_GAIN},
	{"distAmount",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of distortion (for distorted noise textures)",
	 (void *)EXPP_TEX_ATTR_DIST_AMOUNT},
	{"intensityScale",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Intensity output scale (for Musgrave and Voronoi textures)",
	 (void *)EXPP_TEX_ATTR_NS_OUTSCALE}, 
	{"weight1",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Weight 1 (for Voronoi textures)",
	 (void *)EXPP_TEX_ATTR_VN_W1},
	{"weight2",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Weight 2 (for Voronoi textures)",
	 (void *)EXPP_TEX_ATTR_VN_W2},
	{"weight3",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Weight 3 (for Voronoi textures)",
	 (void *)EXPP_TEX_ATTR_VN_W3},
	{"weight4",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Weight 4 (for Voronoi textures)",
	 (void *)EXPP_TEX_ATTR_VN_W4},
	{"exp",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Minkovsky exponent (for Minkovsky Voronoi textures)",
	 (void *)EXPP_TEX_ATTR_VN_MEXP},
	{"cropXMin",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Sets the cropping extents (for image textures)",
	 (void *)EXPP_TEX_ATTR_CROPXMIN},
	{"cropYMin",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Sets the cropping extents (for image textures)",
	 (void *)EXPP_TEX_ATTR_CROPYMIN},
	{"cropXMax",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Sets the cropping extents (for image textures)",
	 (void *)EXPP_TEX_ATTR_CROPXMAX},
	{"cropYMax",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Sets the cropping extents (for image textures)",
	 (void *)EXPP_TEX_ATTR_CROPYMAX},
	{"checkerDist",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Checkers distance (like mortar)",
	 (void *)EXPP_TEX_ATTR_CHECKERDIST},
	{"nabla",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "size of derivative offset used for calculating normal",
	 (void *)EXPP_TEX_ATTR_NABLA},
	{"normalFactor",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "amount the texture affects normal values",
	 (void *)EXPP_TEX_ATTR_NORFAC},
	 /* end floats */
	 
	 /* int */
	{"noiseDepth",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Noise depth (magic, marble, clouds)",
	 (void *)EXPP_TEX_ATTR_NOISEDEPTH},
	{"xRepeat",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Repetition multiplier (for image textures)",
	 (void *)EXPP_TEX_ATTR_XREPEAT},
	{"yRepeat",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Repetition multiplier (for image textures)",
	 (void *)EXPP_TEX_ATTR_YREPEAT},
	{"animFrames",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of frames of a movie",
	 (void *)EXPP_TEX_ATTR_IUSER_FRAMES},
	{"animOffset",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of frames to offset a movie",
	 (void *)EXPP_TEX_ATTR_IUSER_OFFSET},
	{"enableCyclic",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Cycling of animated frames enabled",
	 (void *)EXPP_TEX_ATTR_IUSER_CYCL},
	
	/* Image Flags */
	{"enableColorband",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_COLORBAND},
	{"enableBlendFlip",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_FLIPBLEND},
	{"enableNegAlpha",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_NEGALPHA},
	{"enableCheckerOdd",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_CHECKER_ODD},
	{"enableCheckerEven",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_CHECKER_EVEN},
	{"enableAlpha",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_PRV_ALPHA},
	{"enableNormal",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_PRV_NOR},
	{"enableXMirror",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_REPEAT_XMIR},
	{"enableYMirror",
	 (getter)Texture_getFlags, (setter)Texture_setFlags,
	 "Texture's 'Flag' bits",
	 (void *)TEX_REPEAT_YMIR},
	 	 
	{"enableInterpol",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "Interpolate image's pixels to fit texture mapping enabled ('ImageFlags')",
	 (void *)TEX_INTERPOL},
	{"enableUseAlpha",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "Use of image's alpha channel enabled ('ImageFlags')",
	 (void *)TEX_USEALPHA},
	{"enableMipmap",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "Mipmaps enabled ('ImageFlags')",
	 (void *)TEX_MIPMAP},
	{"enableRot90",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "X/Y flip for rendering enabled ('ImageFlags')",
	 (void *)TEX_IMAROT},
	{"enableCalcAlpha",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "Calculation of image's alpha channel enabled ('ImageFlags')",
	 (void *)TEX_CALCALPHA},
	{"enableNormalMap",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "Use of image RGB values for normal mapping enabled ('ImageFlags')",
	 (void *)TEX_NORMALMAP},
	{"enableCauss",
	 (getter)Texture_getImageFlags, (setter)Texture_setImageFlags,
	 "Use of image RGB values for normal mapping enabled ('ImageFlags')",
	 (void *)TEX_GAUSS_MIP},
	/* IUser flag */
	{"autoRefresh",
	 (getter)Texture_getIUserFlags, (setter)Texture_setIUserFlags,
	 "Refresh image on frame changes enabled",
	 (void *)IMA_ANIM_ALWAYS},
	{NULL}  /* Sentinel */
};


/* this types constructor */
static PyObject *Texture_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Tex";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_texture( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new texture data" );
	
	id->us = 0;
	return Texture_CreatePyObject((Tex *)id);
}

/*****************************************************************************/
/* Python BPyTexture_Type structure definition:                                 */
/*****************************************************************************/
PyTypeObject BPyTexture_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Texture",          /* char *tp_name; */
	sizeof( BPyTextureObject ),      /* int tp_basicsize; */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* long tp_flags; */

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
	BPyTexture_methods,			/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyTexture_getset,			/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc )Texture_new,		/* newfunc tp_new; */
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

PyObject *TextureType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyTexture_Type.tp_dict == NULL ) {
		BPyTexture_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &textureTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &musgraveTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &noiseBasisTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &displaceVeronoiTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &extendTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &noiseTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &cloudTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &woodTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &marbleTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &blendTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &stucciTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &envmapTypes );
		PyConstCategory_AddObjectToDict( BPyTexture_Type.tp_dict, &voronoiTypes );
		PyType_Ready( &BPyTexture_Type ) ;
		BPyTexture_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyTexture_Type ;
}

