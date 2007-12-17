/*
 * $Id: Effect.c 11924 2007-09-02 21:03:59Z campbellbarton $
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
 * Contributor(s): Jacques Guignot, Jean-Michel Soler, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
#include "Effect.h" /*This must come first */

#include "DNA_object_types.h"
#include "DNA_scene_types.h" /* for G.scene->r.cfra */
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_effect.h"
#include "BKE_object.h"
#include "BKE_deform.h"
#include "BKE_scene.h"       /* for G.scene->r.cfra */
#include "BKE_ipo.h"         /* frame_to_float() */
#include "BLI_blenlib.h"
#include "gen_utils.h"
#include "blendef.h"
#include "vector.h"
#include "Const.h"
#include "Group.h"
#include "gen_library.h"
#include "MTC_matrixops.h"
 
#define EXPP_EFFECT_STA_MIN           -250.0f
#define EXPP_EFFECT_END_MIN              1.0f
#define EXPP_EFFECT_LIFETIME_MIN         1.0f
#define EXPP_EFFECT_NORMFAC_MIN         -2.0f
#define EXPP_EFFECT_NORMFAC_MAX          2.0f
#define EXPP_EFFECT_OBFAC_MIN           -1.0f
#define EXPP_EFFECT_OBFAC_MAX            1.0f
#define EXPP_EFFECT_RANDFAC_MIN          0.0f
#define EXPP_EFFECT_RANDFAC_MAX          2.0f
#define EXPP_EFFECT_TEXFAC_MIN           0.0f
#define EXPP_EFFECT_TEXFAC_MAX           2.0f
#define EXPP_EFFECT_RANDLIFE_MIN         0.0f
#define EXPP_EFFECT_RANDLIFE_MAX         2.0f
#define EXPP_EFFECT_NABLA_MIN            0.0001f
#define EXPP_EFFECT_NABLA_MAX            1.0f
#define EXPP_EFFECT_VECTSIZE_MIN         0.0f
#define EXPP_EFFECT_VECTSIZE_MAX         1.0f
#define EXPP_EFFECT_TOTPART_MIN          1.0f
#define EXPP_EFFECT_TOTPART_MAX     100000.0f
#define EXPP_EFFECT_FORCE_MIN           -1.0f
#define EXPP_EFFECT_FORCE_MAX            1.0f
#define EXPP_EFFECT_MULT_MIN             0.0f
#define EXPP_EFFECT_MULT_MAX             1.0f
#define EXPP_EFFECT_LIFE_MIN             1.0f
#define EXPP_EFFECT_DEFVEC_MIN          -1.0f
#define EXPP_EFFECT_DEFVEC_MAX           1.0f
#define EXPP_EFFECT_DAMP_MIN             0.0f
#define EXPP_EFFECT_DAMP_MAX             1.0f

enum eff_float_consts {
	EXPP_EFF_STA = 0,
	EXPP_EFF_END,
	EXPP_EFF_LIFETIME,
	EXPP_EFF_NORMFAC,
	EXPP_EFF_OBFAC,
	EXPP_EFF_RANDFAC,
	EXPP_EFF_TEXFAC,
	EXPP_EFF_RANDLIFE,
	EXPP_EFF_DAMP,
	EXPP_EFF_NABLA,
	EXPP_EFF_VECTSIZE,
	EXPP_EFF_MAXLEN,
	EXPP_EFF_MULT1,
	EXPP_EFF_MULT2,
	EXPP_EFF_MULT3,
	EXPP_EFF_MULT4,
	EXPP_EFF_LIFE1,
	EXPP_EFF_LIFE2,
	EXPP_EFF_LIFE3,
	EXPP_EFF_LIFE4,
};

/* int/short/char */
enum eff_int_consts {
	EXPP_EFF_USERJIT = 0,
	EXPP_EFF_TOTPART,
	EXPP_EFF_TOTKEY,
	EXPP_EFF_SEED,
	/*EXPP_EFF_CURMULT, - active child, not needed */
	EXPP_EFF_STATICSTEP,
	EXPP_EFF_OMAT,
	EXPP_EFF_TIMETEX,
	EXPP_EFF_SPEEDTEX,
	EXPP_EFF_DISP,
	EXPP_EFF_STYPE, /* act like a flag */
	EXPP_EFF_CHILD1,
	EXPP_EFF_CHILD2,
	EXPP_EFF_CHILD3,
	EXPP_EFF_CHILD4,
	EXPP_EFF_CHILDMAT1,
	EXPP_EFF_CHILDMAT2,
	EXPP_EFF_CHILDMAT3,
	EXPP_EFF_CHILDMAT4,
};

/*****************************************************************************/
/* Python BPyEffectObject methods declarations:                                 */
/*****************************************************************************/
static PyObject *getFloatAttr( BPyEffectObject *self, void *type );
static int setFloatAttrClamp( BPyEffectObject *self, PyObject *value, void *type );
static PyObject *getIntAttr( BPyEffectObject *self, void *type );
static int setIntAttrClamp( BPyEffectObject *self, PyObject *value, void *type );
static PyObject *Effect_getGroup( BPyEffectObject * self );
static int Effect_setGroup( BPyEffectObject * self, PyObject * value );
static PyObject *Effect_getFlag( BPyEffectObject * self, void *flag);
static int Effect_setFlag(BPyEffectObject *self, PyObject *value, void *flag);
static PyObject *Effect_getForce( BPyEffectObject * self );
static int Effect_setForce( BPyEffectObject * self, PyObject * value );
static PyObject *Effect_getDefvec( BPyEffectObject * self );
static int Effect_setDefvec( BPyEffectObject * self, PyObject * value );
static PyObject *Effect_getSpeedType( BPyEffectObject * self );
static int Effect_setSpeedType( BPyEffectObject * self, PyObject * value );
static PyObject *Effect_getVertGroup( BPyEffectObject * self );
static int Effect_setVertGroup( BPyEffectObject * self, PyObject * value );
static PyObject *Effect_getSpeedVertGroup( BPyEffectObject * self );
static int Effect_setSpeedVertGroup( BPyEffectObject * self, PyObject * value );
static PyObject *Effect_getParticlesLoc( BPyEffectObject * self  );

/*****************************************************************************/
/* Python BPyEffect_Type callback function prototypes:                           */
/*****************************************************************************/
static PyObject *Effect_repr( void );

/*****************************************************************************/
/* Python BPyEffectObject attributes get/set structure:                           */
/*****************************************************************************/
static PyGetSetDef BPyEffect_getset[] = {
		
		
	/* Custom Values */
	{"particles", /* particle data as vectors */
	 (getter)Effect_getParticlesLoc, (setter)NULL,
	 "Get the particles as a list of vectors", NULL},
	{"group",
	 (getter)Effect_getGroup, (setter)Effect_setGroup,
	 "Only use forces within this group", NULL},
	{"speedType",
	 (getter)Effect_getSpeedType, (setter)Effect_setSpeedType,
	 "Controls which texture property affects particle speeds",
	 NULL},
	{"vertGroupSpeed",
	 (getter)Effect_getSpeedVertGroup, (setter)Effect_setSpeedVertGroup,
	 "Vertex group for speed control",
	 NULL},
	{"vertGroup",
	 (getter)Effect_getVertGroup, (setter)Effect_setVertGroup,
	 "Vertex group for emitted particles",
	 NULL},
	{"defvec",
	 (getter)Effect_getDefvec, (setter)Effect_setDefvec,
	 "The axes of a force, determined by the texture",
	 NULL},
	{"force",
	 (getter)Effect_getForce, (setter)Effect_setForce,
	 "The axes of a continues force",
	 NULL},
	 
	/* flags, dont use EFF_SELECT I dont think we need to */
	{"enableBSpline",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_BSPLINE},
	{"enableStatic",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_STATIC},
	{"enableFaces",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_FACE},
	{"enableAnimated",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_ANIMATED},
	{"enableUnborn",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_UNBORN},
	{"enableVerts",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_OFACE}, /* flip this value */
	{"enableEMesh",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_SHOWE},
	{"enableTrueRand",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_TRAND},
	{"enableEvenDist",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_EDISTR},
	{"enableDied",
	 (getter)Effect_getFlag, (setter)Effect_setFlag,
	 "", (void *)PAF_DIED},
	 
	 /* not a flag but acts like one */
	{"enableVec",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "", (void *)EXPP_EFF_STYPE},

	 /* int/short/char attrs */
	{"jitter",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Jitter table distribution: maximum particles per face",
	 (void *)EXPP_EFF_USERJIT},
	{"totpart",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The total number of particles",
	 (void *)EXPP_EFF_TOTPART},
	{"totkey",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The total number of key positions",
	 (void *)EXPP_EFF_TOTKEY},
	{"seed",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The seed for random variations",
	 (void *)EXPP_EFF_SEED},
	{"staticStep",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The particle static step value",
	 (void *)EXPP_EFF_STATICSTEP},
	{"materialIndex",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The material used for the particles",
	 (void *)EXPP_EFF_OMAT},
	{"texEmissionIndex",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The texture used for texture emission",
	 (void *)EXPP_EFF_TIMETEX},
	{"texForceIndex",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The texture used for texture emission",
	 (void *)EXPP_EFF_SPEEDTEX},
	{"display",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The particle display % value",
	 (void *)EXPP_EFF_DISP},
	{"child1",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The number of children of 1st generation particles that multiply itself",
	 (void *)EXPP_EFF_CHILD1},
	{"child2",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The number of children of 2nd generation particles that multiply itself",
	 (void *)EXPP_EFF_CHILD2},
	{"child3",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The number of children of 3rd generation particles that multiply itself",
	 (void *)EXPP_EFF_CHILD3},
	{"child4",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The number of children of 4th generation particles that multiply itself",
	 (void *)EXPP_EFF_CHILD4},
	{"childMat1",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Specify the material used for the 1st generation particles",
	 (void *)EXPP_EFF_CHILDMAT1},
	{"childMat2",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Specify the material used for the 2nd generation particles",
	 (void *)EXPP_EFF_CHILDMAT2},
	{"childMat3",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Specify the material used for the 3rd generation particles",
	 (void *)EXPP_EFF_CHILDMAT3},
	{"childMat4",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Specify the material used for the 4th generation particles",
	 (void *)EXPP_EFF_CHILDMAT4},
	 
	 /* float attrs */
	 {"startFrame",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The start frame for the effect",
	 (void *)EXPP_EFF_STA},
	 {"endFrame",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The end frame for the effect",
	 (void *)EXPP_EFF_END},
	 {"lifeTime",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The life span of the particles",
	 (void *)EXPP_EFF_LIFETIME},
	{"normalFactor",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Particle's starting speed (from the mesh normals)",
	 (void *)EXPP_EFF_NORMFAC},
	{"objectFactor",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Particle's starting speed (from the object)",
	 (void *)EXPP_EFF_OBFAC},
	{"randomFactor",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The random variation for the starting speed",
	 (void *)EXPP_EFF_RANDFAC},
	{"textureFactor",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The random variation for the starting speed",
	 (void *)EXPP_EFF_TEXFAC},
	{"randomLife",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The random variation for a particle's life",
	 (void *)EXPP_EFF_RANDLIFE},
	{"damping",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The damping factor",
	 (void *)EXPP_EFF_DAMP},
	{"nabla",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The dimension of the area for gradient calculation",
	 (void *)EXPP_EFF_NABLA},
	{"vectorSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The speed for particle's rotation direction",
	 (void *)EXPP_EFF_VECTSIZE},
	{"maxLength",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The maximum length of a particle strand (zero is no limit)",
	 (void *)EXPP_EFF_MAXLEN},
	 
	{"childSpawn1",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Probability 'dying' particle spawns a new one",
	 (void *)EXPP_EFF_MULT1},	 
	{"childSpawn2",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Probability 'dying' particle spawns a new one",
	 (void *)EXPP_EFF_MULT3},
	{"childSpawn3",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Probability 'dying' particle spawns a new one",
	 (void *)EXPP_EFF_MULT3},
	{"childSpawn4",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Probability 'dying' particle spawns a new one",
	 (void *)EXPP_EFF_MULT4},
	 
	{"childLife1",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "lifespan of the next generation particles",
	 (void *)EXPP_EFF_LIFE1},	 
	{"childLife2",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "lifespan of the next generation particles",
	 (void *)EXPP_EFF_LIFE3},
	{"childLife3",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "lifespan of the next generation particles",
	 (void *)EXPP_EFF_LIFE3},
	{"childLife4",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "lifespan of the next generation particles",
	 (void *)EXPP_EFF_LIFE4},
	 
	{NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python BPyEffect_Type structure definition:                                  */
/*****************************************************************************/
PyTypeObject BPyEffect_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Effect",           /* char *tp_name; */
	sizeof( BPyEffectObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) Effect_repr,   /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,                       /* hashfunc tp_hash; */
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
	NULL,						/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyEffect_getset,			/* struct PyGetSetDef *tp_getset; */
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

static constIdents speedTypesIdents[] = {
	{"INTENSITY",	{(int)PAF_TEXINT}},
	{"RGB",			{(int)PAF_TEXRGB}},
	{"GRADIENT",	{(int)PAF_TEXGRAD}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition speedTypes = {
	EXPP_CONST_INT, "SpeedTypes",
		sizeof(speedTypesIdents)/sizeof(constIdents), speedTypesIdents
};

/*****************************************************************************/
/* Python BPyEffectObject methods:                                       */
/*****************************************************************************/

static PyObject *getFloatAttr( BPyEffectObject *self, void *type )
{
	float param;
	PartEff *paf= (PartEff *)self->effect;
	
	switch( (int)type ) {
	case EXPP_EFF_STA:
		param = paf->sta;
		break;
	case EXPP_EFF_END:
		param = paf->end;
		break;
	case EXPP_EFF_LIFETIME:
		param = paf->lifetime;
		break;
	case EXPP_EFF_NORMFAC:
		param = paf->normfac;
		break;
	case EXPP_EFF_OBFAC:
		param = paf->obfac;
		break;
	case EXPP_EFF_RANDFAC:
		param = paf->randfac;
		break;
	case EXPP_EFF_TEXFAC:
		param = paf->texfac;
		break;
	case EXPP_EFF_RANDLIFE:
		param = paf->randlife;
		break;
	case EXPP_EFF_DAMP:
		param = paf->damp;
		break;
	case EXPP_EFF_NABLA:
		param = paf->nabla;
		break;
	case EXPP_EFF_VECTSIZE:
		param = paf->vectsize;
		break;
	case EXPP_EFF_MAXLEN:
		param = paf->maxlen;
		break;
	case EXPP_EFF_MULT1:
		param = paf->mult[0];
		break;
	case EXPP_EFF_MULT2:
		param = paf->mult[1];
		break;
	case EXPP_EFF_MULT3:
		param = paf->mult[2];
		break;
	case EXPP_EFF_MULT4:
		param = paf->mult[3];
		break;
	case EXPP_EFF_LIFE1:
		param = paf->life[0];
		break;
	case EXPP_EFF_LIFE2:
		param = paf->life[1];
		break;
	case EXPP_EFF_LIFE3:
		param = paf->life[2];
		break;
	case EXPP_EFF_LIFE4:
		param = paf->life[3];
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

static int setFloatAttrClamp( BPyEffectObject *self, PyObject *value, void *type )
{
	float *param;
	PartEff *paf= (PartEff *)self->effect;
	float min, max;
	
	switch( (int)type ) {
	case EXPP_EFF_STA:
		min = -250.0f; max = MAXFRAMEF;
		param = &paf->sta;
		break;
	case EXPP_EFF_END:
		min = 1.0f; max = MAXFRAMEF;
		param = &paf->end;
		break;
	case EXPP_EFF_LIFETIME:
		min = 1.0f; max = MAXFRAMEF;
		param = &paf->lifetime;
		break;
	case EXPP_EFF_NORMFAC:
		min = -2.0f; max = 2.0f;
		param = &paf->normfac;
		break;
	case EXPP_EFF_OBFAC:
		min = -1.0f; max = 1.0f;
		param = &paf->obfac;
		break;
	case EXPP_EFF_RANDFAC:
		min = 0.0f; max = 2.0f;
		param = &paf->randfac;
		break;
	case EXPP_EFF_TEXFAC:
		min = 0.0f; max = 2.0f;
		param = &paf->texfac;
		break;
	case EXPP_EFF_RANDLIFE:
		min = 0.0f; max = 2.0f;
		param = &paf->randlife;
		break;
	case EXPP_EFF_DAMP:
		min = 0.0f; max = 1.0f;
		param = &paf->damp;
		break;
	case EXPP_EFF_NABLA:
		min = 0.0001f; max = 1.0f;
		param = &paf->nabla;
		break;
	case EXPP_EFF_VECTSIZE:
		min = 0.0f; max = 1.0f;
		param = &paf->vectsize;
		break;
	case EXPP_EFF_MAXLEN:
		min = 0.0f; max = 100.0f;
		param = &paf->maxlen;
		break;
	case EXPP_EFF_MULT1:
		min = 0.0f; max = 1.0f;
		param = &paf->mult[0];
		break;
	case EXPP_EFF_MULT2:
		min = 0.0f; max = 1.0f;
		param = &paf->mult[1];
		break;
	case EXPP_EFF_MULT3:
		min = 0.0f; max = 1.0f;
		param = &paf->mult[2];
		break;
	case EXPP_EFF_MULT4:
		min = 0.0f; max = 1.0f;
		param = &paf->mult[3];
		break;
	case EXPP_EFF_LIFE1:
		min = 1.0f; max = 600.0f;
		param = &paf->life[0];
		break;
	case EXPP_EFF_LIFE2:
		min = 1.0f; max = 600.0f;
		param = &paf->life[1];
		break;
	case EXPP_EFF_LIFE3:
		min = 1.0f; max = 600.0f;
		param = &paf->life[2];
		break;
	case EXPP_EFF_LIFE4:
		min = 1.0f; max = 600.0f;
		param = &paf->life[3];
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
static PyObject *getIntAttr( BPyEffectObject *self, void *type )
{
	int param;
	PartEff *paf= (PartEff *)self->effect;

	switch( (int)type ) {
	case EXPP_EFF_USERJIT:
		param = (int)paf->userjit;
		break;
	case EXPP_EFF_TOTPART:
		param = (int)paf->totpart;
		break;	
	case EXPP_EFF_TOTKEY:
		param = (int)paf->totkey;
		break;
	case EXPP_EFF_SEED:
		param = (int)paf->seed;
		break;
	case EXPP_EFF_STATICSTEP:
		param = (int)paf->staticstep;
		break;
	case EXPP_EFF_OMAT:
		param = (int)paf->omat;
		break;
	case EXPP_EFF_TIMETEX:
		param = (int)paf->timetex;
		break;
	case EXPP_EFF_SPEEDTEX:
		param = (int)paf->speedtex;
		break;
	case EXPP_EFF_DISP:
		param = (int)paf->disp;
		break;
	case EXPP_EFF_STYPE:
		param = (int)paf->disp;
		break;
	case EXPP_EFF_CHILD1:
		param = (int)paf->child[0];
		break;
	case EXPP_EFF_CHILD2:
		param = (int)paf->child[1];
		break;
	case EXPP_EFF_CHILD3:
		param = (int)paf->child[2];
		break;
	case EXPP_EFF_CHILD4:
		param = (int)paf->child[0];
		break;
	case EXPP_EFF_CHILDMAT1:
		param = (int)paf->mat[0];
		break;
	case EXPP_EFF_CHILDMAT2:
		param = (int)paf->mat[1];
		break;
	case EXPP_EFF_CHILDMAT3:
		param = (int)paf->mat[2];
		break;
	case EXPP_EFF_CHILDMAT4:
		param = (int)paf->mat[0];
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

static int setIntAttrClamp( BPyEffectObject *self, PyObject *value, void *type )
{
	void *param;
	PartEff *paf= (PartEff *)self->effect;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_EFF_USERJIT:
		min = 0; max = 200;
		size = 'h';
		param = (void *)&paf->userjit;
		break;
	case EXPP_EFF_TOTPART:
		min = 1; max = 100000;
		size = 'i';
		param = (void *)&paf->totpart;
		break;	
	case EXPP_EFF_TOTKEY:
		size = 'i';
		min = 1; max = 100;
		param = (void *)&paf->totkey;
		break;
	case EXPP_EFF_SEED:
		size = 'i';
		min = 0; max = 255;
		param = (void *)&paf->seed;
		break;
	case EXPP_EFF_STATICSTEP:
		min = 1; max = 100;
		size = 'h';
		param = (void *)&paf->staticstep;
		break;
	case EXPP_EFF_OMAT:
		min = 1; max = 16;
		size = 'h';
		param = (void *)&paf->omat;
		break;
	case EXPP_EFF_TIMETEX:
		min = 1; max = 10;
		size = 'h';
		param = (void *)&paf->timetex;
		break;
	case EXPP_EFF_SPEEDTEX:
		min = 1; max = 10;
		size = 'h';
		param = (void *)&paf->speedtex;
		break;
	case EXPP_EFF_DISP:
		min = 0; max = 100;
		size = 'h';
		param = (void *)&paf->disp;
		break;
	case EXPP_EFF_STYPE: /* act like a flag */
		min = 0; max = 1;
		size = 'h';
		param = (void *)&paf->stype;
		break;

	case EXPP_EFF_CHILD1:
		min = 1; max = 600;
		size = 'h';
		param = (void *)&paf->child[0];
		break;
	case EXPP_EFF_CHILD2:
		min = 1; max = 600;
		size = 'h';
		param = (void *)&paf->child[1];
		break;
	case EXPP_EFF_CHILD3:
		min = 1; max = 600;
		size = 'h';
		param = (void *)&paf->child[2];
		break;
	case EXPP_EFF_CHILD4:
		min = 1; max = 600;
		size = 'h';
		param = (void *)&paf->child[0];
		break;
	case EXPP_EFF_CHILDMAT1:
		min = 1; max = 8;
		size = 'h';
		param = (void *)&paf->mat[0];
		break;
	case EXPP_EFF_CHILDMAT2:
		min = 1; max = 8;
		size = 'h';
		param = (void *)&paf->mat[1];
		break;
	case EXPP_EFF_CHILDMAT3:
		min = 1; max = 8;
		size = 'h';
		param = (void *)&paf->mat[2];
		break;
	case EXPP_EFF_CHILDMAT4:
		min = 1; max = 8;
		size = 'h';
		param = (void *)&paf->mat[0];
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}
	return EXPP_setIValueClamped( value, param, min, max, size );
}

static PyObject *Effect_getGroup( BPyEffectObject * self )
{	/* None is ok */
	return Group_CreatePyObject( self->effect->group );
}

static int Effect_setGroup( BPyEffectObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->effect->group, NULL, 1, ID_GR, 0);
}

static PyObject *Effect_getSpeedType( BPyEffectObject * self )
{
	return PyConst_NewInt( &speedTypes, self->effect->texmap );	
}
static int Effect_setSpeedType( BPyEffectObject * self, PyObject *value  )
{
	constValue *c = Const_FromPyObject( &speedTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected SpeedTypes constant or string" );
	
	self->effect->texmap = c->i;
	return 0;
}

static PyObject *Effect_getFlag(BPyEffectObject *self, void *flag)
{	/* need to flip PAF_OFACE */
	int f = (int)( self->effect->flag ^ PAF_OFACE );
	
	if (f & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
	
}

static int Effect_setFlag(BPyEffectObject *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->effect->flag |= (int)flag;
	else
		self->effect->flag &= ~(int)flag;
	
	if ((int)flag == PAF_OFACE)
		self->effect->flag ^= PAF_OFACE; /* flip the flag */
	
	return 0;
}

static PyObject *Effect_getForce( BPyEffectObject * self )
{
	return Py_BuildValue( "(f,f,f)", self->effect->force[0],
			self->effect->force[1], self->effect->force[2] );
}

static int Effect_setForce( BPyEffectObject * self, PyObject * value )
{
	float val[3];
	int i;

	if( PyTuple_Check( value ) && PyTuple_Size( value ) == 1 )
		value = PyTuple_GetItem( value, 0 );

	if( !PyArg_ParseTuple( value, "fff", &val[0], &val[1], &val[2] ) )
		return EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of three float arguments" );
	for( i = 0; i < 3; ++i )
		self->effect->force[i] = EXPP_ClampFloat( val[i],
				EXPP_EFFECT_FORCE_MIN, EXPP_EFFECT_FORCE_MAX );
	return 0;
}

static PyObject *Effect_getDefvec( BPyEffectObject * self )
{
	return Py_BuildValue( "(f,f,f)", self->effect->defvec[0],
			self->effect->defvec[1], self->effect->defvec[2] );
}

static int Effect_setDefvec( BPyEffectObject * self, PyObject * value )
{
	float val[3];
	int i;

	if( PyTuple_Check( value ) && PyTuple_Size( value ) == 1 )
		value = PyTuple_GetItem( value, 0 );

	if( !PyArg_ParseTuple( value, "fff", &val[0], &val[1], &val[2] ) )
		return EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of three float arguments" );

	for( i = 0; i < 3; ++i )
		self->effect->defvec[i] = EXPP_ClampFloat( val[i],
				EXPP_EFFECT_DEFVEC_MIN, EXPP_EFFECT_DEFVEC_MAX );
	return 0;
}

static PyObject *Effect_getVertGroup( BPyEffectObject * self )
{
	return PyString_FromString( self->effect->vgroupname );
}


static int Effect_setVertGroup( BPyEffectObject * self, PyObject * value )
{
	char *name;
	bDeformGroup *dg;

	name = PyString_AsString ( value );
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError,
						  "expected string argument" );

	PyOS_snprintf( self->effect->vgroupname,
		sizeof( self->effect->vgroupname )-1, "%s", name );

	dg = get_named_vertexgroup( self->object, self->effect->vgroupname );
	if( dg )
		self->effect->vertgroup = (short)get_defgroup_num( self->object, dg )+1;
	else
		self->effect->vertgroup = 0;

	return 0;
}

static PyObject *Effect_getSpeedVertGroup( BPyEffectObject * self )
{
	return PyString_FromString( self->effect->vgroupname_v );
}

static int Effect_setSpeedVertGroup( BPyEffectObject * self, PyObject * value )
{
	char *name;
	bDeformGroup *dg;

	name = PyString_AsString ( value );
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError,
						  "expected string argument" );

	PyOS_snprintf( self->effect->vgroupname_v,
		sizeof( self->effect->vgroupname_v )-1, "%s", name );

	dg = get_named_vertexgroup( self->object, self->effect->vgroupname_v );
	if( dg )
		self->effect->vertgroup_v = (short)get_defgroup_num( self->object, dg )+1;
	else
		self->effect->vertgroup_v = 0;

	return 0;
}


/*****************************************************************************/
/* Method:              getParticlesLoc                                      */
/* Python equivalent:   effect.getParticlesLoc                               */
/* Description:         Get the current location of each  particle           */
/*                      and return a list of 3D vectors                      */
/*                      or a list of ists of two 3D vectors                  */
/*                      if effect.vect  has any sense                        */
/* Data:                notihng get the current time from   G.scene          */
/* Return:              One python list of 3D vector                         */
/*****************************************************************************/
static PyObject *Effect_getParticlesLoc( BPyEffectObject * self )
{
	Object *ob;
	Effect *eff;
	PartEff *paf;
	Particle *pa=0;
	PyObject  *list, *strand_list, *pyvec, *pyvec2;
	float p_time, c_time, vec[3], vec1[3], cfra, m_time, s_time;
	int a;
	short disp=100 ;
	
	cfra=frame_to_float( G.scene->r.cfra );
	
	/* as we need to update the particles system we try to retrieve
	the object to which the effect is connected */
	eff =(Effect *) self->effect;
	
	ob= self->object;
	if(!ob)
		return ( EXPP_ReturnPyObjError (PyExc_AttributeError,
							   "Effect has no object" ) );
	/*get the particles data */
	paf= (PartEff *)eff;

	/* particles->disp reduce the  display  number of particles */
	/* as we want the complete  list ... we backup the disp value and restore later */ 
	if (paf->disp<100)
		disp= paf->disp; paf->disp=100;
	
	
	build_particle_system(ob);
	pa= paf->keys;

	if(!pa)
		return ( EXPP_ReturnPyObjError (PyExc_AttributeError,
							   "Particles Location : no Keys" ) );

	/* if object is in motion */
	if( ob->ipoflag & OB_OFFS_PARTICLE )
		p_time= ob->sf;
	else
		p_time= 0.0;

	list = PyList_New( 0 );
	if( !list )
		return EXPP_ReturnPyObjError( PyExc_MemoryError, "PyList() failed" );

	c_time= bsystem_time( ob, cfra, p_time );

	for( a=0; a < paf->totpart; a++, pa += paf->totkey ) {
		
		if(paf->flag & PAF_STATIC ) {
			strand_list = PyList_New( 0 );
			m_time= pa->time+pa->lifetime+paf->staticstep-1;
			for(c_time= pa->time; c_time<m_time; c_time+=paf->staticstep) {
				where_is_particle(paf, pa, c_time, vec);
				MTC_Mat4MulVecfl(ob->obmat, vec); /* make worldspace like the others */
				pyvec = Vector_CreatePyObject(vec, 3, (PyObject *)NULL);
				if( PyList_Append( strand_list, pyvec) < 0 ) {
					Py_DECREF( list );
					Py_DECREF( strand_list );
					Py_XDECREF( pyvec );
					
					return EXPP_ReturnPyObjError( PyExc_RuntimeError,
							"Couldn't append item to PyList" );
				}
				Py_DECREF( pyvec );
				
			}
			
			if( PyList_Append( list, strand_list) < 0 ) {
					Py_DECREF( list );
					Py_DECREF( strand_list );
					return EXPP_ReturnPyObjError( PyExc_RuntimeError,
							"Couldn't append item to PyList" );
			}
			Py_DECREF( strand_list );
		} else {
			if(c_time > pa->time && c_time < pa->time+pa->lifetime ) {
				/* vector particles are a tuple of 2 vectors */
				if( paf->stype==PAF_VECT ) {
					s_time= c_time;
					p_time= c_time+1.0f;
					if(c_time < pa->time) {
						if(paf->flag & PAF_UNBORN)
							p_time= pa->time+1.0f;
						else
							continue;
					}
					if(c_time > pa->time+pa->lifetime) {
						if(paf->flag & PAF_DIED)
							s_time= pa->time+pa->lifetime-1.0f;
						else
							continue;
					}
					where_is_particle(paf, pa, s_time, vec);
					where_is_particle(paf, pa, p_time, vec1);
					pyvec  = Vector_CreatePyObject(vec, 3, (PyObject *)NULL);
					pyvec2 = Vector_CreatePyObject(vec1, 3, (PyObject *)NULL);
					if( PyList_Append( list, Py_BuildValue("[OO]", pyvec, pyvec2)) < 0 ) {
						Py_DECREF( list );
						Py_XDECREF( pyvec );
						Py_XDECREF( pyvec2 );
						return EXPP_ReturnPyObjError( PyExc_RuntimeError,
								"Couldn't append item to PyList" );
					}
					Py_DECREF( pyvec );
					Py_DECREF( pyvec2 );
				} else { /* not a vector */
					where_is_particle(paf, pa, c_time, vec);
					pyvec = Vector_CreatePyObject(vec, 3, (PyObject *)NULL);
					if( PyList_Append( list, pyvec) < 0 ) {
						Py_DECREF( list );
						Py_XDECREF( pyvec );
						return EXPP_ReturnPyObjError( PyExc_RuntimeError,
								"Couldn't append item to PyList" );
					}
					Py_DECREF( pyvec );
				}
			}
		}
	}

	/* restore the real disp value */
	if (disp<100){
		paf->disp=disp;	
		build_particle_system(ob);
	}
	
	return list;	
}

/*****************************************************************************/
/* Function:    Effect_repr                                                  */
/* Description: This is a callback function for the BPyEffectObject type. It      */
/*              builds a meaninful string to represent effcte objects.       */
/*****************************************************************************/

static PyObject *Effect_repr( void )
{
	return PyString_FromString( "Particle" );
}

/*****************************************************************************/
/* These are needed by Object.c                                              */
/*****************************************************************************/
PyObject *Effect_CreatePyObject( Effect * effect, Object *ob )
{
	BPyEffectObject *blen_object;

	blen_object =
		( BPyEffectObject * ) PyObject_NEW( BPyEffectObject, &BPyEffect_Type );

	if( blen_object )
		blen_object->effect = (PartEff *)effect;
	blen_object->object = ob;

	return ( PyObject * ) blen_object;
}

/*****************************************************************************/
/* ObjectType_Init(): add constants to the type at run-rime and initialize   */
/*****************************************************************************/
PyObject *EffectType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyEffect_Type.tp_dict == NULL ) {
		BPyEffect_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyEffect_Type.tp_dict, &speedTypes );
		PyType_Ready( &BPyEffect_Type ) ;
		BPyEffect_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyEffect_Type ;
}
