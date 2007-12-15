/*  
 *  $Id$
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
#include "constant.h"
#include "blendef.h"
#include "gen_utils.h"
#include "gen_library.h"

#include "vector.h" /* for V24_Texture_evaluate(vec) */
#include "Material.h" /* for V24_EXPP_Colorband_fromPyList and V24_EXPP_PyList_fromColorband */
#include "RE_shader_ext.h"

/*****************************************************************************/
/* Blender.Texture constants                                                 */
/*****************************************************************************/
#define EXPP_TEX_TYPE_NONE                  0

#define EXPP_TEX_TYPE_MIN                   EXPP_TEX_TYPE_NONE
#define EXPP_TEX_TYPE_MAX                   TEX_DISTNOISE

#define EXPP_TEX_ANIMFRAME_MIN              0
#define EXPP_TEX_ANIMFRAME_MAX              ((int)MAXFRAMEF)
#define EXPP_TEX_ANIMLEN_MIN                0
#define EXPP_TEX_ANIMLEN_MAX                ((int)(MAXFRAMEF)/2)
#define EXPP_TEX_ANIMMONSTART_MIN           0
#define EXPP_TEX_ANIMMONSTART_MAX           ((int)MAXFRAMEF)
#define EXPP_TEX_ANIMMONDUR_MIN             0
#define EXPP_TEX_ANIMMONDUR_MAX             250
#define EXPP_TEX_ANIMOFFSET_MIN             -((int)MAXFRAMEF)
#define EXPP_TEX_ANIMOFFSET_MAX             ((int)MAXFRAMEF)
#define EXPP_TEX_ANIMSTART_MIN              1
#define EXPP_TEX_ANIMSTART_MAX              ((int)MAXFRAMEF)
#define EXPP_TEX_FIEIMA_MIN                 1
#define EXPP_TEX_FIEIMA_MAX                 200
#define EXPP_TEX_NOISEDEPTH_MIN             0
#define EXPP_TEX_NOISEDEPTH_MAX             6
/* max depth is different for magic type textures */
#define EXPP_TEX_NOISEDEPTH_MAX_MAGIC       10
#define EXPP_TEX_REPEAT_MIN                 1
#define EXPP_TEX_REPEAT_MAX                 512

#define EXPP_TEX_FILTERSIZE_MIN             0.1f
#define EXPP_TEX_FILTERSIZE_MAX             25.0f
#define EXPP_TEX_NOISESIZE_MIN              0.0001f
#define EXPP_TEX_NOISESIZE_MAX              2.0f
#define EXPP_TEX_BRIGHTNESS_MIN             0.0f
#define EXPP_TEX_BRIGHTNESS_MAX             2.0f
#define EXPP_TEX_CONTRAST_MIN               0.01f
#define EXPP_TEX_CONTRAST_MAX               5.0f
#define EXPP_TEX_CROP_MIN                   -10.0f
#define EXPP_TEX_CROP_MAX                   10.0f
#define EXPP_TEX_RGBCOL_MIN                 0.0f
#define EXPP_TEX_RGBCOL_MAX                 2.0f
#define EXPP_TEX_TURBULENCE_MIN             0.0f
#define EXPP_TEX_TURBULENCE_MAX             200.0f
#define EXPP_TEX_MH_G_MIN                   0.0001f
#define EXPP_TEX_MH_G_MAX                   2.0f
#define EXPP_TEX_LACUNARITY_MIN             0.0f
#define EXPP_TEX_LACUNARITY_MAX             6.0f
#define EXPP_TEX_OCTS_MIN                   0.0f
#define EXPP_TEX_OCTS_MAX                   8.0f
#define EXPP_TEX_ISCALE_MIN                 0.0f
#define EXPP_TEX_ISCALE_MAX                 10.0f
#define EXPP_TEX_EXP_MIN                    0.010f
#define EXPP_TEX_EXP_MAX                    10.0f
#define EXPP_TEX_WEIGHT1_MIN                -2.0f
#define EXPP_TEX_WEIGHT1_MAX                2.0f
#define EXPP_TEX_WEIGHT2_MIN                -2.0f
#define EXPP_TEX_WEIGHT2_MAX                2.0f
#define EXPP_TEX_WEIGHT3_MIN                -2.0f
#define EXPP_TEX_WEIGHT3_MAX                2.0f
#define EXPP_TEX_WEIGHT4_MIN                -2.0f
#define EXPP_TEX_WEIGHT4_MAX                2.0f
#define EXPP_TEX_DISTAMNT_MIN               0.0f
#define EXPP_TEX_DISTAMNT_MAX               10.0f

/* i can't find these defined anywhere- they're just taken from looking at   */
/* the button creation code in source/blender/src/buttons_shading.c          */
/* cloud stype */
#define EXPP_TEX_STYPE_CLD_DEFAULT          0
#define EXPP_TEX_STYPE_CLD_COLOR            1
/* wood stype */
#define EXPP_TEX_STYPE_WOD_BANDS            0
#define EXPP_TEX_STYPE_WOD_RINGS            1
#define EXPP_TEX_STYPE_WOD_BANDNOISE        2
#define EXPP_TEX_STYPE_WOD_RINGNOISE        3
/* magic stype */
#define EXPP_TEX_STYPE_MAG_DEFAULT          0
/* marble stype */
#define EXPP_TEX_STYPE_MBL_SOFT             0
#define EXPP_TEX_STYPE_MBL_SHARP            1
#define EXPP_TEX_STYPE_MBL_SHARPER          2
/* blend stype */
#define EXPP_TEX_STYPE_BLN_LIN              0
#define EXPP_TEX_STYPE_BLN_QUAD             1
#define EXPP_TEX_STYPE_BLN_EASE             2
#define EXPP_TEX_STYPE_BLN_DIAG             3
#define EXPP_TEX_STYPE_BLN_SPHERE           4
#define EXPP_TEX_STYPE_BLN_HALO             5
/* stucci stype */
#define EXPP_TEX_STYPE_STC_PLASTIC          0
#define EXPP_TEX_STYPE_STC_WALLIN           1
#define EXPP_TEX_STYPE_STC_WALLOUT          2
/* noise stype */
#define EXPP_TEX_STYPE_NSE_DEFAULT          0
/* image stype */
#define EXPP_TEX_STYPE_IMG_DEFAULT          0
/* plug-in stype */
#define EXPP_TEX_STYPE_PLG_DEFAULT          0
/* envmap stype */
#define EXPP_TEX_STYPE_ENV_STATIC           0
#define EXPP_TEX_STYPE_ENV_ANIM             1
#define EXPP_TEX_STYPE_ENV_LOAD             2
/* musgrave stype */
#define EXPP_TEX_STYPE_MUS_MFRACTAL         0
#define EXPP_TEX_STYPE_MUS_RIDGEDMF         1
#define EXPP_TEX_STYPE_MUS_HYBRIDMF         2
#define EXPP_TEX_STYPE_MUS_FBM              3
#define EXPP_TEX_STYPE_MUS_HTERRAIN         4
/* voronoi stype */
#define EXPP_TEX_STYPE_VN_INT               0
#define EXPP_TEX_STYPE_VN_COL1              1
#define EXPP_TEX_STYPE_VN_COL2              2
#define EXPP_TEX_STYPE_VN_COL3              3

#define EXPP_TEX_EXTEND_MIN                 TEX_EXTEND
#define EXPP_TEX_EXTEND_MAX                 TEX_CHECKER

#define	EXPP_TEX_NOISE_SINE					0
#define	EXPP_TEX_NOISE_SAW					1
#define	EXPP_TEX_NOISE_TRI					2
#define	EXPP_TEX_NOISEBASIS2				0xffff

/****************************************************************************/
/* Texture String->Int maps                                                 */
/****************************************************************************/

static const V24_EXPP_map_pair V24_tex_type_map[] = {
	{"None", EXPP_TEX_TYPE_NONE},
	{"Clouds", TEX_CLOUDS},
	{"Wood", TEX_WOOD},
	{"Marble", TEX_MARBLE},
	{"Magic", TEX_MAGIC},
	{"Blend", TEX_BLEND},
	{"Stucci", TEX_STUCCI},
	{"Noise", TEX_NOISE},
	{"Image", TEX_IMAGE},
	{"Plugin", TEX_PLUGIN},
	{"EnvMap", TEX_ENVMAP},
	{"Musgrave", TEX_MUSGRAVE},
	{"Voronoi", TEX_VORONOI},
	{"DistortedNoise", TEX_DISTNOISE},
	{NULL, 0}
};

static const V24_EXPP_map_pair V24_tex_flag_map[] = {
/* NOTE "CheckerOdd" and "CheckerEven" are new */
	{"ColorBand",  TEX_COLORBAND },
	{"FlipBlend", TEX_FLIPBLEND},
	{"NegAlpha", TEX_NEGALPHA},
	{"CheckerOdd",TEX_CHECKER_ODD},
	{"CheckerEven",TEX_CHECKER_EVEN},
	{"PreviewAlpha",TEX_PRV_ALPHA},
	{"RepeatXMirror",TEX_REPEAT_XMIR},
	{"RepeatYMirror",TEX_REPEAT_YMIR}, 
	{NULL, 0}
};

/* NOTE: flags moved to image... */
static const V24_EXPP_map_pair V24_tex_imageflag_map[] = {
	{"InterPol", TEX_INTERPOL},
	{"UseAlpha", TEX_USEALPHA},
	{"MipMap", TEX_MIPMAP},
	{"Rot90", TEX_IMAROT},
	{"CalcAlpha", TEX_CALCALPHA},
	{"NormalMap", TEX_NORMALMAP},
	{NULL, 0}
};

static const V24_EXPP_map_pair V24_tex_extend_map[] = {
	{"Extend", TEX_EXTEND},
	{"Clip", TEX_CLIP},
	{"ClipCube", TEX_CLIPCUBE},
	{"Repeat", TEX_REPEAT},
/* NOTE "Checker" is new */
	{"Checker", TEX_CHECKER},
	{NULL, 0}
};

/* array of maps for stype */
static const V24_EXPP_map_pair V24_tex_stype_default_map[] = {
	{"Default", 0},
	{NULL, 0}
};
static const V24_EXPP_map_pair V24_tex_stype_clouds_map[] = {
	{"Default", 0},
	{"CloudDefault", EXPP_TEX_STYPE_CLD_DEFAULT},
	{"CloudColor", EXPP_TEX_STYPE_CLD_COLOR},
	{NULL, 0}
};
static const V24_EXPP_map_pair V24_tex_stype_wood_map[] = {
	{"Default", 0},
	{"WoodBands", EXPP_TEX_STYPE_WOD_BANDS},
	{"WoodRings", EXPP_TEX_STYPE_WOD_RINGS},
	{"WoodBandNoise", EXPP_TEX_STYPE_WOD_BANDNOISE},
	{"WoodRingNoise", EXPP_TEX_STYPE_WOD_RINGNOISE},
	{NULL, 0}
};
static const V24_EXPP_map_pair V24_tex_stype_marble_map[] = {
	{"Default", 0},
	{"MarbleSoft", EXPP_TEX_STYPE_MBL_SOFT},
	{"MarbleSharp", EXPP_TEX_STYPE_MBL_SHARP},
	{"MarbleSharper", EXPP_TEX_STYPE_MBL_SHARPER},
	{NULL, 0}
};
static const V24_EXPP_map_pair V24_tex_stype_blend_map[] = {
	{"Default", 0},
	{"BlendLin", EXPP_TEX_STYPE_BLN_LIN},
	{"BlendQuad", EXPP_TEX_STYPE_BLN_QUAD},
	{"BlendEase", EXPP_TEX_STYPE_BLN_EASE},
	{"BlendDiag", EXPP_TEX_STYPE_BLN_DIAG},
	{"BlendSphere", EXPP_TEX_STYPE_BLN_SPHERE},
	{"BlendHalo", EXPP_TEX_STYPE_BLN_HALO},
	{NULL, 0}
};
static const V24_EXPP_map_pair V24_tex_stype_stucci_map[] = {
	{"Default", 0},
	{"StucciPlastic", EXPP_TEX_STYPE_STC_PLASTIC},
	{"StucciWallIn", EXPP_TEX_STYPE_STC_WALLIN},
	{"StucciWallOut", EXPP_TEX_STYPE_STC_WALLOUT},
	{NULL, 0}
};
static const V24_EXPP_map_pair V24_tex_stype_envmap_map[] = {
	{"Default", 0},
	{"EnvmapStatic", EXPP_TEX_STYPE_ENV_STATIC},
	{"EnvmapAnim", EXPP_TEX_STYPE_ENV_ANIM},
	{"EnvmapLoad", EXPP_TEX_STYPE_ENV_LOAD},
	{NULL, 0}
};

static const V24_EXPP_map_pair V24_tex_stype_musg_map[] = {
	{"Default", 0},
	{"MultiFractal", EXPP_TEX_STYPE_MUS_MFRACTAL},
	{"HeteroTerrain", EXPP_TEX_STYPE_MUS_HTERRAIN},
	{"RidgedMultiFractal", EXPP_TEX_STYPE_MUS_RIDGEDMF},
	{"HybridMultiFractal", EXPP_TEX_STYPE_MUS_HYBRIDMF},
	{"fBM", EXPP_TEX_STYPE_MUS_FBM},
	{NULL, 0}
};

static const V24_EXPP_map_pair V24_tex_stype_distortednoise_map[] = {
	{"Default", 0},
	{"BlenderOriginal", TEX_BLENDER},
	{"OriginalPerlin", TEX_STDPERLIN},
	{"ImprovedPerlin", TEX_NEWPERLIN},
	{"VoronoiF1", TEX_VORONOI_F1},
	{"VoronoiF2", TEX_VORONOI_F2},
	{"VoronoiF3", TEX_VORONOI_F3},
	{"VoronoiF4", TEX_VORONOI_F4},
	{"VoronoiF2-F1", TEX_VORONOI_F2F1},
	{"VoronoiCrackle", TEX_VORONOI_CRACKLE},
	{"CellNoise", TEX_CELLNOISE},
	{NULL, 0}
};

static const V24_EXPP_map_pair V24_tex_stype_voronoi_map[] = {
	{"Default", 0},
	{"Int", EXPP_TEX_STYPE_VN_INT},
	{"Col1", EXPP_TEX_STYPE_VN_COL1},
	{"Col2", EXPP_TEX_STYPE_VN_COL2},
	{"Col3", EXPP_TEX_STYPE_VN_COL3},
	{NULL, 0}
};

static const V24_EXPP_map_pair V24_tex_distance_voronoi_map[] = {
	{"Default", 0},
	{"Distance", TEX_DISTANCE},
	{"DistanceSquared", TEX_DISTANCE_SQUARED},
	{"Manhattan", TEX_MANHATTAN},
	{"Chebychev", TEX_CHEBYCHEV},
	{"MinkovskyHalf", TEX_MINKOVSKY_HALF},
	{"MinkovskyFour", TEX_MINKOVSKY_FOUR},
	{"Minkovsky", TEX_MINKOVSKY},
	{NULL, 0}
};

static const V24_EXPP_map_pair *V24_tex_stype_map[] = {
	V24_tex_stype_default_map,	/* none */
	V24_tex_stype_clouds_map,
	V24_tex_stype_wood_map,
	V24_tex_stype_marble_map,
	V24_tex_stype_default_map,	/* magic */
	V24_tex_stype_blend_map,
	V24_tex_stype_stucci_map,
	V24_tex_stype_default_map,	/* noise */
	V24_tex_stype_default_map,	/* image */
	V24_tex_stype_default_map,	/* plugin */
	V24_tex_stype_envmap_map,
	V24_tex_stype_musg_map,	/* musgrave */
	V24_tex_stype_voronoi_map,	/* voronoi */
	V24_tex_stype_distortednoise_map,	/* distorted noise */
	V24_tex_distance_voronoi_map
};

/*****************************************************************************/
/* Python API function prototypes for the Texture module.                    */
/*****************************************************************************/
static PyObject *V24_M_Texture_New( PyObject * self, PyObject * args,
				PyObject * keywords );
static PyObject *V24_M_Texture_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* Blender.Texture.__doc__                                                   */
/*****************************************************************************/
static char V24_M_Texture_doc[] = "The Blender Texture module\n\
\n\
This module provides access to **Texture** objects in Blender\n";

static char V24_M_Texture_New_doc[] = "Texture.New (name = 'Tex'):\n\
        Return a new Texture object with the given type and name.";

static char V24_M_Texture_Get_doc[] = "Texture.Get (name = None):\n\
        Return the texture with the given 'name', None if not found, or\n\
        Return a list with all texture objects in the current scene,\n\
        if no argument was given.";

/*****************************************************************************/
/* Python method structure definition for Blender.Texture module:            */
/*****************************************************************************/
struct PyMethodDef V24_M_Texture_methods[] = {
	{"New", ( PyCFunction ) V24_M_Texture_New, METH_VARARGS | METH_KEYWORDS,
	 V24_M_Texture_New_doc},
	{"Get", V24_M_Texture_Get, METH_VARARGS, V24_M_Texture_Get_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_BPy_Texture methods declarations:                                  */
/*****************************************************************************/
#define GETFUNC(name)   static PyObject *V24_Texture_##name(V24_BPy_Texture *self)
#define OLDSETFUNC(name)   static PyObject *V24_Texture_old##name(V24_BPy_Texture *self,   \
                                                        PyObject *args)
#define SETFUNC(name)   static int V24_Texture_##name(V24_BPy_Texture *self,   \
                                                        PyObject *value)
#if 0
GETFUNC( getExtend );
GETFUNC( getImage );
GETFUNC( getType );
GETFUNC( getSType );
GETFUNC( clearIpo );
GETFUNC( getAnimMontage );
GETFUNC( getAnimLength );
SETFUNC( setAnimLength );
SETFUNC( setAnimMontage );
#endif

GETFUNC( oldgetSType );
GETFUNC( oldgetType );

GETFUNC( clearIpo );
GETFUNC( getAnimFrames );
GETFUNC( getAnimOffset );
GETFUNC( getAnimStart );
GETFUNC( getBrightness );
GETFUNC( getContrast );
GETFUNC( getCrop );
GETFUNC( getDistAmnt );
GETFUNC( getDistMetric );
GETFUNC( getExp );
GETFUNC( getExtend );
GETFUNC( getIntExtend );
GETFUNC( getFieldsPerImage );
GETFUNC( getFilterSize );
GETFUNC( getFlags );
GETFUNC( getHFracDim );
GETFUNC( getImage );
GETFUNC( getIpo );
GETFUNC( getIScale );
GETFUNC( getLacunarity );
GETFUNC( getNoiseBasis );
GETFUNC( getNoiseDepth );
GETFUNC( getNoiseSize );
GETFUNC( getNoiseType );
GETFUNC( getOcts );
GETFUNC( getRepeat );
GETFUNC( getRGBCol );
GETFUNC( getSType );
GETFUNC( getTurbulence );
GETFUNC( getType );
GETFUNC( getWeight1 );
GETFUNC( getWeight2 );
GETFUNC( getWeight3 );
GETFUNC( getWeight4 );
#if 0
/* not defined */
GETFUNC( getUsers );
#endif

OLDSETFUNC( setDistMetric );
OLDSETFUNC( setDistNoise );	/* special case used for ".noisebasis = ...  */
OLDSETFUNC( setExtend );
OLDSETFUNC( setFlags );
OLDSETFUNC( setImage );
OLDSETFUNC( setImageFlags );
OLDSETFUNC( setIpo );
OLDSETFUNC( setNoiseBasis );
OLDSETFUNC( setSType );
OLDSETFUNC( setType );

SETFUNC( setAnimFrames );
SETFUNC( setAnimOffset );
SETFUNC( setAnimStart );
SETFUNC( setBrightness );
SETFUNC( setContrast );
SETFUNC( setCrop );
SETFUNC( setDistAmnt );
SETFUNC( setDistMetric );
SETFUNC( setExp );
SETFUNC( setIntExtend );
SETFUNC( setFieldsPerImage );
SETFUNC( setFilterSize );
SETFUNC( setFlags );
SETFUNC( setHFracDim );
SETFUNC( setImage );
SETFUNC( setIpo );
SETFUNC( setIScale );
SETFUNC( setLacunarity );
SETFUNC( setNoiseBasis );
SETFUNC( setNoiseDepth );
SETFUNC( setNoiseSize );
SETFUNC( setNoiseType );
SETFUNC( setOcts );
SETFUNC( setRepeat );
SETFUNC( setRGBCol );
SETFUNC( setSType );
SETFUNC( setTurbulence );
SETFUNC( setType );
SETFUNC( setWeight1 );
SETFUNC( setWeight2 );
SETFUNC( setWeight3 );
SETFUNC( setWeight4 );

static PyObject *V24_Texture_getImageFlags( V24_BPy_Texture *self, void *type );
static PyObject *V24_Texture_getIUserFlags( V24_BPy_Texture *self, void *type );
static PyObject *V24_Texture_getIUserCyclic( V24_BPy_Texture *self );
static PyObject *V24_Texture_getNoiseBasis2( V24_BPy_Texture *self, void *type );
static int V24_Texture_setImageFlags( V24_BPy_Texture *self, PyObject *args,
								void *type );
static int V24_Texture_setIUserFlags( V24_BPy_Texture *self, PyObject *args,
								void *type );
static int V24_Texture_setIUserCyclic( V24_BPy_Texture *self, PyObject *args );
static int V24_Texture_setNoiseBasis2( V24_BPy_Texture *self, PyObject *args,
								void *type );
								
static PyObject *V24_Texture_getColorband( V24_BPy_Texture * self);
int V24_Texture_setColorband( V24_BPy_Texture * self, PyObject * value);
static PyObject *V24_Texture_evaluate( V24_BPy_Texture *self, PyObject *value );
static PyObject *V24_Texture_copy( V24_BPy_Texture *self );

/*****************************************************************************/
/* Python V24_BPy_Texture methods table:                                         */
/*****************************************************************************/
static PyMethodDef V24_BPy_Texture_methods[] = {
	/* name, method, flags, doc */
	{"getExtend", ( PyCFunction ) V24_Texture_getExtend, METH_NOARGS,
	 "() - Return Texture extend mode"},
	{"getImage", ( PyCFunction ) V24_Texture_getImage, METH_NOARGS,
	 "() - Return Texture Image"},
	{"getName", ( PyCFunction ) V24_GenericLib_getName, METH_NOARGS,
	 "() - Return Texture name"},
	{"getSType", ( PyCFunction ) V24_Texture_oldgetSType, METH_NOARGS,
	 "() - Return Texture stype as string"},
	{"getType", ( PyCFunction ) V24_Texture_oldgetType, METH_NOARGS,
	 "() - Return Texture type as string"},
	{"getIpo", ( PyCFunction ) V24_Texture_getIpo, METH_NOARGS,
	 "() - Return Texture Ipo"},
	{"setIpo", ( PyCFunction ) V24_Texture_oldsetIpo, METH_VARARGS,
	 "(Blender Ipo) - Set Texture Ipo"},
	{"clearIpo", ( PyCFunction ) V24_Texture_clearIpo, METH_NOARGS,
	 "() - Unlink Ipo from this Texture."},
	{"setExtend", ( PyCFunction ) V24_Texture_oldsetExtend, METH_VARARGS,
	 "(s) - Set Texture extend mode"},
	{"setFlags", ( PyCFunction ) V24_Texture_oldsetFlags, METH_VARARGS,
	 "(f1,f2,f3,f4,f5) - Set Texture flags"},
	{"setImage", ( PyCFunction ) V24_Texture_oldsetImage, METH_VARARGS,
	 "(Blender Image) - Set Texture Image"},
	{"setImageFlags", ( PyCFunction ) V24_Texture_oldsetImageFlags, METH_VARARGS,
	 "(s,s,s,s,...) - Set Texture image flags"},
	{"setName", ( PyCFunction ) V24_GenericLib_setName_with_method, METH_VARARGS,
	 "(s) - Set Texture name"},
	{"setSType", ( PyCFunction ) V24_Texture_oldsetSType, METH_VARARGS,
	 "(s) - Set Texture stype"},
	{"setType", ( PyCFunction ) V24_Texture_oldsetType, METH_VARARGS,
	 "(s) - Set Texture type"},
	{"setNoiseBasis", ( PyCFunction ) V24_Texture_oldsetNoiseBasis, METH_VARARGS,
	 "(s) - Set Noise basis"},
	{"setDistNoise", ( PyCFunction ) V24_Texture_oldsetDistNoise, METH_VARARGS,
	 "(s) - Set Dist Noise"},
	{"setDistMetric", ( PyCFunction ) V24_Texture_oldsetDistMetric, METH_VARARGS,
	 "(s) - Set Dist Metric"},
	{"evaluate", ( PyCFunction ) V24_Texture_evaluate, METH_O,
	 "(vector) - evaluate the texture at this position"},
	{"__copy__", ( PyCFunction ) V24_Texture_copy, METH_NOARGS,
	 "() - return a copy of the the texture"},
	{"copy", ( PyCFunction ) V24_Texture_copy, METH_NOARGS,
	 "() - return a copy of the the texture"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_Texture_Type attributes get/set structure:                         */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Texture_getseters[] = {
	GENERIC_LIB_GETSETATTR,
	{"animFrames",
	 (getter)V24_Texture_getAnimFrames, (setter)V24_Texture_setAnimFrames,
	 "Number of frames of a movie to use",
	 NULL},
#if 0
	{"animLength",
	 (getter)V24_Texture_getAnimLength, (setter)V24_Texture_setAnimLength,
	 "Number of frames of a movie to use (0 for all)",
	 NULL},
	{"animMontage",
	 (getter)V24_Texture_getAnimMontage, (setter)V24_Texture_setAnimMontage,
	 "Montage mode, start frames and durations",
	 NULL},
#endif
	{"animOffset",
	 (getter)V24_Texture_getAnimOffset, (setter)V24_Texture_setAnimOffset,
	 "Offsets the number of the first movie frame to use",
	 NULL},
	{"animStart",
	 (getter)V24_Texture_getAnimStart, (setter)V24_Texture_setAnimStart,
	 "Starting frame of the movie to use",
	 NULL},
	{"brightness",
	 (getter)V24_Texture_getBrightness, (setter)V24_Texture_setBrightness,
	 "Changes the brightness of a texture's color",
	 NULL},
	{"contrast",
	 (getter)V24_Texture_getContrast, (setter)V24_Texture_setContrast,
	 "Changes the contrast of a texture's color",
	 NULL},
	{"crop",
	 (getter)V24_Texture_getCrop, (setter)V24_Texture_setCrop,
	 "Sets the cropping extents (for image textures)",
	 NULL},
	{"distAmnt",
	 (getter)V24_Texture_getDistAmnt, (setter)V24_Texture_setDistAmnt,
	 "Amount of distortion (for distorted noise textures)",
	 NULL},
	{"distMetric",
	 (getter)V24_Texture_getDistMetric, (setter)V24_Texture_setDistMetric,
	 "The distance metric (for Voronoi textures)",
	 NULL},
	{"exp",
	 (getter)V24_Texture_getExp, (setter)V24_Texture_setExp,
	 "Minkovsky exponent (for Minkovsky Voronoi textures)",
	 NULL},
	{"extend",
	 (getter)V24_Texture_getIntExtend, (setter)V24_Texture_setIntExtend,
	 "Texture's 'Extend' mode (for image textures)",
	 NULL},
	{"fieldsPerImage",
	 (getter)V24_Texture_getFieldsPerImage, (setter)V24_Texture_setFieldsPerImage,
	 "Number of fields per rendered frame",
	 NULL},
	{"filterSize",
	 (getter)V24_Texture_getFilterSize, (setter)V24_Texture_setFilterSize,
	 "The filter size (for image and envmap textures)",
	 NULL},
	{"flags",
	 (getter)V24_Texture_getFlags, (setter)V24_Texture_setFlags,
	 "Texture's 'Flag' bits",
	 NULL},
	{"hFracDim",
	 (getter)V24_Texture_getHFracDim, (setter)V24_Texture_setHFracDim,
	 "Highest fractional dimension (for Musgrave textures)",
	 NULL},
	{"imageFlags",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Texture's 'ImageFlags' bits",
	 NULL},
	{"image",
	 (getter)V24_Texture_getImage, (setter)V24_Texture_setImage,
	 "Texture's image object",
	 NULL},
	{"ipo",
	 (getter)V24_Texture_getIpo, (setter)V24_Texture_setIpo,
	 "Texture Ipo data",
	 NULL},
	{"iScale",
	 (getter)V24_Texture_getIScale, (setter)V24_Texture_setIScale,
	 "Intensity output scale (for Musgrave and Voronoi textures)",
	 NULL},
	{"lacunarity",
	 (getter)V24_Texture_getLacunarity, (setter)V24_Texture_setLacunarity,
	 "Gap between succesive frequencies (for Musgrave textures)",
	 NULL},
	{"noiseBasis",
	 (getter)V24_Texture_getNoiseBasis, (setter)V24_Texture_setNoiseBasis,
	 "Noise basis type (wood, stucci, marble, clouds, Musgrave, distorted noise)",
	 NULL},
	{"noiseBasis2",
	 (getter)V24_Texture_getNoiseBasis2, (setter)V24_Texture_setNoiseBasis2,
	 "Additional noise basis type (wood, marble, distorted noise)",
	 (void *)EXPP_TEX_NOISEBASIS2},
	{"noiseDepth",
	 (getter)V24_Texture_getNoiseDepth, (setter)V24_Texture_setNoiseDepth,
	 "Noise depth (magic, marble, clouds)",
	 NULL},
	{"noiseSize",
	 (getter)V24_Texture_getNoiseSize, (setter)V24_Texture_setNoiseSize,
	 "Noise size (wood, stucci, marble, clouds, Musgrave, distorted noise, Voronoi)",
	 NULL},
/* NOTE for API rewrite: should use dict constants instead of strings */
	{"noiseType",
	 (getter)V24_Texture_getNoiseType, (setter)V24_Texture_setNoiseType,
	 "Noise type (for wood, stucci, marble, clouds textures)",
	 NULL},
	{"octs",
	 (getter)V24_Texture_getOcts, (setter)V24_Texture_setOcts,
	 "Number of frequencies (for Musgrave textures)",
	 NULL},
	{"repeat",
	 (getter)V24_Texture_getRepeat, (setter)V24_Texture_setRepeat,
	 "Repetition multiplier (for image textures)",
	 NULL},
	{"rgbCol",
	 (getter)V24_Texture_getRGBCol, (setter)V24_Texture_setRGBCol,
	 "RGB color tuple",
	 NULL},
	{"stype",
	 (getter)V24_Texture_getSType, (setter)V24_Texture_setSType,
	 "Texture's 'SType' mode",
	 NULL},
	{"turbulence",
	 (getter)V24_Texture_getTurbulence, (setter)V24_Texture_setTurbulence,
	 "Turbulence (for magic, wood, stucci, marble textures)",
	 NULL},
	{"type",
	 (getter)V24_Texture_getType, (setter)V24_Texture_setType,
	 "Texture's 'Type' mode",
	 NULL},
	{"weight1",
	 (getter)V24_Texture_getWeight1, (setter)V24_Texture_setWeight1,
	 "Weight 1 (for Voronoi textures)",
	 NULL},
	{"weight2",
	 (getter)V24_Texture_getWeight2, (setter)V24_Texture_setWeight2,
	 "Weight 2 (for Voronoi textures)",
	 NULL},
	{"weight3",
	 (getter)V24_Texture_getWeight3, (setter)V24_Texture_setWeight3,
	 "Weight 3 (for Voronoi textures)",
	 NULL},
	{"weight4",
	 (getter)V24_Texture_getWeight4, (setter)V24_Texture_setWeight4,
	 "Weight 4 (for Voronoi textures)",
	 NULL},
	{"sine",
	 (getter)V24_Texture_getNoiseBasis2, (setter)V24_Texture_setNoiseBasis2,
	 "Produce bands using sine wave (marble, wood textures)",
	 (void *)EXPP_TEX_NOISE_SINE},
	{"saw",
	 (getter)V24_Texture_getNoiseBasis2, (setter)V24_Texture_setNoiseBasis2,
	 "Produce bands using saw wave (marble, wood textures)",
	 (void *)EXPP_TEX_NOISE_SAW},
	{"tri",
	 (getter)V24_Texture_getNoiseBasis2, (setter)V24_Texture_setNoiseBasis2,
	 "Produce bands using triangle wave (marble, wood textures)",
	 (void *)EXPP_TEX_NOISE_TRI},
	{"interpol",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Interpolate image's pixels to fit texture mapping enabled ('ImageFlags')",
	 (void *)TEX_INTERPOL},
	{"useAlpha",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Use of image's alpha channel enabled ('ImageFlags')",
	 (void *)TEX_USEALPHA},
	{"calcAlpha",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Calculation of image's alpha channel enabled ('ImageFlags')",
	 (void *)TEX_CALCALPHA},
	{"mipmap",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Mipmaps enabled ('ImageFlags')",
	 (void *)TEX_MIPMAP},
	{"rot90",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "X/Y flip for rendering enabled ('ImageFlags')",
	 (void *)TEX_IMAROT},
	{"autoRefresh",
	 (getter)V24_Texture_getIUserFlags, (setter)V24_Texture_setIUserFlags,
	 "Refresh image on frame changes enabled",
	 (void *)IMA_ANIM_ALWAYS},
	{"cyclic",
	 (getter)V24_Texture_getIUserCyclic, (setter)V24_Texture_setIUserCyclic,
	 "Cycling of animated frames enabled",
	 NULL},
#if 0
	/* disabled, moved to image */
	{"fields",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Use of image's fields enabled ('ImageFlags')",
	 (void *)TEX_FIELDS},
	{"movie",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Movie frames as images enabled ('ImageFlags')",
	 (void *)TEX_ANIM5},
	{"anti",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Image anti-aliasing enabled ('ImageFlags')",
	 (void *)TEX_ANTIALI},
	{"stField",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Standard field deinterlacing enabled ('ImageFlags')",
	 (void *)TEX_STD_FIELD},
#endif
	{"normalMap",
	 (getter)V24_Texture_getImageFlags, (setter)V24_Texture_setImageFlags,
	 "Use of image RGB values for normal mapping enabled ('ImageFlags')",
	 (void *)TEX_NORMALMAP},
	{"colorband",
	 (getter)V24_Texture_getColorband, (setter)V24_Texture_setColorband,
	 "The colorband for this texture",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python V24_Texture_Type callback function prototypes:                         */
/*****************************************************************************/
static int V24_Texture_compare( V24_BPy_Texture * a, V24_BPy_Texture * b );
static PyObject *V24_Texture_repr( V24_BPy_Texture * self );

/*****************************************************************************/
/* Python V24_Texture_Type structure definition:                                 */
/*****************************************************************************/
PyTypeObject V24_Texture_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Texture",          /* char *tp_name; */
	sizeof( V24_BPy_Texture ),      /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,						/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Texture_compare, /* cmpfunc tp_compare; */
	( reprfunc ) V24_Texture_repr,  /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	( hashfunc ) V24_GenericLib_hash,	/* hashfunc tp_hash; */
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
	V24_BPy_Texture_methods,        /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Texture_getseters,      /* struct PyGetSetDef *tp_getset; */
	NULL,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	NULL,                       /* newfunc tp_new; */
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

static PyObject *V24_M_Texture_New( PyObject * self, PyObject * args,
				PyObject * kwords )
{
	char *name_str = "Tex";
	static char *kwlist[] = { "name_str", NULL };
	PyObject *pytex;	/* for Texture object wrapper in Python */
	Tex *bltex;		/* for actual Tex we create in Blender */

	/* Parse the arguments passed in by the Python interpreter */
	if( !PyArg_ParseTupleAndKeywords
	    ( args, kwords, "|s", kwlist, &name_str ) )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "expected zero, one or two strings as arguments" );

	bltex = add_texture( name_str );  /* first create the texture in Blender */

	if( bltex )		/* now create the wrapper obj in Python */
		pytex = V24_Texture_CreatePyObject( bltex );
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Texture in Blender" );

	/* let's return user count to zero, because add_texture() incref'd it */
	bltex->id.us = 0;

	if( pytex == NULL )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Tex PyObject" );

	return pytex;
}

static PyObject *V24_M_Texture_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Tex *tex_iter;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument (or nothing)" );

	tex_iter = G.main->tex.first;

	if( name ) {		/* (name) - Search for texture by name */

		PyObject *wanted_tex = NULL;

		while( tex_iter ) {
			if( STREQ( name, tex_iter->id.name + 2 ) ) {
				wanted_tex =
					V24_Texture_CreatePyObject( tex_iter );
				break;
			}

			tex_iter = tex_iter->id.next;
		}

		if( !wanted_tex ) {	/* Requested texture doesn't exist */
			char error_msg[64];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Texture \"%s\" not found", name );
			return V24_EXPP_ReturnPyObjError( PyExc_NameError,
						      error_msg );
		}

		return wanted_tex;
	}

	else {			/* () - return a list of wrappers for all textures in the scene */
		int index = 0;
		PyObject *tex_pylist, *pyobj;

		tex_pylist = PyList_New( BLI_countlist( &( G.main->tex ) ) );
		if( !tex_pylist )
			return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						      "couldn't create PyList" );

		while( tex_iter ) {
			pyobj = V24_Texture_CreatePyObject( tex_iter );
			if( !pyobj ) {
				Py_DECREF(tex_pylist);
				return V24_EXPP_ReturnPyObjError
					( PyExc_MemoryError,
					  "couldn't create Texture PyObject" );
			}
			PyList_SET_ITEM( tex_pylist, index, pyobj );

			tex_iter = tex_iter->id.next;
			index++;
		}

		return tex_pylist;
	}
}

static int V24_Texture_compare( V24_BPy_Texture * a, V24_BPy_Texture * b )
{
	return ( a->texture == b->texture ) ? 0 : -1;
}

static PyObject *V24_Texture_repr( V24_BPy_Texture * self )
{
	return PyString_FromFormat( "[Texture \"%s\"]",
				    self->texture->id.name + 2 );
}

static PyObject *V24_M_Texture_TypesDict( void )
{
	PyObject *Types = V24_PyConstant_New(  );
	if( Types ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Types;
		V24_PyConstant_Insert(d, "NONE", PyInt_FromLong(EXPP_TEX_TYPE_NONE));
		V24_PyConstant_Insert(d, "CLOUDS", PyInt_FromLong(TEX_CLOUDS));
		V24_PyConstant_Insert(d, "WOOD", PyInt_FromLong(TEX_WOOD));
		V24_PyConstant_Insert(d, "MARBLE", PyInt_FromLong(TEX_MARBLE));
		V24_PyConstant_Insert(d, "MAGIC", PyInt_FromLong(TEX_MAGIC));
		V24_PyConstant_Insert(d, "BLEND", PyInt_FromLong(TEX_BLEND));
		V24_PyConstant_Insert(d, "STUCCI", PyInt_FromLong(TEX_STUCCI));
		V24_PyConstant_Insert(d, "NOISE", PyInt_FromLong(TEX_NOISE));
		V24_PyConstant_Insert(d, "IMAGE", PyInt_FromLong(TEX_IMAGE));
		V24_PyConstant_Insert(d, "PLUGIN", PyInt_FromLong(TEX_PLUGIN));
		V24_PyConstant_Insert(d, "ENVMAP", PyInt_FromLong(TEX_ENVMAP));
		V24_PyConstant_Insert(d, "MUSGRAVE", PyInt_FromLong(TEX_MUSGRAVE));
		V24_PyConstant_Insert(d, "VORONOI", PyInt_FromLong(TEX_VORONOI));
		V24_PyConstant_Insert(d, "DISTNOISE", PyInt_FromLong(TEX_DISTNOISE)); 
	}
	return Types;
}

static PyObject *V24_M_Texture_STypesDict( void )
{
	PyObject *STypes = V24_PyConstant_New(  );
	if( STypes ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) STypes;

		V24_PyConstant_Insert(d, "CLD_DEFAULT",
					PyInt_FromLong(EXPP_TEX_STYPE_CLD_DEFAULT));
		V24_PyConstant_Insert(d, "CLD_COLOR",
					PyInt_FromLong(EXPP_TEX_STYPE_CLD_COLOR));
		V24_PyConstant_Insert(d, "WOD_BANDS",
					PyInt_FromLong(EXPP_TEX_STYPE_WOD_BANDS));
		V24_PyConstant_Insert(d, "WOD_RINGS",
					PyInt_FromLong(EXPP_TEX_STYPE_WOD_RINGS));
		V24_PyConstant_Insert(d, "WOD_BANDNOISE",
					PyInt_FromLong(EXPP_TEX_STYPE_WOD_BANDNOISE));
		V24_PyConstant_Insert(d, "WOD_RINGNOISE",
					PyInt_FromLong(EXPP_TEX_STYPE_WOD_RINGNOISE));
		V24_PyConstant_Insert(d, "MAG_DEFAULT",
					PyInt_FromLong(EXPP_TEX_STYPE_MAG_DEFAULT));
		V24_PyConstant_Insert(d, "MBL_SOFT",
					PyInt_FromLong(EXPP_TEX_STYPE_MBL_SOFT));
		V24_PyConstant_Insert(d, "MBL_SHARP",
					PyInt_FromLong(EXPP_TEX_STYPE_MBL_SHARP));
		V24_PyConstant_Insert(d, "MBL_SHARPER",
					PyInt_FromLong(EXPP_TEX_STYPE_MBL_SHARPER));
		V24_PyConstant_Insert(d, "BLN_LIN",
					PyInt_FromLong(EXPP_TEX_STYPE_BLN_LIN));
		V24_PyConstant_Insert(d, "BLN_QUAD",
					PyInt_FromLong(EXPP_TEX_STYPE_BLN_QUAD));
		V24_PyConstant_Insert(d, "BLN_EASE",
					PyInt_FromLong(EXPP_TEX_STYPE_BLN_EASE));
		V24_PyConstant_Insert(d, "BLN_DIAG",
					PyInt_FromLong(EXPP_TEX_STYPE_BLN_DIAG));
		V24_PyConstant_Insert(d, "BLN_SPHERE",
					PyInt_FromLong(EXPP_TEX_STYPE_BLN_SPHERE));
		V24_PyConstant_Insert(d, "BLN_HALO",
					PyInt_FromLong(EXPP_TEX_STYPE_BLN_HALO));
		V24_PyConstant_Insert(d, "STC_PLASTIC",
					PyInt_FromLong(EXPP_TEX_STYPE_STC_PLASTIC));
		V24_PyConstant_Insert(d, "STC_WALLIN",
					PyInt_FromLong(EXPP_TEX_STYPE_STC_WALLIN));
		V24_PyConstant_Insert(d, "STC_WALLOUT",
					PyInt_FromLong(EXPP_TEX_STYPE_STC_WALLOUT));
		V24_PyConstant_Insert(d, "NSE_DEFAULT",
					PyInt_FromLong(EXPP_TEX_STYPE_NSE_DEFAULT));
		V24_PyConstant_Insert(d, "IMG_DEFAULT",
					PyInt_FromLong(EXPP_TEX_STYPE_IMG_DEFAULT));
		V24_PyConstant_Insert(d, "PLG_DEFAULT",
					PyInt_FromLong(EXPP_TEX_STYPE_PLG_DEFAULT));
		V24_PyConstant_Insert(d, "ENV_STATIC",
					PyInt_FromLong(EXPP_TEX_STYPE_ENV_STATIC));
		V24_PyConstant_Insert(d, "ENV_ANIM",
					PyInt_FromLong(EXPP_TEX_STYPE_ENV_ANIM));
		V24_PyConstant_Insert(d, "ENV_LOAD",
					PyInt_FromLong(EXPP_TEX_STYPE_ENV_LOAD));
		V24_PyConstant_Insert(d, "MUS_MFRACTAL",
					PyInt_FromLong(EXPP_TEX_STYPE_MUS_MFRACTAL));
		V24_PyConstant_Insert(d, "MUS_RIDGEDMF",
					PyInt_FromLong(EXPP_TEX_STYPE_MUS_RIDGEDMF));
		V24_PyConstant_Insert(d, "MUS_HYBRIDMF",
					PyInt_FromLong(EXPP_TEX_STYPE_MUS_HYBRIDMF));
		V24_PyConstant_Insert(d, "MUS_FBM",
					PyInt_FromLong(EXPP_TEX_STYPE_MUS_FBM));
		V24_PyConstant_Insert(d, "MUS_HTERRAIN",
					PyInt_FromLong(EXPP_TEX_STYPE_MUS_HTERRAIN));
		V24_PyConstant_Insert(d, "DN_BLENDER",
					PyInt_FromLong(TEX_BLENDER));
		V24_PyConstant_Insert(d, "DN_PERLIN",
					PyInt_FromLong(TEX_STDPERLIN));
		V24_PyConstant_Insert(d, "DN_IMPROVEDPERLIN",
					PyInt_FromLong(TEX_NEWPERLIN));
		V24_PyConstant_Insert(d, "DN_VORONOIF1",
					PyInt_FromLong(TEX_VORONOI_F1));
		V24_PyConstant_Insert(d, "DN_VORONOIF2",
					PyInt_FromLong(TEX_VORONOI_F2));
		V24_PyConstant_Insert(d, "DN_VORONOIF3",
					PyInt_FromLong(TEX_VORONOI_F3));
		V24_PyConstant_Insert(d, "DN_VORONOIF4",
					PyInt_FromLong(TEX_VORONOI_F4));
		V24_PyConstant_Insert(d, "DN_VORONOIF2F1",
					PyInt_FromLong(TEX_VORONOI_F2F1));
		V24_PyConstant_Insert(d, "DN_VORONOICRACKLE",
					PyInt_FromLong(TEX_VORONOI_CRACKLE));
		V24_PyConstant_Insert(d, "DN_CELLNOISE",
					PyInt_FromLong(TEX_CELLNOISE));
		V24_PyConstant_Insert(d, "VN_INT",
					PyInt_FromLong(EXPP_TEX_STYPE_VN_INT));
		V24_PyConstant_Insert(d, "VN_COL1",
					PyInt_FromLong(EXPP_TEX_STYPE_VN_COL1));
		V24_PyConstant_Insert(d, "VN_COL2",
					PyInt_FromLong(EXPP_TEX_STYPE_VN_COL2));
		V24_PyConstant_Insert(d, "VN_COL3",
					PyInt_FromLong(EXPP_TEX_STYPE_VN_COL3));
		V24_PyConstant_Insert(d, "VN_TEX_DISTANCE",
					PyInt_FromLong(TEX_DISTANCE));
		V24_PyConstant_Insert(d, "VN_TEX_DISTANCE_SQUARED",
					PyInt_FromLong(TEX_DISTANCE_SQUARED));
		V24_PyConstant_Insert(d, "VN_TEX_MANHATTAN",
					PyInt_FromLong(TEX_MANHATTAN));
		V24_PyConstant_Insert(d, "VN_TEX_CHEBYCHEV",
					PyInt_FromLong(TEX_CHEBYCHEV));
		V24_PyConstant_Insert(d, "VN_TEX_MINKOVSKY_HALF",
					PyInt_FromLong(TEX_MINKOVSKY_HALF));
		V24_PyConstant_Insert(d, "VN_TEX_MINKOVSKY_FOUR",
					PyInt_FromLong(TEX_MINKOVSKY_FOUR));
		V24_PyConstant_Insert(d, "VN_TEX_MINKOVSKY",
					PyInt_FromLong(TEX_MINKOVSKY));

	}
	return STypes;
}

static PyObject *V24_M_Texture_TexCoDict( void )
{
	PyObject *TexCo = V24_PyConstant_New(  );
	if( TexCo ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) TexCo;
		V24_PyConstant_Insert(d, "ORCO", PyInt_FromLong(TEXCO_ORCO));
		V24_PyConstant_Insert(d, "REFL", PyInt_FromLong(TEXCO_REFL));
		V24_PyConstant_Insert(d, "NOR", PyInt_FromLong(TEXCO_NORM));
		V24_PyConstant_Insert(d, "GLOB", PyInt_FromLong(TEXCO_GLOB));
		V24_PyConstant_Insert(d, "UV", PyInt_FromLong(TEXCO_UV));
		V24_PyConstant_Insert(d, "OBJECT", PyInt_FromLong(TEXCO_OBJECT));
		V24_PyConstant_Insert(d, "WIN", PyInt_FromLong(TEXCO_WINDOW));
		V24_PyConstant_Insert(d, "VIEW", PyInt_FromLong(TEXCO_VIEW));
		V24_PyConstant_Insert(d, "STICK", PyInt_FromLong(TEXCO_STICKY));
		V24_PyConstant_Insert(d, "STRESS", PyInt_FromLong(TEXCO_STRESS));
		V24_PyConstant_Insert(d, "TANGENT", PyInt_FromLong(TEXCO_TANGENT));
	}
	return TexCo;
}

static PyObject *V24_M_Texture_MapToDict( void )
{
	PyObject *MapTo = V24_PyConstant_New(  );
	if( MapTo ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) MapTo;
		V24_PyConstant_Insert(d, "COL", PyInt_FromLong(MAP_COL));
		V24_PyConstant_Insert(d, "NOR", PyInt_FromLong(MAP_NORM));
		V24_PyConstant_Insert(d, "CSP", PyInt_FromLong(MAP_COLSPEC));
		V24_PyConstant_Insert(d, "CMIR", PyInt_FromLong(MAP_COLMIR));
		V24_PyConstant_Insert(d, "REF", PyInt_FromLong(MAP_REF));
		V24_PyConstant_Insert(d, "SPEC", PyInt_FromLong(MAP_SPEC));
		V24_PyConstant_Insert(d, "HARD", PyInt_FromLong(MAP_HAR));
		V24_PyConstant_Insert(d, "ALPHA", PyInt_FromLong(MAP_ALPHA));
		V24_PyConstant_Insert(d, "EMIT", PyInt_FromLong(MAP_EMIT));
		V24_PyConstant_Insert(d, "RAYMIR", PyInt_FromLong(MAP_RAYMIRR));
		V24_PyConstant_Insert(d, "AMB", PyInt_FromLong(MAP_AMB));
		V24_PyConstant_Insert(d, "TRANSLU", PyInt_FromLong(MAP_TRANSLU));
		V24_PyConstant_Insert(d, "DISP", PyInt_FromLong(MAP_DISPLACE));
		V24_PyConstant_Insert(d, "WARP", PyInt_FromLong(MAP_WARP));
	}
	return MapTo;
}

static PyObject *V24_M_Texture_FlagsDict( void )
{
	PyObject *Flags = V24_PyConstant_New(  );
	if( Flags ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Flags;
		V24_PyConstant_Insert(d, "COLORBAND", PyInt_FromLong(TEX_COLORBAND));
		V24_PyConstant_Insert(d, "FLIPBLEND", PyInt_FromLong(TEX_FLIPBLEND));
		V24_PyConstant_Insert(d, "NEGALPHA", PyInt_FromLong(TEX_NEGALPHA));
		V24_PyConstant_Insert(d, "CHECKER_ODD", PyInt_FromLong(TEX_CHECKER_ODD)); 
		V24_PyConstant_Insert(d, "CHECKER_EVEN", PyInt_FromLong(TEX_CHECKER_EVEN));
		V24_PyConstant_Insert(d, "PREVIEW_ALPHA", PyInt_FromLong(TEX_PRV_ALPHA));
		V24_PyConstant_Insert(d, "REPEAT_XMIR", PyInt_FromLong(TEX_REPEAT_XMIR));
		V24_PyConstant_Insert(d, "REPEAT_YMIR", PyInt_FromLong(TEX_REPEAT_YMIR));
	}
	return Flags;
}

static PyObject *V24_M_Texture_ExtendModesDict( void )
{
	PyObject *ExtendModes = V24_PyConstant_New(  );
	if( ExtendModes ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) ExtendModes;
		V24_PyConstant_Insert(d, "EXTEND", PyInt_FromLong(TEX_EXTEND));
		V24_PyConstant_Insert(d, "CLIP", PyInt_FromLong(TEX_CLIP));
		V24_PyConstant_Insert(d, "CLIPCUBE", PyInt_FromLong(TEX_CLIPCUBE));
		V24_PyConstant_Insert(d, "REPEAT", PyInt_FromLong(TEX_REPEAT));
	}
	return ExtendModes;
}

static PyObject *V24_M_Texture_ImageFlagsDict( void )
{
	PyObject *ImageFlags = V24_PyConstant_New(  );
	if( ImageFlags ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) ImageFlags;
		V24_PyConstant_Insert(d, "INTERPOL", PyInt_FromLong(TEX_INTERPOL));
		V24_PyConstant_Insert(d, "USEALPHA", PyInt_FromLong(TEX_USEALPHA));
		V24_PyConstant_Insert(d, "MIPMAP", PyInt_FromLong(TEX_MIPMAP));
		V24_PyConstant_Insert(d, "ROT90", PyInt_FromLong(TEX_IMAROT));
		V24_PyConstant_Insert(d, "CALCALPHA", PyInt_FromLong(TEX_CALCALPHA));
		V24_PyConstant_Insert(d, "NORMALMAP", PyInt_FromLong(TEX_NORMALMAP));
	}
	return ImageFlags;
}

static PyObject *V24_M_Texture_NoiseDict( void )
{
	PyObject *Noise = V24_PyConstant_New(  );
	if( Noise ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Noise;
		V24_PyConstant_Insert(d, "SINE", PyInt_FromLong(EXPP_TEX_NOISE_SINE));
		V24_PyConstant_Insert(d, "SAW", PyInt_FromLong(EXPP_TEX_NOISE_SAW));
		V24_PyConstant_Insert(d, "TRI", PyInt_FromLong(EXPP_TEX_NOISE_TRI));
		V24_PyConstant_Insert(d, "BLENDER", PyInt_FromLong(TEX_BLENDER));
		V24_PyConstant_Insert(d, "PERLIN", PyInt_FromLong(TEX_STDPERLIN));
		V24_PyConstant_Insert(d, "IMPROVEDPERLIN", PyInt_FromLong(TEX_NEWPERLIN));
		V24_PyConstant_Insert(d, "VORONOIF1", PyInt_FromLong(TEX_VORONOI_F1));
		V24_PyConstant_Insert(d, "VORONOIF2", PyInt_FromLong(TEX_VORONOI_F2));
		V24_PyConstant_Insert(d, "VORONOIF3", PyInt_FromLong(TEX_VORONOI_F3));
		V24_PyConstant_Insert(d, "VORONOIF4", PyInt_FromLong(TEX_VORONOI_F4));
		V24_PyConstant_Insert(d, "VORONOIF2F1", PyInt_FromLong(TEX_VORONOI_F2F1));
		V24_PyConstant_Insert(d, "VORONOICRACKLE",
					PyInt_FromLong(TEX_VORONOI_CRACKLE));
		V24_PyConstant_Insert(d, "CELLNOISE", PyInt_FromLong(TEX_CELLNOISE));
	}
	return Noise;
}

static PyObject *V24_M_Texture_BlendModesDict( void )
{
	PyObject *BlendModes = V24_PyConstant_New(  );
	if( BlendModes ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) BlendModes;
		V24_PyConstant_Insert(d, "MIX", PyInt_FromLong(MTEX_BLEND));
		V24_PyConstant_Insert(d, "MULTIPLY", PyInt_FromLong(MTEX_MUL));
		V24_PyConstant_Insert(d, "ADD", PyInt_FromLong(MTEX_ADD));
		V24_PyConstant_Insert(d, "SUBTRACT", PyInt_FromLong(MTEX_SUB));
		V24_PyConstant_Insert(d, "DIVIDE", PyInt_FromLong(MTEX_DIV));
		V24_PyConstant_Insert(d, "DARKEN", PyInt_FromLong(MTEX_DARK));
		V24_PyConstant_Insert(d, "DIFFERENCE", PyInt_FromLong(MTEX_DIFF));
		V24_PyConstant_Insert(d, "LIGHTEN", PyInt_FromLong(MTEX_LIGHT));
		V24_PyConstant_Insert(d, "SCREEN", PyInt_FromLong(MTEX_SCREEN));
	}
	return BlendModes;
}

static PyObject *V24_M_Texture_MappingsDict( void )
{
	PyObject *Mappings = V24_PyConstant_New(  );
	if( Mappings ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Mappings;
		V24_PyConstant_Insert(d, "FLAT", PyInt_FromLong(MTEX_FLAT));
		V24_PyConstant_Insert(d, "CUBE", PyInt_FromLong(MTEX_CUBE));
		V24_PyConstant_Insert(d, "TUBE", PyInt_FromLong(MTEX_TUBE));
		V24_PyConstant_Insert(d, "SPHERE", PyInt_FromLong(MTEX_SPHERE));
	}
	return Mappings;
}

static PyObject *V24_M_Texture_ProjDict( void )
{
	PyObject *Proj = V24_PyConstant_New(  );
	if( Proj ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Proj;
		V24_PyConstant_Insert(d, "NONE", PyInt_FromLong(PROJ_N));
		V24_PyConstant_Insert(d, "X", PyInt_FromLong(PROJ_X));
		V24_PyConstant_Insert(d, "Y", PyInt_FromLong(PROJ_Y));
		V24_PyConstant_Insert(d, "Z", PyInt_FromLong(PROJ_Z));
	}
	return Proj;
}

PyObject *V24_Texture_Init( void )
{
	PyObject *V24_submodule;
	PyObject *dict;

	/* constants */
	PyObject *Types = V24_M_Texture_TypesDict(  );
	PyObject *STypes = V24_M_Texture_STypesDict(  );
	PyObject *TexCo = V24_M_Texture_TexCoDict(  );
	PyObject *MapTo = V24_M_Texture_MapToDict(  );
	PyObject *Flags = V24_M_Texture_FlagsDict(  );
	PyObject *ExtendModes = V24_M_Texture_ExtendModesDict(  );
	PyObject *ImageFlags = V24_M_Texture_ImageFlagsDict(  );
	PyObject *Noise = V24_M_Texture_NoiseDict(  );
	PyObject *BlendModes = V24_M_Texture_BlendModesDict(  );
	PyObject *Mappings = V24_M_Texture_MappingsDict(  );
	PyObject *Proj = V24_M_Texture_ProjDict(  );

	if( PyType_Ready( &V24_Texture_Type ) < 0)
		return NULL;

	V24_submodule = Py_InitModule3( "Blender.Texture",
				    V24_M_Texture_methods, V24_M_Texture_doc );

	if( Types )
		PyModule_AddObject( V24_submodule, "Types", Types );
	if( STypes )
		PyModule_AddObject( V24_submodule, "STypes", STypes );
	if( TexCo )
		PyModule_AddObject( V24_submodule, "TexCo", TexCo );
	if( MapTo )
		PyModule_AddObject( V24_submodule, "MapTo", MapTo );
	if( Flags )
		PyModule_AddObject( V24_submodule, "Flags", Flags );
	if( ExtendModes )
		PyModule_AddObject( V24_submodule, "ExtendModes", ExtendModes );
	if( ImageFlags )
		PyModule_AddObject( V24_submodule, "ImageFlags", ImageFlags );
	if( Noise )
		PyModule_AddObject( V24_submodule, "Noise", Noise );
	if ( BlendModes )
		PyModule_AddObject( V24_submodule, "BlendModes", BlendModes );
	if ( Mappings )
		PyModule_AddObject( V24_submodule, "Mappings", Mappings );
	if ( Proj )
		PyModule_AddObject( V24_submodule, "Proj", Proj );

	/* Add the MTex V24_submodule to this module */
	dict = PyModule_GetDict( V24_submodule );
	PyDict_SetItemString( dict, "MTex", V24_MTex_Init(  ) );

	return V24_submodule;
}

PyObject *V24_Texture_CreatePyObject( Tex * tex )
{
	V24_BPy_Texture *pytex;

	pytex = ( V24_BPy_Texture * ) PyObject_NEW( V24_BPy_Texture, &V24_Texture_Type );
	if( !pytex )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create V24_BPy_Texture PyObject" );

	pytex->texture = tex;
	return ( PyObject * ) pytex;
}

Tex *V24_Texture_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Texture * ) pyobj )->texture;
}

/*****************************************************************************/
/* Python V24_BPy_Texture methods:                                               */
/*****************************************************************************/

static PyObject *V24_Texture_getExtend( V24_BPy_Texture * self )
{
	const char *extend = NULL;

	if( V24_EXPP_map_getStrVal
	    ( V24_tex_extend_map, self->texture->extend, &extend ) )
		return PyString_FromString( extend );

	return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "invalid internal extend mode" );
}

static PyObject *V24_Texture_getImage( V24_BPy_Texture * self )
{
	/* we need this to be an IMAGE texture, and we must have an image */
	if( ( self->texture->type == TEX_IMAGE ||
				self->texture->type == TEX_ENVMAP )
			&& self->texture->ima )
		return V24_Image_CreatePyObject( self->texture->ima );

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_oldgetSType( V24_BPy_Texture * self )
{
	const char *stype = NULL;
	int n_stype;

	if( self->texture->type == TEX_VORONOI )
		n_stype = self->texture->vn_coltype;
#if 0
	else if( self->texture->type == TEX_MUSGRAVE )
		n_stype = self->texture->noisebasis;
#endif
	else if( self->texture->type == TEX_ENVMAP )
		n_stype = self->texture->env->stype;
	else 
		n_stype = self->texture->stype;

	if( V24_EXPP_map_getStrVal( V24_tex_stype_map[self->texture->type],
				n_stype, &stype ) )
		return PyString_FromString( stype );

	
	return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "invalid texture stype internally" );
}

static PyObject *V24_Texture_oldgetType( V24_BPy_Texture * self )
{
	const char *type = NULL;

	if( V24_EXPP_map_getStrVal( V24_tex_type_map, self->texture->type, &type ) )
		return PyString_FromString( type );
	
	return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "invalid texture type internally" );
}

static int V24_Texture_setAnimFrames( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->texture->iuser.frames,
								EXPP_TEX_ANIMFRAME_MIN,
								EXPP_TEX_ANIMFRAME_MAX, 'h' );
}

static int V24_Texture_setIUserCyclic( V24_BPy_Texture * self, PyObject * value )
{
	int param = PyObject_IsTrue( value );
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if( param )
		self->texture->iuser.cycl = 1;
	else
		self->texture->iuser.cycl = 0;
	return 0;
}

#if 0
/* this was stupid to begin with! (ton) */
static int V24_Texture_setAnimLength( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->texture->len,
								EXPP_TEX_ANIMLEN_MIN,
								EXPP_TEX_ANIMLEN_MAX, 'h' );
}

/* this is too simple to keep supporting? disabled for time being (ton) */
static int V24_Texture_setAnimMontage( V24_BPy_Texture * self, PyObject * value )
{
	int fradur[4][2];
	int i;

	if( !PyArg_ParseTuple( value, "(ii)(ii)(ii)(ii)",
			       &fradur[0][0], &fradur[0][1],
			       &fradur[1][0], &fradur[1][1],
			       &fradur[2][0], &fradur[2][1],
			       &fradur[3][0], &fradur[3][1] ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected a tuple of tuples" );

	for( i = 0; i < 4; ++i ) {
		self->texture->fradur[i][0] = 
			(short)V24_EXPP_ClampInt ( fradur[i][0], EXPP_TEX_ANIMMONSTART_MIN,
								EXPP_TEX_ANIMMONSTART_MAX );
		self->texture->fradur[i][1] = 
			(short)V24_EXPP_ClampInt ( fradur[i][1], EXPP_TEX_ANIMMONDUR_MIN,
								EXPP_TEX_ANIMMONDUR_MAX );
	}

	return 0;
}
#endif

static int V24_Texture_setAnimOffset( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->texture->iuser.offset,
								EXPP_TEX_ANIMOFFSET_MIN,
								EXPP_TEX_ANIMOFFSET_MAX, 'h' );
}

static int V24_Texture_setAnimStart( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->texture->iuser.sfra,
								EXPP_TEX_ANIMSTART_MIN,
								EXPP_TEX_ANIMSTART_MAX, 'h' );
}

static int V24_Texture_setBrightness( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->bright,
								EXPP_TEX_BRIGHTNESS_MIN,
								EXPP_TEX_BRIGHTNESS_MAX );
}

static int V24_Texture_setContrast( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->contrast,
								EXPP_TEX_CONTRAST_MIN,
								EXPP_TEX_CONTRAST_MAX );
}

static int V24_Texture_setCrop( V24_BPy_Texture * self, PyObject * value )
{
	float crop[4];

	if( !PyArg_ParseTuple( value, "ffff",
			       &crop[0], &crop[1], &crop[2], &crop[3] ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected tuple of 4 floats" );

	self->texture->cropxmin = V24_EXPP_ClampFloat( crop[0], EXPP_TEX_CROP_MIN,
												EXPP_TEX_CROP_MAX );
	self->texture->cropymin = V24_EXPP_ClampFloat( crop[1], EXPP_TEX_CROP_MIN,
												EXPP_TEX_CROP_MAX );
	self->texture->cropxmax = V24_EXPP_ClampFloat( crop[2], EXPP_TEX_CROP_MIN,
												EXPP_TEX_CROP_MAX );
	self->texture->cropymax = V24_EXPP_ClampFloat( crop[3], EXPP_TEX_CROP_MIN,
												EXPP_TEX_CROP_MAX );

	return 0;
}

static int V24_Texture_setIntExtend( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setIValueRange ( value, &self->texture->extend,
								EXPP_TEX_EXTEND_MIN,
								EXPP_TEX_EXTEND_MAX, 'h' );
}

static int V24_Texture_setFieldsPerImage( V24_BPy_Texture * self,
					    PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->texture->iuser.fie_ima,
								EXPP_TEX_FIEIMA_MIN,
								EXPP_TEX_FIEIMA_MAX, 'h' );

}

static int V24_Texture_setFilterSize( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->filtersize,
								EXPP_TEX_FILTERSIZE_MIN,
								EXPP_TEX_FILTERSIZE_MAX );
}

static int V24_Texture_setFlags( V24_BPy_Texture * self, PyObject * value )
{
	int param;

	if( !PyInt_Check( value ) ) {
		char errstr[128];
		sprintf ( errstr , "expected int bitmask of 0x%08x", TEX_FLAG_MASK );
		return V24_EXPP_ReturnIntError( PyExc_TypeError, errstr );
	}
	param = PyInt_AS_LONG ( value );

	if ( ( param & TEX_FLAG_MASK ) != param )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
						"invalid bit(s) set in mask" );

	self->texture->flag = (short)param;

#if 0
	/* if Colorband enabled, make sure we allocate memory for it */

	if ( ( param & TEX_COLORBAND ) && !self->texture->coba )
		self->texture->coba = add_colorband();
#endif

	return 0;
}

static int V24_Texture_setImage( V24_BPy_Texture * self, PyObject * value )
{
	Image *blimg = NULL;

	if( !BPy_Image_Check (value) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected an Image" );
	blimg = V24_Image_FromPyObject( value );

	if( self->texture->ima ) {
		self->texture->ima->id.us--;
	}

	self->texture->ima = blimg;
	id_us_plus( &blimg->id );

	return 0;
}

static int V24_Texture_setImageFlags( V24_BPy_Texture * self, PyObject * value,
									void *type )
{
	short param;

	/*
	 * if type is non-zero, then attribute is "mipmap", "calcAlpha", etc.,
	 * so set/clear the bit in the bitfield based on the type
	 */

	if( (int)type ) {
		int err;
		param = self->texture->imaflag;
		err = V24_EXPP_setBitfield( value, &param, (int)type, 'h' );
		if( err )
			return err;

	/*
	 * if type is zero, then attribute is "imageFlags", so check
	 * value for a valid bitmap range.
	 */

	} else {
		int bitmask = TEX_INTERPOL
					| TEX_USEALPHA
					| TEX_MIPMAP
					| TEX_IMAROT
					| TEX_CALCALPHA
					| TEX_NORMALMAP;

		if( !PyInt_Check( value ) ) {
			char errstr[128];
			sprintf ( errstr , "expected int bitmask of 0x%08x", bitmask );
			return V24_EXPP_ReturnIntError( PyExc_TypeError, errstr );
		}

		param = (short)PyInt_AS_LONG( value );
		if( ( param & bitmask ) != param )
			return V24_EXPP_ReturnIntError( PyExc_ValueError,
							"invalid bit(s) set in mask" );
	}

	/* everything is OK; save the new flag setting */

	self->texture->imaflag = param;
	return 0;
}

static int V24_Texture_setIUserFlags( V24_BPy_Texture * self, PyObject * value,
									void *flag )
{
	int param = PyObject_IsTrue( value );
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if( param )
		self->texture->iuser.flag |= (int)flag;
	else
		self->texture->iuser.flag &= ~(int)flag;
	return 0;
}

static int V24_Texture_setNoiseDepth( V24_BPy_Texture * self, PyObject * value )
{
	short max = EXPP_TEX_NOISEDEPTH_MAX;

	/* for whatever reason, magic texture has a different max value */

	if( self->texture->type == TEX_MAGIC )
		max = EXPP_TEX_NOISEDEPTH_MAX_MAGIC;

	return V24_EXPP_setIValueClamped ( value, &self->texture->noisedepth,
								EXPP_TEX_NOISEDEPTH_MIN, max, 'h' );
}

static int V24_Texture_setNoiseSize( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->noisesize,
								EXPP_TEX_NOISESIZE_MIN,
								EXPP_TEX_NOISESIZE_MAX );
}

static int V24_Texture_setNoiseType( V24_BPy_Texture * self, PyObject * value )
{
	char *param;

	if( !PyString_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );
	param = PyString_AS_STRING( value );

	if( STREQ( param, "soft" ) )
		self->texture->noisetype = TEX_NOISESOFT;
	else if( STREQ( param, "hard" ) )
		self->texture->noisetype = TEX_NOISEPERL;
	else
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
					      "noise type must be 'soft' or 'hard'" );

	return 0;
}

static int V24_Texture_setNoiseBasis( V24_BPy_Texture * self, PyObject * value )
{
    int param;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, 
				"expected int (see 'Noise' constant dictionary)" );

	param = PyInt_AS_LONG ( value );

	if ( param < TEX_BLENDER
			|| ( param > TEX_VORONOI_CRACKLE
			&& param != TEX_CELLNOISE ) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
					      "invalid noise type" );

	self->texture->noisebasis = (short)param;
	return 0;
}

static int V24_Texture_setNoiseBasis2( V24_BPy_Texture * self, PyObject * value,
								void *type )
{
	/*
	 * if type is EXPP_TEX_NOISEBASIS2, then this is the "noiseBasis2"
	 * attribute, so check the range and set the whole value
	 */

	if( (int)type == EXPP_TEX_NOISEBASIS2 ) {
    	int param;
		if( !PyInt_Check( value ) )
			return V24_EXPP_ReturnIntError( PyExc_TypeError, 
					"expected int (see 'Noise' constant dictionary)" );

		param = PyInt_AS_LONG ( value );

		if ( param < TEX_BLENDER
				|| ( param > TEX_VORONOI_CRACKLE
				&& param != TEX_CELLNOISE ) )
			return V24_EXPP_ReturnIntError( PyExc_ValueError,
							  "invalid noise type" );

		self->texture->noisebasis2 = (short)param;

	/*
	 * for other type values, the attribute is "sine", "saw" or "tri", 
	 * so set the noise basis to the supplied type if value is 1
	 */

	} else {
		if( !PyInt_Check( value ) )
			return V24_EXPP_ReturnIntError( PyExc_TypeError, 
					"expected int value of 1" );

		if( PyInt_AS_LONG ( value ) != 1 )
			return V24_EXPP_ReturnIntError( PyExc_ValueError,
							  "expected int value of 1" );

		self->texture->noisebasis2 = (short)(int)type;
	}
	return 0;
}

static int V24_Texture_setRepeat( V24_BPy_Texture * self, PyObject * args )
{
	int repeat[2];

	if( !PyArg_ParseTuple( args, "ii", &repeat[0], &repeat[1] ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected tuple of 2 ints" );

	self->texture->xrepeat = (short)V24_EXPP_ClampInt( repeat[0], EXPP_TEX_REPEAT_MIN,
											EXPP_TEX_REPEAT_MAX );
	self->texture->yrepeat = (short)V24_EXPP_ClampInt( repeat[1], EXPP_TEX_REPEAT_MIN,
											EXPP_TEX_REPEAT_MAX );

	return 0;
}

static int V24_Texture_setRGBCol( V24_BPy_Texture * self, PyObject * args )
{
	float rgb[3];

	if( !PyArg_ParseTuple( args, "fff", &rgb[0], &rgb[1], &rgb[2] ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected tuple of 3 floats" );

	self->texture->rfac = V24_EXPP_ClampFloat( rgb[0], EXPP_TEX_RGBCOL_MIN,
											EXPP_TEX_RGBCOL_MAX );
	self->texture->gfac = V24_EXPP_ClampFloat( rgb[1], EXPP_TEX_RGBCOL_MIN,
											EXPP_TEX_RGBCOL_MAX );
	self->texture->bfac = V24_EXPP_ClampFloat( rgb[2], EXPP_TEX_RGBCOL_MIN,
											EXPP_TEX_RGBCOL_MAX );

	return 0;
}

static int V24_Texture_setSType( V24_BPy_Texture * self, PyObject * value )
{
	short param;
	const char *dummy = NULL;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected int argument" );

	param = (short)PyInt_AS_LONG ( value );

	/* use the stype map to find out if this is a valid stype for this type *
	 * note that this will allow CLD_COLOR when type is ENVMAP. there's not *
	 * much that we can do about this though.                               */
	if( !V24_EXPP_map_getStrVal
	    ( V24_tex_stype_map[self->texture->type], param, &dummy ) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
					      "invalid stype (for this type)" );

	if( self->texture->type == TEX_VORONOI )
		self->texture->vn_coltype = param;
#if 0
	else if( self->texture->type == TEX_MUSGRAVE )
		self->texture->noisebasis = param;
#endif
	else if( self->texture->type == TEX_ENVMAP )
		self->texture->env->stype = param;
	else 
		self->texture->stype = param;

	return 0;
}

static int V24_Texture_setTurbulence( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->turbul,
								EXPP_TEX_TURBULENCE_MIN,
								EXPP_TEX_TURBULENCE_MAX );
}

static int V24_Texture_setHFracDim( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->mg_H,
								EXPP_TEX_MH_G_MIN,
								EXPP_TEX_MH_G_MAX );
}

static int V24_Texture_setLacunarity( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->mg_lacunarity,
								EXPP_TEX_LACUNARITY_MIN,
								EXPP_TEX_LACUNARITY_MAX );
}

static int V24_Texture_setOcts( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->mg_octaves,
								EXPP_TEX_OCTS_MIN,
								EXPP_TEX_OCTS_MAX );
}

static int V24_Texture_setIScale( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->ns_outscale,
								EXPP_TEX_ISCALE_MIN,
								EXPP_TEX_ISCALE_MAX );
}

static int V24_Texture_setType( V24_BPy_Texture * self, PyObject * value )
{
	int err = V24_EXPP_setIValueRange ( value, &self->texture->type,
								EXPP_TEX_TYPE_MIN,
								EXPP_TEX_TYPE_MAX, 'h' );

	/*
	 * if we set the texture OK, and it's a environment map, and
	 * there is no environment map yet, allocate one (code borrowed
	 * from texture_panel_envmap() in source/blender/src/buttons_shading.c)
	 */

	if( !err && self->texture->type == TEX_ENVMAP 
			&& !self->texture->env ) {
		self->texture->env = BKE_add_envmap();
		self->texture->env->object= OBACT;
	}
	return err;
}

static int V24_Texture_setDistMetric( V24_BPy_Texture * self, PyObject * value )
{
#if 0
	char *dist = NULL;

	if( !PyArg_ParseTuple( value, "s", &dist ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );

	/* can we really trust texture->type? */
	if( self->texture->type == TEX_VORONOI &&
	    !V24_EXPP_map_getShortVal( V24_tex_stype_map[self->texture->type + 2],
				   dist, &self->texture->vn_distm ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid dist metric type" );

	Py_RETURN_NONE;
#else
	return V24_EXPP_setIValueRange ( value, &self->texture->vn_distm,
							TEX_DISTANCE,
							TEX_MINKOVSKY, 'h' );
#endif
}

static int V24_Texture_setExp( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->vn_mexp,
								EXPP_TEX_EXP_MIN,
								EXPP_TEX_EXP_MAX );
}

static int V24_Texture_setWeight1( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->vn_w1,
								EXPP_TEX_WEIGHT1_MIN,
								EXPP_TEX_WEIGHT1_MAX );
}

static int V24_Texture_setWeight2( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->vn_w2,
								EXPP_TEX_WEIGHT2_MIN,
								EXPP_TEX_WEIGHT2_MAX );
}

static int V24_Texture_setWeight3( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->vn_w3,
								EXPP_TEX_WEIGHT3_MIN,
								EXPP_TEX_WEIGHT3_MAX );
}

static int V24_Texture_setWeight4( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->vn_w4,
								EXPP_TEX_WEIGHT4_MIN,
								EXPP_TEX_WEIGHT4_MAX );
}

static int V24_Texture_setDistAmnt( V24_BPy_Texture * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->texture->dist_amount,
								EXPP_TEX_DISTAMNT_MIN,
								EXPP_TEX_DISTAMNT_MAX );
}

static PyObject *V24_Texture_getIpo( V24_BPy_Texture * self )
{
	struct Ipo *ipo = self->texture->ipo;

	if( !ipo )
		Py_RETURN_NONE;

	return V24_Ipo_CreatePyObject( ipo );
}

/*
 * this should accept a Py_None argument and just delete the Ipo link
 * (as V24_Texture_clearIpo() does)
 */

static int V24_Texture_setIpo( V24_BPy_Texture * self, PyObject * value )
{
	Ipo *ipo = NULL;
	Ipo *oldipo = self->texture->ipo;
	ID *id;

	/* if parameter is not None, check for valid Ipo */

	if ( value != Py_None ) {
		if ( !BPy_Ipo_Check( value ) )
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
					      	"expected an Ipo object" );

		ipo = V24_Ipo_FromPyObject( value );

		if( !ipo )
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
					      	"null ipo!" );

		if( ipo->blocktype != ID_TE )
			return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      	"Ipo is not a texture data Ipo" );
	}

	/* if already linked to Ipo, delete link */

	if ( oldipo ) {
		id = &oldipo->id;
		if( id->us > 0 )
			id->us--;
	}

	/* assign new Ipo and increment user count, or set to NULL if deleting */

	self->texture->ipo = ipo;
	if ( ipo ) {
		id = &ipo->id;
		id_us_plus(id);
	}

	return 0;
}

static PyObject *V24_Texture_getAnimFrames( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->iuser.frames );
}

static PyObject *V24_Texture_getIUserCyclic( V24_BPy_Texture *self )
{
	if( self->texture->iuser.cycl )
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

#if 0
/* disabled. this option was too stupid! (ton) */
static PyObject *V24_Texture_getAnimLength( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->len );
}

static PyObject *V24_Texture_getAnimMontage( V24_BPy_Texture *self )
{	
	return Py_BuildValue( "((i,i),(i,i),(i,i),(i,i))",
						self->texture->fradur[0][0],
						self->texture->fradur[0][1],
						self->texture->fradur[1][0],
						self->texture->fradur[1][1],
						self->texture->fradur[2][0],
						self->texture->fradur[2][1],
						self->texture->fradur[3][0],
						self->texture->fradur[3][1] );
}
#endif

static PyObject *V24_Texture_getAnimOffset( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->iuser.offset );
}

static PyObject *V24_Texture_getAnimStart( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->iuser.sfra );
}

static PyObject *V24_Texture_getBrightness( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble ( self->texture->bright );
}

static PyObject *V24_Texture_getContrast( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->contrast );
}

static PyObject *V24_Texture_getCrop( V24_BPy_Texture *self )
{
	return Py_BuildValue( "(f,f,f,f)",
							self->texture->cropxmin,
							self->texture->cropymin,
							self->texture->cropxmax,
							self->texture->cropymax );
}

static PyObject *V24_Texture_getDistAmnt( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->dist_amount );
}

static PyObject *V24_Texture_getDistMetric( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->vn_distm );
}

static PyObject *V24_Texture_getExp( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->vn_mexp );
}

static PyObject *V24_Texture_getIntExtend( V24_BPy_Texture * self )
{
	return PyInt_FromLong( self->texture->extend );
}

static PyObject *V24_Texture_getFieldsPerImage( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->iuser.fie_ima );
}

static PyObject *V24_Texture_getFilterSize( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->filtersize );
}

static PyObject *V24_Texture_getFlags( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->flag );
}

static PyObject *V24_Texture_getHFracDim( V24_BPy_Texture *self )
{
	return PyInt_FromLong( (long)self->texture->mg_H );
}

static PyObject *V24_Texture_getImageFlags( V24_BPy_Texture *self, void *type )
{
	/* type == 0 means attribute "imageFlags"
	 * other types means attribute "mipmap", "calcAlpha", etc
	 */

	if( (int)type )
		return V24_EXPP_getBitfield( &self->texture->imaflag, (int)type, 'h' );
	else
		return PyInt_FromLong( self->texture->imaflag );
}

static PyObject *V24_Texture_getIUserFlags( V24_BPy_Texture *self, void *flag )
{
	if( self->texture->iuser.flag & (int)flag )
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject *V24_Texture_getIScale( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->ns_outscale );
}

static PyObject *V24_Texture_getLacunarity( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->mg_lacunarity );
}

static PyObject *V24_Texture_getNoiseBasis( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->noisebasis );
}

static PyObject *V24_Texture_getNoiseBasis2( V24_BPy_Texture *self, void *type )
{
	/* type == EXPP_TEX_NOISEBASIS2 means attribute "noiseBasis2"
	 * other types means attribute "sine", "saw", or "tri" attribute
	 */

	if( (int)type == EXPP_TEX_NOISEBASIS2 )
		return PyInt_FromLong( self->texture->noisebasis2 );
	else
		return PyInt_FromLong( ( self->texture->noisebasis2 == (int)type ) ? 1 : 0 );
}

static PyObject *V24_Texture_getNoiseDepth( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->noisedepth );
}

static PyObject *V24_Texture_getNoiseSize( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->noisesize );
}

static PyObject *V24_Texture_getNoiseType( V24_BPy_Texture *self )
{
	if ( self->texture->noisetype == TEX_NOISESOFT )
		return PyString_FromString( "soft" );
	else
		return PyString_FromString( "hard" );
}

static PyObject *V24_Texture_getOcts( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->mg_octaves );
}

static PyObject *V24_Texture_getRepeat( V24_BPy_Texture *self )
{
	return Py_BuildValue( "(i,i)", self->texture->xrepeat,
									self->texture->yrepeat );
}

static PyObject *V24_Texture_getRGBCol( V24_BPy_Texture *self )
{
	return Py_BuildValue( "(f,f,f)", self->texture->rfac,
									self->texture->gfac, self->texture->bfac );
}

static PyObject *V24_Texture_getSType( V24_BPy_Texture *self )
{
	if( self->texture->type == TEX_VORONOI )
		return PyInt_FromLong( self->texture->vn_coltype );
#if 0
	if( self->texture->type == TEX_MUSGRAVE )
		return PyInt_FromLong( self->texture->noisebasis );
#endif
	if( self->texture->type == TEX_ENVMAP )
		return PyInt_FromLong( self->texture->env->stype );

	return PyInt_FromLong( self->texture->stype );
}

static PyObject *V24_Texture_getTurbulence( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->turbul );
}

static PyObject *V24_Texture_getType( V24_BPy_Texture *self )
{
	return PyInt_FromLong( self->texture->type );
}

static PyObject *V24_Texture_getWeight1( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->vn_w1 );
}

static PyObject *V24_Texture_getWeight2( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->vn_w2 );
}

static PyObject *V24_Texture_getWeight3( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->vn_w3 );
}

static PyObject *V24_Texture_getWeight4( V24_BPy_Texture *self )
{
	return PyFloat_FromDouble( self->texture->vn_w4 );
}

/* #####DEPRECATED###### */

static PyObject *V24_Texture_oldsetImage( V24_BPy_Texture * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
										(setter)V24_Texture_setImage );
}

static PyObject *V24_Texture_oldsetIpo( V24_BPy_Texture * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Texture_setIpo );
}

/*
 * clearIpo() returns True/False depending on whether material has an Ipo
 */

static PyObject *V24_Texture_clearIpo( V24_BPy_Texture * self )
{
	/* if Ipo defined, delete it and return true */

	if( self->texture->ipo ) {
		PyObject *value = Py_BuildValue( "(O)", Py_None );
		V24_EXPP_setterWrapper( (void *)self, value, (setter)V24_Texture_setIpo );
		Py_DECREF( value );
		return V24_EXPP_incr_ret_True();
	}
	return V24_EXPP_incr_ret_False(); /* no ipo found */
}

/*
 * these older setter methods take strings as parameters; check the list of
 * strings to figure out which bits to set, then call new attribute setters
 * using the wrapper.
 */

static PyObject *V24_Texture_oldsetFlags( V24_BPy_Texture * self, PyObject * args )
{
	unsigned int i, flag = 0;
	PyObject *value, *error;

	/* check that we're passed a tuple */

	if ( !PyTuple_Check( args ) )
		return V24_EXPP_ReturnPyObjError ( PyExc_AttributeError,
					"expected a tuple of string arguments" );

	/* check each argument for type, find its value */

	for ( i = PyTuple_Size( args ); i-- ; ) {
		short thisflag;
		char * name = PyString_AsString( PyTuple_GET_ITEM( args, i ) );
		if( !name )
			return V24_EXPP_ReturnPyObjError ( PyExc_AttributeError,
					"expected string argument" );

		if( !V24_EXPP_map_getShortVal( V24_tex_flag_map, name, &thisflag ) )
			return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
							"unknown Texture flag argument" ) );

		flag |= thisflag;
	}

	/* build tuple, call wrapper */

	value = Py_BuildValue( "(i)", flag );
	error = V24_EXPP_setterWrapper( (void *)self, value, (setter)V24_Texture_setFlags );
	Py_DECREF ( value );
	return error;
}

/*
 * V24_Texture_oldsetType() and V24_Texture_oldsetExtend()
 *
 * These older setter methods convert a string into an integer setting, so
 * doesn't make sense to try wrapping them.
 */

static PyObject *V24_Texture_oldsetType( V24_BPy_Texture * self, PyObject * args )
{
	char *type = NULL;

	if( !PyArg_ParseTuple( args, "s", &type ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );

	if( !V24_EXPP_map_getShortVal( V24_tex_type_map, type, &self->texture->type ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid texture type" );

	/*
	 * if we set the texture OK, and it's a environment map, and
	 * there is no environment map yet, allocate one (code borrowed
	 * from texture_panel_envmap() in source/blender/src/buttons_shading.c)
	 */

	if( self->texture->type == TEX_ENVMAP 
			&& !self->texture->env ) {
		self->texture->env = BKE_add_envmap();
		self->texture->env->object= OBACT;
	}

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_oldsetExtend( V24_BPy_Texture * self, PyObject * args )
{
	char *extend = NULL;
	if( !PyArg_ParseTuple( args, "s", &extend ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );

	if( !V24_EXPP_map_getShortVal
	    ( V24_tex_extend_map, extend, &self->texture->extend ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid extend mode" );

	Py_RETURN_NONE;
}

/*
 * V24_Texture_oldsetNoiseBasis(), V24_Texture_oldsetDistNoise()
 *   V24_Texture_oldsetSType(), V24_Texture_oldsetDistMetric(),
 *   V24_Texture_oldsetImageFlags()
 *
 * these old setter methods behave differently from the attribute
 * setters, so they are V24_left unchanged.
 */

static PyObject *V24_Texture_oldsetNoiseBasis( V24_BPy_Texture * self, PyObject * args )
{
/* NOTE: leave as-is: don't use setterWrapper */
	char *nbasis;

	if( !PyArg_ParseTuple( args, "s", &nbasis ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						  "expected string argument" );
	if( self->texture->type == TEX_MUSGRAVE &&
	    V24_EXPP_map_getShortVal( V24_tex_stype_map[TEX_DISTNOISE],
				  nbasis, &self->texture->noisebasis ) );
	else if( self->texture->type == TEX_DISTNOISE &&
		 !V24_EXPP_map_getShortVal( V24_tex_stype_map[TEX_DISTNOISE],
					nbasis, &self->texture->noisebasis2 ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid noise basis" );

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_oldsetDistNoise( V24_BPy_Texture * self, PyObject * args )
{
/* NOTE: leave as-is: don't use setterWrapper */
	char *nbasis;

	if( !PyArg_ParseTuple( args, "s", &nbasis ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );
	if( self->texture->type == TEX_DISTNOISE &&
	    !V24_EXPP_map_getShortVal( V24_tex_stype_map[TEX_DISTNOISE],
				   nbasis, &self->texture->noisebasis ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid noise basis" );

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_oldsetSType( V24_BPy_Texture * self, PyObject * args )
{
	char *stype = NULL;
	if( !PyArg_ParseTuple( args, "s", &stype ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );

	/* can we really trust texture->type? */
	if( ( self->texture->type == TEX_VORONOI &&
	      V24_EXPP_map_getShortVal( V24_tex_stype_map[self->texture->type],
				    stype, &self->texture->vn_coltype ) ) );
#if 0
	else if( ( self->texture->type == TEX_MUSGRAVE &&
		   V24_EXPP_map_getShortVal( V24_tex_stype_map
					 [TEX_DISTNOISE], stype,
					 &self->texture->noisebasis ) ) );
#endif
	else if( ( self->texture->type == TEX_ENVMAP &&
	      V24_EXPP_map_getShortVal( V24_tex_stype_map[self->texture->type],
				    stype, &self->texture->env->stype ) ) );
	else if( !V24_EXPP_map_getShortVal
		 ( V24_tex_stype_map[self->texture->type], stype,
		   &self->texture->stype ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid texture stype" );

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_oldsetDistMetric( V24_BPy_Texture * self, PyObject * args )
{
/* NOTE: leave as-is: don't use setterWrapper */
	char *dist = NULL;

	if( !PyArg_ParseTuple( args, "s", &dist ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );
	/* can we really trust texture->type? */
	if( self->texture->type == TEX_VORONOI &&
	    !V24_EXPP_map_getShortVal( V24_tex_stype_map[self->texture->type + 2],
				   dist, &self->texture->vn_distm ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					      "invalid dist metric type" );

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_oldsetImageFlags( V24_BPy_Texture * self, PyObject * args )
{
	unsigned int i, flag = 0;

	/* check that we're passed a tuple of no more than 3 args*/

	if( !PyTuple_Check( args ) )
		return V24_EXPP_ReturnPyObjError ( PyExc_AttributeError,
					"expected tuple of string arguments" );

	/* check each argument for type, find its value */

	for( i = PyTuple_Size( args ); i-- ; ) {
		short thisflag;
		char * name = PyString_AsString( PyTuple_GET_ITEM( args, i ) );
		if( !name )
			return V24_EXPP_ReturnPyObjError ( PyExc_AttributeError,
					"expected string argument" );

		if( !V24_EXPP_map_getShortVal( V24_tex_imageflag_map, name, &thisflag ) )
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
						      "unknown Texture image flag name" );

		flag |= thisflag;
	}

	self->texture->imaflag = (short)flag;

	Py_RETURN_NONE;
}

static PyObject *V24_Texture_getColorband( V24_BPy_Texture * self)
{
	return V24_EXPP_PyList_fromColorband( self->texture->coba );
}

int V24_Texture_setColorband( V24_BPy_Texture * self, PyObject * value)
{
	return V24_EXPP_Colorband_fromPyList( &self->texture->coba, value );
}

static PyObject *V24_Texture_evaluate( V24_BPy_Texture * self, PyObject * value )
{
	TexResult texres= {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, NULL};
	float vec[4];
	/* int rgbnor; dont use now */
	
	if (VectorObject_Check(value)) {
		if(((V24_VectorObject *)value)->size < 3)
			return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
					"expects a 3D vector object or a tuple of 3 numbers");
		
		/* rgbnor = .. we don't need this now */
		multitex_ext(self->texture, ((V24_VectorObject *)value)->vec, NULL, NULL, 1, &texres);
	} else {
		float vec_in[3];
		if (!PyTuple_Check(value) || PyTuple_Size(value) < 3)
			return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
					"expects a 3D vector object or a tuple of 3 numbers");
		
		vec_in[0] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, 0));
		vec_in[1] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, 1));
		vec_in[2] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, 2));
		if (PyErr_Occurred())
			return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
					"expects a 3D vector object or a tuple of 3 numbers");
		
		multitex_ext(self->texture, vec_in, NULL, NULL, 1, &texres);
	}
	vec[0] = texres.tr;
	vec[1] = texres.tg;
	vec[2] = texres.tb;
	vec[3] = texres.tin;
	
	return V24_newVectorObject(vec, 4, Py_NEW);
}

static PyObject *V24_Texture_copy( V24_BPy_Texture * self )
{
	Tex *tex = copy_texture(self->texture );
	tex->id.us = 0;
	return V24_Texture_CreatePyObject(tex);
}
