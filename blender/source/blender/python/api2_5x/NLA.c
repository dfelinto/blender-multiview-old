/* 
 * $Id: NLA.c 11924 2007-09-02 21:03:59Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "NLA.h" /*This must come first*/

#include "DNA_curve_types.h"
#include "DNA_scene_types.h"
#include "BKE_action.h"
#include "BKE_utildefines.h"
#include "BKE_nla.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_library.h"
#include "BLI_blenlib.h"
#include "BIF_editaction.h"
#include "Object.h"
#include "Ipo.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "blendef.h"
#include "MEM_guardedalloc.h"
#include "Const.h"

enum mat_float_consts {
	EXPP_NLA_ATTR_START = 0,
	EXPP_NLA_ATTR_END,
	EXPP_NLA_ATTR_ACTSTART,
	EXPP_NLA_ATTR_ACTEND,
	EXPP_NLA_ATTR_ACTOFFS,
	EXPP_NLA_ATTR_STRIDELEN,
	EXPP_NLA_ATTR_REPEAT,
	EXPP_NLA_ATTR_BLENDIN,
	EXPP_NLA_ATTR_BLENDOUT,
};

/* checks for the group being removed */
#define ACTIONSTRIP_DEL_CHECK_PY(actstrip) if (!(actstrip->strip)) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "ActionStrip has been removed" ) )
#define ACTIONSTRIP_DEL_CHECK_INT(actstrip) if (!(actstrip->strip)) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "ActionStrip has been removed" ) )

#define ACTSTRIP_STRIDEAXIS_X         0
#define ACTSTRIP_STRIDEAXIS_Y         1
#define ACTSTRIP_STRIDEAXIS_Z         2


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

static constIdents strideAxisIdents[] = {
	{"X",  		{(int)ACTSTRIP_STRIDEAXIS_X}},
	{"Y",		{(int)ACTSTRIP_STRIDEAXIS_Y}},
	{"Z",		{(int)ACTSTRIP_STRIDEAXIS_Z}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition strideAxis = {
	EXPP_CONST_INT, "StrideAxis",
		sizeof(strideAxisIdents)/sizeof(constIdents), strideAxisIdents
};

static constIdents blendModeIdents[] = {
	{"BLEND",	{(int)ACTSTRIPMODE_BLEND}},
	{"ADD",		{(int)ACTSTRIPMODE_ADD}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition blendMode = {
	EXPP_CONST_INT, "BlendMode",
		sizeof(blendModeIdents)/sizeof(constIdents), blendModeIdents
};

/*****************************************************************************/
/* Python BPyActionObject methods declarations:				*/
/*****************************************************************************/
static PyObject *Action_getFrameNumbers(BPyActionObject *self);
static PyObject *Action_renameChannel( BPyActionObject * self, PyObject * args );
static PyObject *Action_setdefault( BPyActionObject * self, PyObject * args );
static PyObject *Action_values( BPyActionObject * self );	/* IPOs */
static PyObject *Action_items( BPyActionObject * self );	/* IPOs */
static PyObject *Action_keys( BPyActionObject * self );		/* channel names */

/*****************************************************************************/
/* Python BPyActionObject methods table:                                          */
/*****************************************************************************/
/* TODO - replace some/all of these with getsets, also review if some should
 * be kept or replaced with more useful funcs */

static PyMethodDef BPyAction_methods[] = {
	/* name, method, flags, doc */
	{"getFrameNumbers", (PyCFunction) Action_getFrameNumbers, METH_NOARGS,
	"() - get the frame numbers at which keys have been inserted"},
	{"keys", ( PyCFunction ) Action_keys, METH_NOARGS,
	 "() - get the channel names for this action"},
	 {"renameChannel", ( PyCFunction ) Action_renameChannel, METH_VARARGS,
		 "(from, to) -rename the channel from string to string"},
	{"setdefault", ( PyCFunction ) Action_setdefault, METH_VARARGS,
	 "(str, ipo) -verify the channel in this action, ipo can be an IPO or None"},
	{"values", ( PyCFunction ) Action_values,  METH_NOARGS,
	 "() - Return a list of ipos"},
	{"items", ( PyCFunction ) Action_items,  METH_NOARGS,
	 "() - Return a list of tuples (name, ipo) - keys containing each channel in the object's action"},
	{NULL, NULL, 0, NULL}
};

/*-------------------------------------------------------------------------*/

static int Action_len( BPyActionObject * self )
{
	return BLI_countlist( &self->action->chanbase );
}

static PyObject *Action_getFrameNumbers(BPyActionObject *self)
{
	bActionChannel *achan = NULL;
	IpoCurve *icu = NULL;
	BezTriple *bezt = NULL;
	int verts;
	PyObject *py_list = NULL;
	
	py_list = PyList_New(0);
	for(achan = self->action->chanbase.first; achan; achan = achan->next){
		if (achan->ipo) {
			for (icu = achan->ipo->curve.first; icu; icu = icu->next){
				bezt= icu->bezt;
				if(bezt) {
					verts = icu->totvert;
					while(verts--) {
						PyObject *value;
						value = PyInt_FromLong((int)bezt->vec[1][0]);
						if ( PySequence_Contains(py_list, value) == 0){
							PyList_Append(py_list, value);
						}
						Py_DECREF(value);
						bezt++;
					}
				}
			}
		}
	}
	PyList_Sort(py_list);
	return EXPP_incr_ret(py_list);
}

static PyObject *Action_subscript( BPyActionObject * self, PyObject * value )
{
	char *chanName = PyString_AsString(value);
	bActionChannel *chan;

	if( !chanName )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				       "string expected" );

	chan = get_action_channel( self->action, chanName );
	if( !chan )
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				       "no channel with that name" );
	
	/* NULL -> None is OK */
	return Ipo_CreatePyObject( chan->ipo );
}

static PyObject *Action_keys( BPyActionObject * self )
{
	PyObject *list = PyList_New( BLI_countlist(&(self->action->chanbase)) );
	bActionChannel *chan = NULL;
	int index=0;
	for( chan = self->action->chanbase.first; chan; chan = chan->next ) {
		PyList_SetItem( list, index, PyString_FromString(chan->name) );
		index++;
	}
	return list;
}

static PyObject *Action_renameChannel( BPyActionObject * self, PyObject * args )
{
	char *chanFrom, *chanTo;
	bActionChannel *chan;

	if( !PyArg_ParseTuple( args, "ss", &chanFrom, &chanTo ) )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				       "2 strings expected" );
	
	chan = get_action_channel( self->action, chanFrom );
	if( !chan )
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"no channel with that name" );
	if (strlen(chanTo) > 31)
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"new name greater then 31 characters long" );
	
	if (get_action_channel( self->action, chanTo ))
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"channel target name alredy exists" );
	
	strcpy(chan->name, chanTo);
	
	Py_RETURN_NONE;
}

	/* was verifyChannel, WIP - if this is copying a dict, setdefault is very close 
	 * Only problem is that there is no PyType for a channel so that needs doing. */

/*----------------------------------------------------------------------*/
static PyObject *Action_setdefault( BPyActionObject * self, PyObject * args )
{
	char *chanName;
	bActionChannel *chan;
	PyObject *value;
	if( !self->action )
		( EXPP_ReturnPyObjError( PyExc_RuntimeError,
					 "couldn't create channel for a NULL action" ) );

	if(	!PyArg_ParseTuple( args, "sO", &chanName, &value ) ||
		!(strlen(chanName) <= 31) ||
		!(BPyIpo_BlockType_Check( value, GS(self->action->id.name )) || (value==Py_None))
	)
		return ( EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected string shorter then 31 chars and a None or IPO arguments" ) );
	
	chan = get_action_channel(self->action, chanName);
	
	if (chan) /* exists, just return the IPO value, NULL is ok */
		return Ipo_CreatePyObject(chan->ipo);
	
	/* does not exist, add it */
	chan = verify_action_channel(self->action, chanName);
	/* we know the new channel wont have an existing IPO user */
	if (value!=Py_None) {
		/* we know its compatible from BPyIpo_BlockType_Check */
		chan->ipo = ((BPyIpo *)value)->ipo; 
		id_us_plus(&chan->ipo->id);
	}
	
	Py_INCREF(value);
	return value;
}

static int Action_ass_sub(BPyActionObject *self, PyObject *v, PyObject *w)
{
	bActionChannel *chan;
	char *chanName = PyString_AsString(v);
	
	if( !chanName || strlen(chanName) > 31)
		return EXPP_ReturnIntError( PyExc_AttributeError,
				"string expected less then 31 chars long" );
	
	chan = get_action_channel( self->action, chanName );
		
	if (w == NULL) { /* del action['channame'] */
		if( !chan )
			return EXPP_ReturnIntError( PyExc_AttributeError,
					"no channel with that name..." );
		
		if( chan->ipo )
			chan->ipo->id.us--;
		BLI_freelinkN( &self->action->chanbase, chan );
		
	} else { /* action['channame'] = ipo */
		if( !chan ) /* add the channel if its not there */
			chan = verify_action_channel(self->action, chanName);
		
		if (w==Py_None) {
			if( chan->ipo )
				chan->ipo->id.us--;
			chan->ipo = NULL;
		} else if ( BPyIpo_BlockType_Check( w, GS(self->action->id.name ) ) ) {
			if( chan->ipo )
				chan->ipo->id.us--;
			chan->ipo = ((BPyIpo *)w)->ipo;
			id_us_plus( &chan->ipo->id );
		} else {
			EXPP_ReturnIntError( PyExc_AttributeError,
					"value must be an IPO or None type..." );			
		}
	}
	return 0;
}


/* TODO - create a channel type rather then just return IPO's */
static PyObject *Action_items( BPyActionObject * self )
{
	PyObject *list = PyList_New( 0 );
	bActionChannel *chan = NULL;

	for( chan = self->action->chanbase.first; chan; chan = chan->next ) {
		PyObject *key = PyString_FromString(chan->name);
		PyObject *val = Ipo_CreatePyObject(chan->ipo);
		PyObject *pair = PyTuple_Pack(2, key, val );
		Py_DECREF(key); Py_DECREF(val);
		PyList_Append(list, pair);
		Py_DECREF(pair);
	}
	return list;
}

static PyObject *Action_values( BPyActionObject * self )
{
	PyObject *list = PyList_New( 0 );
	bActionChannel *chan = NULL;

	for( chan = self->action->chanbase.first; chan; chan = chan->next ) {
		PyObject *val = Ipo_CreatePyObject(chan->ipo);
		PyList_Append(list, val);
		Py_DECREF(val);
	}
	return list;
}

/*----------------------------------------------------------------------*/
PyObject *Action_CreatePyObject( struct bAction * act )
{
	BPyActionObject *blen_action;

	if(!act) Py_RETURN_NONE;

	blen_action =
		( BPyActionObject * ) PyObject_NEW( BPyActionObject, &BPyAction_Type );

	if( !blen_action) {
		return ( EXPP_ReturnPyObjError
			 ( PyExc_RuntimeError, "failure to create object!" ) );
	}
	blen_action->action = act;
	return ( ( PyObject * ) blen_action );
}

/*----------------------------------------------------------------------*/

///* Hack to implement "key in dict" */
//static PySequenceMethods Action_as_sequence = {
//	0,			/* sq_length */
//	0,			/* sq_concat */
//	0,			/* sq_repeat */
//	0,			/* sq_item */
//	0,			/* sq_slice */
//	0,			/* sq_ass_item */
//	0,			/* sq_ass_slice */
//	PyDict_Contains,	/* sq_contains */
//	0,			/* sq_inplace_concat */
//	0,			/* sq_inplace_repeat */
//};

static PyMappingMethods Action_as_mapping = {
	( inquiry ) Action_len,				/* mp_length */
	( binaryfunc ) Action_subscript,	/* mp_subscript */
	( objobjargproc ) Action_ass_sub,	/* mp_ass_subscript */
};

/* this types constructor */
static PyObject *Action_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Action";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_empty_action( name );
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new action data" );
	id->flag |= LIB_FAKEUSER; /* no need to assign a user because alloc_libblock alredy assigns one */
	return Action_CreatePyObject((bAction *)id);
}

/*****************************************************************************/
/* Python TypeAction structure definition:			        				*/
/*****************************************************************************/
PyTypeObject BPyAction_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"Blender Action",	/* tp_name */
	sizeof( BPyActionObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	NULL,	/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	NULL,	/* tp_compare */
	NULL,	/* tp_repr */
	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	&Action_as_mapping,			/* PyMappingMethods *tp_as_mapping; */

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
	BPyAction_methods,			/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	NULL,						/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Action_new,		/* newfunc tp_new; */
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
/* ActionStrip wrapper                                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Python BPyActionStripObject attributes:                                        */
/*****************************************************************************/

/*
 * return the action for the action strip
 */

static PyObject *ActionStrip_getAction( BPyActionStripObject * self )
{
	ACTIONSTRIP_DEL_CHECK_PY(self);
	return Action_CreatePyObject( self->strip->act );
}

/*
 * return the blend mode of the action strip
 */

static PyObject *ActionStrip_getBlendMode( BPyActionStripObject * self )
{
	ACTIONSTRIP_DEL_CHECK_PY(self);
	return PyConst_NewInt( &blendMode, self->strip->mode );
}

/*
 * set the blend mode value of the action strip
 */

static int ActionStrip_setBlendMode( BPyActionStripObject * self, PyObject * value )
{
	constValue *c = Const_FromPyObject( &blendMode, value);
	ACTIONSTRIP_DEL_CHECK_INT(self);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected BlendMode constant or string" );
	
	self->strip->mode = c->i;
	return 0;
	
}

static PyObject *ActionStrip_getFlag( BPyActionStripObject * self, void * flag )
{
	ACTIONSTRIP_DEL_CHECK_PY(self);
	if (self->strip->flag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/*
 * set the flag settings out value of the action strip
 */

static int ActionStrip_setFlag( BPyActionStripObject * self, PyObject * value, void * flag )
{
	ACTIONSTRIP_DEL_CHECK_INT(self);
	
	if ( PyObject_IsTrue(value) )
		self->strip->flag |= (int)flag;
	else
		self->strip->flag &= ~(int)flag;
	return 0;
}

/*
 * return the stride axis of the action strip
 */

static PyObject *ActionStrip_getStrideAxis( BPyActionStripObject * self )
{
	ACTIONSTRIP_DEL_CHECK_PY(self);
	return PyConst_NewInt( &strideAxis, self->strip->stride_axis );
}

/*
 * set the stride axis of the action strip
 */

static int ActionStrip_setStrideAxis( BPyActionStripObject * self, PyObject * value )
{
	constValue *c = Const_FromPyObject( &strideAxis, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected StrideAxis constant or string" );
	
	ACTIONSTRIP_DEL_CHECK_INT(self);
	self->strip->stride_axis = c->i;
	return 0;
}

/*
 * return the stride bone name
 */

static PyObject *ActionStrip_getStrideBone( BPyActionStripObject * self )
{
	ACTIONSTRIP_DEL_CHECK_PY(self);
	return PyString_FromString( self->strip->stridechannel );
}

/*
 * set the stride bone name
 */

static int ActionStrip_setStrideBone( BPyActionStripObject * self, PyObject * attr )
{
	char *name = PyString_AsString( attr );
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError, "expected string arg" );

	ACTIONSTRIP_DEL_CHECK_INT(self);
	BLI_strncpy( self->strip->stridechannel, name, 32 );
	return 0;
}

static PyObject *ActionStrip_getGroupTarget( BPyActionStripObject * self )
{
	ACTIONSTRIP_DEL_CHECK_PY(self);
	/* None is ok */
	return Object_CreatePyObject( self->strip->object );
}

static int ActionStrip_setGroupTarget( BPyActionStripObject * self, PyObject * args )
{
	ACTIONSTRIP_DEL_CHECK_INT(self);

	if( (PyObject *)args == Py_None )
		self->strip->object = NULL;
	else if( BPyObject_Check( args ) )
		self->strip->object = ((BPyObject *)args)->object;
	else
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected an object or None" );
	return 0;
}


/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( BPyActionStripObject *self, void *type )
{
	float param;
	ACTIONSTRIP_DEL_CHECK_PY(self);
	
	switch( (int)type ) {
	case EXPP_NLA_ATTR_START:
		param = self->strip->start;
		break;
	case EXPP_NLA_ATTR_END:
		param = self->strip->end;
		break;
	case EXPP_NLA_ATTR_ACTSTART:
		param = self->strip->actstart;
		break;
	case EXPP_NLA_ATTR_ACTEND:
		param = self->strip->actend;
		break;
	case EXPP_NLA_ATTR_ACTOFFS:
		param = self->strip->actoffs;
		break;
	case EXPP_NLA_ATTR_STRIDELEN:
		param = self->strip->stridelen;
		break;
	case EXPP_NLA_ATTR_REPEAT:
		param = self->strip->repeat;
		break;
	case EXPP_NLA_ATTR_BLENDIN:
		param = self->strip->blendin;
		break;
	case EXPP_NLA_ATTR_BLENDOUT:
		param = self->strip->blendout;
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

static int setFloatAttrClamp( BPyActionStripObject *self, PyObject *value, void *type )
{
	float *param;
	float min, max;
	int ret;
	
	ACTIONSTRIP_DEL_CHECK_INT(self);
	
	switch( (int)type ) {
	case EXPP_NLA_ATTR_START:
		min= -1000.0f; max= self->strip->end-1;
		param = &self->strip->start;
		break;
	case EXPP_NLA_ATTR_END:
		min= self->strip->start+1; max= MAXFRAMEF;
		param = &self->strip->end;
		break;
	case EXPP_NLA_ATTR_ACTSTART:
		min= -1000.0f; max= self->strip->actend-1;
		param = &self->strip->actstart;
		break;
	case EXPP_NLA_ATTR_ACTEND:
		min= self->strip->actstart+1; max= MAXFRAMEF;
		param = &self->strip->actend;
		break;
	case EXPP_NLA_ATTR_ACTOFFS:
		min= -500.0f; max= 500.0f;
		param = &self->strip->actoffs;
		break;
	case EXPP_NLA_ATTR_STRIDELEN:
		min= 0.0001f; max= 1000.0f;
		param = &self->strip->stridelen;
		break;
	case EXPP_NLA_ATTR_REPEAT:
		min= 0.001f; max= 1000.0f;
		param = &self->strip->repeat;
		break;
	case EXPP_NLA_ATTR_BLENDIN:
		min= 0.0f; max=  self->strip->end - self->strip->start;
		param = &self->strip->blendin;
		break;
	case EXPP_NLA_ATTR_BLENDOUT:
		min= 0.0f; max= self->strip->end - self->strip->start;
		param = &self->strip->blendout;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}
	
	ret = EXPP_setFloatClamped( value, param, min, max );
	
	if (ret != -1) {
		switch( (int)type ) {
		case EXPP_NLA_ATTR_START: /* fallthrough */
		case EXPP_NLA_ATTR_END: {
			float max = self->strip->end - self->strip->start;
			if( self->strip->blendin > max )
				self->strip->blendin = max;
			if( self->strip->blendout > max )
				self->strip->blendout = max;
			break; }
		}
	}
	return ret;
}

/*****************************************************************************/
/* Python BPyConstraintObject attributes get/set structure:                       */
/*****************************************************************************/
static PyGetSetDef BPyActionStrip_getset[] = {
	/* custom types */
	{"action",
	(getter)ActionStrip_getAction, (setter)NULL,
	 "Action associated with the strip", NULL},
	{"strideAxis",
	(getter)ActionStrip_getStrideAxis, (setter)ActionStrip_setStrideAxis,
	 "Dominant axis for stride bone", NULL},
	{"strideBone",
	(getter)ActionStrip_getStrideBone, (setter)ActionStrip_setStrideBone,
  	 "Name of Bone used for stride", NULL},
	{"groupTarget",
	(getter)ActionStrip_getGroupTarget, (setter)ActionStrip_setGroupTarget,
	 "Name of target armature within group", NULL},
	{"blendMode",
	(getter)ActionStrip_getBlendMode, (setter)ActionStrip_setBlendMode,
	 "Setting of blending mode", NULL},
	 
	/* float attrs */
	{"stripStart",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Starting frame of the strip", (void *)EXPP_NLA_ATTR_START},
	{"stripEnd",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Ending frame of the strip", (void *)EXPP_NLA_ATTR_END},
	{"actionStart",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Starting frame of the action", (void *)EXPP_NLA_ATTR_ACTSTART},
	{"actionEnd",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Ending frame of the action", (void *)EXPP_NLA_ATTR_ACTEND},
	{"actionOffset",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Action offset in frames to tweak cycle of the action within the stride", (void *)EXPP_NLA_ATTR_ACTOFFS},
	{"strideLength",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
  	 "Distance covered by one complete cycle of the action", (void *)EXPP_NLA_ATTR_STRIDELEN}, 
	{"repeat",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The number of times to repeat the action range", (void *)EXPP_NLA_ATTR_REPEAT},
	{"blendIn",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Number of frames of motion blending", (void *)EXPP_NLA_ATTR_BLENDIN},
	{"blendOut",
	(getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Number of frames of ease-out", (void *)EXPP_NLA_ATTR_BLENDOUT},
	
	/* flags */
	{"sel",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_SELECT},
	{"enableStride",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_USESTRIDE},
	{"enableHold",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_HOLDLASTFRAME},
	 /* ACTSTRIP_BLENDTONEXT - not used */
	{"enableHold",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_USESTRIDE},
	 /* ACTSTRIP_ACTIVE - can only be one like the active object, look into setting the active */
	{"enableLock",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_LOCK_ACTION},	 
	{"enableMute",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_MUTE},
	 /* ACTSTRIP_REVERSE - not used */	 
	{"enableCyclicX",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_CYCLIC_USEX},
	{"enableCyclicY",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_CYCLIC_USEY},
	{"enableCyclicZ",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_CYCLIC_USEZ},
	{"enableAutoBlend",
	(getter)ActionStrip_getFlag, (setter)ActionStrip_setFlag,
	 "", (void *)ACTSTRIP_AUTO_BLENDS},	
	{NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python BPyActionStripObject methods:                                           */
/*****************************************************************************/

/*
 * restore the values of ActionStart and ActionEnd to their defaults
 */

static PyObject *ActionStrip_resetLimits( BPyActionStripObject *self )
{
	bActionStrip *strip = self->strip;

	if( !strip )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"This strip has been removed!" );
	
	calc_action_range( strip->act, &strip->actstart, &strip->actend, 1 );
	Py_RETURN_NONE;
}

/*
 * reset the strip size
 */

static PyObject *ActionStrip_resetStripSize( BPyActionStripObject *self )
{
	float mapping;
	bActionStrip *strip = self->strip;

	if( !strip )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"This strip has been removed!" );
	
	mapping = (strip->actend - strip->actstart) / (strip->end - strip->start);
	strip->end = strip->start + mapping*(strip->end - strip->start);

	Py_RETURN_NONE;
}

/*
 * snap to start and end to nearest frames
 */

static PyObject *ActionStrip_snapToFrame( BPyActionStripObject *self )
{
	bActionStrip *strip = self->strip;

	if( !strip )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"This strip has been removed!" );
	
	strip->start= (float)floor(strip->start+0.5);
	strip->end= (float)floor(strip->end+0.5);

	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Python BPyActionStripObject methods table:                                    */
/*****************************************************************************/
static PyMethodDef BPyActionStrip_methods[] = {
	/* name, method, flags, doc */
	{"resetActionLimits", ( PyCFunction ) ActionStrip_resetLimits, METH_NOARGS,
	 "Restores the values of ActionStart and ActionEnd to their defaults"},
	{"resetStripSize", ( PyCFunction ) ActionStrip_resetStripSize, METH_NOARGS,
	 "Resets the Action Strip size to its creation values"},
	{"snapToFrame", ( PyCFunction ) ActionStrip_snapToFrame, METH_NOARGS,
	 "Snaps the ends of the action strip to the nearest whole numbered frame"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyActionStrip_Type structure definition:                             */
/*****************************************************************************/
PyTypeObject BPyActionStrip_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender.ActionStrip",     /* char *tp_name; */
	sizeof( BPyActionStripObject ), /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) NULL,          /* reprfunc tp_repr; */

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
	NULL,                        /* getiterfunc tp_iter; */
    NULL,                        /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyActionStrip_methods,    /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyActionStrip_getset,		/* struct PyGetSetDef *tp_getset; */
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

PyObject *ActionStrip_CreatePyObject( struct bActionStrip *strip )
{
	BPyActionStripObject *pyobj;
	
	if (!strip)
		Py_RETURN_NONE;
	
	pyobj = ( BPyActionStripObject * ) PyObject_NEW( BPyActionStripObject,
			&BPyActionStrip_Type );
	if( !pyobj )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyActionStripObject object" );
	pyobj->strip = strip;
	return ( PyObject * ) pyobj;
}

/*****************************************************************************/
/* ActionStrip Sequence wrapper                                              */
/*****************************************************************************/

/*
 * Initialize the iterator
 */

static PyObject *ActionStripSeq_getIter( BPyActionStripSeqObject * self )
{
	self->iter = (bActionStrip *)self->ob->nlastrips.first;
	return EXPP_incr_ret ( (PyObject *) self );
}

/*
 * Get the next action strip
 */

static PyObject *ActionStripSeq_nextIter( BPyActionStripSeqObject * self )
{
	bActionStrip *strip = self->iter;
	if( strip ) {
		self->iter = strip->next;
		return ActionStrip_CreatePyObject( strip );
	}

	return EXPP_ReturnPyObjError( PyExc_StopIteration,
			"iterator at end" );
}

/* return the number of action strips */

static int ActionStripSeq_length( BPyActionStripSeqObject * self )
{
	return BLI_countlist( &self->ob->nlastrips );
}

/* return an action strip */

static PyObject *ActionStripSeq_item( BPyActionStripSeqObject * self, int i )
{
	bActionStrip *strip = NULL;

	/* if index is negative, start counting from the end of the list */
	if( i < 0 )
		i += ActionStripSeq_length( self );

	/* skip through the list until we get the strip or end of list */

	strip = self->ob->nlastrips.first;

	while( i && strip ) {
		--i;
		strip = strip->next;
	}

	if( strip )
		return ActionStrip_CreatePyObject( strip );
	else
		return EXPP_ReturnPyObjError( PyExc_IndexError,
				"array index out of range" );
}

/*****************************************************************************/
/* Python BPyActionStripSeqObject sequence table:                                  */
/*****************************************************************************/
static PySequenceMethods ActionStripSeq_as_sequence = {
	( inquiry ) ActionStripSeq_length,	/* sq_length */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) ActionStripSeq_item,	/* sq_item */
	( intintargfunc ) 0,	/* sq_slice */
	( intobjargproc ) 0,	/* sq_ass_item */
	( intintobjargproc ) 0,	/* sq_ass_slice */
	( objobjproc ) 0,	/* sq_contains */
	( binaryfunc ) 0,		/* sq_inplace_concat */
	( intargfunc ) 0,		/* sq_inplace_repeat */
};


/*****************************************************************************/
/* Python BPyActionStripObject methods:                                           */
/*****************************************************************************/

/*
 * helper function to check for a valid action strip argument
 */

static bActionStrip *locate_strip( BPyActionStripSeqObject *self, 
		PyObject *args, BPyActionStripObject **stripobj )
{
	bActionStrip *strip = NULL;
	BPyActionStripObject *pyobj;

	/* check that argument is a constraint */
	if( !PyArg_ParseTuple( args, "O!", &BPyActionStrip_Type, &pyobj ) ) {
		EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected an action strip as an argument" );
		return NULL;
	}

	if( !pyobj->strip ) {
		EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"This strip has been removed!" );
		return NULL;
	}

	/* if caller needs the object, return it */
	if( stripobj )
		*stripobj = pyobj;

	/* find the action strip in the NLA */
	for( strip = self->ob->nlastrips.first; strip; strip = strip->next )
		if( strip == pyobj->strip )
			return strip;

	/* return exception if we can't find the strip */
	EXPP_ReturnPyObjError( PyExc_AttributeError,
			"action strip does not belong to this object" );
	return NULL;
}

/*
 * remove an action strip from the NLA
 */

static PyObject *ActionStripSeq_remove( BPyActionStripSeqObject *self, PyObject * args )
{
	BPyActionStripObject *pyobj;
	bActionStrip *strip = locate_strip( self, args, &pyobj );

	/* return exception if we can't find the strip */
	if( !strip )
		return (PyObject *)NULL;

	/* do the actual removal */
	free_actionstrip(strip);
	BLI_remlink(&self->ob->nlastrips, strip);
	MEM_freeN(strip);

	pyobj->strip = NULL;
	Py_RETURN_NONE;
}

/*
 * move an action strip up in the strip list
 */

static PyObject *ActionStripSeq_moveUp( BPyActionStripSeqObject *self, PyObject * args )
{
	bActionStrip *strip = locate_strip( self, args, NULL );

	/* return exception if we can't find the strip */
	if( !strip )
		return (PyObject *)NULL;

	/* if strip is not already the first, move it up */
	if( strip != self->ob->nlastrips.first ) {
		BLI_remlink(&self->ob->nlastrips, strip);
		BLI_insertlink(&self->ob->nlastrips, strip->prev->prev, strip);
	}

	Py_RETURN_NONE;
}

/*
 * move an action strip down in the strip list
 */

static PyObject *ActionStripSeq_moveDown( BPyActionStripSeqObject *self, PyObject * args )
{
	bActionStrip *strip = locate_strip( self, args, NULL );

	/* return exception if we can't find the strip */
	if( !strip )
		return (PyObject *)NULL;

	/* if strip is not already the last, move it down */
	if( strip != self->ob->nlastrips.last ) {
		BLI_remlink(&self->ob->nlastrips, strip);
		BLI_insertlink(&self->ob->nlastrips, strip->next, strip);
	}

	Py_RETURN_NONE;
}

static PyObject *ActionStripSeq_append( BPyActionStripSeqObject *self, PyObject * args )
{
	BPyActionObject *pyobj;
	Object *ob;
	bActionStrip *strip;
	bAction *act;

	/* check that argument is an action */
	if( !PyArg_ParseTuple( args, "O!", &BPyAction_Type, &pyobj ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected an action as an argument" );

	ob = self->ob;
	act = pyobj->action;

	/* Initialize the new action block */
	strip = MEM_callocN( sizeof(bActionStrip), "bActionStrip" );

    strip->act = act;
    calc_action_range( strip->act, &strip->actstart, &strip->actend, 1 );
    strip->start = (float)G.scene->r.cfra;
    strip->end = strip->start + ( strip->actend - strip->actstart );
        /* simple prevention of zero strips */
    if( strip->start > strip->end-2 )
        strip->end = strip->start+100;

    strip->flag = ACTSTRIP_LOCK_ACTION;
    find_stridechannel(ob, strip);

    strip->repeat = 1.0;
    act->id.us++;

    BLI_addtail(&ob->nlastrips, strip);

	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Python BPyActionStripSeqObject methods table:                                    */
/*****************************************************************************/
static PyMethodDef BPyActionStripSeq_methods[] = {
	/* name, method, flags, doc */
	{"append", ( PyCFunction ) ActionStripSeq_append, METH_VARARGS,
	 "(action) - append a new actionstrip using existing action"},
	{"remove", ( PyCFunction ) ActionStripSeq_remove, METH_VARARGS,
	 "(strip) - remove an existing strip from this actionstrips"},
	{"moveUp", ( PyCFunction ) ActionStripSeq_moveUp, METH_VARARGS,
	 "(strip) - move an existing strip up in the actionstrips"},
	{"moveDown", ( PyCFunction ) ActionStripSeq_moveDown, METH_VARARGS,
	 "(strip) - move an existing strip down in the actionstrips"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyActionStripSeq_Type structure definition:                            */
/*****************************************************************************/
PyTypeObject BPyActionStripSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender.ActionStripSeq",     /* char *tp_name; */
	sizeof( BPyActionStripSeqObject ), /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,						/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) NULL,          /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&ActionStripSeq_as_sequence,  /* PySequenceMethods *tp_as_sequence; */
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
	( getiterfunc )ActionStripSeq_getIter, /* getiterfunc tp_iter; */
    ( iternextfunc )ActionStripSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyActionStripSeq_methods,   /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	NULL,                       /* struct PyGetSetDef *tp_getset; */
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

PyObject *ActionStripSeq_CreatePyObject( Object *ob )
{
	BPyActionStripSeqObject *pyseq;
	
	if (!ob)
		Py_RETURN_NONE;
	
	pyseq = ( BPyActionStripSeqObject * ) PyObject_NEW( BPyActionStripSeqObject,
			&BPyActionStripSeq_Type );
	if( !pyseq )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyActionStripSeqObject object" );
	pyseq->ob = ob;
	return ( PyObject * ) pyseq;
}

PyObject *ActionType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyAction_Type.tp_dict == NULL ) {
		PyType_Ready( &BPyAction_Type ) ;
	}
	return (PyObject *) &BPyAction_Type ;
}

PyObject *ActionStripType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyActionStrip_Type.tp_dict == NULL ) {
		BPyActionStrip_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyActionStrip_Type.tp_dict, &strideAxis );
		PyConstCategory_AddObjectToDict( BPyActionStrip_Type.tp_dict, &blendMode );
		PyType_Ready( &BPyActionStrip_Type ) ;
		BPyActionStrip_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyActionStrip_Type ;
}

PyObject *ActionStripSeqType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyActionStripSeq_Type.tp_dict == NULL ) {
		PyType_Ready( &BPyActionStripSeq_Type ) ;
	}
	return (PyObject *) &BPyActionStripSeq_Type ;
}

/*****************************************************************************/
/* Function:    NLA_Init                                                     */
/*****************************************************************************/
void NLA_Init( void )
{
	ActionType_Init();
	ActionStripType_Init();
	ActionStripSeqType_Init();
}
