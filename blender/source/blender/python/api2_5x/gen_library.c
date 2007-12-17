#include "gen_library.h"
#include "gen_utils.h" /*This must come first*/

/* use for GenericLib_getProperties */
#include "BKE_idprop.h"
#include "IDProp.h"

#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_library.h"
#include "BKE_curve.h"
#include "BKE_utildefines.h"

/* GenericLib */
#include "World.h"
#include "Font.h"
#include "Metaball.h"
#include "Curve.h"
#include "Camera.h"
#include "NLA.h"
#include "Lattice.h"
#include "Armature.h"
#include "Lamp.h"
#include "Text.h"
#include "CurveText.h"
#include "Sound.h"
#include "Scene.h"
#include "Mesh.h"
#include "Group.h"
#include "Object.h"
#include "Texture.h"
#include "Ipo.h"
#include "DNA_object_types.h"
#include "DNA_ipo_types.h"
#include "bpy_list.h"
#include "layer_set.h"

/* 
 * returns the Blender lib type code from a PyObject
 * -1 for no match, only give this function libdata
 * 
 * At the moment this is only used by GenericLib_assignData
 * so not all types are needed.
 */
short GenericLib_getType(PyObject * pydata)
{
	//~ if (BPyScene_Check(pydata))	return ID_SCE;
	if (BPyObject_Check(pydata))	return ID_OB;
	if (BPyMesh_Check(pydata))		return ID_ME;
	//~ if (BPyCurve_Check(pydata))	return ID_CU;
	//~ if (BPyMetaball_Check(pydata))	return ID_MB;
	//~ if (BPyMaterial_Check(pydata))	return ID_MA;
	if (BPyTexture_Check(pydata))	return ID_TE;
	//~ if (BPyImage_Check(pydata))	return ID_IM;
		//~ //if (BPyLattice_Check(pydata))	return ID_LT;
	//~ if (BPyLamp_Check(pydata))		return ID_LA;
	//~ if (BPyCamera_Check(pydata))	return ID_CA;
	if (BPyIpo_Check(pydata))		return ID_IP;
	if (BPyWorld_Check(pydata))	return ID_WO;
		//~ //if (BPyFont_Check(pydata))		return ID_VF;
	//~ if (BPyText_Check(pydata))		return ID_TXT;
	//~ if (BPySound_Check(pydata))	return ID_SO;
	if (BPyGroup_Check(pydata))	return ID_GR;
	//~ if (BPyArmature_Check(pydata))	return ID_AR;
	if (BPyAction_Check(pydata))	return ID_AC;
	
	return -1;
}

PyObject *GenericLib_getLayers( void *self )
{
	return LayerSet_CreatePyObject((BPyGenericLibObject *)self);
}

int GenericLib_setLayers( void *self, PyObject *value )
{
	return LayerSet_AssignPyObject(self, value);
}

/*
 * This function is used to assign a PyObject representing
 * blender libdata to a pointer.
 * 
 * Python examples of this are...
 * 		ob.DupGroup = dupliGroup
 * 		mesh.texMesh = texme
 * 		ob.ipo = None 
 * 
 * This function deals with type checking, data usercounts,
 * and raising errors.
 * 
 * value - python value
 * data - Blender pointer to assign value to
 * ndata - Use this if there is a value data cannot be.
 * 		for instance, a curve's curve modifier cant point to its self.
 * refcount - non zero values will modify blenders user count.
 * type - ID type.
 * subtype - used only for objects and IPO's to stop the wrong types of obs/ipos
 * 		being assigned.
 * 
 */
int GenericLib_assignData(PyObject *value, void **data, void **ndata, short refcount, short type, short subtype)
{
	ID *id= NULL;
	
	if (*data) {
		id = ((ID*)*data);
		
		if (ndata && *data == *ndata) {
			return EXPP_ReturnIntError( PyExc_TypeError,
				"Cannot set this data to its self" );
		}
	}
	
	if (value == Py_None) {
		*data = NULL;
		if (refcount && id) id->us--;
	} else if (GenericLib_getType(value) == type) {
		
		/* object subtypes */
		if (subtype != 0) {
			if (type == ID_OB) {
				Object *ob= (Object *)(((BPyGenericLibObject *)value)->id);
				if (ob->type != subtype)
					return EXPP_ReturnIntError( PyExc_TypeError,
						"Object type not supported" );
			}
			
			if (type == ID_IP) {
				Ipo *ipo = (Ipo *)(((BPyGenericLibObject *)value)->id);
				if (ipo->blocktype != subtype)
					return EXPP_ReturnIntError( PyExc_TypeError,
						"Ipo type does is not compatible" );
			}
			
			
		}
		if (refcount && id) id->us--;
		id = ((BPyGenericLibObject *)value)->id;
		id->us++;
		*data = id;
	} else {
		return EXPP_ReturnIntError( PyExc_TypeError,
				"Could not assign Python Type - None or Library Object" );
	}
	return 0;
} 

char * GenericLib_libname( short type )
{
	switch (type) {
	case ID_OB:
		return "Object";
	case ID_SCE:
		return "Scene";
	case ID_ME:
		return "Mesh";
	case ID_CU:
		return "Curve";
	case ID_MB:
		return "Metaball";
	case ID_MA:
		return "Material";
	case ID_TE:
		return "Texture";
	case ID_IM: 
		return "Image";
	case ID_LT:
		return "Lattice";
	case ID_LA:
		return "Lamp";
	case ID_CA:
		return "Camera";
	case ID_IP:
		return "Ipo";
	case ID_WO:
		return "World";
	case ID_VF:
		return "Font";
	case ID_TXT:
		return "Text";
	case ID_SO:
		return "Sound";
	case ID_GR:	
		return "Group";
	case ID_AR:
		return "Armature";
	case ID_AC:
		return "Action";
	default:
		return "Unknown";
	}
}

PyObject *GetPyObjectFromID( ID * id )
{
	switch ( MAKE_ID2( id->name[0], id->name[1] ) ) {
	case ID_SCE:
		return Scene_CreatePyObject( ( Scene *) id );
	case ID_OB:
		return Object_CreatePyObject( (Object *) id );
	case ID_ME:
		return Mesh_CreatePyObject( (Mesh *)id, NULL );
	case ID_CU:
		switch (curve_type((Curve *)id)) {
		case OB_FONT:
			return CurveText_CreatePyObject( (Text3d *)id );
		default:
			return Curve_CreatePyObject( (Curve *)id );
		}
	case ID_MB:
		return Metaball_CreatePyObject((MetaBall *)id);
	case ID_MA:
		return Material_CreatePyObject((Material *)id);
	case ID_TE:
		return Texture_CreatePyObject((Tex *)id);
	case ID_IM:
		return Image_CreatePyObject((Image *)id);
	case ID_LT:
		return Lattice_CreatePyObject((Lattice *)id);
	case ID_LA:
		return Lamp_CreatePyObject((Lamp *)id);
	case ID_CA:
		return Camera_CreatePyObject((Camera *)id);
	case ID_IP:
		return Ipo_CreatePyObject((Ipo *)id);
	case ID_WO:
		return World_CreatePyObject((World *)id);
	case ID_VF:
		return Font_CreatePyObject((VFont *)id);
	case ID_TXT:
		return Text_CreatePyObject((Text *)id);
	case ID_SO:
		return Sound_CreatePyObject((bSound *)id);
	case ID_GR:
		return Group_CreatePyObject((Group *)id);
	case ID_AR:
		return Armature_CreatePyObject((bArmature *)id);
	case ID_AC:
		return Action_CreatePyObject((bAction *)id);
	}
	Py_RETURN_NONE;
}

/*
 * 
 * genlib pytype - only to be subtyped
 * 
 * 
 */

static PyObject *GenericLib_repr( BPyGenericLibObject * self )
{
	if (self->id)
		return PyString_FromFormat( "[%s \"%s\"]",
				GenericLib_libname(GS(self->id->name)), self->id->name + 2 );
	
	return PyString_FromString("[<deleted>]");
}


/* return a unique tuple for this libdata*/
static long GenericLib_hash(BPyGenericLibObject * pydata)
{
	return (long)pydata->id;
}

/* Generic get/set attrs */
PyObject *GenericLib_getName( void *self )
{	
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "data has been removed" ) );
	return PyString_FromString( id->name + 2 );
}

int GenericLib_setName( void *self, PyObject *value )
{
	ID *id = ((BPyGenericLibObject *)self)->id;
	char *name = NULL;
	if (!id) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "data has been removed" ) );
	
	name = PyString_AsString ( value );
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );

	rename_id( id, name );

	return 0;
}

PyObject *GenericLib_getFakeUser( void *self )
{	
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "data has been removed" ) );
	if (id->flag & LIB_FAKEUSER)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

int GenericLib_setFakeUser( void *self, PyObject *value )
{
	int param;
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "data has been removed" ) );
	
	param = PyObject_IsTrue( value );
	if( param == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument in range [0,1]" );
	
	if (param) {
		if (!(id->flag & LIB_FAKEUSER)) {
			id->flag |= LIB_FAKEUSER;
			id_us_plus(id);
		}
	} else {
		if (id->flag & LIB_FAKEUSER) {
			id->flag &= ~LIB_FAKEUSER;
			id->us--;
		}
	}
	return 0;
}

PyObject *GenericLib_getTag( void *self )
{	
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "data has been removed" ) );
	if (id->flag & LIB_DOIT)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

int GenericLib_setTag( void *self, PyObject *value )
{
	int param;
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "data has been removed" ) );
	
	param = PyObject_IsTrue( value );
	if( param == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument in range [0,1]" );
	
	if (param)
		id->flag |= LIB_DOIT;
	else
		id->flag &= ~LIB_DOIT;
	return 0;
}

/* read only */
PyObject *GenericLib_getLib( void *self )
{	
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "data has been removed" ) );
	
	if (id->lib)
		return PyString_FromString(id->lib->name);
	else
		return EXPP_incr_ret( Py_None );
}

PyObject *GenericLib_getUsers( void *self )
{	
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "data has been removed" ) );
	return PyInt_FromLong(id->us);
}

PyObject *GenericLib_getProperties( void *self )
{	
	ID *id = ((BPyGenericLibObject *)self)->id;
	if (!id) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "data has been removed" ) );
	return IDProperty_CreatePyObject( id, IDP_GetProperties(id, 1), NULL );
}


static PyGetSetDef GenericLib_getset[] = {
	{"name",
	 (getter)GenericLib_getName, (setter)GenericLib_setName, "\
@ivar name: unique name within each blend file.\n\
	\n\
	The name is case sensitive and 21 characters maximum length.\n\
\n\
	B{Note}: a blend file may have naming collisions when external library data is used,\n\
	be sure to check the value of L{lib}.\n\
\n\
	B{Note}: Setting a value longer then 21 characters will be shortened\n\
@type name: string",
	 NULL},
	{"lib",
	 (getter)GenericLib_getLib, (setter)NULL, "\
@ivar lib: path to the blend file this datablock is stored in (readonly).\n\
\n\
	lib will be None unless you are using external blend files with (File, Append/Link)\n\
\n\
	B{Note}: the path may be relative, to get the full path use L{Blender.sys.expandpath<Sys.expandpath>}\n\
@type lib: string or None",
	 NULL},
	{"users",
	 (getter)GenericLib_getUsers, (setter)NULL, "\
@ivar users: The number of users this datablock has. (readonly)\n\
	Zero user datablocks are de-allocated after reloading and saving.\n\
@type users: int",
	 NULL},
	{"fakeUser",
	 (getter)GenericLib_getFakeUser, (setter)GenericLib_setFakeUser, "\
@ivar fakeUser: When set to True, this datablock wont be removed, even if nothing is using it.\n\
	All data has this disabled by default except for Actions.\n\
@type fakeUser: bool",
	 NULL},
	{"properties",
	 (getter)GenericLib_getProperties, (setter)NULL, "\
@ivar properties: Returns an L{IDGroup<IDProp.IDGroup>} reference to this\n\
datablocks's ID Properties.\n\
@type properties: L{IDGroup<IDProp.IDGroup>}",\
	 NULL},
	{"tag",
	 (getter)GenericLib_getTag, (setter)GenericLib_setTag, "\
@ivar tag: A temporary tag that to flag data as being used within a loop.\n\
	always set all tags to True or False before using since blender uses this flag for its own internal operations.\n\
@type tag: bool",
	 NULL},
	{NULL}  /* Sentinel */

};


PyTypeObject BPyGenericLib_Type = {
	PyObject_HEAD_INIT(NULL)
	0,
	"BPyGenlib",
	sizeof(BPyGenericLibObject),
	0,
	0,									/* tp_dealloc */
	0,									/* tp_print */
	0,									/* tp_getattr */
	0,									/* tp_setattr */
	( cmpfunc ) EXPP_Anonymous_compare,	/* tp_compare */
	( reprfunc ) GenericLib_repr,		/* tp_repr; */
	0,									/* tp_as_number */
	0,									/* tp_as_sequence */
	0,									/* tp_as_mapping */
	( hashfunc ) GenericLib_hash,		/* tp_hash */
	0,									/* tp_call */
	0,									/* tp_str */
	0,									/* tp_getattro */
	0,									/* tp_setattro */
	0,									/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* tp_flags */
	0,									/* tp_doc */
	0,									/* tp_traverse */
	0,									/* tp_clear */
	0,									/* tp_richcompare */
	0,									/* tp_weaklistoffset */
	0,									/* getiterfunc tp_iter; */
	0,									/* iternextfunc tp_iternext; */
	0,									/* tp_methods */
	0,									/* tp_members */
	GenericLib_getset,					/* tp_getset */
	0,									/* tp_base */ 
	0,									/* tp_dict */
	0,									/* tp_descr_get */
	0,									/* tp_descr_set */ 
	0,									/* tp_dictoffset */
	0,									/* tp_init */
	0,									/* tp_alloc */
	0,									/* tp_new */
};

/* Note that this type should never be accessed by the user,
 * only its subtypes */
PyObject *GenericLibType_Init( void  )
{
	PyType_Ready( &BPyGenericLib_Type ) ;
	return (PyObject *) &BPyGenericLib_Type ;
}



/* For internal use so far */

/* Materials */
Material ***GenericLib_materials( ID *id )
{
	/* only care about types that can have materials */
	switch ( GS(id->name) ) {
	case ID_OB:
		return &(((Object *)id)->mat);
	case ID_ME:
		return &(((Mesh *)id)->mat);
	case ID_CU:
		return &(((Curve *)id)->mat);
	case ID_MB:
		return &(((MetaBall *)id)->mat);
	}
	return NULL;
}

short GenericLib_totcol( ID *id )
{
	/* only care about types that can have materials */
	switch ( GS(id->name) ) {
	case ID_OB:
		return (short)(((Object *)id)->totcol); /* is a char */
	case ID_ME:
		return ((Mesh *)id)->totcol;
	case ID_CU:
		return ((Curve *)id)->totcol;
	case ID_MB:
		return ((MetaBall *)id)->totcol;
	}
	return -1;
}

/* Scriptlinks */
ScriptLink *GenericLib_scriptlinks(ID * id)
{
	ScriptLink *slink;
	
	switch ( GS(id->name) ) {
	case ID_SCE:
		slink = &((Scene *)id)->scriptlink;
		break;
	case ID_OB:
		slink = &((Object *)id)->scriptlink;
		break;
	case ID_MA:
		slink = &((Material *)id)->scriptlink;
		break;
	case ID_LA:
		slink = &((Lamp *)id)->scriptlink;
		break;
	case ID_CA:
		slink = &((Camera *)id)->scriptlink;
		break;
	case ID_WO:
		slink = &((World *)id)->scriptlink;
		break;
	default:
		slink = NULL;
	}
	return slink;
}



/* ScriptLink functions */
int BPyScriptLinkTuple_Check(PyObject *value)
{
	if (!PyTuple_Check(value) || PyTuple_Size(value) != 2) {
		return 0;
	} else {
		PyObject *v1 = PyTuple_GetItem(value, 0);
		PyObject *v2 = PyTuple_GetItem(value, 1);
		
		if (((v1 != Py_None) && !BPyText_Check(v1)) || !PyInt_Check(v2))
			return 0;
		
		return 1;
	}
}

PyObject *ScriptLinkTuple_CreatePyObject( BPyGenericLibObject * genlib, int index )
{	
	PyObject *var = PyTuple_New(2);
	ScriptLink * slink = GenericLib_scriptlinks(genlib->id);
	
	/* slink->scripts[index] can be NULL, will just make Text_CreatePyObject ret None */
	PyTuple_SetItem(var, 0, Text_CreatePyObject((Text *)(slink->scripts[index])));
	
	PyTuple_SetItem(var, 1, PyInt_FromLong(slink->flag[index]));
	return var;
}

void ScriptLinkTuple_ToScriptLink( ScriptLink * slink, int index, PyObject * pyob )
{	
	/* This must be alredy checked with ScriptLinkTuple_Check */
	PyObject *v1 = PyTuple_GetItem(pyob, 0);
	PyObject *v2 = PyTuple_GetItem(pyob, 1);
	
	if (BPyText_Check(v1))
		slink->scripts[index] = (ID *)(((BPyTextObject *)v1)->text);
	else
		slink->scripts[index] = NULL;
	
	slink->flag[index] = (short)PyInt_AsLong(v2);
}
