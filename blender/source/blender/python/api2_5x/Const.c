/* 
 * $Id: constant.c,v 1.22 2006/12/26 21:12:34 khughes Exp $
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
 * Contributor(s): Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Const.h" /*This must come first */

#include "gen_utils.h"
#include "gen_library.h"

/*
 * Creates a new constant object
 */

static BPyconst *new_const( constDefinition *defn, PyTypeObject *type )
{
	BPyconst *constant;

	constant = (BPyconst *) PyObject_NEW( BPyconst, type );

	if( constant == NULL )
		return (BPyconst *)EXPP_ReturnPyObjError(PyExc_MemoryError,
				"couldn't create Const object");

	constant->defn = defn;
	return constant;
}

/*------------------TYPE_OBJECT IMPLEMENTATION--------------------------*/
static PyObject *ConstCategory_getAttro(BPyconst *self, PyObject *value)
{
	char *name = PyString_AS_STRING( value );
	unsigned int count= self->defn->tot_members;
	constIdents *ptr;

	if(!strcmp(name, "__members__")) {

		/* return a list containing all the constant names */

		PyObject* list;
		list = PyList_New( count );
		if( !list )
			return EXPP_ReturnPyObjError( PyExc_MemoryError,
					"unable to create list" );
		for( ptr= self->defn->members; count; ++ptr )
			PyList_SET_ITEM( list, --count,
					PyString_FromString( ptr->repr ) );

#ifdef CONST_BITFIELDS
		/* For bitfield constant categories, add pseudo-constants for
		 * for "no bits on" and "all bits on" */
		if( self->utype == EXPP_CONST_BIT ) {
			PyObject *extra = PyString_FromString( "NULL" );
			PyList_Append( list, extra ); Py_DECREF( extra );
			extra = PyString_FromString( "ALL" );
			PyList_Append( list, extra ); Py_DECREF( extra );
		}
#endif
		return list;
	} else {
#ifdef CONST_BITFIELDS
	/* for bitfields, check for special pseudo-constants */
		if( self->utype == EXPP_CONST_BIT ) {
			/* if "no bits on", return empty bit field */
			if( !strcmp(name, "NULL") )
				return PyConst_NewBit( self->defn, (long)0 );
			/* if "all bits on", return full bit field */
			else if( !strcmp(name, "ALL") ) {
				long bits= 0;
				for( ptr= self->defn->members; count; ++ptr, --count )
					bits |= ptr->value.b;
				return PyConst_NewBit( self->defn, bits );
			}
		}
#endif

	/* search for the constant by name, and return a new object */

		for( ptr= self->defn->members; count; ++ptr, --count ) {
			if( *name == *ptr->repr && !strcmp( name, ptr->repr ) )
				switch ( self->utype ) {
				case EXPP_CONST_INT:
					return PyConst_NewInt( self->defn, ptr->value.i );
				case EXPP_CONST_FLOAT:
					return PyConst_NewFloat( self->defn, ptr->value.f );

#ifdef CONST_BITFIELDS
				case EXPP_CONST_BIT:
					return PyConst_NewBit( self->defn, ptr->value.b );
#endif
				}
		}
	}

	/* otherwise pass the argument along */
	return PyObject_GenericGetAttr( (PyObject *)self, value );
}

/*------------------------tp_repr------------------------*/
static PyObject *Const_repr(BPyconst * self)
{
	switch( self->utype ) {

		/* for ints and floats, just return the constant category name */
	case EXPP_CONST_INT:
	case EXPP_CONST_FLOAT:
		{
			unsigned int count= self->defn->tot_members;
			constIdents *ptr;
			for( ptr= self->defn->members; count; ++ptr, --count ) {
				if( self->value.i == ptr->value.i )
					return PyString_FromFormat( "[Const \"%s\" (%s)]",
						self->defn->name, ptr->repr );
			}
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"unknown constant value" );
		}

#ifdef CONST_BITFIELDS
		/* for bitfields, return a string representing the bit settings */
	case EXPP_CONST_BIT:
		{
			char values[1024], *sptr= values;
			constIdents *ptr;
			unsigned int count= self->defn->tot_members;
			*sptr = 0;
			for( ptr= self->defn->members; count; ++ptr, --count ) {
				if( self->value.i & ptr->value.i ) {
					if( sptr != values )
						*sptr++ = ',';
					strcpy( sptr, ptr->repr );
					sptr += strlen( ptr->repr );
				}
			}
			return PyString_FromFormat( "[Const (%s), \"%s\"]",
					values, self->defn->name );
		}
#endif
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"unknown constant type" );
	}
}

static PyObject *ConstCategory_repr(BPyconst * self)
{
	char values[1024], *sptr= values;
	constIdents *ptr;
	unsigned int count= self->defn->tot_members;
	*sptr = 0;
	switch( self->utype ) {
		/* all categories just return the category name also */
	case EXPP_CONST_INT:
	case EXPP_CONST_FLOAT:
		for( ptr= self->defn->members; count; ++ptr, --count ) {
			if( sptr != values )
				*sptr++ = ',';
			strcpy( sptr, ptr->repr );
			sptr += strlen( ptr->repr );
		}
		break;
#ifdef CONST_BITFIELDS
	case EXPP_CONST_BIT:
		for( ptr= self->defn->members; count; ++ptr, --count ) {
			if( self->value.i & ptr->value.i ) {
				if( sptr != values )
					*sptr++ = ',';
				strcpy( sptr, ptr->repr );
				sptr += strlen( ptr->repr );
			}
		}
		break;
#endif
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"unknown constant type" );
	}
	return PyString_FromFormat( "[ConstCategory \"%s\" (%s)]",
			self->defn->name, values );
}

/*------------------PYNUMBERMETHOD IMPLEMENTATION--------------------------*/

#ifdef CONST_BITFIELDS
/*
 * ADD/OR/UNION of bit constants
 */

static PyObject *Const_union(PyObject * v1, PyObject * v2)
{
	BPyconst *c1 = (BPyconst *)v1;
	BPyconst *c2 = (BPyconst *)v2;
	BPyconst *c3 = NULL;
	
	/* inputs must be bit constants of the same type */
	if( !BPyConst_Check(c1) || !BPyConst_Check(c2) || c1->defn != c2->defn )
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"can only combine two constants of the same type" );

	if( c1->utype != EXPP_CONST_BIT )
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"constants must be bitset type" );

	c3 = new_const( c1->defn, &Const_Type );
	c3->utype = EXPP_CONST_BIT;
	c3->value.b = c1->value.b | c2->value.b;
	return (PyObject *)c3;
}

/*
 * MULT/AND/INTERSECTION of bit constants
 */

static PyObject *Const_intersect(PyObject * v1, PyObject * v2)
{
	BPyconst *c1 = (BPyconst *)v1;
	BPyconst *c2 = (BPyconst *)v2;
	BPyconst *c3 = NULL;
	
	/* inputs must be bit constants of the same type */
	if( !BPyConst_Check(c1) || !BPyConst_Check(c2) || c1->defn != c2->defn )
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"can only add combine constants of the same type" );

	if( c1->utype != EXPP_CONST_BIT )
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"constants must be bitset type" );

	c3 = new_const( c1->defn, &Const_Type );
	c3->utype = EXPP_CONST_BIT;
	c3->value.b = c1->value.b & c2->value.b;
	return (PyObject *)c3;
}

/*
 * SUB of bit constants; remove bit(s) specified in v2 from v1
 */

static PyObject *Const_sub(PyObject * v1, PyObject * v2)
{
	BPyconst *c1 = (BPyconst *)v1;
	BPyconst *c2 = (BPyconst *)v2;
	BPyconst *c3 = NULL;
	
	/* inputs must be bit constants of the same type */
	if( !BPyConst_Check(c1) || !BPyConst_Check(c2) || c1->defn != c2->defn )
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"can only add combine constants of the same type" );

	if( c1->utype != EXPP_CONST_BIT )
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"constants must be bitset type" );

	c3 = new_const( c1->defn, &Const_Type );
	c3->utype = EXPP_CONST_BIT;
	c3->value.b = c1->value.b & ~c2->value.b;
	return (PyObject *)c3;
}

static PyNumberMethods Const_NumMethods = {
	(binaryfunc) Const_union,					/* __add__ */
	(binaryfunc) Const_sub,						/* __sub__ */
	(binaryfunc) Const_intersect,				/* __mul__ */
	(binaryfunc) NULL,							/* __div__ */
	(binaryfunc) NULL,							/* __mod__ */
	(binaryfunc) NULL,							/* __divmod__ */
	(ternaryfunc) NULL,							/* __pow__ */
	(unaryfunc) NULL,							/* __neg__ */
	(unaryfunc) NULL,							/* __pos__ */
	(unaryfunc) NULL,							/* __abs__ */
	(inquiry) NULL,								/* __nonzero__ */
	(unaryfunc) NULL,							/* __invert__ */
	(binaryfunc) NULL,							/* __lshift__ */
	(binaryfunc) NULL,							/* __rshift__ */
	(binaryfunc) Const_intersect,				/* __and__ */
	(binaryfunc) NULL,							/* __xor__ */
	(binaryfunc) Const_union,					/* __or__ */
	(coercion)  NULL,							/* __coerce__ */
	(unaryfunc) NULL,							/* __int__ */
	(unaryfunc) NULL,							/* __long__ */
	(unaryfunc) NULL,							/* __float__ */
	(unaryfunc) NULL,							/* __oct__ */
	(unaryfunc) NULL,							/* __hex__ */
	
	/* Added in release 2.0 */
	(binaryfunc) NULL,							/*__iadd__*/
	(binaryfunc) NULL,							/*__isub__*/
	(binaryfunc) NULL,							/*__imul__*/
	(binaryfunc) NULL,							/*__idiv__*/
	(binaryfunc) NULL,							/*__imod__*/
	(ternaryfunc) NULL,							/*__ipow__*/
	(binaryfunc) NULL,							/*__ilshift__*/
	(binaryfunc) NULL,							/*__irshift__*/
	(binaryfunc) NULL,							/*__iand__*/
	(binaryfunc) NULL,							/*__ixor__*/
	(binaryfunc) NULL,							/*__ior__*/
 
	/* Added in release 2.2 */
	/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
	(binaryfunc) NULL,							/*__floordiv__  __rfloordiv__*/
	(binaryfunc) NULL,							/*__truediv__ __rfloordiv__*/
	(binaryfunc) NULL,							/*__ifloordiv__*/
	(binaryfunc) NULL,							/*__itruediv__*/
};
#endif

/*-----------------TYPE_OBJECT DEFINITION------------------------------*/

/*
 * Constant type
 */
PyTypeObject Const_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Const",            /* char *tp_name; */
	sizeof( BPyconst ),        /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) Const_repr,    /* reprfunc tp_repr; */

	/* Method suites for standard classes */

#ifdef CONST_BITFIELDS
	&Const_NumMethods,          /* PyNumberMethods *tp_as_number; */
#else
	NULL,                       /* PyNumberMethods *tp_as_number; */
#endif
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
	NULL,                       /* struct PyMethodDef *tp_methods; */
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

/*
 * Constant category type
 */
PyTypeObject ConstCategory_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender ConstCategory",            /* char *tp_name; */
	sizeof( BPyconst ),        /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) ConstCategory_repr,    /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,                       /* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	(getattrofunc)ConstCategory_getAttro, /* getattrofunc tp_getattro; */
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
	NULL,                       /* struct PyMethodDef *tp_methods; */
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

/*
 * Build a new integer-type BPyConst
 */

PyObject *PyConst_NewInt( constDefinition *defn, int value )
{
	BPyconst *constant = new_const( defn, &Const_Type );

	if( constant != NULL ) {
		constant->utype = EXPP_CONST_INT;
		constant->value.i = value;
	}
	return (PyObject *)constant;
}

/*
 * Build a new float-type BPyConst
 */

PyObject *PyConst_NewFloat( constDefinition *defn, float value )
{
	BPyconst *constant = new_const( defn, &Const_Type );

	if( constant != NULL ) {
		constant->utype = EXPP_CONST_FLOAT;
		constant->value.f = value;
	}
	return (PyObject *)constant;
}


#ifdef CONST_BITFIELDS
/*
 * Build a new bit-type BPyConst
 */

PyObject *PyConst_NewBit( constDefinition *defn, long value )
{
	BPyconst *constant = new_const( defn, &Const_Type );

	if( constant != NULL ) {
		constant->utype = EXPP_CONST_BIT;
		constant->value.b = value;
	}
	return (PyObject *)constant;
}
#endif

/*
 * Search the constant category members for a string.  Returns non-zero if
 * a match is found, updating *value with the constant's value.
 */

int PyConst_Lookup( constDefinition *defn, char *name, constValue *value )
{
	constIdents *ptr = defn->members;
	unsigned int count= defn->tot_members;

	for( ptr= defn->members; count; ++ptr, --count )
		if( *name == *ptr->repr && !strcmp( name, ptr->repr ) ) {
			*value = ptr->value;
			return 1;
		}
	return 0;
}

/*
 * Return the value from the const weather it is a string or
 * a constant, if its an invalid type or not from the defn
 * it will return NULL
 */ 

constValue * Const_FromPyObject( constDefinition *defn, PyObject *pyob)
{
	constValue *value = NULL;
	if( PyString_Check(pyob) ) {
		PyConst_Lookup( defn, PyString_AsString( pyob ), value );
	} else if ( BPyConstType_Check(defn, (BPyconst *)pyob) ) {
		value = &((BPyconst *)pyob)->value;
	}
	return value;	
}

/*
 * Add a new constant category object to a module.
 */

void PyConstCategory_AddObjectToDict( PyObject *dict,
		constDefinition *defn )
{
	BPyconst *constant = new_const( defn, &ConstCategory_Type );
	if( constant != NULL ) {
		constant->utype = defn->utype;
		PyDict_SetItemString( dict, defn->name, (PyObject *)constant );
	}
}

PyObject * ConstType_Init( void )
{
	/* perform type initialization if it hasn't been done already */
	PyType_Ready( &Const_Type );
	Const_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &Const_Type;
}

PyObject * ConstCategoryType_Init( void )
{
	/* perform type initialization if it hasn't been done already */
	PyType_Ready( &ConstCategory_Type );	
	ConstCategory_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &ConstCategory_Type;
}
