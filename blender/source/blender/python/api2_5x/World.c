/* 
 * $Id: World.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * Contributor(s): Jacques Guignot, Johnny Matthews
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

/**
 * \file World.c
 * \ingroup scripts
 * \brief Blender.World Module and World Data PyObject implementation.
 *
 * Note: Parameters between "<" and ">" are optional.  But if one of them is
 * given, all preceding ones must be given, too.  Of course, this only relates
 * to the Python functions and methods described here and only inside Python
 * code. [ This will go to another file later, probably the main exppython
 * doc file].  XXX Better: put optional args with their default value:
 * (self, name = "MyName")
 */

#include "World.h"  /*This must come first*/

#include "DNA_scene_types.h"  /* for G.scene */
#include "BKE_utildefines.h"
#include "BKE_global.h"
#include "BKE_world.h"
#include "BKE_main.h"
#include "BKE_library.h"
#include "BLI_blenlib.h"
#include "BSE_editipo.h"
#include "BIF_space.h"
#include "mydevice.h"
#include "Ipo.h"
#include "MTexSeq.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "bpy_list.h"
#include "color.h"

#define IPOKEY_ZENITH   0
#define IPOKEY_HORIZON  1
#define IPOKEY_MIST     2
#define IPOKEY_STARS    3
#define IPOKEY_OFFSET   4
#define IPOKEY_SIZE     5

enum mat_float_consts {
	EXPP_WORLD_ATTR_EXP = 0,
	EXPP_WORLD_ATTR_RANGE,
	/* linfac logfac */
	EXPP_WORLD_ATTR_GRAVITY,
	/* activityBoxRadius */
	EXPP_WORLD_ATTR_MIST,
	EXPP_WORLD_ATTR_MIST_START,
	EXPP_WORLD_ATTR_MIST_DIST,
	EXPP_WORLD_ATTR_MIST_HEIGHT,
	EXPP_WORLD_ATTR_STAR_SIZE,
	EXPP_WORLD_ATTR_STAR_MIN_DIST,
	EXPP_WORLD_ATTR_STAR_DIST,
	EXPP_WORLD_ATTR_STAR_COL_NOISE,
	EXPP_WORLD_ATTR_AO_DIST,
	
	EXPP_WORLD_ATTR_AO_DIST_FAC,
	EXPP_WORLD_ATTR_AO_ENERGY,
	EXPP_WORLD_ATTR_AO_BIAS,
};

/* int/short/char */
enum mat_int_consts {
	EXPP_WORLD_ATTR_MIST_TYPE = 0,
	EXPP_WORLD_ATTR_AO_SAMP,
	EXPP_WORLD_ATTR_AO_MIX,
	EXPP_WORLD_ATTR_AO_COLOR,
};



/*****************************************************************************/
/* Python BPyWorld methods declarations:                                   */
/*****************************************************************************/
static PyObject *World_getIpo( BPyWorld * self );
static int       World_setIpo( BPyWorld * self, PyObject * args );
static PyObject *World_insertIpoKey( BPyWorld * self, PyObject * args );
static PyObject *World_copy( BPyWorld * self );


/*****************************************************************************/
/* Python API function prototypes for the World module.                     */
/*****************************************************************************/

/*****************************************************************************/
/* Python method structure definition for Blender.World module:              */
/*****************************************************************************/
struct PyMethodDef M_World_methods[] = {
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyWorld methods table:                                          */
/*****************************************************************************/
static PyMethodDef BPyWorld_methods[] = {
	{"insertIpoKey", ( PyCFunction ) World_insertIpoKey, METH_VARARGS,
	 "( World IPO type ) - Inserts a key into the IPO"},
	{"__copy__", ( PyCFunction ) World_copy, METH_NOARGS,
	 "() - Makes a copy of this world."},
	{"copy", ( PyCFunction ) World_copy, METH_NOARGS,
	 "() - Makes a copy of this world."},
	{NULL, NULL, 0, NULL}
};


static PyObject *World_getIpo( BPyWorld * self )
{
	return Ipo_CreatePyObject( self->world->ipo );
}

static int World_setIpo( BPyWorld * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->world->ipo, 0, 1, ID_IP, ID_WO);
}


/* world.__copy__ */
static PyObject *World_copy( BPyWorld * self )
{
	World *world = copy_world(self->world );
	world->id.us = 0;
	return World_CreatePyObject(world);
}

/*@}*/
/*
static int World_compare (BPyWorld *a, BPyWorld *b)
{
	World *pa = a->world, *pb = b->world;
	return (pa == pb) ? 0:-1;
}
*/
PyObject *World_CreatePyObject( World * world )
{
	BPyWorld *blen_object;

	if (!world)
		Py_RETURN_NONE;
	
	blen_object = ( BPyWorld * ) PyObject_NEW( BPyWorld, &BPyWorld_Type );

	if( blen_object == NULL ) {
		return ( NULL );
	}
	blen_object->world = world;
	return ( ( PyObject * ) blen_object );
}

/*
 * World_insertIpoKey()
 *  inserts World IPO key for ZENITH,HORIZON,MIST,STARS,OFFSET,SIZE
 */

static PyObject *World_insertIpoKey( BPyWorld * self, PyObject * args )
{
	int key = 0, map;

	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( EXPP_ReturnPyObjError( PyExc_AttributeError,
										"expected int argument" ) );

	map = texchannel_to_adrcode(self->world->texact);

	if(key == IPOKEY_ZENITH) {
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_ZEN_R, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_ZEN_G, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_ZEN_B, 0);
	}
	if(key == IPOKEY_HORIZON) {
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_HOR_R, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_HOR_G, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_HOR_B, 0);
	}
	if(key == IPOKEY_MIST) {
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_MISI, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_MISTDI, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_MISTSTA, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_MISTHI, 0);
	}
	if(key == IPOKEY_STARS) {
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_STAR_R, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_STAR_G, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_STAR_B, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_STARDIST, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, WO_STARSIZE, 0);
	}
	if(key == IPOKEY_OFFSET) {
		insertkey((ID *)self->world, ID_WO, NULL, NULL, map+MAP_OFS_X, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, map+MAP_OFS_Y, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, map+MAP_OFS_Z, 0);
	}
	if(key == IPOKEY_SIZE) {
		insertkey((ID *)self->world, ID_WO, NULL, NULL, map+MAP_SIZE_X, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, map+MAP_SIZE_Y, 0);
		insertkey((ID *)self->world, ID_WO, NULL, NULL, map+MAP_SIZE_Z, 0);
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

static PyObject *getFloatAttr( BPyWorld *self, void *type )
{
	float param;
	World *world= self->world;

	switch( (int)type ) {
	case EXPP_WORLD_ATTR_EXP:
		param = world->exp;
		break;
	case EXPP_WORLD_ATTR_RANGE:
		param = world->range;
		break;
	case EXPP_WORLD_ATTR_GRAVITY:
		param = world->gravity;
		break;
	case EXPP_WORLD_ATTR_MIST:
		param = world->misi;
		break;
	case EXPP_WORLD_ATTR_MIST_START:
		param = world->miststa;
		break;
	case EXPP_WORLD_ATTR_MIST_DIST:
		param = world->mistdist;
		break;
	case EXPP_WORLD_ATTR_MIST_HEIGHT:
		param = world->misthi;
		break;
	case EXPP_WORLD_ATTR_STAR_SIZE:
		param = world->starsize;
		break;
	case EXPP_WORLD_ATTR_STAR_MIN_DIST:
		param = world->starmindist;
		break;
	case EXPP_WORLD_ATTR_STAR_DIST:
		param = world->stardist;
		break;
	case EXPP_WORLD_ATTR_STAR_COL_NOISE:
		param = world->starcolnoise;
		break;
	case EXPP_WORLD_ATTR_AO_DIST:
		param = world->aodist;
		break;
	case EXPP_WORLD_ATTR_AO_DIST_FAC:
		param = world->aodistfac;
		break;
	case EXPP_WORLD_ATTR_AO_ENERGY:
		param = world->aoenergy;
		break;
	case EXPP_WORLD_ATTR_AO_BIAS:
		param = world->aobias;
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

static int setFloatAttrClamp( BPyWorld *self, PyObject *value, void *type )
{
	float *param;
	World *world= self->world;
	float min, max;
	
	switch( (int)type ) {
	case EXPP_WORLD_ATTR_EXP:
		min= 0.0f; max= 1.0f;
		param = &world->exp;
		break;
	case EXPP_WORLD_ATTR_RANGE:
		min= 0.2f; max= 5.0f;
		param = &world->range;
		break;
	case EXPP_WORLD_ATTR_GRAVITY:
		min= 0.0f; max= 25.0f;
		param = &world->gravity;
		break;
	/* mist */
	case EXPP_WORLD_ATTR_MIST:
		min= 0.0f; max= 1.0f;
		param = &world->misi;
		break;
	case EXPP_WORLD_ATTR_MIST_START:
		min= 0.0f; max= 1000.0f;
		param = &world->miststa;
		break;
	case EXPP_WORLD_ATTR_MIST_DIST:
		min= 0.0f; max= 1000.0f;
		param = &world->mistdist;
		break;
	case EXPP_WORLD_ATTR_MIST_HEIGHT:
		min= 0.0f; max= 100.0f;
		param = &world->misthi;
		break;
	/* star */
	case EXPP_WORLD_ATTR_STAR_SIZE:
		min= 0.0f; max= 10.0f;
		param = &world->starsize;
		break;
	case EXPP_WORLD_ATTR_STAR_MIN_DIST:
		min= 0.0f; max= 1000.0f;
		param = &world->starmindist;
		break;
	case EXPP_WORLD_ATTR_STAR_DIST:
		min= 2.0f; max= 1000.0f;
		param = &world->stardist;
		break;
	case EXPP_WORLD_ATTR_STAR_COL_NOISE:
		min= 0.0f; max= 1.0f;
		param = &world->starcolnoise;
		break;
	/* ao */
	case EXPP_WORLD_ATTR_AO_DIST:
		min= 0.001f; max= 5000.0f;
		param = &world->aodist;
		break;
	case EXPP_WORLD_ATTR_AO_DIST_FAC:
		min= 0.00001f; max= 10.0f;
		param = &world->aodistfac;
		break;
	case EXPP_WORLD_ATTR_AO_ENERGY:
		min= 0.1f; max= 3.0f;
		param = &world->aoenergy;
		break;
	case EXPP_WORLD_ATTR_AO_BIAS:
		min= 0.0f; max= 0.5f;
		param = &world->aobias;
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
static PyObject *getIntAttr( BPyWorld *self, void *type )
{
	int param; 
	World *world= self->world;

	switch( (int)type ) {
	case EXPP_WORLD_ATTR_MIST_TYPE:
		param = (int)world->mistype;
		break;
	case EXPP_WORLD_ATTR_AO_SAMP:
		param = (int)world->aosamp;
		break;
	case EXPP_WORLD_ATTR_AO_MIX:
		param = (int)world->aomix;
		break;
	case EXPP_WORLD_ATTR_AO_COLOR:
		param = (int)world->aocolor;
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
static int setIntAttrClamp( BPyWorld *self, PyObject *value, void *type )
{
	void *param;
	World *world = self->world;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_WORLD_ATTR_MIST_TYPE:
		min= 0; max= 2;
		size = 'h';
		param = (void *)&world->mistype;
		break;
	case EXPP_WORLD_ATTR_AO_SAMP:
		min= 1; max= 16;
		size = 'h';
		param = (void *)&world->aosamp;
		break;
	case EXPP_WORLD_ATTR_AO_MIX:
		min= 0; max= 2;
		size = 'h';
		param = (void *)&world->aomix;
		break;
	case EXPP_WORLD_ATTR_AO_COLOR:
		min= 0; max= 2;
		size = 'h';
		param = (void *)&world->aocolor;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}
	return EXPP_setIValueClamped( value, param, min, max, size );
}

/* modes */
static PyObject *World_getModeSky(BPyWorld *self, void *flag)
{
	if (self->world->skytype & (long)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
	
}

static int World_setModeSky(BPyWorld *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->world->skytype |= (long)flag;
	else
		self->world->skytype &= ~(long)flag;
	return 0;
}

static PyObject *World_getModeAO(BPyWorld *self, void *flag)
{
	if (self->world->aomode & (long)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
	
}

static int World_setModeAO(BPyWorld *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->world->aomode |= (long)flag;
	else
		self->world->aomode &= ~(long)flag;
	return 0;
}

static PyObject *World_getMode(BPyWorld *self, void *flag)
{
	if (self->world->mode & (long)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
	
}

static int World_setMode(BPyWorld *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->world->mode |= (long)flag;
	else
		self->world->mode &= ~(long)flag;
	return 0;
}

static PyObject *World_getCol( BPyWorld * self, void * type )
{
	return (PyObject *)Color_CreatePyObject(NULL, 3, (int)type, 0, (PyObject *)self);	
}

static int World_setColHoz( BPyWorld * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->world->horr, &self->world->horg, &self->world->horb);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->world->horr, 0.0f, 1.0f);
	CLAMP(self->world->horg, 0.0f, 1.0f);
	CLAMP(self->world->horb, 0.0f, 1.0f);
	return 0;
}

static int World_setColZen( BPyWorld * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->world->zenr, &self->world->zeng, &self->world->zenb);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->world->zenr, 0.0f, 1.0f);
	CLAMP(self->world->zeng, 0.0f, 1.0f);
	CLAMP(self->world->zenb, 0.0f, 1.0f);
	return 0;
}

static int World_setColAmb( BPyWorld * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->world->ambr, &self->world->ambg, &self->world->ambb);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->world->ambr, 0.0f, 1.0f);
	CLAMP(self->world->ambg, 0.0f, 1.0f);
	CLAMP(self->world->ambb, 0.0f, 1.0f);
	return 0;
}


/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyWorld_getset[] = {
	GENERIC_LIB_GETSETATTR_SCRIPTLINK,
	GENERIC_LIB_GETSETATTR_TEXTURE,
	
	 /* colors */
	{"horizonCol",
	 (getter)World_getCol, (setter)World_setColHoz,
	 "", (void *)BPY_COLOR_WORLD_HOR},
	{"zenithCol",
	 (getter)World_getCol, (setter)World_setColZen,
	 "", (void *)BPY_COLOR_WORLD_ZEN},
	{"ambientCol",
	 (getter)World_getCol, (setter)World_setColAmb,
	 "", (void *)BPY_COLOR_WORLD_AMB},
	
	/* custom */
	{"ipo", (getter)World_getIpo, (setter)World_setIpo,
	 "world ipo", NULL},
	 
	/* float attrs */
	{"exposure", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_EXP},
	{"range", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_RANGE},
	{"gravity", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_GRAVITY},
	{"mist", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_MIST},
	{"mistStart", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_MIST_START},
	{"mistDist", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_MIST_DIST},
	{"mistHeight", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_MIST_HEIGHT},
	{"starSize", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_STAR_SIZE},
	{"starMinDist", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_STAR_MIN_DIST},
	{"starDist", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_STAR_DIST},
	{"starColorNoise", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_STAR_COL_NOISE},
	{"aoDist", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_DIST},
	{"aoDistFac", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_DIST_FAC},
	{"aoEnergy", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_ENERGY},
	{"aoBias", (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_BIAS},
	 
	/* int attrs */
	{"mistType", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_MIST_TYPE},
	{"aoSamples", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_SAMP},
	{"aoMix", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_MIX},
	{"aoColorType", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "", (void *)EXPP_WORLD_ATTR_AO_COLOR},

	/* flags */
	{"enableSkyBlend",
	 (getter)World_getModeSky, (setter)World_setModeSky,
	 "",
	 (void *)WO_SKYBLEND},
	{"enableSkyReal",
	 (getter)World_getModeSky, (setter)World_setModeSky,
	 "",
	 (void *)WO_SKYBLEND},
	{"enableSkyPaper",
	 (getter)World_getModeSky, (setter)World_setModeSky,
	 "",
	 (void *)WO_SKYPAPER},

	{"enableAODist",
	 (getter)World_getModeAO, (setter)World_setModeAO,
	 "",
	 (void *)WO_AODIST},
	{"enableAORandomSamples",
	 (getter)World_getModeAO, (setter)World_setModeAO,
	 "",
	 (void *)WO_AORNDSMP},

	{"enableMist",
	 (getter)World_getMode, (setter)World_setMode,
	 "",
	 (void *)WO_MIST},
	{"enableStars",
	 (getter)World_getMode, (setter)World_setMode,
	 "",
	 (void *)WO_STARS},
	{"enableAO",
	 (getter)World_getMode, (setter)World_setMode,
	 "",
	 (void *)WO_AMB_OCC},

	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *World_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="World";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_world( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new world data" );
	
	id->us = 0;
	return World_CreatePyObject((World *)id);
}

/*****************************************************************************/
/* Python BPyWorld_Type structure definition:			          */
/*****************************************************************************/
PyTypeObject BPyWorld_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"World",		/* tp_name */
	sizeof( BPyWorld ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	0,		/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	NULL,	/* tp_compare */
	NULL,	/* tp_repr */

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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* long tp_flags; */

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
	BPyWorld_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyWorld_getset,			/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) World_new,		/* newfunc tp_new; */
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

PyObject * WorldType_Init( void )
{
	PyType_Ready( &BPyWorld_Type );
	return (PyObject *) &BPyWorld_Type;

	/*// TODO - constants
	PyModule_AddIntConstant( submodule, "ZENITH",      IPOKEY_ZENITH );
	PyModule_AddIntConstant( submodule, "HORIZON",     IPOKEY_HORIZON );
	PyModule_AddIntConstant( submodule, "MIST",        IPOKEY_MIST );
	PyModule_AddIntConstant( submodule, "STARS",       IPOKEY_STARS );
	PyModule_AddIntConstant( submodule, "OFFSET",      IPOKEY_OFFSET );
	PyModule_AddIntConstant( submodule, "SCALE",       IPOKEY_SIZE ); */
}
