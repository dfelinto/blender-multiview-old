/*
 * $Id: Armature.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include <stddef.h>

#include "Armature.h" //This must come first

#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_armature.h"
#include "BKE_library.h"
#include "BKE_depsgraph.h"
#include "BKE_utildefines.h"
#include "BLI_blenlib.h"
#include "BLI_arithb.h"
#include "MEM_guardedalloc.h"
#include "Bone.h"
#include "NLA.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "Const.h"

#include "DNA_object_types.h" //This must come before BIF_editarmature.h...
#include "BIF_editarmature.h"

//------------------EXTERNAL PROTOTYPES--------------------
extern void make_boneList(ListBase* list, ListBase *bones, EditBone *parent);
extern void editbones_to_armature (ListBase *list, Object *ob);

//------------------------ERROR CODES---------------------------------
//This is here just to make me happy and to have more consistant error strings :)
static const char sBoneDictError[] = "ArmatureType.bones - Error: ";
static const char sBoneDictBadArgs[] = "ArmatureType.bones - Bad Arguments: ";
static const char sArmatureError[] = "ArmatureType - Error: ";
static const char sArmatureBadArgs[] = "ArmatureType - Bad Arguments: ";
static const char sModuleError[] = "Blender.Armature - Error: ";
static const char sModuleBadArgs[] = "Blender.Armature - Bad Arguments: ";

PyObject * arm_weakref_callback_weakref_dealloc(PyObject *self, PyObject *weakref);
/* python callable */
PyObject * arm_weakref_callback_weakref_dealloc__pyfunc;

//################## BonesDict_Type (internal) ########################
/*This is an internal psuedo-dictionary type that allows for manipulation
* of bones inside of an armature. It is a subobject of armature.
* i.e. Armature.bones['key']*/
//#####################################################################

//------------------METHOD IMPLEMENTATIONS-----------------------------
//------------------------Armature.bones.items()
//Returns a list of key:value pairs like dict.items()
static PyObject* BonesDict_items(BPyBoneDictObject *self)
{
	if (self->editmode_flag){
		return PyDict_Items(self->editbonesMap); 
	}else{
		return PyDict_Items(self->bonesMap); 
	}
}
//------------------------Armature.bones.keys()
//Returns a list of keys like dict.keys()
static PyObject* BonesDict_keys(BPyBoneDictObject *self)
{
	if (self->editmode_flag){
		return PyDict_Keys(self->editbonesMap);
	}else{
		return PyDict_Keys(self->bonesMap);
	}
}
//------------------------Armature.bones.values()
//Returns a list of values like dict.values()
static PyObject* BonesDict_values(BPyBoneDictObject *self)
{
	if (self->editmode_flag){
		return PyDict_Values(self->editbonesMap);
	}else{
		return PyDict_Values(self->bonesMap);
	}
}
//------------------ATTRIBUTE IMPLEMENTATION---------------------------
//------------------TYPE_OBECT IMPLEMENTATION-----------------------
//------------------------tp_doc
//The __doc__ string for this object
static char BPyBonesDict_doc[] = "This is an internal subobject of armature\
designed to act as a Py_Bone dictionary.";

//------------------------tp_methods
//This contains a list of all methods the object contains
static PyMethodDef BPyBonesDict_methods[] = {
	{"items", (PyCFunction) BonesDict_items, METH_NOARGS, 
		"() - Returns the key:value pairs from the dictionary"},
	{"keys", (PyCFunction) BonesDict_keys, METH_NOARGS, 
		"() - Returns the keys the dictionary"},
	{"values", (PyCFunction) BonesDict_values, METH_NOARGS, 
		"() - Returns the values from the dictionary"},
	{NULL, NULL, 0, NULL}
};
//-----------------(internal)
static int BoneMapping_Init(PyObject *dictionary, ListBase *bones){
	Bone *bone = NULL;
	PyObject *py_bone = NULL;

	for (bone = bones->first; bone; bone = bone->next){
		py_bone = PyBone_FromBone(bone);
		if (!py_bone)
			return -1;
		
		if(PyDict_SetItemString(dictionary, bone->name, py_bone) == -1) {
			/* unlikely but possible */
			Py_DECREF(py_bone);
			return -1;
		}
		
		Py_DECREF(py_bone);
		if (bone->childbase.first) 
			BoneMapping_Init(dictionary, &bone->childbase);
	}
	return 0;
}
//-----------------(internal)
static int EditBoneMapping_Init(PyObject *dictionary, ListBase *editbones){
	EditBone *editbone = NULL;
	PyObject *py_editbone = NULL;

	for (editbone = editbones->first; editbone; editbone = editbone->next){
		py_editbone = PyEditBone_FromEditBone(editbone);
		if (!py_editbone)
			return -1;
		
		if(PyDict_SetItemString(dictionary, editbone->name, py_editbone) == -1) {
			Py_DECREF(py_editbone);
			return -1;
		}
		Py_DECREF(py_editbone);
	}
	return 0;
}
//----------------- BonesDict_InitBones
static int BonesDict_InitBones(BPyBoneDictObject *self)
{
	PyDict_Clear(self->bonesMap);
	if (BoneMapping_Init(self->bonesMap, self->bones) == -1)
		return 0;
	return 1;
} 
//----------------- BonesDict_InitEditBones
static int BonesDict_InitEditBones(BPyBoneDictObject *self)
{
	PyDict_Clear(self->editbonesMap);
	if (EditBoneMapping_Init(self->editbonesMap, &self->editbones) == -1)
		return 0;
	return 1;
}
//------------------------tp_repr
//This is the string representation of the object
static PyObject *BonesDict_repr(BPyBoneDictObject *self)
{
	char str[2048];
	PyObject *key, *value;
	int pos = 0;
	char *p = str;
	char *keys, *vals;

	p += sprintf(str, "[Bone Dict: {");

	if (self->editmode_flag){
		while (PyDict_Next(self->editbonesMap, &pos, &key, &value)) {
			keys = PyString_AsString(key);
			vals = PyString_AsString(value->ob_type->tp_repr(value));
			if( strlen(str) + strlen(keys) + strlen(vals) < sizeof(str)-20 )
				p += sprintf(p, "%s : %s, ", keys, vals );
			else {
				p += sprintf(p, "...." );
				break;
			}
		}
	}else{
		while (PyDict_Next(self->bonesMap, &pos, &key, &value)) {
			keys = PyString_AsString(key);
			vals = PyString_AsString(value->ob_type->tp_repr(value));
			if( strlen(str) + strlen(keys) + strlen(vals) < sizeof(str)-20 )
				p += sprintf(p, "%s : %s, ", keys, vals );
			else {
				p += sprintf(p, "...." );
				break;
			}
		}
	}
	sprintf(p, "}]");
	return PyString_FromString(str);
}

//------------------------tp_dealloc
//This tells how to 'tear-down' our object when ref count hits 0
static void BonesDict_dealloc(BPyBoneDictObject * self)
{
	Py_DECREF(self->bonesMap);
	Py_DECREF(self->editbonesMap);
	BLI_freelistN(&self->editbones); 
	PyObject_DEL( self );
	return;
}
//------------------------mp_length
//This gets the size of the dictionary
static int BonesDict_len(BPyBoneDictObject *self)
{
	if (self->editmode_flag){
		return BLI_countlist(&self->editbones);
	}else{
		return BLI_countlist(self->bones);
	}
}
//-----------------------mp_subscript
//This defines getting a bone from the dictionary - x = Bones['key']
static PyObject *BonesDict_GetItem(BPyBoneDictObject *self, PyObject* key)
{ 
	PyObject *value = NULL;

	if (self->editmode_flag){
		value = PyDict_GetItem(self->editbonesMap, key);
	}else{
		value = PyDict_GetItem(self->bonesMap, key);
	}
	if(value == NULL){  /* item not found in dict. throw exception */
		char* key_str = PyString_AsString( key );
		if (key_str) {
			return EXPP_ReturnPyObjError(PyExc_KeyError, "bone key must be a string" );
		} else {
			char buffer[128];
			PyOS_snprintf( buffer, sizeof(buffer), "bone %s not found", key_str);
			return EXPP_ReturnPyObjError(PyExc_KeyError, buffer );
		}
	}
	return EXPP_incr_ret(value);
}
//-----------------------mp_ass_subscript
//This does dict assignment - Bones['key'] = value
static int BonesDict_SetItem(BPyBoneDictObject *self, PyObject *key, PyObject *value)
{
	BPyEditBoneObject *editbone_for_deletion;
	struct EditBone *editbone = NULL;
	char *key_str = PyString_AsString(key);

	if (!self->editmode_flag)
		return EXPP_intError(PyExc_AttributeError, "%s%s", 
				sBoneDictBadArgs,  "You must call makeEditable() first");
	
	if (!key_str)
		return EXPP_intError(PyExc_AttributeError, "%s%s", 
				sBoneDictBadArgs,  "The key must be the name of an editbone");
	
	if (value && !BPyEditBone_Check(value))
		return EXPP_intError(PyExc_AttributeError, "%s%s",
				sBoneDictBadArgs,  "Can only assign editbones as values");
	
	//parse value for assignment
	if (value){ /* we know this must be an editbone from the above check */
		//create a new editbone
		editbone = MEM_callocN(sizeof(EditBone), "eBone");
		BLI_strncpy(editbone->name, key_str, 32);
		unique_editbone_name(NULL, editbone->name);
		editbone->dist = ((BPyEditBoneObject*)value)->dist;
		editbone->ease1 = ((BPyEditBoneObject*)value)->ease1;
		editbone->ease2 = ((BPyEditBoneObject*)value)->ease2;
		editbone->flag = ((BPyEditBoneObject*)value)->flag;
		editbone->parent = ((BPyEditBoneObject*)value)->parent;
		editbone->rad_head = ((BPyEditBoneObject*)value)->rad_head;
		editbone->rad_tail = ((BPyEditBoneObject*)value)->rad_tail;
		editbone->roll = ((BPyEditBoneObject*)value)->roll;
		editbone->segments = ((BPyEditBoneObject*)value)->segments;
		editbone->weight = ((BPyEditBoneObject*)value)->weight;
		editbone->xwidth = ((BPyEditBoneObject*)value)->xwidth;
		editbone->zwidth = ((BPyEditBoneObject*)value)->zwidth;
		VECCOPY(editbone->head, ((BPyEditBoneObject*)value)->head);
		VECCOPY(editbone->tail, ((BPyEditBoneObject*)value)->tail);
		
		// FIXME, should be exposed via python. this avoids creating bones with no layers.
		editbone->layer= 1;
		
		//set object pointer
		((BPyEditBoneObject*)value)->editbone = editbone;

		//fix the bone's head position if flags indicate that it is 'connected'
		if (editbone->flag & BONE_CONNECTED){
			if(!editbone->parent){
				((BPyEditBoneObject*)value)->editbone = NULL;
				MEM_freeN(editbone);
					return EXPP_intError(PyExc_AttributeError, "%s%s", 
							sBoneDictBadArgs,  "The 'connected' flag is set but the bone has no parent!");
			}else{
				VECCOPY(editbone->head, editbone->parent->tail);
			}
		}

		//set in editbonelist
		BLI_addtail(&self->editbones, editbone);

		//set the new editbone in the mapping
		if(PyDict_SetItemString(self->editbonesMap, key_str, value) == -1){
			((BPyEditBoneObject*)value)->editbone = NULL;
			BLI_freelinkN(&self->editbones, editbone);
			return EXPP_intError(PyExc_RuntimeError, "%s%s", 
					sBoneDictError,  "Unable to access dictionary!");
		}
	}else {
		//they are trying to delete the bone using 'del'
		editbone_for_deletion = (BPyEditBoneObject*)PyDict_GetItem(self->editbonesMap, key);

		if (!editbone_for_deletion)
			return EXPP_intError(PyExc_KeyError, "%s%s%s%s", 
					sBoneDictError,  "The key: ", key_str, " is not present in this dictionary!");

		/*first kill the datastruct then remove the item from the dict
		and wait for GC to pick it up.
		We have to delete the datastruct here because the tp_dealloc
		doesn't handle it*/
		
		/*this is ugly but you have to set the parent to NULL for else 
		editbones_to_armature will crash looking for this bone*/
		for (editbone = self->editbones.first; editbone; editbone = editbone->next){
			if (editbone->parent == editbone_for_deletion->editbone) {
				editbone->parent = NULL;
				 /* remove the connected flag or else the 'root' ball
				  * doesn't get drawn */
				editbone->flag &= ~BONE_CONNECTED;
			}
		}
		BLI_freelinkN(&self->editbones, editbone_for_deletion->editbone);
		if(PyDict_DelItem(self->editbonesMap, key) == -1)
			return EXPP_intError(PyExc_RuntimeError, "%s%s", 
					sBoneDictError,  "Unable to access dictionary!");
	}
	return 0;
}
//------------------TYPE_OBJECT DEFINITION--------------------------
//Mapping Protocol
static PyMappingMethods BonesDict_MapMethods = {
	(inquiry) BonesDict_len,					//mp_length
	(binaryfunc)BonesDict_GetItem,		//mp_subscript
	(objobjargproc)BonesDict_SetItem,	//mp_ass_subscript
};
//BonesDict TypeObject
PyTypeObject BPyBoneDict_Type = {
	PyObject_HEAD_INIT(NULL)			//tp_head
	0,												//tp_internal
	"BonesDict",								//tp_name
	sizeof(BPyBoneDictObject),					//tp_basicsize
	0,												//tp_itemsize
	(destructor)BonesDict_dealloc,		//tp_dealloc
	0,												//tp_print
	0,												//tp_getattr
	0,												//tp_setattr
	0,												//tp_compare
	(reprfunc) BonesDict_repr,				//tp_repr
	0,												//tp_as_number
	0,												//tp_as_sequence
	&BonesDict_MapMethods,				//tp_as_mapping
	0,												//tp_hash
	0,												//tp_call
	0,												//tp_str
	0,												//tp_getattro
	0,												//tp_setattro
	0,												//tp_as_buffer
	Py_TPFLAGS_DEFAULT,					//tp_flags
	BPyBonesDict_doc,						//tp_doc
	0,												//tp_traverse
	0,												//tp_clear
	0,												//tp_richcompare
	0,												//tp_weaklistoffset
	0,												//tp_iter
	0,												//tp_iternext
	BPyBonesDict_methods,				//tp_methods
	0,												//tp_members
	0,												//tp_getset
	0,												//tp_base
	0,												//tp_dict
	0,												//tp_descr_get
	0,												//tp_descr_set
	0,												//tp_dictoffset
	0, 				                                //tp_init
	0,												//tp_alloc
	0,												//tp_new
	0,												//tp_free
	0,												//tp_is_gc
	0,												//tp_bases
	0,												//tp_mro
	0,												//tp_cache
	0,												//tp_subclasses
	0,												//tp_weaklist
	0												//tp_del
};
//-----------------------PyBonesDict_FromPyArmature
static PyObject *PyBonesDict_FromPyArmature(BPyArmatureObject *py_armature)
{
	BPyBoneDictObject *py_BonesDict = (BPyBoneDictObject *)PyObject_NEW( BPyBoneDictObject, &BPyBoneDict_Type );
	if (!py_BonesDict)
		goto RuntimeError;

	py_BonesDict->bones = NULL;
	py_BonesDict->editbones.first = py_BonesDict->editbones.last = NULL;

	//create internal dictionaries
	py_BonesDict->bonesMap = PyDict_New();
	py_BonesDict->editbonesMap = PyDict_New();
	if (!py_BonesDict->bonesMap || !py_BonesDict->editbonesMap)
		goto RuntimeError;

	//set listbase pointer
	py_BonesDict->bones = &py_armature->armature->bonebase;

	//now that everything is setup - init the mappings
	if (!BonesDict_InitBones(py_BonesDict))
		goto RuntimeError;
	if (!BonesDict_InitEditBones(py_BonesDict))
		goto RuntimeError;

	//set editmode flag
	py_BonesDict->editmode_flag = 0; 

	return (PyObject*)py_BonesDict;

RuntimeError:
	return EXPP_objError(PyExc_RuntimeError, "%s%s", 
		sBoneDictError, "Failed to create class");
}

//######################### BPyArmature_Type #############################
/*This type represents a thin wrapper around bArmature data types
* internal to blender. It contains the psuedo-dictionary BonesDict
* as an assistant in manipulating it's own bone collection*/
//#################################################################

//------------------METHOD IMPLEMENTATION------------------------------
//------------------------Armature.makeEditable()
static PyObject *Armature_makeEditable(BPyArmatureObject *self)
{
	if (self->armature->flag & ARM_EDITMODE)
		goto AttributeError;

	make_boneList(&self->Bones->editbones, self->Bones->bones, NULL);
	if (!BonesDict_InitEditBones(self->Bones))
		return NULL;
	self->Bones->editmode_flag = 1;
	return EXPP_incr_ret(Py_None);

AttributeError:
	return EXPP_objError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "The armature cannot be placed manually in editmode before you call makeEditable()!");
}

//------------------------Armature.update()
//This is a bit ugly because you need an object link to do this
static PyObject *Armature_update(BPyArmatureObject *self)
{
	Object *obj = NULL;

	for (obj = G.main->object.first; obj; obj = obj->id.next){
		if (obj->data == self->armature)
			break;
	}
	if (obj){
		editbones_to_armature (&self->Bones->editbones, obj);
		if (!BonesDict_InitBones(self->Bones))
			return NULL;
		self->Bones->editmode_flag = 0;
		BLI_freelistN(&self->Bones->editbones);
	}else{
		goto AttributeError;

	}
	return EXPP_incr_ret(Py_None);

AttributeError:
	return EXPP_objError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "The armature must be linked to an object before you can save changes!");
}

//------------------------Armature.__copy__()
static PyObject *Armature_copy(BPyArmatureObject *self)
{
	bArmature *bl_armature;
	bl_armature= copy_armature(self->armature);
	bl_armature->id.us= 0;
	return Armature_CreatePyObject( bl_armature );
}

//------------------ATTRIBUTE IMPLEMENTATION---------------------------
//------------------------Armature.autoIK (getter)
static PyObject *Armature_getAutoIK(BPyArmatureObject *self, void *closure)
{
	if (self->armature->flag & ARM_AUTO_IK)
		return EXPP_incr_ret(Py_True);
	else
		return EXPP_incr_ret(Py_False);
}
//------------------------Armature.autoIK (setter)
static int Armature_setAutoIK(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->flag |= ARM_AUTO_IK;
				return 0;
			}else if (value == Py_False){
				self->armature->flag &= ~ARM_AUTO_IK;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}
//------------------------Armature.layers (getter)
static PyObject *Armature_getLayers(BPyArmatureObject *self, void *closure)
{
	int layers, bit = 0, val = 0;
	PyObject *item = NULL, *laylist = PyList_New( 0 );

	if( !laylist )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
			"couldn't create pylist!" );

	layers = self->armature->layer;

	while( bit < 20 ) {
		val = 1 << bit;
		if( layers & val ) {
			item = Py_BuildValue( "i", bit + 1 );
			PyList_Append( laylist, item );
			Py_DECREF( item );
		}
		bit++;
	}
	return laylist;
}
//------------------------Armature.layer (setter)
static int Armature_setLayers(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyList_Check(value)){
			int layers = 0, len_list = 0;
			int val;
			PyObject *item = NULL;

			len_list = PyList_Size(value);

			if( len_list == 0 )
				return EXPP_ReturnIntError( PyExc_AttributeError,
				  "list can't be empty, at least one layer must be set" );

			while( len_list ) {
				--len_list;
				item = PyList_GetItem( value, len_list );
				if( !PyInt_Check( item ) )
					return EXPP_ReturnIntError( PyExc_AttributeError,
							"list must contain only integer numbers" );

				val = ( int ) PyInt_AsLong( item );
				if( val < 1 || val > 20 )
					return EXPP_ReturnIntError( PyExc_AttributeError,
						  "layer values must be in the range [1, 20]" );

				layers |= 1 << ( val - 1 );
			}

			/* update any bases pointing to our object */
			self->armature->layer = (short)layers;

			return 0;
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_ReturnIntError( PyExc_TypeError,
			"expected a list of integers" );
}

//------------------------Armature.mirrorEdit (getter)
static PyObject *Armature_getMirrorEdit(BPyArmatureObject *self, void *closure)
{
	if (self->armature->flag & ARM_MIRROR_EDIT)
		return EXPP_incr_ret(Py_True);
	else
		return EXPP_incr_ret(Py_False);
}
//------------------------Armature.mirrorEdit (setter)
static int Armature_setMirrorEdit(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->flag |= ARM_MIRROR_EDIT;
				return 0;
			}else if (value == Py_False){
				self->armature->flag &= ~ARM_MIRROR_EDIT;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
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

static constIdents drawTypesIdents[] = {
	{"BOUNDBOX",	{(int)ARM_OCTA}},
	{"STICK",		{(int)ARM_LINE}},
	{"BBONE",		{(int)ARM_B_BONE}},
	{"ENVELOPE",	{(int)ARM_ENVELOPE}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition drawTypes = {
	EXPP_CONST_INT, "DrawTypes",
		sizeof(drawTypesIdents)/sizeof(constIdents), drawTypesIdents
};

//------------------------Armature.drawType (getter)
static PyObject *Armature_getDrawType(BPyArmatureObject *self)
{
	return PyConst_NewInt( &drawTypes, self->armature->drawtype );
}
//------------------------Armature.drawType (setter)
static int Armature_setDrawType(BPyArmatureObject *self, PyObject *value)
{
	constValue *c = Const_FromPyObject( &drawTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected DrawType constant or string" );
	
	self->armature->drawtype = c->i;
	return 0;
}

//------------------------Armature.ghostStep (getter)
static PyObject *Armature_getStep(BPyArmatureObject *self, void *closure)
{
	return PyInt_FromLong((long)self->armature->ghostsize);
}
//------------------------Armature.ghostStep (setter)
static int Armature_setStep(BPyArmatureObject *self, PyObject *value, void *closure)
{
	long numerical_value;

	if(value){
		if(PyInt_Check(value)){
			numerical_value = PyInt_AS_LONG(value);
			if (numerical_value > 20.0f || numerical_value < 1.0f)
				goto ValueError;
			self->armature->ghostsize = (short)numerical_value;
			return 0;
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects Integer");

ValueError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Argument must fall within 1-20");
}
//------------------------Armature.ghost (getter)
static PyObject *Armature_getGhost(BPyArmatureObject *self, void *closure)
{
	return PyInt_FromLong((long)self->armature->ghostep);
}
//------------------------Armature.ghost (setter)
static int Armature_setGhost(BPyArmatureObject *self, PyObject *value, void *closure)
{
	long numerical_value;

	if(value){
		if(PyInt_Check(value)){
			numerical_value = PyInt_AS_LONG(value);
			if (numerical_value > 30.0f || numerical_value < 0.0f)
				goto ValueError;
			self->armature->ghostep = (short)numerical_value;
			return 0;
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects Integer");

ValueError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Argument must fall within 0-30");
}
//------------------------Armature.drawNames (getter)
static PyObject *Armature_getDrawNames(BPyArmatureObject *self, void *closure)
{
	if (self->armature->flag & ARM_DRAWNAMES)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//------------------------Armature.drawNames (setter)
static int Armature_setDrawNames(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->flag |= ARM_DRAWNAMES;
				return 0;
			}else if (value == Py_False){
				self->armature->flag &= ~ARM_DRAWNAMES;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}
//------------------------Armature.drawAxes (getter)
static PyObject *Armature_getDrawAxes(BPyArmatureObject *self, void *closure)
{
	if (self->armature->flag & ARM_DRAWAXES)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//------------------------Armature.drawAxes (setter)
static int Armature_setDrawAxes(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->flag |= ARM_DRAWAXES;
				return 0;
			}else if (value == Py_False){
				self->armature->flag &= ~ARM_DRAWAXES;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}
//------------------------Armature.delayDeform (getter)
static PyObject *Armature_getDelayDeform(BPyArmatureObject *self, void *closure)
{
	if (self->armature->flag & ARM_DELAYDEFORM)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//------------------------Armature.delayDeform (setter)
static int Armature_setDelayDeform(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->flag |= ARM_DELAYDEFORM;
				return 0;
			}else if (value == Py_False){
				self->armature->flag &= ~ARM_DELAYDEFORM;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}
//------------------------Armature.restPosition (getter)
static PyObject *Armature_getRestPosition(BPyArmatureObject *self, void *closure)
{
	if (self->armature->flag & ARM_RESTPOS)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//------------------------Armature.restPosition (setter)
static int Armature_setRestPosition(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->flag |= ARM_RESTPOS;
				return 0;
			}else if (value == Py_False){
				self->armature->flag &= ~ARM_RESTPOS;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}
//------------------------Armature.envelopes (getter)
static PyObject *Armature_getEnvelopes(BPyArmatureObject *self, void *closure)
{
	if (self->armature->deformflag & ARM_DEF_ENVELOPE)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//------------------------Armature.envelopes (setter)
static int Armature_setEnvelopes(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->deformflag |= ARM_DEF_ENVELOPE;
				return 0;
			}else if (value == Py_False){
				self->armature->deformflag &= ~ARM_DEF_ENVELOPE;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}
//------------------------Armature.vertexGroups (getter)
static PyObject *Armature_getVertexGroups(BPyArmatureObject *self, void *closure)
{
	if (self->armature->deformflag & ARM_DEF_VGROUP)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
//------------------------Armature.vertexGroups (setter)
static int Armature_setVertexGroups(BPyArmatureObject *self, PyObject *value, void *closure)
{
	if(value){
		if(PyBool_Check(value)){
			if (value == Py_True){
				self->armature->deformflag |= ARM_DEF_VGROUP;
				return 0;
			}else if (value == Py_False){
				self->armature->deformflag &= ~ARM_DEF_VGROUP;
				return 0;
			}
		}
	}
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureBadArgs, "Expects True or False");
}

//------------------------Armature.bones (getter)
//Gets the name of the armature
static PyObject *Armature_getBoneDict(BPyArmatureObject *self, void *closure)
{
    return EXPP_incr_ret((PyObject*)self->Bones);
}
//------------------------Armature.bones (setter)
//Sets the name of the armature
/*TODO*/
/*Copy Bones through x = y*/
static int Armature_setBoneDict(BPyArmatureObject *self, PyObject *value, void *closure)
{
	goto AttributeError;

AttributeError:
	return EXPP_intError(PyExc_AttributeError, "%s%s", 
		sArmatureError, "You are not allowed to change the .Bones attribute");
}

//------------------------Bone.layerMask (get)
static PyObject *Armature_getLayerMask(BPyArmatureObject *self)
{
	/* do this extra stuff because the short's bits can be negative values */
	unsigned short laymask = 0;
	laymask |= self->armature->layer;
	return PyInt_FromLong((int)laymask);
}
//------------------------Bone.layerMask (set)
static int Armature_setLayerMask(BPyArmatureObject *self, PyObject *value)
{
	int laymask;
	if (!PyInt_Check(value)) {
		return EXPP_ReturnIntError( PyExc_AttributeError,
									"expected an integer (bitmask) as argument" );
	}
	
	laymask = PyInt_AsLong(value);

	if (laymask <= 0 || laymask > (1<<16) - 1)
		return EXPP_ReturnIntError( PyExc_AttributeError,
									"bitmask must have from 1 up to 16 bits set");

	self->armature->layer = 0;
	self->armature->layer |= laymask;

	return 0;
}

//------------------TYPE_OBECT IMPLEMENTATION--------------------------
//------------------------tp_doc
//The __doc__ string for this object
static char BPyArmature_doc[] = "This object wraps a Blender Armature object.";
//------------------------tp_methods
//This contains a list of all methods the object contains
static PyMethodDef BPyArmature_methods[] = {
	{"makeEditable", (PyCFunction) Armature_makeEditable, METH_NOARGS, 
		"() - Unlocks the ability to modify armature bones"},
	{"update", (PyCFunction) Armature_update, METH_NOARGS, 
		"() - Rebuilds the armature based on changes to bones since the last call to makeEditable"},
	{"__copy__", (PyCFunction) Armature_copy, METH_NOARGS, 
		"() - Return a copy of the armature."},
	{"copy", (PyCFunction) Armature_copy, METH_NOARGS, 
		"() - Return a copy of the armature."},
	{NULL, NULL, 0, NULL}
};
//------------------------tp_getset
//This contains methods for attributes that require checking
static PyGetSetDef BPyArmature_getset[] = {
	{"bones", (getter)Armature_getBoneDict, (setter)Armature_setBoneDict, 
		"The armature's Bone dictionary", NULL},
	{"vertexGroups", (getter)Armature_getVertexGroups, (setter)Armature_setVertexGroups, 
		"Enable/Disable vertex group defined deformation", NULL},
	{"envelopes", (getter)Armature_getEnvelopes, (setter)Armature_setEnvelopes, 
		"Enable/Disable bone envelope defined deformation", NULL},
	{"restPosition", (getter)Armature_getRestPosition, (setter)Armature_setRestPosition, 
		"Show armature rest position - disables posing", NULL},
	{"delayDeform", (getter)Armature_getDelayDeform, (setter)Armature_setDelayDeform, 
		"Don't deform children when manipulating bones in pose mode", NULL},
	{"drawAxes", (getter)Armature_getDrawAxes, (setter)Armature_setDrawAxes, 
		"Enable/Disable  drawing  the bone axes", NULL},
	{"drawNames", (getter)Armature_getDrawNames, (setter)Armature_setDrawNames, 
		"Enable/Disable  drawing the bone names", NULL},
	{"ghost", (getter)Armature_getGhost, (setter)Armature_setGhost, 
		"Draw a number of ghosts around the current frame for current Action", NULL},
	{"ghostStep", (getter)Armature_getStep, (setter)Armature_setStep, 
		"The number of frames between ghost instances", NULL},
	{"drawType", (getter)Armature_getDrawType, (setter)Armature_setDrawType, 
		"The type of drawing currently applied to the armature", NULL},
	{"mirrorEdit", (getter)Armature_getMirrorEdit, (setter)Armature_setMirrorEdit, 
		"Enable/Disable X-axis mirrored editing", NULL},
	{"autoIK", (getter)Armature_getAutoIK, (setter)Armature_setAutoIK, 
		"Adds temporal IK chains while grabbing bones", NULL},
	{"layers", (getter)Armature_getLayers, (setter)Armature_setLayers, 
		"List of layers for the armature", NULL},
	{"layerMask", (getter)Armature_getLayerMask, (setter)Armature_setLayerMask, 
		"Layer bitmask", NULL },
	{NULL}
};

//------------------------tp_dealloc
//This tells how to 'tear-down' our object when ref count hits 0
///tp_dealloc
static void Armature_dealloc(BPyArmatureObject * self)
{
	if (self->weaklist != NULL)
		PyObject_ClearWeakRefs((PyObject *) self); /* this causes the weakref dealloc func to be called */
	
	Py_DECREF(self->Bones);
	PyObject_DEL( self );
}

/* this types constructor */
static PyObject *Armature_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Armature";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_armature( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new armature data" );

	id->us= 0;
	return Armature_CreatePyObject((bArmature *)id);
}

//------------------TYPE_OBECT DEFINITION--------------------------
PyTypeObject BPyArmature_Type = {
	PyObject_HEAD_INIT(NULL)		//tp_head
	0,								//tp_internal
	"Armature",						//tp_name
	sizeof(BPyArmatureObject),			//tp_basicsize
	0,								//tp_itemsize
	(destructor)Armature_dealloc,	//tp_dealloc
	0,								//tp_print
	0,								//tp_getattr
	0,								//tp_setattr
	0,								//tp_compare
	0,								//tp_repr
	0,								//tp_as_number
	0,								//tp_as_sequence
	0,								//tp_as_mapping
	0,								//tp_hash
	0,								//tp_call
	0,								//tp_str
	0,								//tp_getattro
	0,								//tp_setattro
	0,								//tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_WEAKREFS | Py_TPFLAGS_BASETYPE,				//tp_flags
	BPyArmature_doc,				//tp_doc
	0,								//tp_traverse
	0,								//tp_clear
	0, 								//tp_richcompare
	offsetof(BPyArmatureObject, weaklist),	//tp_weaklistoffset
	0,								//tp_iter
	0,								//tp_iternext
	BPyArmature_methods,			//tp_methods
	0,								//tp_members
	BPyArmature_getset,				//tp_getset
	&BPyGenericLib_Type,			//tp_base
	0,								//tp_dict
	0,								//tp_descr_get
	0,								//tp_descr_set
	0,								//tp_dictoffset
	0,								//tp_init
	0,								//tp_alloc
	(newfunc)Armature_new,			//tp_new
	0,								//tp_free
	0,								//tp_is_gc
	0,								//tp_bases
	0,								//tp_mro
	0,								//tp_cache
	0,								//tp_subclasses
	0,								//tp_weaklist
	0								//tp_del
};

//------------------VISIBLE PROTOTYPE IMPLEMENTATION-----------------------
//------------------------Armature_RebuildEditbones (internal)
PyObject * Armature_RebuildEditbones(PyObject *pyarmature)
{
	return Armature_makeEditable((BPyArmatureObject*)pyarmature);
}

//------------------------Armature_RebuildBones (internal)
PyObject *Armature_RebuildBones(PyObject *pyarmature)
{
	return Armature_update((BPyArmatureObject*)pyarmature);
}

/* internal func to remove weakref from weakref list */
PyObject * arm_weakref_callback_weakref_dealloc(PyObject *self, PyObject *weakref)
{
	char *list_name = ARM_WEAKREF_LIST_NAME;
	PyObject *maindict = NULL, *armlist = NULL;
	int i;
	
	maindict= PyModule_GetDict(PyImport_AddModule(	"__main__"));
	armlist = PyDict_GetItemString(maindict, list_name);
	if( !armlist){
		printf("Oops - update_armature_weakrefs()\n");
		Py_RETURN_NONE;
	}
	
	i = PySequence_Index(armlist, weakref);
	if (i==-1) {
		printf("callback weakref internal error, weakref not in list\n\tthis should never happen.\n");
		Py_RETURN_NONE;
	}
	PySequence_DelItem(armlist, i);
	
	/*PyObject_Print(	armlist, stderr, 0);*/
	
	Py_RETURN_NONE;
}
/*-----------------(internal)
 * Converts a bArmature to a PyArmature */

PyObject *Armature_CreatePyObject(struct bArmature *armature)
{
	BPyArmatureObject *py_armature = NULL;
	PyObject *maindict = NULL, *weakref = NULL;
	PyObject *armlist = NULL;  /* list of armature weak refs */
	char *list_name = ARM_WEAKREF_LIST_NAME;
	int i;

	//put a weakreference in __main__
	maindict= PyModule_GetDict(PyImport_AddModule(	"__main__"));

	armlist = PyDict_GetItemString(maindict, list_name);
	if(!armlist) {
		printf("Oops - can't get the armature weakref list\n");
		goto RuntimeError;
	}

	/* see if we alredy have it */
	for (i=0; i< PyList_Size(armlist); i++) { 
		py_armature = (BPyArmatureObject *)PyWeakref_GetObject(PyList_GET_ITEM(armlist, i));
		if (BPyArmature_Check(py_armature) && py_armature->armature == armature) {
			Py_INCREF(py_armature);
			/*printf("reusing armature\n");*/
			return (PyObject *)py_armature;
		}
	}

	
	/*create armature type*/
	py_armature = PyObject_NEW( BPyArmatureObject, &BPyArmature_Type );
	
	if (!py_armature){
		printf("Oops - can't create py armature\n");
		goto RuntimeError;
	}

	py_armature->armature = armature;
	py_armature->weaklist = NULL; //init the weaklist
	
	//create armature.bones
	py_armature->Bones = (BPyBoneDictObject*)PyBonesDict_FromPyArmature(py_armature);
	if (!py_armature->Bones){
		printf("Oops - creating armature.bones\n");
		goto RuntimeError;
	}
	
	weakref = PyWeakref_NewRef((PyObject*)py_armature, arm_weakref_callback_weakref_dealloc__pyfunc);
	if (PyList_Append(armlist, weakref) == -1){
		printf("Oops - list-append failed\n");
		goto RuntimeError;
	}
	Py_DECREF(weakref);
	
	return (PyObject *) py_armature;

RuntimeError:
	return EXPP_objError(PyExc_RuntimeError, "%s%s%s", 
		sModuleError, "Armature_CreatePyObject: ", "Internal Error Ocurred");
}

/* internal use only */
static PyMethodDef bpy_arm_weakref_callback_weakref_dealloc[] = {
	{"arm_weakref_callback_weakref_dealloc", arm_weakref_callback_weakref_dealloc, METH_O, ""}
};

/*****************************************************************************/
/* ObjectType_Init(): add constants to the type at run-rime and initialize   */
/*****************************************************************************/
PyObject *ArmatureType_Init( void )
{	
	/* only allocate new dictionary once */
	if( BPyArmature_Type.tp_dict == NULL ) {
		BPyArmature_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyArmature_Type.tp_dict, &drawTypes );
		PyType_Ready( &BPyArmature_Type ) ;
		
		/* Weakref management - used for callbacks so we can
		 * tell when a callback has been removed that a UI button referenced */
		arm_weakref_callback_weakref_dealloc__pyfunc = PyCFunction_New(bpy_arm_weakref_callback_weakref_dealloc, NULL);
	}
	return (PyObject *) &BPyArmature_Type ;
}

PyObject *BoneDictType_Init( void )
{
	PyType_Ready( &BPyBoneDict_Type );
	return (PyObject *) &BPyBoneDict_Type;
}


