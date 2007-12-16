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

/*
 * BPyLists are for accessing small lists in blender
 * say 1-20 items, its not that fast but has the advantage
 * that it IS a list, but it also keeps its self in sync with Blenders data.
 * 
 * Being a list means that we dont need to impliment our own append, remove, extend, len, iter sort etc
 * and we can be sure that users can treat these exactly as lists and dont need to read any documents.
 * 
 * Before retriving and after modifying the data, functions run that make sure the list data
 * matches blenders internal data, this has to compare the list and the blender data a lot which is why
 * its only appropriate for a small list like materials, texteres, renderlayers etc. */


#include "bpy_list.h" /* This must come first */
#include "MEM_guardedalloc.h"
#include "gen_utils.h"
#include "DNA_object_types.h"
#include "DNA_curve_types.h"
#include "DNA_meta_types.h"
#include "DNA_mesh_types.h"
#include "Material.h"
#include "BKE_material.h"
#include "BKE_utildefines.h"

#define MTLLS_CMP_BPYMAT_BMAT(bpymat, mat) ((bpymat==Py_None && !mat) || ( BPyMaterial_Check(bpymat) && ((BPyMaterialObject *)bpymat)->material == mat))
/* TODO - Scriptlink/ScriptlinkTuple comparison */




static int BPyList_MaxLen(char type)
{
	switch (type) {
	case BPY_LIST_TYPE_MATERIAL:
		return 16;
	case BPY_LIST_TYPE_SCRIPTLINK:
		return 256; /* is there a limit */
	case BPY_LIST_TYPE_COLORBAND:
		return 32;
	case BPY_LIST_TYPE_RENDERLAYERS:
		return 256; /* is there a limit */ 
	}
	return 0;
}

static short BPyList_Compat(PyObject *value, char type)
{
	switch (type) {
	case BPY_LIST_TYPE_MATERIAL:
		if ((value==Py_None) || BPyMaterial_Check(value)) return 1;
		else return 0;
	case BPY_LIST_TYPE_SCRIPTLINK:
		if (BPyScriptLinkTuple_Check(value)) return 1;
		else return 0;
	case BPY_LIST_TYPE_COLORBAND:
		if ((value==Py_None) || BPyMaterial_Check(value)) return 1;
		else return 0;
	case BPY_LIST_TYPE_RENDERLAYERS:
		if ((value==Py_None) || BPyMaterial_Check(value)) return 1;
		else return 0;
	}
	return 0;
}

static int BPyList_CompatSeq(PyObject *seq, char type)
{
	int i;
	PyObject *pyob;
	if (!PySequence_Check(seq))
		return 0;
	
	i=PySequence_Size(seq);
	while (i) {
		i--;
		pyob = PySequence_GetItem(seq, i);
		if (!BPyList_Compat(pyob, type)) {
			Py_DECREF(pyob);
			return 0;
		}
		Py_DECREF(pyob);
	}
	return 1;
}

static char BPyList_Type_Error[] = "Invalid type for this list";
static char BPyList_Size_Error[] = "Cannot exceed maximum length for this type";

/* store these methods so we dont have to lookup the list methods each time */
PyObject * CONST_PyList_subscript;
PyObject * CONST_PyList_reversed;
/*PyObject * CONST_PyList_append;*/ //PyList_Append is fine
PyObject * CONST_PyList_insert;
PyObject * CONST_PyList_extend;
PyObject * CONST_PyList_pop;
PyObject * CONST_PyList_remove;
PyObject * CONST_PyList_index;
PyObject * CONST_PyList_count;
PyObject * CONST_PyList_reverse;
PyObject * CONST_PyList_sort;

void material_set_totcol(ID *id, short totcol)
{
	/* only care about types that can have materials */
	switch ( GS(id->name) ) {
	case ID_OB:
		((Object *)id)->totcol = (char)totcol;
		return;
	case ID_ME:
		((Mesh *)id)->totcol = totcol;
		return;
	case ID_CU:
		((Curve *)id)->totcol = totcol;
		return;
	case ID_MB:
		((MetaBall *)id)->totcol = totcol;
		return;
	}
}

static void sync_list_from_materials__internal(BPyListObject *self)
{
	int i, list_len = PyList_Size((PyObject *)self);
	short totcol;
	Material ***mat;
	PyObject *pyob;
	ID *id;
	if (!self->genlib || !self->genlib->id)
		return;
	
	id = self->genlib->id;
	totcol = GenericLib_totcol(id);
	mat = GenericLib_materials(id);
	
	if (totcol > (short)list_len) {
		/* adding until same size */
		for(i=list_len; i<totcol; i++) {
			PyList_Append((PyObject *)self, Py_None);
		}
		
	} else if (totcol < (short)list_len) {
		/* this is how python does pop internally anyway */
		PyList_SetSlice((PyObject *)self, totcol-1, list_len, NULL);
	}
	for(i=0; i<totcol; i++) {
		pyob = PyList_GET_ITEM(self, i);
		if (!MTLLS_CMP_BPYMAT_BMAT(pyob, (*mat)[i])) {
			/* ((*mat)[i]) - can be NULL, will just set None in the list */
			PyList_SetItem((PyObject *)self, i, Material_CreatePyObject((*mat)[i]));
		}
	}
}

static void sync_materials_from_list__internal(BPyListObject *self)
{
	PyObject *pyob;
	ID *id;
	short totcol;
	short totcol_new = (short)PyList_Size((PyObject *)self);
	short i;
	Material ***matarar;
	
	if (!self->genlib || !self->genlib->id)
		return;
	
	id = self->genlib->id;
	
	totcol = GenericLib_totcol(id);
	matarar = GenericLib_materials(id);
	
	if (!id || (totcol_new >= 16) || (totcol+totcol_new == 0))
		return;
	
	if (totcol != totcol_new) {
		/* clear all existing lists */
		for(i=0; i<totcol; i++) {
			if ((*matarar)[i]) {
				((*matarar)[i])->id.us--;
			}
		}
		if (*matarar)
			MEM_freeN(*matarar);
		if (totcol_new)
			*matarar= MEM_callocN(sizeof(void *)*totcol_new, "ob->mat");
		else
			*matarar= NULL;
		
		material_set_totcol(id, totcol_new);
	} else {
		/* decref materials */
		for(i=0; i<totcol_new; i++) {
			if ((*matarar)[i]) {
				((*matarar)[i])->id.us--;
			}
		}
	}
	
	/* now set up materials */
	for(i=0; i<totcol_new; i++) {
		
		pyob = PyList_GET_ITEM(self, i);
		if (BPyMaterial_Check(pyob)) {
			(*matarar)[i] = ((BPyMaterialObject *)pyob)->material;
			((*matarar)[i])->id.us++; //id_us_plus
		} else {
			(*matarar)[i]= NULL; 
		}
	}

	
   /* 
    * test_object_materials( id );
    * 
    * This is another ugly fix due to the weird material handling of blender.
    * it makes sure that object material lists get updated (by their length)
    * according to their data material lists, otherwise blender crashes.
    * It just stupidly runs through all objects...BAD BAD BAD.
    */
	if (totcol != totcol_new) {
		if (GS(id->name) == ID_OB) {
			((Object *)id)->recalc |= OB_RECALC_OB;
		} else {
			test_object_materials( id );
		}
	} 
}
/*********************************************************************/
/* ScriptLink Functions */
/*********************************************************************/
static void sync_list_from_scriptlinks__internal(BPyListObject *self)
{
	int i, list_len = PyList_Size((PyObject *)self);
	
	ScriptLink *slink;
	PyObject *pyob;
	ID *id;
	if (!self->genlib || !self->genlib->id)
		return;
	
	id = self->genlib->id;
	
	slink = GenericLib_scriptlinks(id);
	
	if (slink->totscript > list_len) {
		/* adding until same size */
		for(i=list_len; i<slink->totscript; i++) {
			PyList_Append((PyObject *)self, Py_None);
		}
		
	} else if (slink->totscript < list_len) {
		/* this is how python does pop internally anyway */
		PyList_SetSlice((PyObject *)self, slink->totscript-1, list_len, NULL);
	}
	for(i=0; i<slink->totscript; i++) {
		pyob = PyList_GET_ITEM(self, i);
		
		/* TODO - Check if these are the same as the existing,  this is very lazy */
		PyList_SetItem((PyObject *)self, i, ScriptLinkTuple_CreatePyObject(self->genlib, i));
		
		/*
		ScriptLinkTuple_Check
		if (!SLLS_CMP_BPYSL_BSL(self->genlib, pyob, slink, i) {
			if (slink->scripts[i]) {
				PyList_SetItem((PyObject *)self, i, ScriptLink_CreatePyObject(self->genlib, i);
			} else {
				Py_INCREF(Py_None);
				PyList_SetItem((PyObject *)self, i, Py_None);
			}
		}
		*/
	}

}

static void sync_scriptlinks_from_list__internal(BPyListObject *self)
{
	PyObject *pyob;
	ID *id;
	
	int totslink_new = PyList_Size((PyObject *)self);
	int i;
	ScriptLink *slink;
	
	if (!self->genlib || !self->genlib->id)
		return;
	
	id = self->genlib->id;
	
	slink = GenericLib_scriptlinks(id); 
	
	if ( !id )
		return;
	
	if (slink->totscript != totslink_new) {
		if( slink->scripts )	MEM_freeN( slink->scripts );
		if( slink->flag )		MEM_freeN( slink->flag );
		
		if (totslink_new) {
			slink->scripts = MEM_mallocN( sizeof( ID * ) * ( slink->totscript ), "bpySlinkL" );
			slink->flag = MEM_mallocN( sizeof( short * ) * ( slink->totscript  ), "bpySlinkF" );
			slink->totscript = totslink_new;
		}
		
		if (slink->actscript > totslink_new)
			slink->actscript= totslink_new;
	}
	
	/* now set up scriptlinks */
	for(i=0; i<totslink_new; i++) {
		/* no error checking for pyob, all the error checking is done while dealing with the wrappers
		 * if for any reason they prove not to be water tight, we need to have some type checking here */
		pyob = PyList_GET_ITEM(self, i);
		ScriptLinkTuple_ToScriptLink( slink, i, pyob );
	}	
}

static void sync_list_from_colorband__internal(BPyListObject *self) {}
static void sync_list_from_renderlayers__internal(BPyListObject *self) {}

static void sync_colorband_from_list__internal(BPyListObject *self) {}
static void sync_renderlayers_from_list__internal(BPyListObject *self) {}

void sync_list_from_data__internal(BPyListObject *self)
{
	switch (self->type) {
	case BPY_LIST_TYPE_MATERIAL:
		sync_list_from_materials__internal(self);
		return;
	case BPY_LIST_TYPE_SCRIPTLINK:
		sync_list_from_scriptlinks__internal(self);
		return;
	case BPY_LIST_TYPE_COLORBAND: /* fallthrough */
	case BPY_LIST_TYPE_COLORBAND_SPEC:
		sync_list_from_colorband__internal(self);
		return;
	case BPY_LIST_TYPE_RENDERLAYERS:
		sync_list_from_renderlayers__internal(self);
		return;
	}
}

void sync_data_from_list__internal(BPyListObject *self)
{
	switch (self->type) {
	case BPY_LIST_TYPE_MATERIAL:
		sync_materials_from_list__internal(self);
		return;
	case BPY_LIST_TYPE_SCRIPTLINK:
		sync_scriptlinks_from_list__internal(self);
		return;
	case BPY_LIST_TYPE_COLORBAND: /* fallthrough */
	case BPY_LIST_TYPE_COLORBAND_SPEC:
		sync_colorband_from_list__internal(self);
		return;
	case BPY_LIST_TYPE_RENDERLAYERS:
		sync_renderlayers_from_list__internal(self);
		return;
	}
}


static int BPyList_length(BPyListObject *self)
{	
	
	if (!self->genlib || !self->genlib->id) {
		/* not linked to blender data, so just len the list */
		return PyList_Type.tp_as_sequence->sq_length((PyObject *)self);
	} else {
		/* less work to return blenders length */
		sync_list_from_data__internal(self);
		//return (int)GenericLib_totcol(self->genlib->id);
		
		return PyList_Type.tp_as_sequence->sq_length((PyObject *)self);
	}
}

static PyObject * BPyList_concat(BPyListObject *self, PyObject *value)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_concat((PyObject *)self, value);
}

static PyObject * BPyList_repeat(BPyListObject *self, int n)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_repeat((PyObject *)self, n);
}

static PyObject * BPyList_item(BPyListObject *self, int i)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_item((PyObject *)self, i);
}

static PyObject * BPyList_slice(BPyListObject *self, int l, int h)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_slice((PyObject *)self, l, h);
}

static int BPyList_ass_item(BPyListObject *self, int i, PyObject * value)
{
	int ret;
	if (!BPyList_Compat(value, self->type))
		return ( EXPP_ReturnIntError( PyExc_ValueError, BPyList_Type_Error ) );
	
	sync_list_from_data__internal(self);
	ret = PyList_Type.tp_as_sequence->sq_ass_item((PyObject *)self, i, value);
	if (ret!=-1)
		sync_data_from_list__internal(self);
	return ret;
}

static int BPyList_ass_slice(BPyListObject *self, int l, int h, PyObject * value)
{
	int ret;
	
	if (!BPyList_CompatSeq(value, self->type))
		return ( EXPP_ReturnIntError( PyExc_ValueError, BPyList_Type_Error ) );
	
	sync_list_from_data__internal(self);
	ret = PyList_Type.tp_as_sequence->sq_ass_slice((PyObject *)self, l, h, value);
	if (ret!=-1) {
		/* TODO - calculate if the length is too big,
		 * the following code is not incorrect but its a bit slow */
		if (PyList_Size((PyObject *)self) > BPyList_MaxLen(self->type)) {
			sync_list_from_data__internal(self); /* Too Big, revert list from data */
			/* TODO - better error */
			return EXPP_intError(PyExc_ValueError, "list too long for this type");
		} else {
			sync_data_from_list__internal(self);
		}
	}
	return ret;
}

static int BPyList_contains(BPyListObject *self, PyObject * value)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_contains((PyObject *)self, value);
}

static PyObject * BPyList_inplace_concat(BPyListObject *self, PyObject * value)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_inplace_concat((PyObject *)self, value);
}

static PyObject * BPyList_inplace_repeat(BPyListObject *self, int i)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_sequence->sq_inplace_repeat((PyObject *)self, i);
}

static PySequenceMethods BPyList_as_sequence = {
	(lenfunc)BPyList_length,			/* sq_length */
	(binaryfunc)BPyList_concat,		/* sq_concat */
	(ssizeargfunc)BPyList_repeat,		/* sq_repeat */
	(ssizeargfunc)BPyList_item,		/* sq_item */
	(ssizessizeargfunc)BPyList_slice,		/* sq_slice */
	(ssizeobjargproc)BPyList_ass_item,		/* sq_ass_item */
	(ssizessizeobjargproc)BPyList_ass_slice,	/* sq_ass_slice */
	(objobjproc)BPyList_contains,		/* sq_contains */
	(binaryfunc)BPyList_inplace_concat,	/* sq_inplace_concat */
	(ssizeargfunc)BPyList_inplace_repeat,	/* sq_inplace_repeat */
};

static PyObject * BPyList_subscript(BPyListObject *self, PyObject *item)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_as_mapping->mp_subscript((PyObject *)self, item);
}

static int BPyList_ass_subscript(BPyListObject *self, PyObject *item, PyObject *value)
{
	int ret;
	if (!BPyList_Compat(value, self->type))
		return ( EXPP_ReturnIntError( PyExc_ValueError, BPyList_Type_Error ) );
	sync_list_from_data__internal(self);
	ret = PyList_Type.tp_as_mapping->mp_ass_subscript((PyObject *)self, item, value);
	if (ret!=-1) 
		sync_data_from_list__internal(self);
	return ret;
}

static PyMappingMethods BPyList_as_mapping = {
	(lenfunc)BPyList_length,
	(binaryfunc)BPyList_subscript,
	(objobjargproc)BPyList_ass_subscript
};

static int BPyList_print(BPyListObject *self,  FILE *fp, int flags)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_print((PyObject *)self, fp, flags);
}

static PyObject * BPyList_repr(BPyListObject *self)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_repr((PyObject *)self);
}

/*// It seems This is not accessed directly so no need to wrap it 
static int BPyList_clear(MaterialList *self)
{
	int ret;
	sync_list_from_data__internal(self);
	ret = PyList_Type.tp_clear((PyObject *)self);
	sync_data_from_list__internal(self);
	return ret;
}
*/

static PyObject * BPyList_iter(BPyListObject *self)
{
	sync_list_from_data__internal(self);
	return PyList_Type.tp_iter((PyObject *)self);
}

static int BPyList_init(BPyListObject *self, PyObject *args, PyObject *kwds)
{
	if (PyList_Type.tp_init((PyObject *)self, args, kwds) < 0)
		return -1;
	self->genlib = NULL;
	return 0;
}

/* ignoring tp_traverse - we dont have to worry about cyclic
 * garbage collection since the MaterialList can never contain a list */

//static PyGetSetDef BPyList_getsets[] = {
//	{"state", (getter)BPyList_state_get, NULL,
//	 "an int variable for demonstration purposes"},
//	{0}
//};


/*
 * Wrap Funcs
 */

/* I dont understand what the heck unused is here for but pythons list_reversed in 2.5.1 has it */
static PyObject * BPyList_reversed(BPyListObject *self, PyObject *unused)
{
	PyObject *ret, *newargs = PyTuple_Pack(1, (PyObject *)self);
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_reversed, newargs, NULL);
	Py_DECREF(newargs);
	
	return ret;
	
	
	/* TODO - If you want to be totally knit picky,
	 * Its possibly that you could get the wrong results from PyListRevIter_Type
	 * IF!!! you did a reversed iter and during the iter some other part of the
	 * code changed the material (and not through this list) - you could get an invalid value.
	 * 
	 * A way to make this less likely to happen is to Modify each struct to point to a material PyObject.
	 * That way all material access will be done through the one List.
	 * however this dosnt account for blender or modifying the materials, but thats still very unlikely during the loop.
	 * 
	 * The propper solution is to subtype or have out own PyListRevIter_Type but thats a PITA
	 * 
	 * This code would give wrong loop results at the moment since there can be multiple material list instances.
	 * 
	 * mls = mesh.materials
	 * def set_mat():
	 * 		mesh.materials[0] = None
	 * 
	 * 
	 * for mat in reversed(mesh.materials):
	 * 		set_mat()
	 * 		print mat # This materail will be wrong.
	 * ----
	 * 
	 * Must check that PySequence_GetItem does not run our internal sync functions.
	 * 
	 * */
}

static PyObject * BPyList_append(BPyListObject *self, PyObject *value)
{
	if (PyList_Size((PyObject *)self) >= BPyList_MaxLen(self->type))
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
					BPyList_Size_Error ) );
	if (!BPyList_Compat(value, self->type))
		return ( EXPP_ReturnPyObjError( PyExc_ValueError,
					BPyList_Type_Error ) );
	
	sync_list_from_data__internal(self);
	
	if (PyList_Append((PyObject *)self, value)==-1)
		return (PyObject*)NULL;
	
	sync_data_from_list__internal(self);
	Py_RETURN_NONE;
}

static PyObject * BPyList_insert(BPyListObject *self, PyObject *args)
{
	PyObject *value, *ret, *newargs;
	int i;
	if (PyList_Size((PyObject *)self) >= BPyList_MaxLen(self->type))
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
					BPyList_Size_Error ) );
	
	if (!PyArg_ParseTuple(args, "nO:insert", &i, &value))
		return NULL;
	
	if (!BPyList_Compat(value, self->type))
		return ( EXPP_ReturnPyObjError( PyExc_ValueError,
					BPyList_Type_Error ) );
	
	newargs = EXPP_PyTuple_New_Prepend(args, (PyObject *)self);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_insert, newargs, NULL);
	Py_DECREF(newargs);
	
	if (ret)	
		sync_data_from_list__internal(self);
	
	return ret;
}

static PyObject * BPyList_extend(BPyListObject *self, PyObject *value)
{
	PyObject *ret;
	PyObject *newargs;
	
	if (PyList_Size((PyObject *)self) + PySequence_Size(value) >= BPyList_MaxLen(self->type))
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
					BPyList_Size_Error ) );
	if (!BPyList_CompatSeq(value, self->type))
		return ( EXPP_ReturnPyObjError( PyExc_ValueError,
					BPyList_Type_Error ) );
	
	newargs = PyTuple_Pack(2, (PyObject *)self, value);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_extend, newargs, NULL);
	Py_DECREF(newargs);
		
	if (ret)
		sync_data_from_list__internal(self);
	
	return ret;
}

static PyObject * BPyList_pop(BPyListObject *self, PyObject *args)
{
	PyObject *ret;
	PyObject *newargs = EXPP_PyTuple_New_Prepend(args, (PyObject *)self);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_pop, newargs, NULL);
	
	if (ret)
		sync_data_from_list__internal(self);
	
	return ret;
}

static PyObject * BPyList_remove(BPyListObject *self, PyObject *value)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, value);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_remove, newargs, NULL);
	Py_DECREF(newargs);
	
	if (ret)
		sync_data_from_list__internal(self);
	
	return ret;
}

static PyObject * BPyList_index(BPyListObject *self, PyObject *args)
{
	PyObject *ret;
	PyObject *newargs = EXPP_PyTuple_New_Prepend(args, (PyObject *)self);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_index, newargs, NULL);
	Py_DECREF(newargs);
	return ret;
}

static PyObject * BPyList_count(BPyListObject *self, PyObject *value)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(2, (PyObject *)self, value);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_count, newargs, NULL);
	Py_DECREF(newargs);
	
	return ret;
}

static PyObject * BPyList_reverse(BPyListObject *self, PyObject *unused)
{
	PyObject *ret;
	PyObject *newargs = PyTuple_Pack(1, (PyObject *)self);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_reverse, newargs, NULL);
	Py_DECREF(newargs);
	
	if (ret)
		sync_data_from_list__internal(self);
	
	return ret;
}

static PyObject * BPyList_sort(BPyListObject *self, PyObject *args, PyObject *keywds )
{
	PyObject *ret;
	PyObject *newargs = EXPP_PyTuple_New_Prepend(args, (PyObject *)self);
	
	sync_list_from_data__internal(self);
	
	ret = PyObject_Call(CONST_PyList_sort, newargs, keywds);
	Py_DECREF(newargs);
	
	if (ret)
		sync_data_from_list__internal(self);
	
	return ret;
}

static PyMethodDef BPyList_methods[] = {
	/* name, method, flags, doc */
	{"__getitem__", (PyCFunction)BPyList_subscript, METH_O|METH_COEXIST, ""},
	{"__reversed__",(PyCFunction)BPyList_reversed, METH_NOARGS, ""},
	{"append", ( PyCFunction ) BPyList_append, METH_O, ""}, 
	{"insert", ( PyCFunction ) BPyList_insert, METH_VARARGS, ""}, 
	{"extend", ( PyCFunction ) BPyList_extend, METH_O, ""}, 
	{"pop", ( PyCFunction ) BPyList_pop, METH_VARARGS, ""}, 
	{"remove", ( PyCFunction ) BPyList_remove, METH_O, ""}, 
	{"index", ( PyCFunction ) BPyList_index, METH_VARARGS, ""}, 
	{"count", ( PyCFunction ) BPyList_count, METH_O, ""},
	{"reverse", ( PyCFunction ) BPyList_reverse, METH_NOARGS, ""},
	{"sort", ( PyCFunction ) BPyList_sort, METH_VARARGS | METH_KEYWORDS, ""}, 
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/* Function:	BPyList_dealloc          */
/* Description: This is a callback function for the BPyListObject type. It is */
/*		the destructor function.				 */
/*****************************************************************************/
static void BPyList_dealloc( BPyListObject * self )
{
	if (self->genlib) {
		Py_DECREF((PyObject *)(self->genlib));
	}
	PyList_Type.tp_dealloc((PyObject *)self);
}


PyTypeObject BPyList_Type = {
	PyObject_HEAD_INIT( NULL )
	0,
	"BPyList",
	sizeof(BPyListObject),
	0,
	(destructor)BPyList_dealloc,					/* tp_dealloc */
	(printfunc)BPyList_print,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	(reprfunc)BPyList_repr,					/* tp_repr */
	0,					/* tp_as_number */
	&BPyList_as_sequence,					/* tp_as_sequence */
	&BPyList_as_mapping,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0,					/* tp_doc */
	0,					/* tp_traverse */
	/* TODO - tp_clear crashes! - do we need to sync materials before and after clearing? 
	 * It looks like this is only used internally anyhow */
	0,/*(inquiry)BPyBPyList_clear*/					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	(getiterfunc)BPyList_iter,					/* tp_iter */
	0,					/* tp_iternext */
	BPyList_methods,	/* tp_methods */
	0,					/* tp_members */
	0, /*BPyList_getset,*/			/* tp_getset */
	&PyList_Type,		/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */ 
	0,					/* tp_dictoffset */
	(initproc)BPyList_init, /* tp_init */
	0,					/* tp_alloc */
	0,					/* tp_new */
};



/* Can be accessed directly from a getset 
 * Could also be called BPyList_CreatePyObject */
PyObject *BPyList_get( BPyGenericLibObject *genlib, void * type)
{
	BPyListObject *obj;
	PyObject *args = PyTuple_New(0);
	
	/* passing NULL for keywords is fine*/
	obj = (BPyListObject *)BPyList_Type.tp_new(&BPyList_Type, args, NULL);
	BPyList_Type.tp_init((PyObject *)obj, args, NULL);
	obj->genlib = genlib;
	obj->type = (char)type;
	if (genlib) {/* this is a genlib user */
		Py_INCREF(genlib); 
	}
	
	sync_list_from_data__internal(obj);
	//sync_list_from_materials__internal((BPyListObject *)obj);
	
	Py_DECREF(args);
	return (PyObject *)obj;
}

/* Can be accessed directly from a getset */
int BPyList_set( BPyGenericLibObject *genlib, PyObject * value, void * type)
{
	/* This is a bit lazy, but make a MaterialList just for assigning, can optimize later
	 * 
	 * Also note here that we cant just use the list even though this is a list subtype
	 * Because that would mean imposing limits on the list being assigned and thats a NONO
	 * */
	PyObject *pyob;
	if (!BPyList_CompatSeq(value, (char)type))
		return ( EXPP_ReturnIntError( PyExc_ValueError, "this type cannot be assigned to a list" ) );
	
	pyob = BPyList_get(genlib, type);
	
	return BPyList_ass_slice((BPyListObject *)pyob, 0, INT_MAX, value); /* python uses IN_MAX internally for [:]*/
}

PyObject *BPyListType_Init( void )
{
	PyType_Ready( &BPyList_Type );
	return (PyObject *) &BPyList_Type;
}


void BPyList_Init( void )
{	
	/* store these so as not to have to do lookups each time they are called */
	CONST_PyList_subscript =PyObject_GetAttrString( ((PyObject *)&PyList_Type), "__getitem__");
	CONST_PyList_reversed =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "__reversed__");
	/*CONST_PyList_append =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "append");*/
	CONST_PyList_insert =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "insert");
	CONST_PyList_extend =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "extend");
	CONST_PyList_pop =		PyObject_GetAttrString( ((PyObject *)&PyList_Type), "pop");
	CONST_PyList_remove =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "remove");
	CONST_PyList_index =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "index");
	CONST_PyList_count =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "count");
	CONST_PyList_reverse =	PyObject_GetAttrString( ((PyObject *)&PyList_Type), "reverse");
	CONST_PyList_sort =		PyObject_GetAttrString( ((PyObject *)&PyList_Type), "sort");

}
