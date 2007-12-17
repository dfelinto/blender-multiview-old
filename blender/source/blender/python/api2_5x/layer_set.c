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

#include "layer_set.h" /* This must come first */
#include "MEM_guardedalloc.h"
#include "blendef.h"
#include "gen_utils.h"
#include "DNA_object_types.h"
#include "DNA_group_types.h"
#include "DNA_scene_types.h"
#include "Material.h"
#include "BKE_material.h"
#include "BKE_utildefines.h"
#include "BKE_global.h"
#include "BKE_depsgraph.h"
#include "BSE_edit.h"

#define LAYSEQ_COMPAT_INT(i) (i>=1 && i<=20)
#define LAYSEQ_COMPAT_INT_BLEN(i) (i>=0 && i<=19)

/* can the layers be empty? */
#define LAYSEQ_EMPTY_SUPPORT(genlib) ((!genlib) || (genlib && genlib->id && (GS(genlib->id->name) == ID_GR)))

/* used for removing */
#define LAYSEQ_RET_NULLCHECK(_self, _ret) \
	if (_ret) {\
		if (PySet_Size((PyObject *)_self)==0 && !LAYSEQ_EMPTY_SUPPORT(_self->genlib)) {\
			sync_set_from_layer__internal(_self);\
			return ( EXPP_ReturnPyObjError( PyExc_ValueError,\
					"Cannot discard the last layer from an object or scene" ) );\
		} else {\
			sync_layer_from_set__internal(_self);\
		}\
	}\
	return ret;

/* This is used to return from the value from an internal
 * python set function.
 * 
 * Only use this in a PyC function that returns a (PyObject *)
 * 
 * If _ret is NULL it means that the there was an error.
 * 
 * If sync_layer_from_set__internal is zero it means that the set was not compatible
 * with layers.
 *  
 * This checks the set before returning,
 * raises an error and reverts if the set
 * contains ints that cant be used as layers */
#define LAYSEQ_RET_ERRORCHECK(_self, _ret) \
	if (!_ret || !sync_layer_from_set__internal(_self))\
			return ( EXPP_ReturnPyObjError( PyExc_ValueError,\
					"Unsupported values for a layer in this set." ) );\
	return _ret;


/* store these methods so we dont have to lookup the list methods each time */
PyObject * CONST_PySet_add;
PyObject * CONST_PySet_clear;
PyObject * CONST_PySet_contains;
PyObject * CONST_PySet_copy;
PyObject * CONST_PySet_discard;
PyObject * CONST_PySet_difference;
PyObject * CONST_PySet_difference_update;
PyObject * CONST_PySet_intersection;
PyObject * CONST_PySet_intersection_update;
PyObject * CONST_PySet_issubset;
PyObject * CONST_PySet_issuperset;
PyObject * CONST_PySet_pop;
PyObject * CONST_PySet_reduce;
PyObject * CONST_PySet_remove;
PyObject * CONST_PySet_symmetric_difference;
PyObject * CONST_PySet_symmetric_difference_update;
PyObject * CONST_PySet_union;
PyObject * CONST_PySet_update;

void id_set_layer(ID *id, int lay) {
	/* only care about types that can have materials */
	switch ( GS(id->name) ) {
	case ID_OB:
	{
		Base *base;
		int local;
		/* update any bases pointing to our object */
		for(base=FIRSTBASE; base; base=base->next) {
			if( base->object == (Object *)id ) {
				base->lay &= 0xFFF00000;
				local = base->lay;
				base->lay = local | lay;
				((Object *)id)->lay = base->lay;
				break;
			}
		}
		
		/* these to calls here are overkill! (ton) */
		if (base) { /* was the object in the current scene? */
			countall();
			DAG_scene_sort( G.scene );
		}
		break;
	}
	case ID_GR:
		((Group *)id)->layer = lay;
		break;
	case ID_SCE:
		((Scene *)id)->lay = lay;
		break;
	}
}

int layer_from_id( ID *id )
{
	/* only care about types that can have materials */
	switch ( GS(id->name) ) {
	case ID_OB:
		return ((Object *)id)->lay;
	case ID_GR:
		return ((Group *)id)->layer;
	case ID_SCE:
		return ((Scene *)id)->lay;
	}
	return -1;
}

static void sync_set_from_layer__internal(BPyLayerSetObject *self)
{
	int bit, layer= 0;

	PyObject *pyob;
	ID *id;
	if (!self->genlib || !self->genlib->id)
		return;
	
	id = self->genlib->id;
	layer = layer_from_id(self->genlib->id);
	
	PySet_Clear((PyObject *)self); /* TODO - should check if the set needs changing before rebuilding */
	
	
	for(bit=0; bit<20; bit++) {
		if (layer & (1<<bit)) {
			pyob = PyInt_FromLong( (int)bit+1 ); /* add one so python sees layers as from 1 to 20 */
			PySet_Add((PyObject *)self, pyob);
			Py_DECREF(pyob);
		}
	}
}

/* return 0 on unsupported set keys, 1 when it works */
static int sync_layer_from_set__internal(BPyLayerSetObject *self)
{
	PyObject *item, *iter;
	ID *id;
	int bit, layer = 0;
	
	if (!self->genlib)
		return 1; /* not wrapped so this is ok */
	
	id = self->genlib->id;
	
	if (!id)
		return 0;
	
	/* we cant use PyObject_GetIter because that runs
	 * our own wrapped iter function and 
	 * overwrites this set from the layer */
	/*iter = PyObject_GetIter((PyObject *)self);*/
	iter = PySet_Type.tp_iter((PyObject *)self);
	
	while ((item = PyIter_Next(iter)) != NULL) {
		
		/* type checking is alredy been done! */
		if (!PyInt_Check(item)) {
			Py_DECREF(item);
			Py_DECREF(iter);
			sync_set_from_layer__internal(self); /* revert */
			return 0;
		}
		
		bit = PyInt_AS_LONG(item)-1;
		
		if (!LAYSEQ_COMPAT_INT_BLEN(bit)) {
			Py_DECREF(iter);
			Py_DECREF(item);
			sync_set_from_layer__internal(self); /* revert */
			return 0;
		}		
		
		layer |= 1<<bit; 
		Py_DECREF(item);
	}
	Py_DECREF(iter);
	
	id_set_layer(id, layer);
	return 1;
}

/* returns the layer, -1 if the there is an error */
static int layer_from_iter__internal(PyObject *seq)
{
	PyObject *item, *iter;
	int layer = 0, bit;	
	
	iter = PyObject_GetIter(seq);
	
	if (!iter)
		return -1;
	
	item = PyIter_Next(iter);
	
	while (item) {
		/* type checking is alredy been done! */
		if (!PyInt_Check(item)) {
			Py_DECREF(iter);
			return -1;
		}
		
		bit = (int)PyInt_AsLong(item);
		Py_DECREF(item);
		if (!LAYSEQ_COMPAT_INT(bit)) { /* not an int or out of range */
			Py_DECREF(iter);
			return 0;
		}
		
		layer |= 1<<(bit-1);	/* subtract 1 because the python set is 1 to 20 */
		item = PyIter_Next(iter);
	}
	Py_DECREF(iter);
	return layer; /* zero is ok here */
}

static int LayerSet_length(BPyLayerSetObject *self)
{
	/* no need to update the set here */
	if (self->genlib && self->genlib->id) {
		int layer = layer_from_id(self->genlib->id);
		int bit;
		int count;
		for(bit=0; bit<20; bit++) {
			if (layer & (1<<bit)) {
				count++;
			}
		}
		return count;
	} else { 
		return PySet_Size((PyObject *)self);
	}
}

static PyObject * LayerSet_contains(BPyLayerSetObject *self, PyObject *key);
static PySequenceMethods LayerSet_as_sequence = {
	(lenfunc)LayerSet_length,		/* sq_length */
	0,								/* sq_concat */
	0,								/* sq_repeat */
	0,								/* sq_item */
	0,								/* sq_slice */
	0,								/* sq_ass_item */
	0,								/* sq_ass_slice */
	(objobjproc)LayerSet_contains,	/* sq_contains */
};

static int LayerSet_print(BPyLayerSetObject *self,  FILE *fp, int flags)
{
	sync_set_from_layer__internal(self);
	return PySet_Type.tp_print((PyObject *)self, fp, flags);
}

static PyObject * LayerSet_repr(BPyLayerSetObject *self)
{
	sync_set_from_layer__internal(self);
	return PySet_Type.tp_repr((PyObject *)self);
}

/*// It seems This is not accessed directly so no need to wrap it 
static int LayerSet_clear(LayerSet *self)
{
	int ret;
	sync_set_from_layer__internal(self);
	ret = PySet_Type.tp_clear((PyObject *)self);
	sync_layer_from_set__internal(self);
	return ret;
}
*/

static PyObject * LayerSet_iter(BPyLayerSetObject *self)
{
	sync_set_from_layer__internal(self);
	return PySet_Type.tp_iter((PyObject *)self);
}

static PyObject * LayerSet_richcompare(PyObject *v, PyObject *w, int op)
{
	sync_set_from_layer__internal((BPyLayerSetObject *)v);
	return PySet_Type.tp_richcompare(v, w, op);
}

static int LayerSet_init(BPyLayerSetObject *self, PyObject *args, PyObject *kwds)
{
	if (PySet_Type.tp_init((PyObject *)self, args, kwds) < 0)
		return -1;
	self->genlib = NULL;
	return 0;
}

/* Methods */
static PyObject * LayerSet_add(BPyLayerSetObject *self, PyObject *value)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, value);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_add, newargs, NULL);
	Py_DECREF(newargs);
	
	LAYSEQ_RET_ERRORCHECK(self, ret); /* checks set is valid and reverts to original if its not */
}

static PyObject * LayerSet_clear(BPyLayerSetObject *self )
{
	/* Groups support no layers.. odd */
	if (LAYSEQ_EMPTY_SUPPORT(self->genlib)) {
		PyObject *ret;
		PyObject *newargs = PyTuple_Pack(1, (PyObject *)self);
		
		ret = PyObject_Call(CONST_PySet_clear, newargs, NULL);
		Py_DECREF(newargs);
		
		if (ret)
			sync_layer_from_set__internal(self);
		
		return ret;
	} else {
		return ( EXPP_ReturnPyObjError( PyExc_ValueError,
				"Scene and object layers cannot be cleared because they dont support all layers disabled." ) );
	}
}

static PyObject * LayerSet_contains(BPyLayerSetObject *self, PyObject *key)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, key);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_contains, newargs, NULL);
	Py_DECREF(newargs);
	return ret;
}

static PyObject * LayerSet_copy(BPyLayerSetObject *self, PyObject *key)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(1, (PyObject *)self);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_copy, newargs, NULL);
	Py_DECREF(newargs);
	return ret;
}

static PyObject * LayerSet_discard(BPyLayerSetObject *self, PyObject *key)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, key);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_discard, newargs, NULL);
	Py_DECREF(newargs);
	
	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_difference(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_difference, newargs, NULL);
	Py_DECREF(newargs);
	
	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_difference_update(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_difference_update, newargs, NULL);
	Py_DECREF(newargs);
	
	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_intersection(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_intersection, newargs, NULL);
	Py_DECREF(newargs);

	return ret;
}

static PyObject * LayerSet_intersection_update(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_intersection_update, newargs, NULL);
	Py_DECREF(newargs);

	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_issubset(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_issubset, newargs, NULL);
	Py_DECREF(newargs);

	return ret;
}

static PyObject * LayerSet_issuperset(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_issuperset, newargs, NULL);
	Py_DECREF(newargs);

	return ret;
}

static PyObject * LayerSet_pop(BPyLayerSetObject *self)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(1, (PyObject *)self);

	sync_set_from_layer__internal(self);

	ret = PyObject_Call(CONST_PySet_pop, newargs, NULL);
	Py_DECREF(newargs);

	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_reduce(BPyLayerSetObject *self)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(1, (PyObject *)self);

	sync_set_from_layer__internal(self);

	ret = PyObject_Call(CONST_PySet_reduce, newargs, NULL);
	Py_DECREF(newargs);

	return ret;
}

static PyObject * LayerSet_remove(BPyLayerSetObject *self, PyObject *key)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, key);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_remove, newargs, NULL);
	Py_DECREF(newargs);
	
	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_symmetric_difference(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_symmetric_difference, newargs, NULL);
	Py_DECREF(newargs);

	return ret;
}

static PyObject * LayerSet_symmetric_difference_update(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_symmetric_difference_update, newargs, NULL);
	Py_DECREF(newargs);

	LAYSEQ_RET_NULLCHECK(self, ret); /* return */
}

static PyObject * LayerSet_union(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_union, newargs, NULL);
	Py_DECREF(newargs);
	
	return ret;
}

static PyObject * LayerSet_update(BPyLayerSetObject *self, PyObject *other)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, other);
	
	sync_set_from_layer__internal(self);
	
	ret = PyObject_Call(CONST_PySet_update, newargs, NULL);
	Py_DECREF(newargs);
	
	LAYSEQ_RET_ERRORCHECK(self, ret); /* checks set is valid and reverts to original if its not */
}

static PyMethodDef LayerSet_methods[] = {
	
	{"add",		(PyCFunction)LayerSet_add,		METH_O,	 ""},
	{"clear",	(PyCFunction)LayerSet_clear,		METH_NOARGS,	 ""},
	{"__contains__",(PyCFunction)LayerSet_contains,	METH_O | METH_COEXIST,	""},
	{"copy",	(PyCFunction)LayerSet_copy,		METH_NOARGS,	 ""},
	{"discard",	(PyCFunction)LayerSet_discard,	METH_O,	 ""},
	{"difference",	(PyCFunction)LayerSet_difference,	METH_O,	 ""},
	{"difference_update",	(PyCFunction)LayerSet_difference_update,	METH_O,	 ""},
	{"intersection",(PyCFunction)LayerSet_intersection,	METH_O,	 ""},
	{"intersection_update",(PyCFunction)LayerSet_intersection_update,	METH_O,	 ""},
	{"issubset",	(PyCFunction)LayerSet_issubset,	METH_O,	 ""},
	{"issuperset",	(PyCFunction)LayerSet_issuperset,	METH_O,	 ""},
	{"pop",		(PyCFunction)LayerSet_pop,		METH_NOARGS,	 ""},
	{"__reduce__",	(PyCFunction)LayerSet_reduce,	METH_NOARGS,	 ""},
	{"remove",	(PyCFunction)LayerSet_remove,	METH_O,	 ""},
	{"symmetric_difference",(PyCFunction)LayerSet_symmetric_difference,	METH_O,	 ""},
	{"symmetric_difference_update",(PyCFunction)LayerSet_symmetric_difference_update,	METH_O,	 ""},
	{"union",	(PyCFunction)LayerSet_union,		METH_O,	 ""},
	{"update",	(PyCFunction)LayerSet_update,	METH_O,	 ""},
	{NULL,		NULL}	/* sentinel */
};

static PyObject * LayerSet_ior(BPyLayerSetObject *self, PyObject *other)
{
	sync_set_from_layer__internal(self);
	return PySet_Type.tp_as_number->nb_inplace_or((PyObject *)self, other);
}

static PyNumberMethods LayerSet_as_number = {
	0,				/*nb_add*/
	
	/* TODO - pythons set_sub disallows this for subtypes,
	 * not sure why but it should work... check this is ok */
	(binaryfunc)LayerSet_difference,		/*nb_subtract*/
	0,				/*nb_multiply*/
	0,				/*nb_divide*/
	0,				/*nb_remainder*/
	0,				/*nb_divmod*/
	0,				/*nb_power*/
	0,				/*nb_negative*/
	0,				/*nb_positive*/
	0,				/*nb_absolute*/
	0,				/*nb_nonzero*/
	0,				/*nb_invert*/
	0,				/*nb_lshift*/
	0,				/*nb_rshift*/
	(binaryfunc)LayerSet_intersection,		/*nb_and*/
	(binaryfunc)LayerSet_symmetric_difference,		/*nb_xor*/
	(binaryfunc)LayerSet_union,		/*nb_or*/
	0,				/*nb_coerce*/
	0,				/*nb_int*/
	0,				/*nb_long*/
	0,				/*nb_float*/
	0,				/*nb_oct*/
	0, 				/*nb_hex*/
	0,				/*nb_inplace_add*/
	(binaryfunc)LayerSet_difference_update,		/*nb_inplace_subtract*/
	0,				/*nb_inplace_multiply*/
	0,				/*nb_inplace_divide*/
	0,				/*nb_inplace_remainder*/
	0,				/*nb_inplace_power*/
	0,				/*nb_inplace_lshift*/
	0,				/*nb_inplace_rshift*/
	(binaryfunc)LayerSet_intersection_update,		/*nb_inplace_and*/
	(binaryfunc)LayerSet_symmetric_difference_update,		/*nb_inplace_xor*/
	(binaryfunc)LayerSet_ior,		/*nb_inplace_or*/
};

/*****************************************************************************/
/* Function:	Material_dealloc          */
/* Description: This is a callback function for the BPyMaterialObject type. It is */
/*		the destructor function.				 */
/*****************************************************************************/
static void LayerSet_dealloc( BPyLayerSetObject * self )
{
	if (self->genlib) {
		Py_DECREF((PyObject *)(self->genlib));
	}
	PySet_Type.tp_dealloc((PyObject *)self);
}


PyTypeObject BPyLayerSet_Type = {
	PyObject_HEAD_INIT( NULL )
	0,
	"BPyLayerSetObject",
	sizeof(BPyLayerSetObject),
	0,
	(destructor)LayerSet_dealloc,					/* tp_dealloc */
	(printfunc)LayerSet_print,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	(reprfunc)LayerSet_repr,					/* tp_repr */
	&LayerSet_as_number,					/* tp_as_number */
	&LayerSet_as_sequence,					/* tp_as_sequence */
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
	0,/*(inquiry)BPyLayerSet_clear - internal */					/* tp_clear */
	(richcmpfunc)LayerSet_richcompare,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	(getiterfunc)LayerSet_iter,					/* tp_iter */
	0,					/* tp_iternext */
	LayerSet_methods,	/* tp_methods */
	0,					/* tp_members */
	0, /*LayerSet_getset,*/			/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */ 
	0,					/* tp_dictoffset */
	(initproc)LayerSet_init, /* tp_init */
	0,					/* tp_alloc */
	0,					/* tp_new */
};



/* called by GenericLib_getMaterials */
PyObject *LayerSet_CreatePyObject( BPyGenericLibObject *genlib )
{
	PyObject *obj;
	PyObject *args = PyTuple_New(0);
	
	/* passing NULL for keywords is fine*/
	obj = BPyLayerSet_Type.tp_new(&BPyLayerSet_Type, args, NULL);
	BPyLayerSet_Type.tp_init(obj, args, NULL);
	((BPyLayerSetObject*)obj)->genlib = genlib;
	
	if (genlib) {/* this is a genlib user */
		Py_INCREF(genlib); 
	}
	sync_set_from_layer__internal((BPyLayerSetObject *)obj);
	Py_DECREF(args);
	return obj;
}

/* called by GenericLib_setMaterials */
int LayerSet_AssignPyObject( BPyGenericLibObject *genlib, PyObject * value)
{
	int layer;
	
	layer = layer_from_iter__internal(value);
	if (layer == -1) /* bad value */
		return ( EXPP_ReturnIntError( PyExc_ValueError, "The type assigned does not contain a sequence" ) );
	
	if (!layer && !LAYSEQ_EMPTY_SUPPORT(genlib)) /* Null layer and type cant support it */ 
		return ( EXPP_ReturnIntError( PyExc_ValueError, "Can only assign zero layers to group.layers" ) );
	
	id_set_layer(genlib->id, layer);
	return 0;
}

PyObject *LayerSetType_Init( void )
{
	BPyLayerSet_Type.tp_base= &PySet_Type;
	PyType_Ready( &BPyLayerSet_Type );
	return (PyObject *) &BPyLayerSet_Type;
}

void LayerSet_Init( void )
{
	/* store these so as not to have to do lookups each time they are called */
	CONST_PySet_add =							PyObject_GetAttrString( ((PyObject *)&PySet_Type), "add");
	CONST_PySet_clear =							PyObject_GetAttrString( ((PyObject *)&PySet_Type), "clear");
	CONST_PySet_contains =						PyObject_GetAttrString( ((PyObject *)&PySet_Type), "__contains__");
	CONST_PySet_copy =							PyObject_GetAttrString( ((PyObject *)&PySet_Type), "copy");
	CONST_PySet_discard =						PyObject_GetAttrString( ((PyObject *)&PySet_Type), "discard");
	CONST_PySet_difference =					PyObject_GetAttrString( ((PyObject *)&PySet_Type), "difference");
	CONST_PySet_difference_update =				PyObject_GetAttrString( ((PyObject *)&PySet_Type), "difference_update");
	CONST_PySet_intersection =					PyObject_GetAttrString( ((PyObject *)&PySet_Type), "intersection");
	CONST_PySet_intersection_update =			PyObject_GetAttrString( ((PyObject *)&PySet_Type), "intersection_update");
	CONST_PySet_issubset =						PyObject_GetAttrString( ((PyObject *)&PySet_Type), "issubset");
	CONST_PySet_issuperset =					PyObject_GetAttrString( ((PyObject *)&PySet_Type), "issuperset");
	CONST_PySet_pop =							PyObject_GetAttrString( ((PyObject *)&PySet_Type), "pop");
	CONST_PySet_reduce =						PyObject_GetAttrString( ((PyObject *)&PySet_Type), "__reduce__");
	CONST_PySet_remove =						PyObject_GetAttrString( ((PyObject *)&PySet_Type), "remove");
	CONST_PySet_symmetric_difference =			PyObject_GetAttrString( ((PyObject *)&PySet_Type), "symmetric_difference");
	CONST_PySet_symmetric_difference_update =	PyObject_GetAttrString( ((PyObject *)&PySet_Type), "symmetric_difference_update");
	CONST_PySet_union =							PyObject_GetAttrString( ((PyObject *)&PySet_Type), "union");
	CONST_PySet_update =						PyObject_GetAttrString( ((PyObject *)&PySet_Type), "update");
}
