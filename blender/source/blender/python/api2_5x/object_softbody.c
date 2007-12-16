/*
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
 * Contributor(s): Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "object_softbody.h" /* This must come first */
#include "BKE_softbody.h"
#include "gen_utils.h"
#include "BLI_blenlib.h"
/*
 * get integer attributes
 */

enum mat_float_consts {
	EXPP_SOFT_ATTR_NODEMASS = 0,
	EXPP_SOFT_ATTR_GRAV,
	EXPP_SOFT_ATTR_MEDIAFRICT,
	EXPP_SOFT_ATTR_RKLIMIT,
	EXPP_SOFT_ATTR_PHYSICS_SPEED,
	EXPP_SOFT_ATTR_GOALSTRING,
	EXPP_SOFT_ATTR_GOALFRICT,
	EXPP_SOFT_ATTR_MINGOAL,
	EXPP_SOFT_ATTR_MAXGOAL,
	EXPP_SOFT_ATTR_DEFGOAL,
	EXPP_SOFT_ATTR_INSPRING,
	EXPP_SOFT_ATTR_INFRICT,
	
	EXPP_SOFT_ATTR_SECONDSPRING,
	EXPP_SOFT_ATTR_COLBALL,	
	EXPP_SOFT_ATTR_BALLDAMP,
	EXPP_SOFT_ATTR_BALLSTIFF,
};

/* int/short/char */
enum mat_int_consts {
	EXPP_SOFT_ATTR_VERTGROUP = 0,
	EXPP_SOFT_ATTR_FUZZYNESS,
	
	EXPP_SOFT_ATTR_AEROEDGE,
	EXPP_SOFT_ATTR_MINLOOPS,
	EXPP_SOFT_ATTR_MAXLOOPS,
	EXPP_SOFT_ATTR_CHOKE,
};

static int setupSB( Object* ob )
{
	ob->soft= sbNew();
	ob->softflag |= OB_SB_GOAL|OB_SB_EDGES;

	if( !ob->soft ) 	
		return 0;
	
	return 1;
}

static SoftBody * SoftBody_get_pointer( BPySoftBodyObject * self )
{
	if( !self->bpyobject->object->soft && !setupSB(self->bpyobject->object ))
		return (SoftBody *)EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"This Object has no softbody" );
	
	return self->bpyobject->object->soft;
}


static PyObject *SoftBody_getSoftFlag(BPySoftBodyObject *self, void *flag)
{
	Object *ob = self->bpyobject->object;
	if (ob->softflag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int SoftBody_setSoftFlag(BPySoftBodyObject *self, PyObject *value, void *flag)
{
	int param;
	Object *ob = self->bpyobject->object;
	
	param = PyObject_IsTrue(value);
	
	if( param == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected true/false argument" );
	
	if ( param )
		ob->softflag |= (int)flag;
	else
		ob->softflag &= ~(int)flag;
	return 0;
}

static PyObject *SoftBody_getSBCMode(BPySoftBodyObject *self, void *flag)
{
	SoftBody *soft = SoftBody_get_pointer(self);
	if (!soft)
		return NULL; /* error is set */
	if (soft->sbc_mode & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int SoftBody_setSBCMode(BPySoftBodyObject *self, PyObject *value, void *flag)
{
	int param;
	SoftBody *soft = SoftBody_get_pointer(self);
	if (!soft)
		return -1; /* error is set */
	
	param = PyObject_IsTrue(value);
	
	if( param == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected true/false argument" );
	
	if ( param )
		soft->sbc_mode |= (int)flag;
	else
		soft->sbc_mode &= ~(int)flag;
	return 0;
}


/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( BPySoftBodyObject *self, void *type )
{
	float param;
	SoftBody *soft = SoftBody_get_pointer(self);
	if (!soft)
		return NULL; /* error is set */
	
	switch( (int)type ) {
	case EXPP_SOFT_ATTR_NODEMASS: 
		param = soft->nodemass;
		break;
	case EXPP_SOFT_ATTR_GRAV: 
		param = soft->grav;
		break;
	case EXPP_SOFT_ATTR_MEDIAFRICT: 
		param = soft->mediafrict;
		break;
	case EXPP_SOFT_ATTR_RKLIMIT: 
		param = soft->rklimit;
		break;
	case EXPP_SOFT_ATTR_PHYSICS_SPEED: 
		param = soft->physics_speed;
		break;
	case EXPP_SOFT_ATTR_GOALSTRING: 
		param = soft->goalspring;
		break;
	case EXPP_SOFT_ATTR_GOALFRICT: 
		param = soft->goalfrict;
		break;
	case EXPP_SOFT_ATTR_MINGOAL: 
		param = soft->mingoal;
		break;
	case EXPP_SOFT_ATTR_MAXGOAL: 
		param = soft->maxgoal;
		break;
	case EXPP_SOFT_ATTR_DEFGOAL: 
		param = soft->defgoal;
		break;
	case EXPP_SOFT_ATTR_INSPRING: 
		param = soft->inspring;
		break;
	case EXPP_SOFT_ATTR_INFRICT: 
		param = soft->infrict;
		break;
	case EXPP_SOFT_ATTR_SECONDSPRING: 
		param = soft->secondspring;
		break;
	case EXPP_SOFT_ATTR_COLBALL: 
		param = soft->colball;
		break;
	case EXPP_SOFT_ATTR_BALLDAMP: 
		param = soft->balldamp;
		break;
	case EXPP_SOFT_ATTR_BALLSTIFF: 
		param = soft->ballstiff;
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

static int setFloatAttrClamp( BPySoftBodyObject *self, PyObject *value, void *type )
{
	float *param;
	float min, max;

	SoftBody *soft = SoftBody_get_pointer(self);
	if (!soft)
		return -1; /* error is set */
	
	switch( (int)type ) {
	case EXPP_SOFT_ATTR_NODEMASS:
		min= 0.001f; max= 50000.0f;
		param = &soft->nodemass;
		break;
	case EXPP_SOFT_ATTR_GRAV:
		min= 0.0f; max= 10.0f;
		param = &soft->grav;
		break;
	case EXPP_SOFT_ATTR_MEDIAFRICT:
		min= 0.0f; max= 50.0f;
		param = &soft->mediafrict;
		break;
	case EXPP_SOFT_ATTR_RKLIMIT: 
		min= 0.001f; max= 10.0f;
		param = &soft->rklimit;
		break;
	case EXPP_SOFT_ATTR_PHYSICS_SPEED: 
		min= 0.01; max= 100.0;
		param = &soft->physics_speed;
		break;
	case EXPP_SOFT_ATTR_GOALSTRING: 
		min= 0.0; max= 0.999;
		param = &soft->goalspring;
		break;
	case EXPP_SOFT_ATTR_GOALFRICT: 
		min= 0.0f; max= 50.0f;
		param = &soft->goalfrict;
		break;
	case EXPP_SOFT_ATTR_MINGOAL:
		min= 0.0; max= 1.0;
		param = &soft->mingoal;
		break;
	case EXPP_SOFT_ATTR_MAXGOAL:
		min= 0.0; max= 1.0;
		param = &soft->maxgoal;
		break;
	case EXPP_SOFT_ATTR_DEFGOAL: 
		min= 0.0; max= 1.0;
		param = &soft->defgoal;
		break;
	case EXPP_SOFT_ATTR_INSPRING: 
		min= 0.0; max= 0.999;
		param = &soft->inspring;
		break;
	case EXPP_SOFT_ATTR_INFRICT: 
		min= 0.0f; max= 50.0f;
		param = &soft->infrict;
		break;
	case EXPP_SOFT_ATTR_SECONDSPRING: 
		min= 0.0f; max= 10.0f;
		param = &soft->secondspring;
		break;
	case EXPP_SOFT_ATTR_COLBALL: 
		min= -10.0; max= 10.0;
		param = &soft->colball;
		break;
	case EXPP_SOFT_ATTR_BALLDAMP: 
		min= 0.001; max= 1.0;
		param = &soft->balldamp;
		break;
	case EXPP_SOFT_ATTR_BALLSTIFF: 
		min= 0.001; max= 100.0;
		param = &soft->ballstiff;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	self->bpyobject->object->recalc |= OB_RECALC_OB;  
	return EXPP_setFloatClamped( value, param, min, max );
}

static PyObject *getIntAttr( BPySoftBodyObject *self, void *type )
{
	int param;
	SoftBody *soft = SoftBody_get_pointer(self);
	if (!soft)
		return NULL; /* error is set */

	switch( (int)type ) {
	case EXPP_SOFT_ATTR_VERTGROUP:
		param = (int)soft->vertgroup;
		break;
	case EXPP_SOFT_ATTR_FUZZYNESS:
		param = (int)soft->fuzzyness;
		break;
	case EXPP_SOFT_ATTR_AEROEDGE:
		param = (int)soft->aeroedge;
		break;
	case EXPP_SOFT_ATTR_MINLOOPS:
		param = (int)soft->minloops;
		break;
	case EXPP_SOFT_ATTR_MAXLOOPS:
		param = (int)soft->maxloops;
		break;
	case EXPP_SOFT_ATTR_CHOKE:
		param = (int)soft->choke;
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

static int setIntAttrClamp( BPySoftBodyObject *self, PyObject *value, void *type )
{
	void *param;
	int min, max, size;
	
	SoftBody *soft = SoftBody_get_pointer(self);
	if (!soft)
		return -1; /* error is set */
	
	switch( (int)type ) {
	case EXPP_SOFT_ATTR_VERTGROUP:
		min = 0; max = BLI_countlist(&self->bpyobject->object->defbase); size = 'h';
		param = (void *)&soft->vertgroup;
		break;
	case EXPP_SOFT_ATTR_FUZZYNESS:
		min = 1; max = 100; size = 'h';
		param = (void *)&soft->fuzzyness;
		break;
	case EXPP_SOFT_ATTR_AEROEDGE:
		min = 0; max = 30000; size = 'h';
		param = (void *)&soft->aeroedge;
		break;
	case EXPP_SOFT_ATTR_MINLOOPS:
		min = 0; max = 30000; size = 'h';
		param = (void *)&soft->minloops;
		break;
	case EXPP_SOFT_ATTR_MAXLOOPS:
		min = 0; max = 30000; size = 'h';
		param = (void *)&soft->maxloops;
		break;
	case EXPP_SOFT_ATTR_CHOKE:
		min = 0; max = 100; size = 'h';
		param = (void *)&soft->choke;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}

	self->bpyobject->object->recalc |= OB_RECALC_OB;  
	return EXPP_setIValueClamped( value, param, min, max, size );
}

/*****************************************************************************/
/* PythonTypeObject callback function prototypes			 */
/*****************************************************************************/
static PyObject *SoftBody_repr( BPySoftBodyObject * obj );
static int SoftBody_compare( BPySoftBodyObject * a, BPySoftBodyObject * b );


/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/

static PyGetSetDef BPySoftBody_getset[] = {
	/* flags */
	{"enableSoftBody",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_ENABLE },
	{"enableGoal",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_GOAL },
	{"enableEdges",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_EDGES },
	{"enableStiffQuads",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_QUADS},
	/*
	 * Internal use flags
	 * 
	 * OB_SB_POSTDEF*/
	/*OB_SB_REDO*/
	/*OB_SB_BAKESET*/
	/*OB_SB_BAKEDO*/
	{"enableCollisionSelf",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_RESET },
	{"enableCollisionSelf",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_SELF },
	{"enableCollisionFace",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_FACECOLL },
	{"enableCollisionEdge",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_EDGECOLL },
	{"enableCollisionFinal",
	 (getter)SoftBody_getSoftFlag, (setter)SoftBody_setSoftFlag, "",
	 (void *)OB_SB_COLLFINAL },
	
	/* These values should be in DNA were copied from buttons_object.c*/
	{"enableCollisionManual",
	 (getter)SoftBody_getSBCMode, (setter)SoftBody_setSBCMode, "",
	 (void *)0 },
	{"enableCollisionAverage",
	 (getter)SoftBody_getSBCMode, (setter)SoftBody_setSBCMode, "",
	 (void *)1 },
	{"enableCollisionMin",
	 (getter)SoftBody_getSBCMode, (setter)SoftBody_setSBCMode, "",
	 (void *)2 },
	{"enableCollisionMax",
	 (getter)SoftBody_getSBCMode, (setter)SoftBody_setSBCMode, "",
	 (void *)3 },
	{"enableCollisionAverageMinMax",
	 (getter)SoftBody_getSBCMode, (setter)SoftBody_setSBCMode, "",
	 (void *)4 },
	 
	/* float settings */
	{"mass",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_NODEMASS },
	{"gravity",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_GRAV },
	{"friction",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_MEDIAFRICT },
	{"errorLimit",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_RKLIMIT },
	{"speed",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_PHYSICS_SPEED },
	{"goalStiff",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_GOALFRICT },
	{"goalDamp",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_GOALSTRING },
	{"goalMin",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_MINGOAL },
	{"goalMax",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_MAXGOAL },
	{"goal",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_DEFGOAL },
	 
	 /* int/short/char */
	{"vertGroup", /* TODO - set as a string! */
	 (getter)getIntAttr, (setter)setIntAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_VERTGROUP },
	{"fuzzyness",
	 (getter)getIntAttr, (setter)setIntAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_FUZZYNESS },
	{"aero",
	 (getter)getIntAttr, (setter)setIntAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_AEROEDGE },
	{"loopsMin",
	 (getter)getIntAttr, (setter)setIntAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_MINLOOPS },
	{"loopsMax",
	 (getter)getIntAttr, (setter)setIntAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_MAXLOOPS },
	{"choke",
	 (getter)getIntAttr, (setter)setIntAttrClamp, "",
	 (void *)EXPP_SOFT_ATTR_CHOKE },
	{NULL}  /* Sentinel */
};

PyObject *SoftBody_CreatePyObject( BPyObject * bpyobject )
{
	BPySoftBodyObject *bpysoft;
	
	if( !bpyobject->object->soft && !setupSB(bpyobject->object) )
		Py_RETURN_NONE;
	
	bpysoft =
		( BPySoftBodyObject * ) PyObject_NEW( BPySoftBodyObject, &BPySoftBody_Type );

	if( bpysoft == NULL ) {
		return NULL;
	}
	bpysoft->bpyobject = bpyobject;
	Py_INCREF(bpyobject);
	return ( PyObject * ) bpysoft;
}

static void SoftBody_dealloc( BPySoftBodyObject * self )
{
	Py_DECREF(self->bpyobject);
	PyObject_DEL( self );
}

static int SoftBody_compare( BPySoftBodyObject * a, BPySoftBodyObject * b )
{
	Object *pa = a->bpyobject->object, *pb = b->bpyobject->object;
	return ( pa == pb ) ? 0 : -1;
}

static PyObject *SoftBody_repr( BPySoftBodyObject * self )
{
	return PyString_FromFormat( "[SoftBody \"%s\"]",
				    self->bpyobject->object->id.name + 2 );
}


/*****************************************************************************/
/* Python TypeGroup structure definition:                                     */
/*****************************************************************************/
PyTypeObject BPySoftBody_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender SoftBody",             /* char *tp_name; */
	sizeof( BPySoftBodyObject ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) SoftBody_dealloc, /* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) SoftBody_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) SoftBody_repr,     /* reprfunc tp_repr; */

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

	NULL,						/*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,						/* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,						/* inquiry tp_clear; */

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
	BPySoftBody_getset,         /* struct PyGetSetDef *tp_getset; */
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

PyObject *SoftBodyType_Init( void )
{
	PyType_Ready( &BPySoftBody_Type );
	return (PyObject *) &BPySoftBody_Type;
}
