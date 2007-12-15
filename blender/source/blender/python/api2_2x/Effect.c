/*
 * $Id$
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

#define EXPP_EFFECT_TOTKEY_MIN           1
#define EXPP_EFFECT_TOTKEY_MAX         100
#define EXPP_EFFECT_SEED_MIN             0
#define EXPP_EFFECT_SEED_MAX           255
#define EXPP_EFFECT_CHILD_MIN            1
#define EXPP_EFFECT_CHILD_MAX          600
#define EXPP_EFFECT_CHILDMAT_MIN         1
#define EXPP_EFFECT_CHILDMAT_MAX        16
#define EXPP_EFFECT_JITTER_MIN           0
#define EXPP_EFFECT_JITTER_MAX         200
#define EXPP_EFFECT_DISPMAT_MIN          1
#define EXPP_EFFECT_DISPMAT_MAX         16
#define EXPP_EFFECT_TIMETEX_MIN          1
#define EXPP_EFFECT_TIMETEX_MAX         10
#define EXPP_EFFECT_SPEEDTEX_MIN         1
#define EXPP_EFFECT_SPEEDTEX_MAX        10
#define EXPP_EFFECT_TEXMAP_MIN           1
#define EXPP_EFFECT_TEXMAP_MAX           3

#define EXPP_EFFECT_SPEEDTYPE_INTENSITY  0
#define EXPP_EFFECT_SPEEDTYPE_RGB        1
#define EXPP_EFFECT_SPEEDTYPE_GRADIENT   2

#define EXPP_EFFECT_STATICSTEP_MIN      1
#define EXPP_EFFECT_STATICSTEP_MAX      100
#define EXPP_EFFECT_DISP_MIN      0
#define EXPP_EFFECT_DISP_MAX      100

/*****************************************************************************/
/* Python API function prototypes for the Blender module.		             */
/*****************************************************************************/
static PyObject *V24_M_Effect_New( PyObject * self, PyObject * args );
static PyObject *V24_M_Effect_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* Python V24_BPy_Effect methods declarations:                                 */
/*****************************************************************************/
static PyObject *V24_Effect_getType( V24_BPy_Effect * self );
static int V24_Effect_setType( void );
static PyObject *V24_Effect_getStype( V24_BPy_Effect * self );
static int V24_Effect_setStype( V24_BPy_Effect * self, PyObject * args );
static PyObject *V24_Effect_getFlag( V24_BPy_Effect * self );
static int V24_Effect_setFlag( V24_BPy_Effect * self, PyObject * args );
static PyObject *V24_Effect_getSta( V24_BPy_Effect * self );
static int V24_Effect_setSta( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getEnd( V24_BPy_Effect * self );
static int V24_Effect_setEnd( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getLifetime( V24_BPy_Effect * self );
static int V24_Effect_setLifetime( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getNormfac( V24_BPy_Effect * self );
static int V24_Effect_setNormfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getObfac( V24_BPy_Effect * self );
static int V24_Effect_setObfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getRandfac( V24_BPy_Effect * self );
static int V24_Effect_setRandfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getTexfac( V24_BPy_Effect * self );
static int V24_Effect_setTexfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getRandlife( V24_BPy_Effect * self );
static int V24_Effect_setRandlife( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getNabla( V24_BPy_Effect * self );
static int V24_Effect_setNabla( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getVectsize( V24_BPy_Effect * self );
static int V24_Effect_setVectsize( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getTotpart( V24_BPy_Effect * self );
static int V24_Effect_setTotpart( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getTotkey( V24_BPy_Effect * self );
static int V24_Effect_setTotkey( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getSeed( V24_BPy_Effect * self );
static int V24_Effect_setSeed( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getForce( V24_BPy_Effect * self );
static int V24_Effect_setForce( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getMult( V24_BPy_Effect * self );
static int V24_Effect_setMult( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getLife( V24_BPy_Effect * self );
static int V24_Effect_setLife( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getChildMat( V24_BPy_Effect * self );
static int V24_Effect_setChildMat( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getChild( V24_BPy_Effect * self );
static int V24_Effect_setChild( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getDefvec( V24_BPy_Effect * self );
static int V24_Effect_setDefvec( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getJitter( V24_BPy_Effect * self );
static int V24_Effect_setJitter( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getDispMat( V24_BPy_Effect * self );
static int V24_Effect_setDispMat( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getEmissionTex( V24_BPy_Effect * self );
static int V24_Effect_setEmissionTex( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getForceTex( V24_BPy_Effect * self );
static int V24_Effect_setForceTex( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getDamping( V24_BPy_Effect * self );
static int V24_Effect_setDamping( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getSpeedType( V24_BPy_Effect * self );
static int V24_Effect_setSpeedType( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getVertGroup( V24_BPy_Effect * self );
static int V24_Effect_setVertGroup( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getSpeedVertGroup( V24_BPy_Effect * self );
static int V24_Effect_setSpeedVertGroup( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_getStaticStep( V24_BPy_Effect * self );
static int V24_Effect_setStaticStep( V24_BPy_Effect * self , PyObject * a);
static PyObject *V24_Effect_getDisp( V24_BPy_Effect * self );
static int V24_Effect_setDisp( V24_BPy_Effect * self , PyObject * a);
static PyObject *V24_Effect_getParticlesLoc( V24_BPy_Effect * self  );

static PyObject *V24_Effect_oldsetType( void );
static PyObject *V24_Effect_oldsetStype( V24_BPy_Effect * self, PyObject * args );
static PyObject *V24_Effect_oldsetFlag( V24_BPy_Effect * self, PyObject * args );
static PyObject *V24_Effect_oldsetSta( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetEnd( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetLifetime( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetNormfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetObfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetRandfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetTexfac( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetRandlife( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetNabla( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetVectsize( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetTotpart( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetTotkey( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetSeed( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetForce( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetMult( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetLife( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetMat( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetChild( V24_BPy_Effect * self, PyObject * a );
static PyObject *V24_Effect_oldsetDefvec( V24_BPy_Effect * self, PyObject * a );

/*****************************************************************************/
/* Python V24_Effect_Type callback function prototypes:                           */
/*****************************************************************************/
static PyObject *V24_Effect_repr( void );

/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* Blender.Particle.__doc__                                                  */
/*****************************************************************************/
static char V24_M_Particle_doc[] = "The Blender Effect module\n\n\
This module provides access to **Object Data** in Blender.\n\
Functions :\n\
	New(name) : creates a new part object and adds it to the given mesh object \n\
	Get(name) : retreives a particle  with the given name (mandatory)\n\
	get(name) : same as Get.  Kept for compatibility reasons.\n";
static char V24_M_Effect_New_doc[] = "New(name) : creates a new part object and adds it to the given mesh object\n";
static char V24_M_Effect_Get_doc[] = "xxx";

/*****************************************************************************/
/* Python method structure definition for Blender.Particle module:           */
/*****************************************************************************/
static struct PyMethodDef M_Particle_methods[] = {
	{"New", ( PyCFunction ) V24_M_Effect_New, METH_VARARGS, V24_M_Effect_New_doc},
	{"Get", V24_M_Effect_Get, METH_VARARGS, V24_M_Effect_Get_doc},
	{"get", V24_M_Effect_Get, METH_VARARGS, V24_M_Effect_Get_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_BPy_Effect methods table:                                          */
/*****************************************************************************/
static PyMethodDef V24_BPy_Effect_methods[] = {
	{"getType", ( PyCFunction ) V24_Effect_getType,
	 METH_NOARGS, "() - Return Effect type"},
	{"setType", ( PyCFunction ) V24_Effect_oldsetType,
	 METH_VARARGS, "() - Set Effect type"},
	{"getStype", ( PyCFunction ) V24_Effect_getStype,
	 METH_NOARGS, "() - Return Effect stype"},
	{"setStype", ( PyCFunction ) V24_Effect_oldsetStype,
	 METH_VARARGS, "() - Set Effect stype"},  
	{"getFlag", ( PyCFunction ) V24_Effect_getFlag,
	 METH_NOARGS, "() - Return Effect flag"},
	{"setFlag", ( PyCFunction ) V24_Effect_oldsetFlag,
	 METH_VARARGS, "() - Set Effect flag"},
	{"getStartTime", ( PyCFunction ) V24_Effect_getSta,
	 METH_NOARGS, "()-Return particle start time"},
	{"setStartTime", ( PyCFunction ) V24_Effect_oldsetSta, METH_VARARGS,
	 "()- Sets particle start time"},
	{"getEndTime", ( PyCFunction ) V24_Effect_getEnd,
	 METH_NOARGS, "()-Return particle end time"},
	{"setEndTime", ( PyCFunction ) V24_Effect_oldsetEnd, METH_VARARGS,
	 "()- Sets particle end time"},
	{"getLifetime", ( PyCFunction ) V24_Effect_getLifetime,
	 METH_NOARGS, "()-Return particle life time"},
	{"setLifetime", ( PyCFunction ) V24_Effect_oldsetLifetime, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getNormfac", ( PyCFunction ) V24_Effect_getNormfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setNormfac", ( PyCFunction ) V24_Effect_oldsetNormfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getObfac", ( PyCFunction ) V24_Effect_getObfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setObfac", ( PyCFunction ) V24_Effect_oldsetObfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getRandfac", ( PyCFunction ) V24_Effect_getRandfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setRandfac", ( PyCFunction ) V24_Effect_oldsetRandfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getTexfac", ( PyCFunction ) V24_Effect_getTexfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setTexfac", ( PyCFunction ) V24_Effect_oldsetTexfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getRandlife", ( PyCFunction ) V24_Effect_getRandlife,
	 METH_NOARGS, "()-Return particle life time"},
	{"setRandlife", ( PyCFunction ) V24_Effect_oldsetRandlife, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getNabla", ( PyCFunction ) V24_Effect_getNabla,
	 METH_NOARGS, "()-Return particle life time"},
	{"setNabla", ( PyCFunction ) V24_Effect_oldsetNabla, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getVectsize", ( PyCFunction ) V24_Effect_getVectsize,
	 METH_NOARGS, "()-Return particle life time"},
	{"setVectsize", ( PyCFunction ) V24_Effect_oldsetVectsize, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getTotpart", ( PyCFunction ) V24_Effect_getTotpart,
	 METH_NOARGS, "()-Return particle life time"},
	{"setTotpart", ( PyCFunction ) V24_Effect_oldsetTotpart, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getTotkey", ( PyCFunction ) V24_Effect_getTotkey,
	 METH_NOARGS, "()-Return the number of key positions."},
	{"setTotkey", ( PyCFunction ) V24_Effect_oldsetTotkey, METH_VARARGS,
	 "()-Set the number of key positions. "},
	{"getSeed", ( PyCFunction ) V24_Effect_getSeed,
	 METH_NOARGS, "()-Return particle life time"},
	{"setSeed", ( PyCFunction ) V24_Effect_oldsetSeed, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getForce", ( PyCFunction ) V24_Effect_getForce,
	 METH_NOARGS, "()-Return particle life time"},
	{"setForce", ( PyCFunction ) V24_Effect_oldsetForce, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getMult", ( PyCFunction ) V24_Effect_getMult,
	 METH_NOARGS, "()-Return particle life time"},
	{"setMult", ( PyCFunction ) V24_Effect_oldsetMult, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getLife", ( PyCFunction ) V24_Effect_getLife,
	 METH_NOARGS, "()-Return particle life time"},
	{"setLife", ( PyCFunction ) V24_Effect_oldsetLife, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getMat", ( PyCFunction ) V24_Effect_getChildMat,
	 METH_NOARGS, "()-Return particle life time"},
	{"setMat", ( PyCFunction ) V24_Effect_oldsetMat, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getChild", ( PyCFunction ) V24_Effect_getChild,
	 METH_NOARGS, "()-Return particle life time"},
	{"setChild", ( PyCFunction ) V24_Effect_oldsetChild, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getDefvec", ( PyCFunction ) V24_Effect_getDefvec,
	 METH_NOARGS, "()-Return particle life time"},
	{"setDefvec", ( PyCFunction ) V24_Effect_oldsetDefvec, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getParticlesLoc", ( PyCFunction ) V24_Effect_getParticlesLoc, METH_NOARGS,
	 "()- Sets particle life time "},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_BPy_Effect attributes get/set structure:                           */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Effect_getseters[] = {
	{"flag",
	 (getter)V24_Effect_getFlag, (setter)V24_Effect_setFlag,
	 "The particle flag bitfield",
	 NULL},
	{"stype",
	 (getter)V24_Effect_getStype, (setter)V24_Effect_setStype,
	 "The particle stype bitfield",
	 NULL},
	{"disp",
	 (getter)V24_Effect_getDisp, (setter)V24_Effect_setDisp,
	 "The particle display value",
	 NULL},
	{"staticStep",
	 (getter)V24_Effect_getStaticStep, (setter)V24_Effect_setStaticStep,
	 "The particle static step value",
	 NULL},
	{"type",
	 (getter)V24_Effect_getType, (setter)V24_Effect_setType,
	 "The effect's type (deprecated)",
	 NULL},
	{"child",
	 (getter)V24_Effect_getChild, (setter)V24_Effect_setChild,
	 "The number of children of a particle that multiply itself",
	 NULL},
	{"childMat",
	 (getter)V24_Effect_getChildMat, (setter)V24_Effect_setChildMat,
	 "Specify the material used for the particles",
	 NULL},
	{"damping",
	 (getter)V24_Effect_getDamping, (setter)V24_Effect_setDamping,
	 "The damping factor",
	 NULL},
	{"defvec",
	 (getter)V24_Effect_getDefvec, (setter)V24_Effect_setDefvec,
	 "The axes of a force, determined by the texture",
	 NULL},
	{"dispMat",
	 (getter)V24_Effect_getDispMat, (setter)V24_Effect_setDispMat,
	 "The material used for the particles",
	 NULL},
	{"emissionTex",
	 (getter)V24_Effect_getEmissionTex, (setter)V24_Effect_setEmissionTex,
	 "The texture used for texture emission",
	 NULL},
	{"end",
	 (getter)V24_Effect_getEnd, (setter)V24_Effect_setEnd,
	 "The endframe for the effect",
	 NULL},
	{"force",
	 (getter)V24_Effect_getForce, (setter)V24_Effect_setForce,
	 "The axes of a continues force",
	 NULL},
	{"forceTex",
	 (getter)V24_Effect_getForceTex, (setter)V24_Effect_setForceTex,
	 "The texture used for force",
	 NULL},
	{"jitter",
	 (getter)V24_Effect_getJitter, (setter)V24_Effect_setJitter,
	 "Jitter table distribution: maximum particles per face",
	 NULL},
	{"life",
	 (getter)V24_Effect_getLife, (setter)V24_Effect_setLife,
	 "The life span of the next generation of particles",
	 NULL},
	{"lifetime",
	 (getter)V24_Effect_getLifetime, (setter)V24_Effect_setLifetime,
	 "The life span of the particles",
	 NULL},
	{"mult",
	 (getter)V24_Effect_getMult, (setter)V24_Effect_setMult,
	 "The probabilities that a \"dying\" particle spawns a new one",
	 NULL},
	{"nabla",
	 (getter)V24_Effect_getNabla, (setter)V24_Effect_setNabla,
	 "The dimension of the area for gradient calculation",
	 NULL},
	{"normfac",
	 (getter)V24_Effect_getNormfac, (setter)V24_Effect_setNormfac,
	 "Particle's starting speed (from the mesh)",
	 NULL},
	{"obfac",
	 (getter)V24_Effect_getObfac, (setter)V24_Effect_setObfac,
	 "Particle's starting speed (from the object)",
	 NULL},
	{"randfac",
	 (getter)V24_Effect_getRandfac, (setter)V24_Effect_setRandfac,
	 "The random variation for the starting speed",
	 NULL},
	{"randlife",
	 (getter)V24_Effect_getRandlife, (setter)V24_Effect_setRandlife,
	 "The random variation for a particle's life",
	 NULL},
	{"seed",
	 (getter)V24_Effect_getSeed, (setter)V24_Effect_setSeed,
	 "The seed for random variations",
	 NULL},
	{"speedType",
	 (getter)V24_Effect_getSpeedType, (setter)V24_Effect_setSpeedType,
	 "Controls which texture property affects particle speeds",
	 NULL},
	{"speedVGroup",
	 (getter)V24_Effect_getSpeedVertGroup, (setter)V24_Effect_setSpeedVertGroup,
	 "Vertex group for speed control",
	 NULL},
	{"sta",
	 (getter)V24_Effect_getSta, (setter)V24_Effect_setSta,
	 "The startframe for the effect",
	 NULL},
	{"texfac",
	 (getter)V24_Effect_getTexfac, (setter)V24_Effect_setTexfac,
	 "Particle's starting speed (from the texture)",
	 NULL},
	{"totpart",
	 (getter)V24_Effect_getTotpart, (setter)V24_Effect_setTotpart,
	 "The total number of particles",
	 NULL},
	{"totkey",
	 (getter)V24_Effect_getTotkey, (setter)V24_Effect_setTotkey,
	 "The total number of key positions",
	 NULL},
	{"vectsize",
	 (getter)V24_Effect_getVectsize, (setter)V24_Effect_setVectsize,
	 "The speed for particle's rotation direction",
	 NULL},
	{"vGroup",
	 (getter)V24_Effect_getVertGroup, (setter)V24_Effect_setVertGroup,
	 "Vertex group for emitted particles",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python V24_Effect_Type structure definition:                                  */
/*****************************************************************************/
PyTypeObject V24_Effect_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Effect",           /* char *tp_name; */
	sizeof( V24_BPy_Effect ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,						/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) V24_Effect_repr,   /* reprfunc tp_repr; */

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
	V24_BPy_Effect_methods,         /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Effect_getseters,       /* struct PyGetSetDef *tp_getset; */
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
/* Python method structure definition for Blender.Effect module:             */
/*****************************************************************************/

struct PyMethodDef M_Effect_methods[] = {
	{"New", ( PyCFunction ) V24_M_Effect_New, METH_VARARGS, NULL},
	{"Get", V24_M_Effect_Get, METH_VARARGS, NULL},
	{"get", V24_M_Effect_Get, METH_VARARGS, NULL},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Function:              V24_M_Effect_New                                       */
/* Python equivalent:     Blender.Effect.New                                 */
/*****************************************************************************/
PyObject *V24_M_Effect_New( PyObject * self, PyObject * args )
{
	Effect *bleffect = 0;
	Object *ob;
	char *name = NULL;

	if( !PyArg_ParseTuple( args, "s", &name ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected string argument" );

	for( ob = G.main->object.first; ob; ob = ob->id.next )
		if( !strcmp( name, ob->id.name + 2 ) )
			break;

	if( !ob )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError, 
				"object does not exist" );

	if( ob->type != OB_MESH )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError, 
				"object is not a mesh" );

	bleffect = add_effect( EFF_PARTICLE );
	if( !bleffect )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"couldn't create Effect Data in Blender" );

	BLI_addtail( &ob->effect, bleffect );

	return V24_EffectCreatePyObject( bleffect, ob );
}

/*****************************************************************************/
/* Function:              V24_M_Effect_Get                                       */
/* Python equivalent:     Blender.Effect.Get                                 */
/*****************************************************************************/
PyObject *V24_M_Effect_Get( PyObject * self, PyObject * args )
{
	/*arguments : string object name
	   int : position of effect in the obj's effect list  */
	char *name = NULL;
	Object *object_iter;
	Effect *eff;
	int num = -1, i;

	if( !PyArg_ParseTuple( args, "|si", &name, &num ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected string int argument" ) );

	object_iter = G.main->object.first;

	if( !object_iter )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"Scene contains no object" ) );

	if( name ) { /* (name, num = -1) - try to find the given object */

		while( object_iter ) {

			if( !strcmp( name, object_iter->id.name + 2 ) ) {

				eff = object_iter->effect.first; /*can be NULL: None will be returned*/

				if (num >= 0) { /* return effect in given num position if available */

					for( i = 0; i < num; i++ ) {
						if (!eff) break;
						eff = eff->next;
					}

					if (eff) {
						return V24_EffectCreatePyObject( eff, object_iter );
					} else { /* didn't find any effect in the given position */
						Py_RETURN_NONE;
					}
				}

				else {/*return a list with all effects linked to the given object*/
							/* this was pointed by Stephen Swaney */
					PyObject *effectlist = PyList_New( 0 );

					while (eff) {
						PyObject *found_eff = V24_EffectCreatePyObject( eff,
								object_iter );
						PyList_Append( effectlist, found_eff );
						Py_DECREF( found_eff ); /* PyList_Append incref'ed it */
						eff = eff->next;
					}
					return effectlist;
				}
			}
			
			object_iter = object_iter->id.next;
		}

		if (!object_iter)
			return V24_EXPP_ReturnPyObjError (PyExc_AttributeError,
				"no such object");
	}
	
	else { /* () - return a list with all effects currently in Blender */
		PyObject *effectlist = PyList_New( 0 );

		while( object_iter ) {
			if( object_iter->effect.first != NULL ) {
				eff = object_iter->effect.first;
				while( eff ) {
					PyObject *found_eff = V24_EffectCreatePyObject( eff,
							object_iter );
					PyList_Append( effectlist, found_eff );
					Py_DECREF( found_eff );
					eff = eff->next;
				}
			}
			object_iter = object_iter->id.next;
		}
		return effectlist;
	}
	Py_INCREF( Py_None );
	return Py_None;
}

/* create the Blender.Effect.Flags constant dict */

static PyObject *V24_Effect_FlagsDict( void )
{
	PyObject *Flags = V24_PyConstant_New(  );

	if( Flags ) {
		V24_BPy_constant *c = ( V24_BPy_constant * ) Flags;

		V24_PyConstant_Insert( c, "SELECTED",
				 PyInt_FromLong( EFF_SELECT ) );
		V24_PyConstant_Insert( c, "BSPLINE",
				 PyInt_FromLong( PAF_BSPLINE ) );
		V24_PyConstant_Insert( c, "STATIC",
				 PyInt_FromLong( PAF_STATIC ) );
		V24_PyConstant_Insert( c, "FACES",
				 PyInt_FromLong( PAF_FACE ) );
		V24_PyConstant_Insert( c, "ANIMATED",
				 PyInt_FromLong( PAF_ANIMATED ) );
		V24_PyConstant_Insert( c, "UNBORN",
				 PyInt_FromLong( PAF_UNBORN ) );
		V24_PyConstant_Insert( c, "VERTS",
				 PyInt_FromLong( PAF_OFACE ) );
		V24_PyConstant_Insert( c, "EMESH",
				 PyInt_FromLong( PAF_SHOWE ) );
		V24_PyConstant_Insert( c, "TRUERAND",
				 PyInt_FromLong( PAF_TRAND ) );
		V24_PyConstant_Insert( c, "EVENDIST",
				 PyInt_FromLong( PAF_EDISTR ) );
		V24_PyConstant_Insert( c, "DIED",
				 PyInt_FromLong( PAF_DIED ) );
	}
	return Flags;
}

static PyObject *V24_Effect_SpeedTypeDict( void )
{
	PyObject *Type = V24_PyConstant_New(  );

	if( Type ) {
		V24_BPy_constant *c = ( V24_BPy_constant * ) Type;

		V24_PyConstant_Insert( c, "INTENSITY",
				 PyInt_FromLong( EXPP_EFFECT_SPEEDTYPE_INTENSITY ) );
		V24_PyConstant_Insert( c, "RGB",
				 PyInt_FromLong( EXPP_EFFECT_SPEEDTYPE_RGB ) );
		V24_PyConstant_Insert( c, "GRADIENT",
				 PyInt_FromLong( EXPP_EFFECT_SPEEDTYPE_GRADIENT ) );
	}
	return Type;
}

/*****************************************************************************/
/* Function:              V24_Effect_Init                                        */
/*****************************************************************************/

PyObject *V24_Effect_Init( void )
{
	PyObject *submodule, *dict;
	PyObject *particle;
	PyObject *Flags;
	PyObject *Types;

	if( PyType_Ready( &V24_Effect_Type ) < 0)
		return NULL;

	Flags = V24_Effect_FlagsDict(  );
	Types = V24_Effect_SpeedTypeDict( );

	submodule = Py_InitModule3( "Blender.Effect", M_Effect_methods, 0 );
	if( Flags )
		PyModule_AddObject( submodule, "Flags", Flags );
	if( Types )
		PyModule_AddObject( submodule, "SpeedTypes", Types );

	particle = Py_InitModule3( "Blender.Particle", M_Particle_methods,
			V24_M_Particle_doc );

	dict = PyModule_GetDict( submodule );

	PyDict_SetItemString( dict, "Particle", particle );
	return ( submodule );
}

/*****************************************************************************/
/* Python V24_BPy_Effect methods:                                       */
/*****************************************************************************/

static PyObject *V24_Effect_getType( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long ) self->effect->type );
}

/* does nothing since there is only one type of effect */

static int V24_Effect_setType( void )
{
	return 0;
}

static int V24_Effect_setStype( V24_BPy_Effect * self, PyObject * args )
{
	short param;
	if( !PyArg_Parse( args, "h", &param ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected an int as argument" );
	self->effect->stype = param;
	return 0;
}

static PyObject *V24_Effect_getStype( V24_BPy_Effect * self )
{
	return PyInt_FromLong( (long)( self->effect->stype ) );
}

static PyObject *V24_Effect_getFlag( V24_BPy_Effect * self )
{
	return PyInt_FromLong( (long)( self->effect->flag ^ PAF_OFACE ) );
}

static int V24_Effect_setFlag( V24_BPy_Effect * self, PyObject * args )
{
	short param;
	static short bitmask = PAF_BSPLINE | PAF_STATIC | PAF_FACE | PAF_ANIMATED |
		PAF_UNBORN | PAF_OFACE | PAF_SHOWE | PAF_TRAND | PAF_EDISTR | PAF_DIED;

	if( !PyArg_Parse( args, "h", &param ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected an int as argument" );

	/* we don't allow users to change the select bit at this time */
	param &= ~EFF_SELECT;

	if ( ( param & bitmask ) != param )
			return V24_EXPP_ReturnIntError( PyExc_ValueError,
					"invalid bit(s) set in mask" );

	/* the sense of "Verts" is inverted (clear is enabled) */
	param ^= PAF_OFACE;

	/* leave select bit alone, and add in the others */
	self->effect->flag &= EFF_SELECT;
	self->effect->flag |= param;
	return 0;
}

static PyObject *V24_Effect_getSta( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->sta );
}

static int V24_Effect_setSta( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->sta,
			EXPP_EFFECT_STA_MIN, MAXFRAMEF );
}

static PyObject *V24_Effect_getEnd( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( ((PartEff *) self->effect)->end );
}

static int V24_Effect_setEnd( V24_BPy_Effect * self, PyObject * args )
{
	float val;

	if( !PyArg_Parse( args, "f", &val ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
						"expected float argument" );

	self->effect->end = V24_EXPP_ClampFloat( val,
			EXPP_EFFECT_END_MIN, MAXFRAMEF );
	return 0;
}

static PyObject *V24_Effect_getLifetime( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->lifetime );
}

static int V24_Effect_setLifetime( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->lifetime,
			EXPP_EFFECT_LIFETIME_MIN, MAXFRAMEF );
}

static PyObject *V24_Effect_getNormfac( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->normfac );
}

static int V24_Effect_setNormfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->normfac,
			EXPP_EFFECT_NORMFAC_MIN, EXPP_EFFECT_NORMFAC_MAX );
}

static PyObject *V24_Effect_getObfac( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->obfac );
}

static int V24_Effect_setObfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->obfac,
			EXPP_EFFECT_OBFAC_MIN, EXPP_EFFECT_OBFAC_MAX );
}

static PyObject *V24_Effect_getRandfac( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->randfac );
}

static int V24_Effect_setRandfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->randfac,
			EXPP_EFFECT_RANDFAC_MIN, EXPP_EFFECT_RANDFAC_MAX );
}

static PyObject *V24_Effect_getTexfac( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->texfac );
}

static int V24_Effect_setTexfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->texfac,
			EXPP_EFFECT_TEXFAC_MIN, EXPP_EFFECT_TEXFAC_MAX );
}

static PyObject *V24_Effect_getRandlife( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->randlife );
}

static int V24_Effect_setRandlife( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->randlife,
			EXPP_EFFECT_RANDLIFE_MIN, EXPP_EFFECT_RANDLIFE_MAX );
}

static PyObject *V24_Effect_getNabla( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->nabla );
}

static int V24_Effect_setNabla( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->nabla,
			EXPP_EFFECT_NABLA_MIN, EXPP_EFFECT_NABLA_MAX );
}

static PyObject *V24_Effect_getVectsize( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( self->effect->vectsize );
}

static int V24_Effect_setVectsize( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->vectsize,
			EXPP_EFFECT_VECTSIZE_MIN, EXPP_EFFECT_VECTSIZE_MAX );
}

static PyObject *V24_Effect_getTotpart( V24_BPy_Effect * self )
{
	return PyInt_FromLong( self->effect->totpart );
}

static int V24_Effect_setTotpart( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->totpart,
			(int)EXPP_EFFECT_TOTPART_MIN, (int)EXPP_EFFECT_TOTPART_MAX, 'i' );
}

static PyObject *V24_Effect_getTotkey( V24_BPy_Effect * self )
{
	return PyInt_FromLong( self->effect->totkey );
}

static int V24_Effect_setTotkey( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->totkey,
			EXPP_EFFECT_TOTKEY_MIN, EXPP_EFFECT_TOTKEY_MAX, 'i' );
}

static PyObject *V24_Effect_getSeed( V24_BPy_Effect * self )
{
	return PyInt_FromLong( self->effect->seed );
}

static int V24_Effect_setSeed( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->seed,
			EXPP_EFFECT_SEED_MIN, EXPP_EFFECT_SEED_MAX, 'i' );
}

static PyObject *V24_Effect_getForce( V24_BPy_Effect * self )
{
	return Py_BuildValue( "(f,f,f)", self->effect->force[0],
			self->effect->force[1], self->effect->force[2] );
}

static int V24_Effect_setForce( V24_BPy_Effect * self, PyObject * args )
{
	float val[3];
	int i;

	if( PyTuple_Check( args ) && PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );

	if( !PyArg_ParseTuple( args, "fff", &val[0], &val[1], &val[2] ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of three float arguments" );
	for( i = 0; i < 3; ++i )
		self->effect->force[i] = V24_EXPP_ClampFloat( val[i],
				EXPP_EFFECT_FORCE_MIN, EXPP_EFFECT_FORCE_MAX );
	return 0;
}

static PyObject *V24_Effect_getMult( V24_BPy_Effect * self )
{
	return Py_BuildValue( "(f,f,f,f)", self->effect->mult[0],
			self->effect->mult[1], self->effect->mult[2],
			self->effect->mult[3] );
}

static int V24_Effect_setMult( V24_BPy_Effect * self, PyObject * args )
{
	float val[4];
	int i;

	if( PyTuple_Check( args ) && PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );

	if( !PyArg_ParseTuple( args, "ffff", &val[0], &val[1], &val[2], &val[3] ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of four float arguments" );
	for( i = 0; i < 4; ++i )
		self->effect->mult[i] = V24_EXPP_ClampFloat( val[i],
				EXPP_EFFECT_MULT_MIN, EXPP_EFFECT_MULT_MAX );
	return 0;
}

static PyObject *V24_Effect_getLife( V24_BPy_Effect * self )
{
	return Py_BuildValue( "(f,f,f,f)", self->effect->life[0],
			self->effect->life[1], self->effect->life[2],
			self->effect->life[3] );
}

static int V24_Effect_setLife( V24_BPy_Effect * self, PyObject * args )
{
	float val[4];
	int i;

	if( PyTuple_Check( args ) && PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );

	if( !PyArg_ParseTuple( args, "ffff", &val[0], &val[1], &val[2], &val[3] ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of four float arguments" );
	for( i = 0; i < 4; ++i )
		self->effect->life[i] = V24_EXPP_ClampFloat( val[i],
				EXPP_EFFECT_LIFE_MIN, MAXFRAMEF );
	return 0;
}

static PyObject *V24_Effect_getChild( V24_BPy_Effect * self )
{
	return Py_BuildValue( "(h,h,h,h)", self->effect->child[0],
			self->effect->child[1], self->effect->child[2],
			self->effect->child[3] );
}


static int V24_Effect_setChild( V24_BPy_Effect * self, PyObject * args )
{
	short val[4];
	int i;

	if( PyTuple_Check( args ) && PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );

	if( !PyArg_ParseTuple( args, "hhhh", &val[0], &val[1], &val[2], &val[3] ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of four int argument" );
	for( i = 0; i < 4; ++i )
		self->effect->child[i] = (short)V24_EXPP_ClampInt( val[i],
				EXPP_EFFECT_CHILD_MIN, EXPP_EFFECT_CHILD_MAX );
	return 0;
}

static PyObject *V24_Effect_getChildMat( V24_BPy_Effect * self )
{
	return Py_BuildValue( "(h,h,h,h)", self->effect->mat[0],
			self->effect->mat[1], self->effect->mat[2],
			self->effect->mat[3] );
}

static int V24_Effect_setChildMat( V24_BPy_Effect * self, PyObject * args )
{
	short val[4];
	int i;

	if( PyTuple_Check( args ) && PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );

	if( !PyArg_ParseTuple( args, "hhhh", &val[0], &val[1], &val[2], &val[3] ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of four int argument" );
	for( i = 0; i < 4; ++i )
		self->effect->mat[i] = (short)V24_EXPP_ClampInt( val[i],
				EXPP_EFFECT_CHILDMAT_MIN, EXPP_EFFECT_CHILDMAT_MAX );
	return 0;
}

static PyObject *V24_Effect_getDefvec( V24_BPy_Effect * self )
{
	return Py_BuildValue( "(f,f,f)", self->effect->defvec[0],
			self->effect->defvec[1], self->effect->defvec[2] );
}

static int V24_Effect_setDefvec( V24_BPy_Effect * self, PyObject * args )
{
	float val[3];
	int i;

	if( PyTuple_Check( args ) && PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );

	if( !PyArg_ParseTuple( args, "fff", &val[0], &val[1], &val[2] ) )
		return V24_EXPP_ReturnIntError( PyExc_AttributeError,
				"expected a tuple of three float arguments" );

	for( i = 0; i < 3; ++i )
		self->effect->defvec[i] = V24_EXPP_ClampFloat( val[i],
				EXPP_EFFECT_DEFVEC_MIN, EXPP_EFFECT_DEFVEC_MAX );
	return 0;
}

static PyObject *V24_Effect_getJitter( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->userjit );
}

static int V24_Effect_setJitter( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->userjit,
			EXPP_EFFECT_JITTER_MIN, EXPP_EFFECT_JITTER_MAX, 'h' );
}

static PyObject *V24_Effect_getDispMat( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->omat );
}

static int V24_Effect_setDispMat( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->omat,
			EXPP_EFFECT_DISPMAT_MIN, EXPP_EFFECT_DISPMAT_MAX, 'h' );
}

static PyObject *V24_Effect_getEmissionTex( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->timetex );
}

static int V24_Effect_setEmissionTex( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->timetex,
			EXPP_EFFECT_TIMETEX_MIN, EXPP_EFFECT_TIMETEX_MAX, 'h' );
}

static PyObject *V24_Effect_getForceTex( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->speedtex );
}

static int V24_Effect_setForceTex( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueClamped( args, &self->effect->speedtex,
			EXPP_EFFECT_SPEEDTEX_MIN, EXPP_EFFECT_SPEEDTEX_MAX, 'h' );
}

static PyObject *V24_Effect_getSpeedType( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->texmap );
}

static int V24_Effect_setSpeedType( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueRange( args, &self->effect->texmap,
			EXPP_EFFECT_SPEEDTYPE_INTENSITY, EXPP_EFFECT_SPEEDTYPE_GRADIENT,
			'h' );
}

static PyObject *V24_Effect_getDamping( V24_BPy_Effect * self )
{
	return PyFloat_FromDouble( ( double )self->effect->damp );
}

static int V24_Effect_setDamping( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setFloatClamped( args, &self->effect->damp,
			EXPP_EFFECT_DAMP_MIN, EXPP_EFFECT_DAMP_MAX );
}

static PyObject *V24_Effect_getVertGroup( V24_BPy_Effect * self )
{
	return PyString_FromString( self->effect->vgroupname );
}


static int V24_Effect_setVertGroup( V24_BPy_Effect * self, PyObject * value )
{
	char *name;
	bDeformGroup *dg;

	name = PyString_AsString ( value );
	if( !name )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
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

static PyObject *V24_Effect_getSpeedVertGroup( V24_BPy_Effect * self )
{
	return PyString_FromString( self->effect->vgroupname_v );
}

static int V24_Effect_setSpeedVertGroup( V24_BPy_Effect * self, PyObject * value )
{
	char *name;
	bDeformGroup *dg;

	name = PyString_AsString ( value );
	if( !name )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
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
/* attribute:           getDisp                                              */
/* Description:         the current value of the display number button       */
/* Data:                self effect                                          */
/* Return:              integer value between 0  and 100                     */
/*****************************************************************************/
static PyObject *V24_Effect_getDisp( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->disp );
}

static int V24_Effect_setDisp( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setIValueRange( args, &self->effect->disp,
			EXPP_EFFECT_DISP_MIN, EXPP_EFFECT_DISP_MAX, 'h' );
}

/*****************************************************************************/
/* attribute:           getStep                                              */
/* Description:         the current value of the Step number button          */
/* Data:                self effect                                          */
/* Return:              integer value between 1 and 100                      */
/*****************************************************************************/
static PyObject *V24_Effect_getStaticStep( V24_BPy_Effect * self )
{
	return PyInt_FromLong( ( long )self->effect->staticstep );
}

static int V24_Effect_setStaticStep( V24_BPy_Effect * self , PyObject * args )
{
	return V24_EXPP_setIValueRange( args, &self->effect->staticstep,
			EXPP_EFFECT_STATICSTEP_MIN, EXPP_EFFECT_STATICSTEP_MAX,
			'h' );
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
static PyObject *V24_Effect_getParticlesLoc( V24_BPy_Effect * self )
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
		return ( V24_EXPP_ReturnPyObjError (PyExc_AttributeError,
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
		return ( V24_EXPP_ReturnPyObjError (PyExc_AttributeError,
							   "Particles Location : no Keys" ) );

	/* if object is in motion */
	if( ob->ipoflag & OB_OFFS_PARTICLE )
		p_time= ob->sf;
	else
		p_time= 0.0;

	list = PyList_New( 0 );
	if( !list )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError, "PyList() failed" );

	c_time= bsystem_time( ob, cfra, p_time );

	for( a=0; a < paf->totpart; a++, pa += paf->totkey ) {
		
		if(paf->flag & PAF_STATIC ) {
			strand_list = PyList_New( 0 );
			m_time= pa->time+pa->lifetime+paf->staticstep-1;
			for(c_time= pa->time; c_time<m_time; c_time+=paf->staticstep) {
				where_is_particle(paf, pa, c_time, vec);
				MTC_Mat4MulVecfl(ob->obmat, vec); /* make worldspace like the others */
				pyvec = newVectorObject(vec, 3, Py_NEW);
				if( PyList_Append( strand_list, pyvec) < 0 ) {
					Py_DECREF( list );
					Py_DECREF( strand_list );
					Py_XDECREF( pyvec );
					
					return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
							"Couldn't append item to PyList" );
				}
				Py_DECREF( pyvec );
				
			}
			
			if( PyList_Append( list, strand_list) < 0 ) {
					Py_DECREF( list );
					Py_DECREF( strand_list );
					return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
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
					pyvec  = newVectorObject(vec, 3, Py_NEW);
					pyvec2 = newVectorObject(vec1, 3, Py_NEW);
					if( PyList_Append( list, Py_BuildValue("[OO]", pyvec, pyvec2)) < 0 ) {
						Py_DECREF( list );
						Py_XDECREF( pyvec );
						Py_XDECREF( pyvec2 );
						return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
								"Couldn't append item to PyList" );
					}
					Py_DECREF( pyvec );
					Py_DECREF( pyvec2 );
				} else { /* not a vector */
					where_is_particle(paf, pa, c_time, vec);
					pyvec = newVectorObject(vec, 3, Py_NEW);
					if( PyList_Append( list, pyvec) < 0 ) {
						Py_DECREF( list );
						Py_XDECREF( pyvec );
						return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
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
/* Function:    V24_Effect_repr                                                  */
/* Description: This is a callback function for the V24_BPy_Effect type. It      */
/*              builds a meaninful string to represent effcte objects.       */
/*****************************************************************************/

static PyObject *V24_Effect_repr( void )
{
	return PyString_FromString( "Particle" );
}

/*****************************************************************************/
/* These are needed by Object.c                                              */
/*****************************************************************************/
PyObject *V24_EffectCreatePyObject( Effect * effect, Object *ob )
{
	V24_BPy_Effect *blen_object;

	blen_object =
		( V24_BPy_Effect * ) PyObject_NEW( V24_BPy_Effect, &V24_Effect_Type );

	if( blen_object )
		blen_object->effect = (PartEff *)effect;
	blen_object->object = ob;

	return ( PyObject * ) blen_object;
}

int V24_EffectCheckPyObject( PyObject * py_obj )
{
	return ( py_obj->ob_type == &V24_Effect_Type );
}

/* #####DEPRECATED###### */

static PyObject *V24_Effect_oldsetChild( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Effect_setChild );
}

static PyObject *V24_Effect_oldsetDefvec( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Effect_setDefvec );
}

static PyObject *V24_Effect_oldsetForce( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Effect_setForce );
}

static PyObject *V24_Effect_oldsetMat( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Effect_setChildMat );
}

static PyObject *V24_Effect_oldsetEnd( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setEnd );
}

static PyObject *V24_Effect_oldsetLife( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Effect_setLife );
}

static PyObject *V24_Effect_oldsetLifetime( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setLifetime );
}

static PyObject *V24_Effect_oldsetMult( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Effect_setMult );
}

static PyObject *V24_Effect_oldsetNabla( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setNabla );
}

static PyObject *V24_Effect_oldsetNormfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setNormfac );
}

static PyObject *V24_Effect_oldsetObfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setObfac );
}

static PyObject *V24_Effect_oldsetRandfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setRandfac );
}

static PyObject *V24_Effect_oldsetRandlife( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setRandlife );
}

static PyObject *V24_Effect_oldsetSeed( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setSeed );
}

static PyObject *V24_Effect_oldsetSta( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setSta );
}

static PyObject *V24_Effect_oldsetTexfac( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setTexfac );
}

static PyObject *V24_Effect_oldsetTotkey( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setTotkey );
}

static PyObject *V24_Effect_oldsetTotpart( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setTotpart );
}

static PyObject *V24_Effect_oldsetVectsize( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setVectsize );
}

static PyObject *V24_Effect_oldsetFlag( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setFlag );
}

static PyObject *V24_Effect_oldsetStype( V24_BPy_Effect * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Effect_setStype );
}

static PyObject *V24_Effect_oldsetType( void )
{
	return V24_EXPP_incr_ret( Py_None );
}
