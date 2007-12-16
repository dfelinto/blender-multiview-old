/*
 * $Id: CurveText.c 11342 2007-07-23 02:24:07Z campbellbarton $
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
 * Contributor(s): Joilnen Leite
 *                 Johnny Matthews
 *                 Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "CurveText.h" /*This must come first*/
 
#include "DNA_object_types.h"
#include "MEM_guardedalloc.h"
#include "BKE_curve.h"
#include "BKE_library.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BIF_editfont.h"	/* do_textedit() */
#include "Curve.h"
#include "Font.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "bpy_list.h"
#include "Const.h"


enum t3d_consts {
	EXPP_T3D_ATTR_FRAME_WIDTH = 0,
	EXPP_T3D_ATTR_FRAME_HEIGHT,
	EXPP_T3D_ATTR_FRAME_X,
	EXPP_T3D_ATTR_FRAME_Y,
	EXPP_T3D_ATTR_SHEAR,
	EXPP_T3D_ATTR_SIZE,
	EXPP_T3D_ATTR_LINEDIST,
	EXPP_T3D_ATTR_SPACING,
	EXPP_T3D_ATTR_XOF,
	EXPP_T3D_ATTR_YOF,
	EXPP_T3D_ATTR_UL_POS,
	EXPP_T3D_ATTR_UL_HEIGHT,
};


/*no prototypes declared in header files - external linkage outside of python*/
extern VFont *get_builtin_font(void);  
extern void freedisplist(struct ListBase *lb);
extern VFont *give_vfontpointer(int);
extern VFont *exist_vfont(char *str);
extern VFont *load_vfont(char *name);
extern int BLI_exist(char *name);

/*****************************************************************************/
/* Python BPyCurveText_Type callback function prototypes:                          */
/*****************************************************************************/
/* int CurveTextPrint (BPyCurveBaseObject *msh, FILE *fp, int flags); */

/*****************************************************************************/
/* Python BPyCurveBaseObject methods declarations:                                   */
/*****************************************************************************/
/*PyObject *CurveText_getType(BPyCurveBaseObject *self);*/
static int CurveText_setText( BPyCurveBaseObject * self, PyObject * args );
static PyObject *CurveText_getText( BPyCurveBaseObject * self );
static PyObject *CurveText_getAlignment( BPyCurveBaseObject * self );
static int CurveText_setAlignment( BPyCurveBaseObject * self, PyObject * value );
static PyObject *CurveText_getFont( BPyCurveBaseObject * self );
static int CurveText_setFont( BPyCurveBaseObject * self, PyObject * args );
static PyObject *CurveText_addFrame( BPyCurveBaseObject * self );
static PyObject *CurveText_removeFrame( BPyCurveBaseObject * self, PyObject * args );
static PyObject *CurveText_update( BPyCurveBaseObject * self );
PyObject *CurveText_CreatePyObject( Text3d * t3d );

/*****************************************************************************/
/* Python BPyCurveBaseObject methods table:                                            */
/*****************************************************************************/
char M_CurveText_doc[] = "The Blender CurveText module\n\n\
	This module provides control over Text Curve objects in Blender.\n";

static PyMethodDef BPyCurveText_methods[] = {
 	{"addFrame", ( PyCFunction ) CurveText_addFrame,
 	METH_NOARGS, "() - adds a new text frame"},
 	{"removeFrame", ( PyCFunction ) CurveText_removeFrame,
 	METH_VARARGS, "(index) - remove this frame"},
 	{"update", ( PyCFunction ) CurveText_update,
 	METH_O, "() - update the data"}, 	
	{NULL, NULL, 0, NULL}
};


static PyObject *CurveText_getTotalFrames( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( (long)(self->curve->totbox ) );
}

static PyObject *CurveText_getActiveFrame( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( (long)(self->curve->actbox-1) );
}

static int CurveText_setActiveFrame( BPyCurveBaseObject * self, PyObject * value )
{
	struct Curve *curve= self->curve;	
	PyObject* frame_int = PyNumber_Int( value );
	int index;
	

	if( !frame_int )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );
	
	index = ( int )PyInt_AS_LONG( frame_int );
	index ++;
	if (index < 1 || index > curve->totbox)
		return EXPP_ReturnIntError( PyExc_IndexError,
				"index out of range" );
	
	curve->actbox = index;
	
	return 0;
}


static PyObject *getFloatAttr( BPyCurveBaseObject *self, void *type )
{
	float param;
	struct Curve *curve= self->curve;
	
	switch( (int)type ) {
	case EXPP_T3D_ATTR_FRAME_WIDTH: 
		param = curve->tb[curve->actbox-1].w;
		break;
	case EXPP_T3D_ATTR_FRAME_HEIGHT: 
		param = curve->tb[curve->actbox-1].h;
		break;
	case EXPP_T3D_ATTR_FRAME_X: 
		param = curve->tb[curve->actbox-1].x;
		break;
	case EXPP_T3D_ATTR_FRAME_Y: 
		param = curve->tb[curve->actbox-1].y;
		break;
	case EXPP_T3D_ATTR_SHEAR: 
		param = curve->shear;
		break;
	case EXPP_T3D_ATTR_SIZE: 
		param = curve->fsize;
		break;
	case EXPP_T3D_ATTR_LINEDIST: 
		param = curve->linedist;
		break;
	case EXPP_T3D_ATTR_SPACING: 
		param = curve->spacing;
		break;
	case EXPP_T3D_ATTR_XOF: 
		param = curve->xof;
		break;
	case EXPP_T3D_ATTR_YOF: 
		param = curve->yof;
		break;
	case EXPP_T3D_ATTR_UL_POS: 
		param = curve->ulpos;
		break;
	case EXPP_T3D_ATTR_UL_HEIGHT: 
		param = curve->ulheight;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getFloatAttr" );
	}
	return PyFloat_FromDouble( param );
}

static int setFloatAttrClamp( BPyCurveBaseObject *self, PyObject *value, void *type )
{
	float *param;
	struct Curve *curve= self->curve;
	float min, max;

	switch( (int)type ) {
	case EXPP_T3D_ATTR_FRAME_WIDTH:
		min = 0.0; max = 50.0;
		param = &(curve->tb[curve->actbox-1].w);
		break;
	case EXPP_T3D_ATTR_FRAME_HEIGHT:
		min = 0.0; max = 50.0;
		param = &(curve->tb[curve->actbox-1].h);
		break;
	case EXPP_T3D_ATTR_FRAME_X:
		min = 0.0; max = 50.0;
		param = &(curve->tb[curve->actbox-1].x);
		break;
	case EXPP_T3D_ATTR_FRAME_Y:
		min = 0.0; max = 50.0;
		param = &(curve->tb[curve->actbox-1].y);
		break;
	case EXPP_T3D_ATTR_SHEAR:
		min = -1.0f; max = 1.0f;
		param = &curve->shear;
		break;
	case EXPP_T3D_ATTR_SIZE:
		min = 0.1f; max = 10.0f;
		param = &curve->fsize;
		break;
	case EXPP_T3D_ATTR_LINEDIST:
		min = 0.0f; max = 10.0f;
		param = &curve->linedist;
		break;
	case EXPP_T3D_ATTR_SPACING:
		min = 0.0f; max = 10.0f;
		param = &curve->spacing;
		break;
	case EXPP_T3D_ATTR_XOF:
		min = -50.0f; max = 50.0f;
		param = &curve->xof;
		break;
	case EXPP_T3D_ATTR_YOF:
		min = -50.0f; max = 50.0f;
		param = &curve->yof;
		break;
	case EXPP_T3D_ATTR_UL_POS:
		min = -0.2f; max = 0.8f;
		param = &curve->ulpos;
		break;
	case EXPP_T3D_ATTR_UL_HEIGHT:
		min = 0.01f; max = 0.5f;
		param = &curve->ulheight;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	return EXPP_setFloatClamped( value, param, min, max );
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyCurveText_getset[] = {
	{"text",
	 (getter)CurveText_getText, (setter)CurveText_setText,
	 "the content of this CurveText",
	 NULL},
	{"alignment",
	 (getter)CurveText_getAlignment, (setter)CurveText_setAlignment,
	 "the alignment of this CurveText",
	 NULL},
	{"font",
	 (getter)CurveText_getFont, (setter)CurveText_setFont,
	 "the alignment of this CurveText",
	 NULL},
	
	/* frame */
	{"activeFrame",
	 (getter)CurveText_getActiveFrame, (setter)CurveText_setActiveFrame,
	 "the index of the active text frame",
	 NULL},
	{"totalFrames",
	 (getter)CurveText_getTotalFrames, (setter)NULL,
	 "the total number of text frames",
	 NULL},

	{"frameWidth",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the width of the active text frame",
	 (void *)EXPP_T3D_ATTR_FRAME_WIDTH},
	{"frameHeight",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the height of the active text frame",
	 (void *)EXPP_T3D_ATTR_FRAME_HEIGHT},
	{"frameX",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the X position of the active text frame",
	 (void *)EXPP_T3D_ATTR_FRAME_X},
	{"frameY",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the Y position of the active text frame",
	 (void *)EXPP_T3D_ATTR_FRAME_Y},
	{"shear",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_SHEAR},
	{"size",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_SIZE},
	{"lineDist",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_LINEDIST},
	{"spacing",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_SPACING},
	{"xOffset",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_XOF},
	{"yOffset",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_YOF},
	{"underLinePos",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_UL_POS},
	{"underLineHeight",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "",
	 (void *)EXPP_T3D_ATTR_UL_HEIGHT},

	 /* TODO - add more flags */
	 
	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *CurveText_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Text3d *t3d;
	char *name="CurveText";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_curve( name, OB_FONT );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new CurveText data" );
	
	/* text 3d only */
	t3d = (Text3d *)id;
	t3d->vfont= get_builtin_font();
	t3d->vfont->id.us++;
	t3d->str= MEM_mallocN(sizeof(wchar_t), "str");
	t3d->str[0] = '\0';
	t3d->totbox= t3d->actbox= 1;
	t3d->tb= MEM_callocN(MAXTEXTBOX*sizeof(TextBox), "textbox");
	t3d->tb[0].w = t3d->tb[0].h = 0.0;
	
	id->us = 0;
	return CurveText_CreatePyObject((Text3d *)id);
}


PyTypeObject BPyCurveText_Type = {
	PyObject_HEAD_INIT(NULL)
	0,
	"BPyFloat",
	sizeof(BPyCurveObject),
	0,
	0,					/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0,					/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* getiterfunc tp_iter; */
	0,					/* iternextfunc tp_iternext; */
	BPyCurveText_methods,/* tp_methods */
	0,					/* tp_members */
	BPyCurveText_getset,/* tp_getset */
	&BPyCurveBase_Type,	/* tp_base */ 
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */ 
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	( newfunc ) CurveText_new,	/* tp_new */
};

/* 
 *   CurveText_update( )
 *   method to update display list for a Curve.
 */
static PyObject *CurveText_update( BPyCurveBaseObject * self )
{
	freedisplist( &self->curve->disp );
	Py_RETURN_NONE;
}


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

static constIdents alignTypesIdents[] = {
	{"LEFT",   		{(int)CU_LEFT}},
	{"MIDDLE",		{(int)CU_MIDDLE}},
	{"RIGHT",		{(int)CU_RIGHT}},
	{"FLUSH",		{(int)CU_FLUSH}},
	{"JUSTIFY"	,	{(int)CU_JUSTIFY}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition alignTypes = {
	EXPP_CONST_INT, "AlignTypes",
		sizeof(alignTypesIdents)/sizeof(constIdents), alignTypesIdents
};

static int CurveText_setText( BPyCurveBaseObject * self, PyObject * value )
{
	char *text;

	if (!PyString_Check(value))
		return EXPP_ReturnIntError( PyExc_AttributeError,
				"expected string argument" );
	
	text = PyString_AsString(value);
	
	/*
	 * If the text is currently being edited, then we have to put the
	 * text into the edit buffer.
	 */

	if( G.obedit && G.obedit->data == self->curve ) {
		short qual = G.qual;
		G.qual = 0;		/* save key qualifier, then clear it */
		self->curve->pos = self->curve->len = 0;
		while ( *text )
			do_textedit( 0, 0, *text++ );
		G.qual = qual;
	} else {
		short len = (short)strlen(text);
		MEM_freeN( self->curve->str );
		self->curve->str = MEM_callocN( len+sizeof(wchar_t), "str" );
		strcpy( self->curve->str, text );
		self->curve->pos = len;
		self->curve->len = len;

		if( self->curve->strinfo )
			MEM_freeN( self->curve->strinfo );
		/* don't know why this is +4, just duplicating load_editText() */
		self->curve->strinfo = MEM_callocN( (len+4) *sizeof(CharInfo),
				"strinfo");
	}
	return 0;
}

static PyObject *CurveText_getText( BPyCurveBaseObject * self )
{
	if( self->curve->str )
		return PyString_FromString( self->curve->str );

	return PyString_FromString( "" );
}

static PyObject *CurveText_getAlignment( BPyCurveBaseObject * self )
{
	return PyConst_NewInt( &alignTypes, self->curve->spacemode );
}

static int CurveText_setAlignment( BPyCurveBaseObject * self, PyObject * value )
{
	constValue *c = Const_FromPyObject( &alignTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected AlignTypes constant or string" );
	
	self->curve->spacemode = c->i;
	return 0;	
}


/*****************************************************************************
 * Function:    CurveText_CreatePyObject                                       
 * Description: This function will create a new BPyCurveBaseObject from an existing   
 *               Blender structure.                                     
 *****************************************************************************/

PyObject *CurveText_CreatePyObject( Text3d * t3d )
{
	BPyCurveBaseObject *bpyt3d;
	
	if (!t3d)
		Py_RETURN_NONE;
	
	bpyt3d = ( BPyCurveBaseObject * ) PyObject_NEW( BPyCurveBaseObject, &BPyCurveText_Type );

	if( !bpyt3d )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyCurveBaseObject object" );

	bpyt3d->curve = t3d;

	return ( PyObject * ) bpyt3d;
}

static PyObject *CurveText_getFont( BPyCurveBaseObject * self )
{
	/* TODO - make the builtin_font should be None */
	return Font_CreatePyObject (self->curve->vfont);
}

static int CurveText_setFont( BPyCurveBaseObject * self, PyObject * value )
{
	if (!BPyFont_Check(value))
		return EXPP_ReturnIntError( PyExc_TypeError,
					      "expected a font" );
	
	self->curve->vfont= ((BPyFontObject *)value)->font;
	if (self->curve->vfont) /* is this allowed to be null? */
		id_us_plus((ID *)self->curve->vfont);
	return 0;
}

static PyObject *CurveText_addFrame( BPyCurveBaseObject * self )
{
	Curve *cu = self->curve;
	
	if (cu->totbox >= 256)	
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"limited to 256 frames" );
	
	cu->totbox++;	
	cu->tb[cu->totbox-1]= cu->tb[cu->totbox-2];
	Py_RETURN_NONE;
}

static PyObject *CurveText_removeFrame( BPyCurveBaseObject * self, PyObject * args )
{
	Curve *cu = self->curve;
	int index, i;
	
	if (cu->totbox == 1)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"cannot remove the last frame" );
	
	index = cu->totbox-1;
	
	if( !PyArg_ParseTuple( args, "|i", &index ) )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"expected an int" );
	
	if (index < 0 || index >= cu->totbox )
		return EXPP_ReturnPyObjError( PyExc_IndexError,
			"index out of range" );
	
	for (i = index; i < cu->totbox; i++) cu->tb[i]= cu->tb[i+1];
	cu->totbox--;
	cu->actbox--;
	Py_RETURN_NONE;
}

PyObject *CurveTextType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyCurveText_Type.tp_dict == NULL ) {
		BPyCurveText_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyCurveText_Type.tp_dict, &alignTypes );
		PyType_Ready( &BPyCurveText_Type ) ;
	}
	return (PyObject *) &BPyCurveText_Type ;
}
