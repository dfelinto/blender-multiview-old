/* 
 * $Id: Lamp.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * Contributor(s): Willian P. Germano, Nathan Letwory, Stephen Swaney,
 * Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Lamp.h" /*This must come first*/

#include "BKE_main.h"
#include "BKE_utildefines.h"
#include "BKE_global.h"
#include "BKE_object.h"
#include "BKE_library.h"
#include "BLI_blenlib.h"
#include "BIF_space.h"
#include "BSE_editipo.h"
#include "mydevice.h"
#include "Ipo.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "bpy_list.h"
#include "bpy_float.h"
#include "Const.h"
#include "MTexSeq.h"

/*****************************************************************************/
/* Python BPyLamp defaults:                                                 */
/*****************************************************************************/

#define IPOKEY_RGB       0
#define IPOKEY_ENERGY    1
#define IPOKEY_SPOTSIZE  2
#define IPOKEY_OFFSET    3
#define IPOKEY_SIZE      4

enum lamp_float_consts {
	/*EXPP_LAMP_ATTR_R = 0,
	EXPP_LAMP_ATTR_G,
	EXPP_LAMP_ATTR_B,*/
	EXPP_LAMP_ATTR_ENERGY = 0,
	EXPP_LAMP_ATTR_DIST,
	EXPP_LAMP_ATTR_SPOTSIZE,
	EXPP_LAMP_ATTR_SPOTBLEND,
	EXPP_LAMP_ATTR_HAINT,
	EXPP_LAMP_ATTR_QUAD1,
	EXPP_LAMP_ATTR_QUAD2,
	EXPP_LAMP_ATTR_CLIPSTA,
	EXPP_LAMP_ATTR_CLIPEND,
	/*EXPP_LAMP_ATTR_SHADSPOTSIZE,  not used anywhere */
	EXPP_LAMP_ATTR_BIAS,
	EXPP_LAMP_ATTR_SOFT,
	EXPP_LAMP_ATTR_AREA_SIZE,
	EXPP_LAMP_ATTR_AREA_SIZEY,
	EXPP_LAMP_ATTR_AREA_SIZEZ,
	EXPP_LAMP_ATTR_YF_CAUSTICBLUR,
	EXPP_LAMP_ATTR_YF_LTRADIUS,
	EXPP_LAMP_ATTR_YF_GLOWINT,
	EXPP_LAMP_ATTR_YF_GLOWOFS,
};

/* int/short/char */
enum lamp_int_consts {
	/*EXPP_LAMP_ATTR_TYPE, see consts */
	/*EXPP_LAMP_ATTR_MODE,*/
	EXPP_LAMP_ATTR_BUFSIZE = 0,
	EXPP_LAMP_ATTR_SAMP,
	EXPP_LAMP_ATTR_BUFFERS,
	EXPP_LAMP_ATTR_FILTERTYPE,
	EXPP_LAMP_ATTR_BUFTYPE,
	EXPP_LAMP_ATTR_RAY_SAMP,
	EXPP_LAMP_ATTR_RAY_SAMPY,
	/*EXPP_LAMP_ATTR_RAY_SAMPZ, - unused so far */
	EXPP_LAMP_ATTR_AREA_SHAPE,
	EXPP_LAMP_ATTR_AREA_SHADHALOSTEP,
	EXPP_LAMP_ATTR_AREA_YF_NUMPHOTONS,
	EXPP_LAMP_ATTR_AREA_YF_NUMSEARCH,
	EXPP_LAMP_ATTR_AREA_YF_PHDEPTH,
	EXPP_LAMP_ATTR_AREA_YF_USEQMC,
	EXPP_LAMP_ATTR_AREA_YF_BUFSIZE,
	EXPP_LAMP_ATTR_AREA_YF_GLOWTYPE,
};

/* int/short/char */
enum lamp_const_consts {
	EXPP_LAMP_ATTR_TYPE = 0,
	EXPP_LAMP_ATTR_BUFFER,
	EXPP_LAMP_ATTR_FILTER,
	EXPP_LAMP_ATTR_AREA,
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

static constIdents lampTypesIdents[] = {
	{"LOCAL",   		{(int)LA_LOCAL}},
	{"SUN",				{(int)LA_SUN}},
	{"SPOT",			{(int)LA_SPOT}},
	{"HEMI",			{(int)LA_HEMI}},
	{"AREA",			{(int)LA_AREA}},
	{"PHOTON",			{(int)LA_YF_PHOTON}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition lampTypes = {
	EXPP_CONST_INT, "LampTypes",
		sizeof(lampTypesIdents)/sizeof(constIdents), lampTypesIdents
};

static constIdents bufferTypesIdents[] = {
	{"REGULAR",   		{(int)LA_SHADBUF_REGULAR}},
	{"IRREGULAR",		{(int)LA_SHADBUF_IRREGULAR}},
	{"HALFWAY",			{(int)LA_SHADBUF_HALFWAY}},
};
static constDefinition bufferTypes = {
	EXPP_CONST_INT, "BufferTypes",
		sizeof(bufferTypesIdents)/sizeof(constIdents), bufferTypesIdents
};

static constIdents filterTypesIdents[] = {
	{"REGULAR",   		{(int)LA_SHADBUF_BOX}},
	{"IRREGULAR",		{(int)LA_SHADBUF_TENT}},
	{"HALFWAY",			{(int)LA_SHADBUF_GAUSS}},
};
static constDefinition filterTypes = {
	EXPP_CONST_INT, "FilterTypes",
		sizeof(filterTypesIdents)/sizeof(constIdents), filterTypesIdents
};

static constIdents areaTypesIdents[] = {
	{"SQUARE",   		{(int)LA_AREA_SQUARE}},
	{"RECT",			{(int)LA_AREA_RECT}},
	{"CUBE",			{(int)LA_AREA_CUBE}},
	/*{"BOX",			{(int)LA_AREA_BOX}},*/
};
static constDefinition areaTypes = {
	EXPP_CONST_INT, "AreaTypes",
		sizeof(areaTypesIdents)/sizeof(constIdents), areaTypesIdents
};



/*****************************************************************************/
/* Python API function prototypes for the Lamp module.                       */
/*****************************************************************************/

/*****************************************************************************/
/* Python method structure definition for Blender.Lamp module:               */
/*****************************************************************************/
struct PyMethodDef M_Lamp_methods[] = {
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyLamp methods declarations:                                     */
/*****************************************************************************/
static PyObject *Lamp_getMode(BPyLamp *self, void *flag);
static PyObject *Lamp_getModeBuf(BPyLamp *self, void *flag);
static PyObject *Lamp_getModeRaySampType(BPyLamp *self, void *flag);

static PyObject *Lamp_getCol( BPyLamp * self );
static PyObject *Lamp_getIpo( BPyLamp * self );
static PyObject *Lamp_insertIpoKey( BPyLamp * self, PyObject * args );
static PyObject *Lamp_copy( BPyLamp * self );
static int Lamp_setIpo( BPyLamp * self, PyObject * args );
static int Lamp_setMode(BPyLamp *self, PyObject *value, void *flag);
static int Lamp_setModeBuf(BPyLamp *self, PyObject *value, void *flag);
static int Lamp_setModeRaySampType(BPyLamp *self, PyObject *value, void *flag);
static int Lamp_setCol( BPyLamp * self, PyObject * args );

/*****************************************************************************/
/* Python BPyLamp methods table:                                            */
/*****************************************************************************/
static PyMethodDef BPyLamp_methods[] = {
	/* name, method, flags, doc */
	 {"insertIpoKey", ( PyCFunction ) Lamp_insertIpoKey, METH_VARARGS,
	 "( Lamp IPO type ) - Inserts a key into IPO"},
	{"__copy__", ( PyCFunction ) Lamp_copy, METH_NOARGS,
	 "() - Makes a copy of this lamp."},
	{"copy", ( PyCFunction ) Lamp_copy, METH_NOARGS,
	 "() - Makes a copy of this lamp."},
	{NULL, NULL, 0, NULL}
};

/*
 * get floating point attributes
 */
static PyObject *getFloatAttr( BPyLamp *self, void *type )
{
	float param;
	Lamp *la = self->lamp;
	int ipocurve= -1; /* means no ipo */
	
	switch( (int)type ) {
	case EXPP_LAMP_ATTR_ENERGY:
		param = la->energy;
		break;
	case EXPP_LAMP_ATTR_DIST:
		param = la->dist;
		break;
	case EXPP_LAMP_ATTR_SPOTSIZE:
		param = la->spotsize / (180.0/M_PI);
		break;
	case EXPP_LAMP_ATTR_SPOTBLEND:
		param = la->spotblend;
		break;
	case EXPP_LAMP_ATTR_HAINT:
		param = la->haint;
		break;
	case EXPP_LAMP_ATTR_QUAD1:
		param = la->att1;
		break;
	case EXPP_LAMP_ATTR_QUAD2:
		param = la->att2;
		break;
	case EXPP_LAMP_ATTR_CLIPSTA:
		param = la->clipsta;
		break;
	case EXPP_LAMP_ATTR_CLIPEND:
		param = la->clipend;
		break;
	/*case EXPP_LAMP_ATTR_SHADSPOTSIZE:
		param = la->shadspotsize;
		break;*/
	case EXPP_LAMP_ATTR_BIAS:
		param = la->bias;
		break;
	case EXPP_LAMP_ATTR_SOFT:
		param = la->soft;
		break;
	case EXPP_LAMP_ATTR_AREA_SIZE:
		param = la->area_size;
		break;
	case EXPP_LAMP_ATTR_AREA_SIZEY:
		param = la->area_sizey;
		break;
	case EXPP_LAMP_ATTR_AREA_SIZEZ:
		param = la->area_sizez;
		break;
	case EXPP_LAMP_ATTR_YF_CAUSTICBLUR:
		param = la->YF_causticblur;
		break;
	case EXPP_LAMP_ATTR_YF_LTRADIUS:
		param = la->YF_ltradius;
		break;
	case EXPP_LAMP_ATTR_YF_GLOWINT:
		param = la->YF_glowint;
		break;
	case EXPP_LAMP_ATTR_YF_GLOWOFS:
		param = la->YF_glowofs;
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
static int setFloatAttrClamp( BPyLamp *self, PyObject *value, void *type )
{
	float *param;
	Lamp *la = self->lamp;
	float min, max;
	int ret;
	
	switch( (int)type ) {
	case EXPP_LAMP_ATTR_ENERGY:
		min= 0.0f; max= 10.0f;
		param = &la->energy;
		break;
	case EXPP_LAMP_ATTR_DIST:
		min= 0.1f; max= 5000.0f;
		param = &la->dist;
		break;
	case EXPP_LAMP_ATTR_SPOTSIZE:
		min= 1.0f; max= 180.0f;
		param = &la->spotsize;
		break;
	case EXPP_LAMP_ATTR_SPOTBLEND:
		min= 0.0f; max= 1.0f;
		param = &la->spotblend;
		break;
	case EXPP_LAMP_ATTR_HAINT:
		min= 0.0f; max= 5.0f;
		param = &la->haint;
		break;
	case EXPP_LAMP_ATTR_QUAD1:
		min= 0.0f; max= 1.0f;
		param = &la->att1;
		break;
	case EXPP_LAMP_ATTR_QUAD2:
		min= 0.0f; max= 1.0f;
		param = &la->att2;
		break;
	case EXPP_LAMP_ATTR_CLIPSTA:
		min= 0.1f; max= 1000.0f;
		param = &la->clipsta;
		break;
	case EXPP_LAMP_ATTR_CLIPEND:
		min= 1.0f; max= 5000.0f;
		param = &la->clipend;
		break;
	/*case EXPP_LAMP_ATTR_SHADSPOTSIZE:
		min= 0.0f; max= 1.0f;
		param = &la->shadspotsize;
		break;*/
	case EXPP_LAMP_ATTR_BIAS:
		min= 0.01f; max= 5.0f;
		param = &la->bias;
		break;
	case EXPP_LAMP_ATTR_SOFT:
		min= 0.0f; max= 5.0f;
		param = &la->soft;
		break;
	case EXPP_LAMP_ATTR_AREA_SIZE:
		min= 0.01f; max= 100.0f;
		param = &la->area_size;
		break;
	case EXPP_LAMP_ATTR_AREA_SIZEY:
		min= 0.01f; max= 100.0f;
		param = &la->area_sizey;
		break;
	case EXPP_LAMP_ATTR_AREA_SIZEZ:
		min= 0.01f; max= 100.0f;
		param = &la->area_sizez;
		break;
	case EXPP_LAMP_ATTR_YF_CAUSTICBLUR:
		min= 0.01f; max= 1.0f;
		param = &la->YF_causticblur;
		break;
	case EXPP_LAMP_ATTR_YF_LTRADIUS:
		min= 0.0f; max= 100.0f;
		param = &la->YF_ltradius;
		break;
	case EXPP_LAMP_ATTR_YF_GLOWINT:
		min= 0.0f; max= 1.0f;
		param = &la->YF_glowint;
		break;
	case EXPP_LAMP_ATTR_YF_GLOWOFS:
		min= 0.0f; max= 2.0f;
		param = &la->YF_glowofs;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}
	
	ret= EXPP_setFloatClamped( value, param, min, max );

	/* convert bach to degrees */
	if (ret != -1 && (int)type == EXPP_LAMP_ATTR_SPOTSIZE)
		la->spotsize *= (180.0/M_PI); 
	
	return ret;
}

/*
 * get integer attributes
 */
static PyObject *getIntAttr( BPyLamp *self, void *type )
{
	int param; 
	Lamp *la = self->lamp;

	switch( (int)type ) {
	case EXPP_LAMP_ATTR_BUFSIZE:
		param = (int)la->bufsize;
		break;
	case EXPP_LAMP_ATTR_SAMP:
		param = (int)la->samp;
		break;
	case EXPP_LAMP_ATTR_FILTERTYPE:
		param = (int)la->filtertype;
		break;
	case EXPP_LAMP_ATTR_BUFTYPE:
		param = (int)la->buftype;
		break;
	case EXPP_LAMP_ATTR_RAY_SAMP:
		param = (int)la->ray_samp;
		break;
	case EXPP_LAMP_ATTR_RAY_SAMPY:
		param = (int)la->ray_sampy;
		break;
	/*case EXPP_LAMP_ATTR_RAY_SAMPZ:
		param = (int)la->ray_sampz;
		break;*/
	case EXPP_LAMP_ATTR_AREA_SHAPE:
		param = (int)la->area_shape;
		break;
	case EXPP_LAMP_ATTR_AREA_SHADHALOSTEP:
		param = (int)la->shadhalostep;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_NUMPHOTONS:
		param = (int)la->YF_numphotons;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_NUMSEARCH:
		param = (int)la->YF_numsearch;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_PHDEPTH:
		param = (int)la->YF_phdepth;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_USEQMC:
		param = (int)la->YF_useqmc;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_BUFSIZE:
		param = (int)la->YF_bufsize;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_GLOWTYPE:
		param = (int)la->YF_glowtype;
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

static int setIntAttrClamp( BPyLamp *self, PyObject *value, void *type )
{
	void *param;
	Lamp *la = self->lamp;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_LAMP_ATTR_BUFSIZE:
		min= 512; max= 10240;
		size = 'h';
		param = (void *)&la->bufsize;
		break;
	case EXPP_LAMP_ATTR_SAMP:
		min= 1; max= 16;
		size = 'h';
		param = (void *)&la->samp;
		break;
	case EXPP_LAMP_ATTR_FILTERTYPE: /* TODO - replace with constants */
		min= LA_SHADBUF_BOX; max= LA_SHADBUF_GAUSS;
		size = 'h';
		param = (void *)&la->filtertype;
		break;
	case EXPP_LAMP_ATTR_BUFTYPE: /* TODO - replace with constant */
		min= LA_SHADBUF_REGULAR; max= LA_SHADBUF_HALFWAY;
		size = 'b';
		param = (void *)&la->buftype;
		break;
	case EXPP_LAMP_ATTR_RAY_SAMP:
		min= 0; max= 16;
		size = 'h';
		param = (void *)&la->ray_samp;
		break;
	case EXPP_LAMP_ATTR_RAY_SAMPY:
		min= 0; max= 16;
		size = 'h';
		param = (void *)&la->ray_sampy;
		break;
	/*case EXPP_LAMP_ATTR_RAY_SAMPZ:
		min= 0; max= 16;
		size = 'h';
		param = (void *)&la->ray_sampz;
		break;*/
	case EXPP_LAMP_ATTR_AREA_SHAPE: /* TODO - constants */
		min= LA_AREA_SQUARE; max= LA_AREA_BOX;
		size = 'h';
		param = (void *)&la->area_shape;
		break;
	case EXPP_LAMP_ATTR_AREA_SHADHALOSTEP:
		min= 0; max= 12;
		size = 'h';
		param = (void *)&la->shadhalostep;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_NUMPHOTONS:
		min= 10000; max= 100000000;
		size = 'i';
		param = (void *)&la->YF_numphotons;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_NUMSEARCH:
		min= 100; max= 1000;
		size = 'i';
		param = (void *)&la->YF_numsearch;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_PHDEPTH:
		min= 1; max= 100;
		size = 'h';
		param = (void *)&la->YF_phdepth;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_USEQMC: /* This is a BOOL - maybe we should have it somewhere else */
		min= 0; max= 1;
		size = 'h';
		param = (void *)&la->YF_useqmc;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_BUFSIZE:
		min= 128; max= 1024;
		size = 'h';
		param = (void *)&la->YF_bufsize;
		break;
	case EXPP_LAMP_ATTR_AREA_YF_GLOWTYPE:
		min= 0; max= 1;
		size = 'h';
		param = (void *)&la->YF_glowtype;
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
static PyObject *getConstAttr( BPyLamp *self, void *type )
{
	constDefinition *constType;
	constValue param;

	switch( (int)type ) {
	case EXPP_LAMP_ATTR_TYPE:
		param.i = self->lamp->type;
		constType = &lampTypes;
		break;
	case EXPP_LAMP_ATTR_BUFFER:
		param.i = self->lamp->buftype;
		constType = &bufferTypes;
		break;
	case EXPP_LAMP_ATTR_FILTER:
		param.i = self->lamp->filtertype;
		constType = &filterTypes;
		break;
	case EXPP_LAMP_ATTR_AREA:
		param.i = self->lamp->area_shape;
		constType = &areaTypes;
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

static int setConstAttr( BPyLamp *self, PyObject *value, void *type )
{
	void *param;
	constDefinition *constType;
	constValue *c;
	
	/* for each constant, assign pointer to its storage and set the type
 	 * of constant class we should match */
	switch( (int)type ) {
	case EXPP_LAMP_ATTR_TYPE:
		param = (void *)&self->lamp->type;
		constType = &lampTypes;
		break;
	case EXPP_LAMP_ATTR_BUFFER:
		param = (void *)&self->lamp->buftype;
		constType = &bufferTypes;
		break;
	case EXPP_LAMP_ATTR_FILTER:
		param = (void *)&self->lamp->filtertype;
		constType = &filterTypes;
		break;
	case EXPP_LAMP_ATTR_AREA:
		param = (void *)&self->lamp->area_shape;
		constType = &areaTypes;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setConstAttr" );
	}

	/*
	 * this checks for a constant of the correct type, and if so use the
	 * value
	 */
	/* check that we have a valid constant */
	c = Const_FromPyObject( constType, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected DrawType constant or string" );
	
	*(int *)param = c->i;  
	return 0;
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/

static PyGetSetDef BPyLamp_getset[] = {
	GENERIC_LIB_GETSETATTR_SCRIPTLINK,
	GENERIC_LIB_GETSETATTR_TEXTURE,
	/* custom types */
	{"color",
	 (getter)Lamp_getCol, (setter)Lamp_setCol,
	 "Lamp RGB color triplet",
	 NULL},
	{"ipo",
	 (getter)Lamp_getIpo, (setter)Lamp_setIpo,
	 "Lamp Ipo",
	 NULL},
	 
	/* float attrs */
	{"energy",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp light intensity",
	 (void *)EXPP_LAMP_ATTR_ENERGY},
	{"dist",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp clipping distance",
	 (void *)EXPP_LAMP_ATTR_DIST},
	{"spotSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp spotlight beam angle (in radians)",
	 (void *)EXPP_LAMP_ATTR_SPOTSIZE},
	{"spotBlend",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_LAMP_ATTR_SPOTBLEND},	 
	{"haloInt",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp spotlight halo intensity",
	 (void *)EXPP_LAMP_ATTR_HAINT},	 
	{"quad1",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Quad lamp linear distance attenuation",
	 (void *)EXPP_LAMP_ATTR_QUAD1},
	{"quad2",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Quad lamp quadratic distance attenuation",
	 (void *)EXPP_LAMP_ATTR_QUAD2},
	{"clipStart",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp shadow map clip start",
	 (void *)EXPP_LAMP_ATTR_CLIPSTA},
	{"clipEnd",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp shadow map clip end",
	 (void *)EXPP_LAMP_ATTR_CLIPEND},
	{"clipEnd",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp shadow map clip end",
	 (void *)EXPP_LAMP_ATTR_CLIPEND},
	{"bias",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp shadow map sampling bias",
	 (void *)EXPP_LAMP_ATTR_BIAS},
	{"softness",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp shadow sample area size",
	 (void *)EXPP_LAMP_ATTR_SOFT},
	{"areaSizeX",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp X size for an arealamp",
	 (void *)EXPP_LAMP_ATTR_AREA_SIZE},
	{"areaSizeY",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp Y size for an arealamp",
	 (void *)EXPP_LAMP_ATTR_AREA_SIZEY},
	/*{"areaSizeZ",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Lamp Y size for an arealamp",
	 (void *)EXPP_LAMP_ATTR_AREA_SIZEZ},*/
	{"yfCausticBlur",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of caustics blurring (also depends on search)",
	 (void *)EXPP_LAMP_ATTR_YF_CAUSTICBLUR},
	{"yfRadius",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "radius of the lightsource, 0 is same as pointlight",
	 (void *)EXPP_LAMP_ATTR_YF_LTRADIUS},
	{"yfGlow",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Sets light glow intensity, 0 is off",
	 (void *)EXPP_LAMP_ATTR_YF_GLOWINT},
	{"yfGlowOfs",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "light glow offset, the higher, the less 'peaked' the glow",
	 (void *)EXPP_LAMP_ATTR_YF_GLOWOFS}, 
	 
	 /* int/short/char settings */
	{"bufferSize",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Lamp shadow buffer size",
	 (void *)EXPP_LAMP_ATTR_BUFSIZE},
	{"samples",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Lamp shadow map samples",
	 (void *)EXPP_LAMP_ATTR_SAMP},
	{"filterType",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *)EXPP_LAMP_ATTR_FILTERTYPE},
	{"bufferType",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *)EXPP_LAMP_ATTR_BUFTYPE},	 
	
	{"raySamplesX",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Lamp raytracing samples on the X axis",
	 (void *)EXPP_LAMP_ATTR_RAY_SAMP},
	{"raySamplesY",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Lamp raytracing samples on the Y axis",
	 (void *)EXPP_LAMP_ATTR_RAY_SAMPY},
	/*{"raySamplesZ",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Lamp raytracing samples on the Y axis",
	 (void *)EXPP_LAMP_ATTR_RAY_SAMPZ},*/
	{"areaShape",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *)EXPP_LAMP_ATTR_AREA_SHAPE},
	{"haloStep",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Lamp volumetric halo sampling frequency",
	 (void *)EXPP_LAMP_ATTR_AREA_SHADHALOSTEP},
	
	{"yfPhotons",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Maximum number of photons to shoot",
	 (void *)EXPP_LAMP_ATTR_AREA_YF_NUMPHOTONS},	 
	{"yfPhotonBlur",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of photons to mix (blur)",
	 (void *)EXPP_LAMP_ATTR_AREA_YF_NUMSEARCH},
	{"yfPhotonDepth",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Maximum caustic bounce depth",
	 (void *)EXPP_LAMP_ATTR_AREA_YF_PHDEPTH},
	{"enableQMC",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Use QMC sampling (sometimes visible patterns)",
	 (void *)EXPP_LAMP_ATTR_AREA_YF_USEQMC},
	{"yfBufSize",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Sets the size of the shadow buffer (yafray)",
	 (void *)EXPP_LAMP_ATTR_AREA_YF_BUFSIZE},
	{"yfGlowType",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Sets light glow type",
	 (void *)EXPP_LAMP_ATTR_AREA_YF_GLOWTYPE},

	{"enableShadowBuffer",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_SHAD_BUF},
	{"enableHalo",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_HALO},
	{"enableLayer",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_LAYER},	 
	{"enableQuad",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_QUAD},
	{"enableQuad",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_NEG},
	{"enableQuad",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_ONLYSHADOW},
	{"enableQuad",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_SPHERE},
	{"enableQuad",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_SQUARE},
	 /* LA_TEXTURE, LA_OSATEX, LA_DEEP_SHADOW - no user access */
	{"enableNoDiffuse",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_NO_DIFF},
	{"enableNoSpecular",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_NO_SPEC},
	{"enableRayShadow",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_SHAD_RAY},
	{"enableShadowBufferYafray",
	 (getter)Lamp_getMode, (setter)Lamp_setMode,
	 "",
	 (void *)LA_YF_SOFT},
	 
	{"enableAutoClipStart",
	 (getter)Lamp_getModeBuf, (setter)Lamp_setModeBuf,
	 "",
	 (void *)LA_SHADBUF_AUTO_START},
	{"enableAutoClipEnd",
	 (getter)Lamp_getModeBuf, (setter)Lamp_setModeBuf,
	 "",
	 (void *)LA_SHADBUF_AUTO_END},
	 
	{"enableRaySampleRound",
	 (getter)Lamp_getModeRaySampType, (setter)Lamp_setModeRaySampType,
	 "",
	 (void *)LA_SAMP_ROUND},
	{"enableRaySampleUmbra",
	 (getter)Lamp_getModeRaySampType, (setter)Lamp_setModeRaySampType,
	 "",
	 (void *)LA_SAMP_UMBRA},
	{"enableRaySampleDither",
	 (getter)Lamp_getModeRaySampType, (setter)Lamp_setModeRaySampType,
	 "",
	 (void *)LA_SAMP_DITHER},
	{"enableRaySampleJitter",
	 (getter)Lamp_getModeRaySampType, (setter)Lamp_setModeRaySampType,
	 "",
	 (void *)LA_SAMP_JITTER},

	/* consts */
	{"type",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_LAMP_ATTR_TYPE},
	{"bufferType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_LAMP_ATTR_BUFFER},
	{"filterType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_LAMP_ATTR_FILTER},
	{"areaType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_LAMP_ATTR_AREA},	 
	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *Lamp_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Lamp";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_lamp( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new lamp data" );
	
	id->us = 0;
	return Lamp_CreatePyObject((Lamp *)id);
}

/*****************************************************************************/
/* Python TypeLamp structure definition:                                     */
/*****************************************************************************/
PyTypeObject BPyLamp_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Lamp",             /* char *tp_name; */
	sizeof( BPyLamp ),         /* int tp_basicsize; */
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
	BPyLamp_methods,			/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyLamp_getset,				/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	(newfunc)Lamp_new,			/* newfunc tp_new; */
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

/* Three Python BPyLamp_Type helper functions needed by the Object module: */

/*****************************************************************************/
/* Function:    Lamp_CreatePyObject                                          */
/* Description: This function will create a new BPyLamp from an existing    */
/*              Blender lamp structure.                                      */
/*****************************************************************************/
PyObject *Lamp_CreatePyObject( Lamp * lamp )
{
	BPyLamp *pylamp;

	pylamp = ( BPyLamp * ) PyObject_NEW( BPyLamp, &BPyLamp_Type );

	if( !pylamp )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyLamp object" );

	pylamp->lamp = lamp;
	pylamp->color = NULL; /* only init when used */ 
	return ( PyObject * ) pylamp;
}

/*****************************************************************************/
/* Python BPyLamp methods:                                                  */
/*****************************************************************************/

/* Lamp.__copy__ */
static PyObject *Lamp_copy( BPyLamp * self )
{
	Lamp *lamp = copy_lamp(self->lamp );
	lamp->id.us = 0;
	return Lamp_CreatePyObject(lamp);
}

static PyObject *Lamp_getCol( BPyLamp * self )
{
	if (!self->color)
		self->color = (BPyColorObject *)Color_CreatePyObject(NULL, 3, BPY_COLOR_IMPLICIT, 0, (PyObject *)self);
	
	return (PyObject *)self->color;
}

static int Lamp_setCol( BPyLamp * self, PyObject * value )
{
	int ret = EXPP_setVec3(value, &self->lamp->r, &self->lamp->g, &self->lamp->b);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->lamp->r, 0.0f, 1.0f);
	CLAMP(self->lamp->g, 0.0f, 1.0f);
	CLAMP(self->lamp->b, 0.0f, 1.0f);
	return 0;
}

static PyObject *Lamp_getMode(BPyLamp *self, void *flag)
{
	if (self->lamp->mode & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int Lamp_setMode(BPyLamp *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->lamp->mode |= (int)flag;
	else
		self->lamp->mode &= ~(int)flag;
	return 0;
}

static PyObject *Lamp_getModeBuf(BPyLamp *self, void *flag)
{
	if (self->lamp->bufflag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int Lamp_setModeBuf(BPyLamp *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->lamp->bufflag |= (int)flag;
	else
		self->lamp->bufflag &= ~(int)flag;
	return 0;
}

static PyObject *Lamp_getModeRaySampType(BPyLamp *self, void *flag)
{
	if (self->lamp->ray_samp_type & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int Lamp_setModeRaySampType(BPyLamp *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->lamp->ray_samp_type |= (int)flag;
	else
		self->lamp->ray_samp_type &= ~(int)flag;
	return 0;
}

static PyObject *Lamp_getIpo( BPyLamp * self )
{
	return Ipo_CreatePyObject( self->lamp->ipo );
}

static int Lamp_setIpo( BPyLamp * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->lamp->ipo, 0, 1, ID_IP, ID_LA);
}

/*
 * Lamp_insertIpoKey()
 *  inserts Lamp IPO key for RGB,ENERGY,SPOTSIZE,OFFSET,SIZE
 */

static PyObject *Lamp_insertIpoKey( BPyLamp * self, PyObject * args )
{
	int key = 0, map;

	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( EXPP_ReturnPyObjError( PyExc_AttributeError,
										"expected int argument" ) );

	map = texchannel_to_adrcode(self->lamp->texact);

	if (key == IPOKEY_RGB ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, LA_COL_R, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_COL_G, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_COL_B, 0);      
	}
	if (key == IPOKEY_ENERGY ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_ENERGY, 0);    
	}	
	if (key == IPOKEY_SPOTSIZE ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_SPOTSI, 0);    
	}
	if (key == IPOKEY_OFFSET ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_OFS_X, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_OFS_Y, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_OFS_Z, 0);  
	}
	if (key == IPOKEY_SIZE ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_SIZE_X, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_SIZE_Y, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_SIZE_Z, 0);  
	}

	allspace(REMAKEIPO, 0);
	EXPP_allqueue(REDRAWIPO, 0);
	EXPP_allqueue(REDRAWVIEW3D, 0);
	EXPP_allqueue(REDRAWACTION, 0);
	EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}

PyObject *LampType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyLamp_Type.tp_dict == NULL ) {
		BPyLamp_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyLamp_Type.tp_dict, &areaTypes );
		PyConstCategory_AddObjectToDict( BPyLamp_Type.tp_dict, &filterTypes );
		PyConstCategory_AddObjectToDict( BPyLamp_Type.tp_dict, &bufferTypes );
		PyConstCategory_AddObjectToDict( BPyLamp_Type.tp_dict, &lampTypes );
		PyType_Ready( &BPyLamp_Type ) ;
		BPyLamp_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyLamp_Type ;
}

