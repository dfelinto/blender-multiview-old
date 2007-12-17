/* 
 * $Id: MTex.c 11936 2007-09-04 05:22:42Z campbellbarton $
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
 * Contributor(s): Alex Mole, Yehoshua Sapir
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/
#include "MTex.h" /*This must come first*/

#include "MEM_guardedalloc.h"

#include "BKE_texture.h"
#include "BKE_utildefines.h"
#include "BLI_blenlib.h"
#include "Texture.h"
#include "Object.h"
#include "color.h"
#include "Const.h"
#include "gen_utils.h"
#include "gen_library.h"

#include <DNA_material_types.h>
#include <DNA_lamp_types.h>
#include <DNA_world_types.h>

enum mtex_float_consts {
	EXPP_TEX_ATTR_NOISESIZE = 0,
};

enum tex_float_consts {
	EXPP_MTEX_ATTR_DEF_VAR = 0,
	EXPP_MTEX_ATTR_COLFAC,
	EXPP_MTEX_ATTR_NORFAC,
	EXPP_MTEX_ATTR_VARFAC,
	EXPP_MTEX_ATTR_DISPFAC,
	EXPP_MTEX_ATTR_WARPFAC,
	EXPP_MTEX_ATTR_XOFS,
	EXPP_MTEX_ATTR_YOFS,
	EXPP_MTEX_ATTR_ZOFS,
	EXPP_MTEX_ATTR_XSCA,
	EXPP_MTEX_ATTR_YSCA,
	EXPP_MTEX_ATTR_ZSCA,
};

enum tex_const_consts {
	EXPP_MTEX_ATTR_COORD = 0,
	EXPP_MTEX_ATTR_MAPPING,
	EXPP_MTEX_ATTR_BLEND,
};

/* 
 * structure of "tuples" of constant's string name and int value
 *
 * For example, these two structures will define the constant category
 * "bpy.types.Object.DrawTypes" the constant 
 * "bpy.types.Object.DrawTypes.BOUNDBOX" and others.
 */

static constIdents coordTypesIdents[] = {
	{"ORCO",			{(int)TEXCO_ORCO}},
	{"REFLECT",			{(int)TEXCO_REFL}},
	{"NORMAL",			{(int)TEXCO_NORM}},
	{"GLOBAL",			{(int)TEXCO_GLOB}},
	{"UV",				{(int)TEXCO_UV}},
	{"OBJECT",			{(int)TEXCO_OBJECT}},
	/*{"LAVECTOR",		{(int)TEXCO_LAVECTOR}}, - not used? */
	{"VIEW",			{(int)TEXCO_VIEW}},
	{"STICKY",			{(int)TEXCO_STICKY}},
	/*{"OSA",			{(int)TEXCO_OSA}}, - internal use only */
	{"WINDOW",			{(int)TEXCO_WINDOW}},
	/*{"UV",			{(int)NEED_UV}}, - internal use only */
	{"TANGENT",			{(int)TEXCO_TANGENT}},
	{"STRESS",			{(int)TEXCO_STRESS}},
};

static constDefinition coordTypes = {
	EXPP_CONST_INT, "MappingCoords",
		sizeof(coordTypesIdents)/sizeof(constIdents), coordTypesIdents
};

static constIdents mappingTypesIdents[] = {
	{"FLAT",			{(int)MTEX_FLAT}},
	{"CUBE",			{(int)MTEX_CUBE}},
	{"TUBE",			{(int)MTEX_TUBE}},
	{"SPHERE",			{(int)MTEX_SPHERE}},
};

static constDefinition mappingTypes = {
	EXPP_CONST_INT, "MappingTypes",
		sizeof(mappingTypesIdents)/sizeof(constIdents), mappingTypesIdents
};

static constIdents blendTypesIdents[] = {
	{"BLEND",			{(int)MTEX_BLEND}},
	{"MULTIPLY",		{(int)MTEX_MUL}},
	{"ADD",				{(int)MTEX_ADD}},
	{"SUBTRACT",		{(int)MTEX_SUB}},
	{"DIVIDE",			{(int)MTEX_DIV}},
	{"DARKEN",			{(int)MTEX_DARK}},
	{"DIFFERENCE",		{(int)MTEX_DIFF}},
	{"LISGHEN",			{(int)MTEX_LIGHT}},
	{"SCREEN",			{(int)MTEX_SCREEN}},
	{"OVERLAY",			{(int)MTEX_OVERLAY}},
};
static constDefinition blendTypes = {
	EXPP_CONST_INT, "BlendTypes",
		sizeof(blendTypesIdents)/sizeof(constIdents), blendTypesIdents
};

/* internal use only */
MTex *MTex_get_pointer( BPyMTexObject *self )
{
	switch (GS(self->id->name)) {
	case ID_MA:
		return ((Material *)self->id)->mtex[self->index];
	case ID_LA:
		return ((Lamp *)self->id)->mtex[self->index];
	case ID_WO:
		return ((World *)self->id)->mtex[self->index];
	}
	PyErr_SetString( PyExc_RuntimeError, "This MTex has no texture, cannot access data" );
	return NULL;
}

/* internal use only - dont overwrite existing MTex's */
void MTex_set_pointer( BPyMTexObject *self, MTex * mtex )
{
	switch (GS(self->id->name)) {
	case ID_MA:
		((Material *)self->id)->mtex[self->index] = mtex;
	case ID_LA:
		((Lamp *)self->id)->mtex[self->index] = mtex;
	case ID_WO:
		((World *)self->id)->mtex[self->index] = mtex;
	}
}

/*
 * get floating point attributes
 */
static PyObject *getFloatAttr( BPyMTexObject *self, void *type )
{
	float param;
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;
	
	switch( (int)type ) {
	case EXPP_MTEX_ATTR_DEF_VAR:
		param = mtex->def_var;
		break;
	case EXPP_MTEX_ATTR_COLFAC:
		param = mtex->colfac;
		break;
	case EXPP_MTEX_ATTR_NORFAC:
		param = mtex->norfac;
		break;
	case EXPP_MTEX_ATTR_VARFAC:
		param = mtex->varfac;
		break;
	case EXPP_MTEX_ATTR_DISPFAC:
		param = mtex->dispfac;
		break;
	case EXPP_MTEX_ATTR_WARPFAC:
		param = mtex->warpfac;
		break;
	case EXPP_MTEX_ATTR_XOFS:
		param = mtex->ofs[0];
		break;
	case EXPP_MTEX_ATTR_YOFS:
		param = mtex->ofs[1];
		break;
	case EXPP_MTEX_ATTR_ZOFS:
		param = mtex->ofs[2];
		break;
	case EXPP_MTEX_ATTR_XSCA:
		param = mtex->size[0];
		break;
	case EXPP_MTEX_ATTR_YSCA:
		param = mtex->size[1];
		break;
	case EXPP_MTEX_ATTR_ZSCA:
		param = mtex->size[2];
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

static int setFloatAttrClamp( BPyMTexObject *self, PyObject *value, void *type )
{
	float *param;
	float min, max;
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return -1;
	
	switch( (int)type ) {
	
	/* Color First */
	case EXPP_MTEX_ATTR_DEF_VAR:
		min = 0.0f; max = 1.0f;
		param = &mtex->def_var;
		break;
	case EXPP_MTEX_ATTR_COLFAC:
		min = 0.0f; max = 1.0f;
		param = &mtex->colfac;
		break;
	case EXPP_MTEX_ATTR_NORFAC:
		min = 0.0f; max = 1.0f;
		param = &mtex->norfac;
		break;
	case EXPP_MTEX_ATTR_VARFAC:
		min = 0.0f; max = 1.0f;
		param = &mtex->varfac;
		break;
	case EXPP_MTEX_ATTR_DISPFAC:
		min = 0.0f; max = 1.0f;
		param = &mtex->dispfac;
		break;
	case EXPP_MTEX_ATTR_WARPFAC:
		min = 0.0f; max = 1.0f;
		param = &mtex->warpfac;
		break;
	case EXPP_MTEX_ATTR_XOFS:
		min = -10.0f; max = 10.0f;
		param = &mtex->ofs[0];
		break;
	case EXPP_MTEX_ATTR_YOFS:
		min = -10.0f; max = 10.0f;
		param = &mtex->ofs[1];
		break;
	case EXPP_MTEX_ATTR_ZOFS:
		min = -10.0f; max = 10.0f;
		param = &mtex->ofs[2];
		break;
	case EXPP_MTEX_ATTR_XSCA:
		min = -100.0f; max = 100.0f;
		param = &mtex->size[0];
		break;
	case EXPP_MTEX_ATTR_YSCA:
		min = -100.0f; max = 100.0f;
		param = &mtex->size[1];
		break;
	case EXPP_MTEX_ATTR_ZSCA:
		min = -100.0f; max = 100.0f;
		param = &mtex->size[2];
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	return EXPP_setFloatClamped( value, param, min, max );
}


/*
 * get constant attributes
 */

static PyObject *getConstAttr( BPyMTexObject *self, void *type )
{
	constDefinition *constType;
	constValue param;
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;
	
	switch( (int)type ) {
	case EXPP_MTEX_ATTR_COORD:
		param.i = mtex->texco;
		constType = &coordTypes;
		break;
	case EXPP_MTEX_ATTR_MAPPING:
		param.i = mtex->mapping;
		constType = &mappingTypes;
		break;
	case EXPP_MTEX_ATTR_BLEND:
		param.i = mtex->blendtype;
		constType = &blendTypes;
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

static int setConstAttr( BPyMTexObject *self, PyObject *value, void *type )
{
	void *param;
	constDefinition *constType;
	constValue *c;
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return -1;
	
	/* for each constant, assign pointer to its storage and set the type
 	 * of constant class we should match */
	switch( (int)type ) {
	case EXPP_MTEX_ATTR_COORD:
		param = (void *)&mtex->texco;
		constType = &coordTypes;
		break;
	case EXPP_MTEX_ATTR_MAPPING:
		param = (void *)&mtex->mapping;
		constType = &mappingTypes;
		break;
	case EXPP_MTEX_ATTR_BLEND:
		param = (void *)&mtex->blendtype;
		constType = &blendTypes;
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
	
	if ((int)type == EXPP_MTEX_ATTR_MAPPING)
		*(char *)param = (char)c->i;
	else
		*(short *)param = (short)c->i;
	return 0;
}


/*****************************************************************************/
/* Python BPyMTex_Type callback function prototypes:                         */
/*****************************************************************************/
static int MTex_compare( BPyMTexObject * a, BPyMTexObject * b );
static PyObject *MTex_repr( BPyMTexObject * self );

PyObject *MTex_CreatePyObject( ID * id, short index )
{
	BPyMTexObject *pymtex;

	pymtex = ( BPyMTexObject * ) PyObject_NEW( BPyMTexObject, &BPyMTex_Type );
	if( !pymtex )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyMTexObject PyObject" );

	pymtex->id = id;
	pymtex->index = index;
	return ( PyObject * ) pymtex;
}

/*****************************************************************************/
/* Python BPyMTexObject methods:                                                  */
/*****************************************************************************/

static int MTex_compare( BPyMTexObject * a, BPyMTexObject * b )
{
	return ( a->id == b->id && a->index == b->index ) ? 0 : -1;
}

static PyObject *MTex_repr( BPyMTexObject * self )
{
	return PyString_FromFormat( "[MTex \"%s\", %i]", self->id->name+2, (int)self->index );
}

/*****************************************************************************/
/* Python BPyMTexObject get and set functions:                                    */
/*****************************************************************************/

static PyObject *MTex_getTexture( BPyMTexObject *self )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) { /* NULL is ok, mapps to NULL */
		PyErr_Clear();
		Py_RETURN_NONE;
	}
	return Texture_CreatePyObject( mtex->tex );
}

/* material.textures[i].texture = Texture/None */
static int MTex_setTexture( BPyMTexObject *self, PyObject *value )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) /* NULL is ok, mapps to NULL */
		PyErr_Clear();
	
	if (BPyTexture_Check(value)) {
		
		if (!mtex) { /* allocate it */
			mtex = add_mtex();
			MTex_set_pointer(self, mtex);
		}
		
		/* deal with removing old adding new and user counts */
		return GenericLib_assignData(value, (void **) &mtex->tex, 0, 1, ID_TE, 0);
	
	} else if (value == Py_None) {
		if (!mtex)
			return 0; /* nothing to do */
	
		if(mtex->tex) mtex->tex->id.us--;
		MEM_freeN(mtex);
		return 0;	
	}
	
	return EXPP_ReturnIntError( PyExc_TypeError,
			"expected None or a Texture" );
	
}

static PyObject *MTex_getObject( BPyMTexObject *self )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;
	return Object_CreatePyObject( mtex->object );
}

static int MTex_setObject( BPyMTexObject *self, PyObject *value )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return -1;
	return GenericLib_assignData(value, (void **) &mtex->object, 0, 1, ID_OB, 0);
}

static PyObject *MTex_getUVLayer( BPyMTexObject *self )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;
	return PyString_FromString(mtex->uvname);
}

static int MTex_setUVLayer( BPyMTexObject *self, PyObject *value )
{
	char *str = PyString_AsString(value);
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex || !str) return -1;
	BLI_strncpy(mtex->uvname, str, 31);
	return 0;
}

static PyObject *MTex_getCol( BPyMTexObject *self )
{
	/* missing mtex will raise an error later, thats ok */
	return Color_CreatePyObject(NULL, 3, BPY_COLOR_MTEX, (int)self->index, (PyObject *)self);
}

static int MTex_setCol( BPyMTexObject *self, PyObject *value )
{
	MTex * mtex = MTex_get_pointer( self );
	int ret = EXPP_setVec3(value, &mtex->r, &mtex->g, &mtex->b);
	if (!mtex || ret == -1) return -1;
	
	CLAMP(mtex->r, 0.0f, 1.0f);
	CLAMP(mtex->g, 0.0f, 1.0f);
	CLAMP(mtex->b, 0.0f, 1.0f);
	return 0;
}

static PyObject *MTex_getFlag( BPyMTexObject *self, void *flag )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;
	return PyBool_FromLong( mtex->texflag & ((int) flag) );
}

static int MTex_setFlag( BPyMTexObject *self, PyObject *value, void *flag)
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return -1;
	if ( !PyBool_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected a bool");

	if ( value == Py_True )
		mtex->texflag |= (int)flag;
	else
		mtex->texflag &= ~((int)flag);

	return 0;
}

static PyObject *MTex_getProj( BPyMTexObject *self, void *axis )
{
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;
	
	switch((int)axis) {
	case 0:
		return PyInt_FromLong( mtex->projx );
		break;
	case 1:
		return PyInt_FromLong( mtex->projy );
		break;
	}
		return PyInt_FromLong( mtex->projz );
}

static int MTex_setProj( BPyMTexObject *self, PyObject *value, void *axis)
{
	MTex * mtex = MTex_get_pointer( self );
	int proj = PyInt_AsLong( value );
	if (!mtex || (proj == -1 && PyErr_Occurred()))
		return -1;

	/* valid values are from PROJ_N to PROJ_Z = 0 to 3 */
	if (proj < 0 || proj > 3)
		return EXPP_ReturnIntError( PyExc_ValueError,
			"Value must be a member of between 0 and 3" );

	switch((int)axis) {
	case 0:
		mtex->projx = (char)proj;
		return 0;
	case 1:
		mtex->projy = (char)proj;
		return 0;
	}
		mtex->projz = (char)proj;
	return 0;
}

static PyObject *MTex_getMapToFlag( BPyMTexObject *self, void *closure )
{
	int flag = (int) closure;
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return NULL;

	if ( mtex->mapto & flag )
	{
		return PyInt_FromLong( ( mtex->maptoneg & flag ) ? -1 : 1 );
	} else {
		return PyInt_FromLong( 0 );
	}
}

static int MTex_setMapToFlag( BPyMTexObject *self, PyObject *value, void *closure)
{
	int flag = (int) closure;
	int intVal;
	MTex * mtex = MTex_get_pointer( self );
	if (!mtex) return -1;
	
	if ( !PyInt_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected an int");

	intVal = PyInt_AsLong( value );

	if (flag == MAP_COL || flag == MAP_COLSPEC || flag == MAP_COLMIR ||
		flag == MAP_WARP) {
		if (intVal < 0 || intVal > 1) {
			return EXPP_ReturnIntError( PyExc_ValueError,
				"value for that mapping must be 0 or 1" );
		}
	} else {
		if (intVal < -1 || intVal > 1) {
			return EXPP_ReturnIntError( PyExc_ValueError,
				"value for that mapping must be -1, 0 or 1" );
		}
	}

	switch (intVal) {
	case 0:
		mtex->mapto &= ~flag;
		mtex->maptoneg &= ~flag;
		break;
	case 1:
		mtex->mapto |= flag;
		mtex->maptoneg &= ~flag;
		break;
	case -1:
		mtex->mapto |= flag;
		mtex->maptoneg |= flag;
		break;
	}

	return 0;
}


/*****************************************************************************/
/* Python get/set methods table                                              */
/*****************************************************************************/

static PyGetSetDef MTex_getset[] = {
	/* custom getsets */
	{ "tex", (getter) MTex_getTexture, (setter) MTex_setTexture,
		"Texture whose mapping this MTex describes", NULL },
	{ "object", (getter) MTex_getObject, (setter) MTex_setObject,
		"Object whose space to use when texco is Object", NULL },
	{ "uvlayer", (getter) MTex_getUVLayer, (setter) MTex_setUVLayer,
		"Name of the UV layer to use", NULL },
	{ "col", (getter) MTex_getCol, (setter) MTex_setCol,
		"Color that the texture blends with", NULL },
		
	/* constants */
	{"texco", (getter)getConstAttr, (setter)setConstAttr,
	 "Texture coordinate space (UV, Global, etc.)", (void *)EXPP_MTEX_ATTR_COORD},
	{ "mapping", (getter)getConstAttr, (setter)setConstAttr,
		"Mapping of texture coordinates (flat, cube, etc.)", (void *)EXPP_MTEX_ATTR_MAPPING },
	{ "blend", (getter) getConstAttr, (setter) setConstAttr,
		"Texture blending mode", NULL },
		
	 /* float */
	{ "dvar", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Value that the texture blends with when not blending colors", (void *)EXPP_MTEX_ATTR_DEF_VAR },
	{ "colfac", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Factor by which texture affects color", (void *)EXPP_MTEX_ATTR_COLFAC },
	{ "norfac", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Factor by which texture affects normal", (void *)EXPP_MTEX_ATTR_NORFAC },
	{ "varfac", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Factor by which texture affects most variables", (void *)EXPP_MTEX_ATTR_VARFAC },
	{ "dispfac", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Factor by which texture affects displacement", (void *)EXPP_MTEX_ATTR_DISPFAC },
	{ "warpfac", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Factor by which texture affects warp", (void *)EXPP_MTEX_ATTR_WARPFAC },
	
	/* these COULD be vectors but its unlikely youd need to do vector math on them
	 * so setting as attributes, they are also not accessed much */
	{ "xofs", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Offset to adjust texture space", (void *)EXPP_MTEX_ATTR_XOFS },
	{ "yofs", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Offset to adjust texture space", (void *)EXPP_MTEX_ATTR_YOFS },
	{ "zofs", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Offset to adjust texture space", (void *)EXPP_MTEX_ATTR_ZOFS },
	
	{ "xscale", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Size to scale texture space", (void *)EXPP_MTEX_ATTR_XSCA },
	{ "yscale", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Size to scale texture space", (void *)EXPP_MTEX_ATTR_YSCA },
	{ "zscale", (getter) getFloatAttr, (setter) setFloatAttrClamp,
		"Size to scale texture space", (void *)EXPP_MTEX_ATTR_ZSCA },
	
		
	/* flags */
	{ "enableStencil", (getter) MTex_getFlag, (setter) MTex_setFlag,
		"Stencil mode", (void*) MTEX_STENCIL },
	{ "enableNeg", (getter) MTex_getFlag, (setter) MTex_setFlag,
		"Negate texture values mode", (void*) MTEX_NEGATIVE },
	{ "enableNoRGB", (getter) MTex_getFlag, (setter) MTex_setFlag,
		"Convert texture RGB values to intensity values",
		(void*) MTEX_RGBTOINT },
	{ "enableCorrectNor", (getter) MTex_getFlag, (setter) MTex_setFlag,
		"Correct normal mapping for Texture space and Object space",
		(void*) MTEX_VIEWSPACE },

	{ "xproj", (getter) MTex_getProj, (setter) MTex_setProj,
		"Projection of X axis to Texture space", (void *)0 },
	{ "yproj", (getter) MTex_getProj, (setter) MTex_setProj,
		"Projection of Y axis to Texture space", (void *)1 },
	{ "zproj", (getter) MTex_getProj, (setter) MTex_setProj,
		"Projection of Z axis to Texture space", (void *)2 },
	
	
	{ "mtCol", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to color", (void*) MAP_COL },
	{ "mtNor", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to normals", (void*) MAP_NORM },
	{ "mtCsp", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to specularity color", (void*) MAP_COLSPEC },
	{ "mtCmir", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to mirror color", (void*) MAP_COLMIR },
	{ "mtRef", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to reflectivity", (void*) MAP_REF },
	{ "mtSpec", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to specularity", (void*) MAP_SPEC },
	{ "mtEmit", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to emit value", (void*) MAP_EMIT },
	{ "mtAlpha", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to alpha value", (void*) MAP_ALPHA },
	{ "mtHard", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to hardness", (void*) MAP_HAR },
	{ "mtRayMir", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to RayMir value", (void*) MAP_RAYMIRR },
	{ "mtTranslu", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to translucency", (void*) MAP_TRANSLU },
	{ "mtAmb", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to ambient value", (void*) MAP_AMB },
	{ "mtDisp", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to displacement", (void*) MAP_DISPLACE },
	{ "mtWarp", (getter) MTex_getMapToFlag, (setter) MTex_setMapToFlag,
		"How texture maps to warp", (void*) MAP_WARP },
	
	{ NULL }
};



/*****************************************************************************/
/* Python BPyMTex_Type structure definition:                                    */
/*****************************************************************************/

PyTypeObject BPyMTex_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"Blender MTex",		/* tp_name */
	sizeof( BPyMTexObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,                       /* tp_dealloc; */
	0,			/* tp_print */
	0,	/* tp_getattr */
	0,	/* tp_setattr */
	( cmpfunc ) MTex_compare,	/* tp_compare */
	( reprfunc ) MTex_repr,	/* tp_repr */
	0,			/* tp_as_number */
	0,			/* tp_as_sequence */
	0,			/* tp_as_mapping */
	0,			/* tp_as_hash */
	0, 0, 0, 0, 0,
  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT,	/*    long tp_flags; */
	0,			/* tp_doc */
	0, 0, 0, 0, 0, 0,
	0,			/* tp_methods */
	0,			/* tp_members */
	MTex_getset,/*    struct PyGetSetDef *tp_getset; */
	0,			/*    struct _typeobject *tp_base; */
	0,			/*    PyObject *tp_dict; */
	0,			/*    descrgetfunc tp_descr_get; */
	0,			/*    descrsetfunc tp_descr_set; */
	0,			/*    long tp_dictoffset; */
	0,			/*    initproc tp_init; */
	0,			/*    allocfunc tp_alloc; */
	0,			/*    newfunc tp_new; */
	/*  Low-level free-memory routine */
	0,			/*    freefunc tp_free;  */
	/* For PyObject_IS_GC */
	0,			/*    inquiry tp_is_gc;  */
	0,			/*    PyObject *tp_bases; */
	/* method resolution order */
	0,			/*    PyObject *tp_mro;  */
	0,			/*    PyObject *tp_cache; */
	0,			/*    PyObject *tp_subclasses; */
	0,			/*    PyObject *tp_weaklist; */
	0
};


PyObject *MTexType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyMTex_Type.tp_dict == NULL ) {
		BPyMTex_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyMTex_Type.tp_dict, &coordTypes );
		PyConstCategory_AddObjectToDict( BPyMTex_Type.tp_dict, &mappingTypes );
		PyConstCategory_AddObjectToDict( BPyMTex_Type.tp_dict, &blendTypes );
		PyType_Ready( &BPyMTex_Type ) ;
		BPyMTex_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyMTex_Type ;
}
