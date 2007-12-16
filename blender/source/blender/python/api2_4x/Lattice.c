/* 
 * $Id: Lattice.c 12898 2007-12-15 21:44:40Z campbellbarton $
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

#include "Lattice.h" /*This must come first*/

#include "BKE_utildefines.h"
#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_lattice.h"
#include "BLI_blenlib.h"
#include "DNA_object_types.h"
#include "DNA_key_types.h"
#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"
#include "DNA_curve_types.h"
#include "DNA_scene_types.h"
#include "BIF_editkey.h"
#include "BIF_editdeform.h"
#include "BIF_space.h"
#include "blendef.h"
#include "gen_utils.h"
#include "gen_library.h"

#include "Key.h"

/*****************************************************************************/
/* Python API function prototypes for the Lattice module.	*/
/*****************************************************************************/
static PyObject *V24_M_Lattice_New( PyObject * self, PyObject * args );
static PyObject *V24_M_Lattice_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/*	Lattice Module strings	 */
/* The following string definitions are used for documentation strings.	 */
/* In Python these will be written to the console when doing a		 */
/* Blender.Lattice.__doc__	*/
/*****************************************************************************/
static char V24_M_Lattice_doc[] = "The Blender Lattice module\n\n";

static char V24_M_Lattice_New_doc[] = "() - return a new Lattice object";

static char V24_M_Lattice_Get_doc[] = "() - get a Lattice from blender";

/*****************************************************************************/
/* Python method structure definition for Blender.Lattice module:	*/
/*****************************************************************************/
struct PyMethodDef V24_M_Lattice_methods[] = {
	{"New", ( PyCFunction ) V24_M_Lattice_New, METH_VARARGS,
	 V24_M_Lattice_New_doc},
	{"Get", ( PyCFunction ) V24_M_Lattice_Get, METH_VARARGS,
	 V24_M_Lattice_Get_doc},
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/*  Lattice Strings			 */
/* The following string definitions are used for documentation strings.	 */
/* In Python these will be written to the console when doing a		 */
/* Blender.Lattice.__doc__			*/
/*****************************************************************************/
static char V24_Lattice_getName_doc[] = "() - Return Lattice Object name";

static char V24_Lattice_setName_doc[] = "(str) - Change Lattice Object name";

static char V24_Lattice_setPartitions_doc[] =
	"(str) - Set the number of Partitions in x,y,z";

static char V24_Lattice_getPartitions_doc[] =
	"(str) - Get the number of Partitions in x,y,z";

static char V24_Lattice_getKey_doc[] =
	"() - Get the Key object attached to this Lattice";

static char V24_Lattice_setKeyTypes_doc[] =
	"(str) - Set the key types for x,y,z dimensions";

static char V24_Lattice_getKeyTypes_doc[] =
	"(str) - Get the key types for x,y,z dimensions";

static char V24_Lattice_setMode_doc[] = "(str) - Make an outside or grid lattice";

static char V24_Lattice_getMode_doc[] = "(str) - Get lattice mode type";

static char V24_Lattice_setPoint_doc[] =
	"(str) - Set the coordinates of a point on the lattice";

static char V24_Lattice_getPoint_doc[] =
	"(str) - Get the coordinates of a point on the lattice";

static char V24_Lattice_insertKey_doc[] =
	"(str) - Set a new key for the lattice at specified frame";

static char V24_Lattice_copy_doc[] =
	"() - Return a copy of the lattice.";

//***************************************************************************
// Function:      V24_Lattice_CreatePyObject   
//***************************************************************************
PyObject *V24_Lattice_CreatePyObject( Lattice * lt )
{
	V24_BPy_Lattice *pyLat;

	pyLat = ( V24_BPy_Lattice * ) PyObject_NEW( V24_BPy_Lattice, &V24_Lattice_Type );

	if( !pyLat )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create V24_BPy_Lattice PyObject" );

	pyLat->lattice = lt;

	return ( PyObject * ) pyLat;
}

//***************************************************************************
// Function:       V24_Lattice_FromPyObject     
//***************************************************************************

Lattice *V24_Lattice_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Lattice * ) pyobj )->lattice;
}

//***************************************************************************
// Function:       V24_M_Lattice_New      
// Python equivalent:          Blender.Lattice.New 
//***************************************************************************
static PyObject *V24_M_Lattice_New( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Lattice *bl_Lattice;	// blender Lattice object 
	PyObject *py_Lattice;	// python wrapper 

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "expected string and int arguments (or nothing)" );

	bl_Lattice = add_lattice( "Lattice" );

	if( bl_Lattice ) {
		bl_Lattice->id.us = 0;
		py_Lattice = V24_Lattice_CreatePyObject( bl_Lattice );
	} else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Lattice Object in Blender" );
	if( !py_Lattice )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Lattice Object wrapper" );

	if( name )
		rename_id( &bl_Lattice->id, name );

	return py_Lattice;
}

//***************************************************************************
// Function:   V24_M_Lattice_Get   
// Python equivalent:        Blender.Lattice.Get  
//***************************************************************************
static PyObject *V24_M_Lattice_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Lattice *lat_iter;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected string argument (or nothing)" ) );

	lat_iter = G.main->latt.first;

	if( name ) {		/* (name) - Search Lattice by name */

		PyObject *wanted_lat = NULL;

		while( ( lat_iter ) && ( wanted_lat == NULL ) ) {
			if( strcmp( name, lat_iter->id.name + 2 ) == 0 ) {
				wanted_lat =
					V24_Lattice_CreatePyObject( lat_iter );
			}

			lat_iter = lat_iter->id.next;
		}

		if( wanted_lat == NULL ) {	/* Requested Lattice doesn't exist */
			char error_msg[64];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Lattice \"%s\" not found", name );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_NameError, error_msg ) );
		}

		return wanted_lat;
	}

	else {			/* () - return a list of all Lattices in the scene */
		int index = 0;
		PyObject *latlist, *pyobj;

		latlist = PyList_New( BLI_countlist( &( G.main->latt ) ) );

		if( latlist == NULL )
			return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
							"couldn't create PyList" ) );

		while( lat_iter ) {
			pyobj = V24_Lattice_CreatePyObject( lat_iter );

			if( !pyobj ) {
				Py_DECREF(latlist);
				return ( V24_EXPP_ReturnPyObjError
					 ( PyExc_MemoryError,
					   "couldn't create PyString" ) );
			}
			PyList_SET_ITEM( latlist, index, pyobj );

			lat_iter = lat_iter->id.next;
			index++;
		}

		return ( latlist );
	}
}

//***************************************************************************
// Function:       V24_Lattice_Init   
//***************************************************************************
PyObject *V24_Lattice_Init( void )
{
	PyObject *mod;
	PyObject *dict;
	
	if( PyType_Ready( &V24_Lattice_Type ) < 0 )
		return NULL;
	
	mod = Py_InitModule3( "Blender.Lattice", V24_M_Lattice_methods,
				V24_M_Lattice_doc );
	
	dict = PyModule_GetDict( mod );

	//Module dictionary
#define EXPP_ADDCONST(x) V24_EXPP_dict_set_item_str(dict, #x, PyInt_FromLong(LT_##x))
	EXPP_ADDCONST( GRID );
	EXPP_ADDCONST( OUTSIDE );

#undef EXPP_ADDCONST
#define EXPP_ADDCONST(x) V24_EXPP_dict_set_item_str(dict, #x, PyInt_FromLong(KEY_##x))
	EXPP_ADDCONST( LINEAR );
	EXPP_ADDCONST( CARDINAL );
	EXPP_ADDCONST( BSPLINE );
	
	return ( mod );
}

static PyObject *Lattice_setPartitions( V24_BPy_Lattice * self, PyObject * args )
{
	int x = 0;
	int y = 0;
	int z = 0;
	Lattice *bl_Lattice;

	if( !PyArg_ParseTuple( args, "iii", &x, &y, &z ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int,int,int argument" ) );

	bl_Lattice = self->lattice;

	if( x < 2 || y < 2 || z < 2 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"partition values must be 2 or greater" ) );

	resizelattice(bl_Lattice, x, y, z, NULL);

	Py_RETURN_NONE;
}

static PyObject *Lattice_getPartitions( V24_BPy_Lattice * self )
{
	Lattice *bl_Lattice;
	bl_Lattice = self->lattice;

	return Py_BuildValue( "[i,i,i]", ( int ) bl_Lattice->pntsu,
			      ( int ) bl_Lattice->pntsv,
			      ( int ) bl_Lattice->pntsw );
}

static PyObject *Lattice_getKey( V24_BPy_Lattice * self )
{
	Key *key = self->lattice->key;

	if (key)
		return V24_Key_CreatePyObject(key);
	else
		Py_RETURN_NONE;
}

static PyObject *Lattice_getKeyTypes( V24_BPy_Lattice * self )
{
	Lattice *bl_Lattice;
	char *linear = "linear";
	char *cardinal = "cardinal";
	char *bspline = "bspline";
	char *s_x = NULL, *s_y = NULL, *s_z = NULL;

	bl_Lattice = self->lattice;

	if( ( bl_Lattice->typeu ) == KEY_LINEAR )
		s_x = linear;
	else if( ( bl_Lattice->typeu ) == KEY_CARDINAL )
		s_x = cardinal;
	else if( ( bl_Lattice->typeu ) == KEY_BSPLINE )
		s_x = bspline;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "bad key type..." );

	if( ( bl_Lattice->typev ) == KEY_LINEAR )
		s_y = linear;
	else if( ( bl_Lattice->typev ) == KEY_CARDINAL )
		s_y = cardinal;
	else if( ( bl_Lattice->typev ) == KEY_BSPLINE )
		s_y = bspline;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "bad key type..." );

	if( ( bl_Lattice->typew ) == KEY_LINEAR )
		s_z = linear;
	else if( ( bl_Lattice->typew ) == KEY_CARDINAL )
		s_z = cardinal;
	else if( ( bl_Lattice->typew ) == KEY_BSPLINE )
		s_z = bspline;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "bad key type..." );

	/* we made sure no s_[xyz] is NULL */
	return Py_BuildValue( "[s,s,s]", s_x, s_y, s_z );
}

static PyObject *V24_Lattice_setKeyTypes( V24_BPy_Lattice * self, PyObject * args )
{
	int x;
	int y;
	int z;
	Lattice *bl_Lattice;

	if( !PyArg_ParseTuple( args, "iii", &x, &y, &z ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int,int,int argument" ) );

	bl_Lattice = self->lattice;

	if( x == KEY_LINEAR )
		bl_Lattice->typeu = KEY_LINEAR;
	else if( x == KEY_CARDINAL )
		bl_Lattice->typeu = KEY_CARDINAL;
	else if( x == KEY_BSPLINE )
		bl_Lattice->typeu = KEY_BSPLINE;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "type must be LINEAR, CARDINAL OR BSPLINE" );

	if( y == KEY_LINEAR )
		bl_Lattice->typev = KEY_LINEAR;
	else if( y == KEY_CARDINAL )
		bl_Lattice->typev = KEY_CARDINAL;
	else if( y == KEY_BSPLINE )
		bl_Lattice->typev = KEY_BSPLINE;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "type must be LINEAR, CARDINAL OR BSPLINE" );

	if( z == KEY_LINEAR )
		bl_Lattice->typew = KEY_LINEAR;
	else if( z == KEY_CARDINAL )
		bl_Lattice->typew = KEY_CARDINAL;
	else if( z == KEY_BSPLINE )
		bl_Lattice->typew = KEY_BSPLINE;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "type must be LINEAR, CARDINAL OR BSPLINE" );

	Py_RETURN_NONE;
}

static PyObject *V24_Lattice_setMode( V24_BPy_Lattice * self, PyObject * args )
{
	short type;
	Lattice *bl_Lattice;
	bl_Lattice = self->lattice;

	if( !PyArg_ParseTuple( args, "h", &type ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument" ) );

	if( type == LT_GRID )
		bl_Lattice->flag = LT_GRID;
	else if( type == LT_OUTSIDE ) {
		bl_Lattice->flag = LT_OUTSIDE + LT_GRID;
		outside_lattice( bl_Lattice );
	} else
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "type must be either GRID or OUTSIDE" );

	Py_RETURN_NONE;
}

static PyObject *V24_Lattice_getMode(V24_BPy_Lattice * self)
{
	if( self->lattice->flag == 1 )
		return PyString_FromString( "Grid" );
	else if( self->lattice->flag == 3 )
		return PyString_FromString( "Outside" );
	Py_RETURN_NONE;
}

static PyObject *V24_Lattice_setPoint( V24_BPy_Lattice * self, PyObject * args )
{
	BPoint *bp, *bpoint;
	short size;
	Lattice *bl_Lattice;
	int index, x;
	float tempInt;
	PyObject *listObject;

	if( !PyArg_ParseTuple
	    ( args, "iO!", &index, &PyList_Type, &listObject ) )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected int & list argument" ) );

	if( !PyList_Check( listObject ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"2nd parameter should be a python list" ) );

	if( !( PyList_Size( listObject ) == 3 ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"Please pass 3 parameters in the list [x,y,z]" ) );

	//init
	bp = 0;
	bl_Lattice = self->lattice;

	//get bpoints
	bp = bl_Lattice->def;

	if( bp == 0 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"no lattice points!" ) );

	//calculate size of lattice
	size = bl_Lattice->pntsu * bl_Lattice->pntsv * bl_Lattice->pntsw;

	if( index < 0 || index > size )
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"index outside of lattice size!" ) );

	//get the bpoint
	while( index ) {
		index--;
		bp++;
	}
	bpoint = bp;

	for( x = 0; x < PyList_Size( listObject ); x++ ) {
		if( !
		    ( PyArg_Parse
		      ( ( PyList_GetItem( listObject, x ) ), "f",
			&tempInt ) ) )
			return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						      "python list integer not parseable" );
		bpoint->vec[x] = tempInt;
	}

	Py_RETURN_NONE;
}

static PyObject *V24_Lattice_getPoint( V24_BPy_Lattice * self, PyObject * args )
{
	BPoint *bp, *bpoint;
	short size;
	Lattice *bl_Lattice;
	int index;

	if( !PyArg_ParseTuple( args, "i", &index ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument" ) );

	//init
	bp = 0;
	bl_Lattice = self->lattice;

	//get bpoints
	bp = bl_Lattice->def;

	if( bp == 0 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"no lattice points!" ) );

	//calculate size of lattice
	size = bl_Lattice->pntsu * bl_Lattice->pntsv * bl_Lattice->pntsw;

	if( index < 0 || index > size )
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"index outside of lattice size!" ) );

	//get the bpoint
	while( index ) {
		index--;
		bp++;
	}
	bpoint = bp;

	if( bpoint == 0 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"bpoint does not exist" ) );

	return Py_BuildValue( "[f,f,f]", bp->vec[0], bp->vec[1], bp->vec[2] );
}

static PyObject *V24_Lattice_insertKey( V24_BPy_Lattice * self, PyObject * args )
{
	Lattice *lt;
	int frame = -1, oldfra = -1;

	if( !PyArg_ParseTuple( args, "i", &frame ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument" ) );

	lt = self->lattice;

	//set the current frame
	if( frame > 0 ) {
		frame = V24_EXPP_ClampInt( frame, 1, MAXFRAME );
		oldfra = G.scene->r.cfra;
		G.scene->r.cfra = (int)frame;
	}
//      else just use current frame, then
//              return (V24_EXPP_ReturnPyObjError (PyExc_RuntimeError,
//                                              "frame value has to be greater than 0"));

	//insert a keybock for the lattice (1=relative)
	insert_lattkey( lt , 1);
	allspace(REMAKEIPO, 0);

	if( frame > 0 )
		G.scene->r.cfra = (int)oldfra;

	Py_RETURN_NONE;
}

static PyObject *V24_Lattice_copy( V24_BPy_Lattice * self )
{
	Lattice *bl_Lattice;	// blender Lattice object 
	PyObject *py_Lattice;	// python wrapper 

	bl_Lattice = copy_lattice( self->lattice );
	bl_Lattice->id.us = 0;

	if( bl_Lattice )
		py_Lattice = V24_Lattice_CreatePyObject( bl_Lattice );
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Lattice Object in Blender" );
	if( !py_Lattice )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Lattice Object wrapper" );

	return py_Lattice;
}

static int V24_Lattice_compare( V24_BPy_Lattice * a, V24_BPy_Lattice * b )
{
	return ( a->lattice == b->lattice ) ? 0 : -1;
}


//***************************************************************************
// Function:  Lattice_repr   
// Description: This is a callback function for the V24_BPy_Lattice type. It 
//                  builds a meaninful string to represent Lattice objects. 
//***************************************************************************
static PyObject *Lattice_repr( V24_BPy_Lattice * self )
{
	if( self->lattice )
		return PyString_FromFormat( "[Lattice \"%s\"]",
					    self->lattice->id.name + 2 );
	else
		return PyString_FromString( "[Lattice <deleted>]" );
}

/*****************************************************************************/
/* Python V24_BPy_Lattice methods table:	*/
/*****************************************************************************/
static PyMethodDef V24_BPy_Lattice_methods[] = {
	/* name, method, flags, doc */
	{"getName", ( PyCFunction ) V24_GenericLib_getName, METH_NOARGS,
	 V24_Lattice_getName_doc},
	{"setName", ( PyCFunction ) V24_GenericLib_setName_with_method, METH_VARARGS,
	 V24_Lattice_setName_doc},
	{"setPartitions", ( PyCFunction ) Lattice_setPartitions, METH_VARARGS,
	 V24_Lattice_setPartitions_doc},
	{"getPartitions", ( PyCFunction ) Lattice_getPartitions, METH_NOARGS,
	 V24_Lattice_getPartitions_doc},
	{"getKey", ( PyCFunction ) Lattice_getKey, METH_NOARGS,
	 V24_Lattice_getKey_doc},
	{"setKeyTypes", ( PyCFunction ) V24_Lattice_setKeyTypes, METH_VARARGS,
	 V24_Lattice_setKeyTypes_doc},
	{"getKeyTypes", ( PyCFunction ) Lattice_getKeyTypes, METH_NOARGS,
	 V24_Lattice_getKeyTypes_doc},
	{"setMode", ( PyCFunction ) V24_Lattice_setMode, METH_VARARGS,
	 V24_Lattice_setMode_doc},
	{"getMode", ( PyCFunction ) V24_Lattice_getMode, METH_NOARGS,
	 V24_Lattice_getMode_doc},
	{"setPoint", ( PyCFunction ) V24_Lattice_setPoint, METH_VARARGS,
	 V24_Lattice_setPoint_doc},
	{"getPoint", ( PyCFunction ) V24_Lattice_getPoint, METH_VARARGS,
	 V24_Lattice_getPoint_doc},
	{"insertKey", ( PyCFunction ) V24_Lattice_insertKey, METH_VARARGS,
	 V24_Lattice_insertKey_doc},
	{"__copy__", ( PyCFunction ) V24_Lattice_copy, METH_NOARGS,
	 V24_Lattice_copy_doc},
	{"copy", ( PyCFunction ) V24_Lattice_copy, METH_NOARGS,
	 V24_Lattice_copy_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python attributes get/set functions:                                      */
/*****************************************************************************/
static PyObject *V24_Lattice_getWidth(V24_BPy_Lattice * self)
{
	return PyInt_FromLong( self->lattice->pntsu );
}
static PyObject *V24_Lattice_getHeight(V24_BPy_Lattice * self)
{
	return PyInt_FromLong( self->lattice->pntsv );
}
static PyObject *V24_Lattice_getDepth(V24_BPy_Lattice * self)
{
	return PyInt_FromLong( self->lattice->pntsw );
}
static PyObject *V24_Lattice_getLatSize(V24_BPy_Lattice * self)
{
	return PyInt_FromLong(
		self->lattice->pntsu * self->lattice->pntsv * self->lattice->pntsw );
}


static PyObject *V24_Lattice_getAxisType(V24_BPy_Lattice * self, void * type)
{
	char interp_type = 0;
	switch ( (int)type ) {
	case 0:
		interp_type = self->lattice->typeu;
		break;
	case 1:
		interp_type = self->lattice->typev;
		break;
	case 2:
		interp_type = self->lattice->typew;
		break;
	}
	
	switch (interp_type) {
	case 0:
		return PyString_FromString( "Linear" );
	case 1:
		return PyString_FromString( "Cardinal" );
	case 2:
		return PyString_FromString( "Bspline" );
	}
	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Lattice_getseters[] = {
	GENERIC_LIB_GETSETATTR,
	{"width", (getter)V24_Lattice_getWidth, (setter)NULL,
	 "lattice U subdivision ", NULL},
	{"height", (getter)V24_Lattice_getHeight, (setter)NULL,
	 "lattice V subdivision", NULL},
	{"depth", (getter)V24_Lattice_getDepth, (setter)NULL,
	 "lattice W subdivision", NULL},
	{"latSize", (getter)V24_Lattice_getLatSize, (setter)NULL,
	 "lattice W subdivision", NULL},	 
	 
	{"widthType", (getter)V24_Lattice_getAxisType, NULL,
	 "lattice U interpolation type", (void *)0},
	{"heightType", (getter)V24_Lattice_getAxisType, NULL,
	 "lattice V interpolation type", (void *)1},
	{"depthType", (getter)V24_Lattice_getAxisType, NULL,
	 "lattice W interpolation type", (void *)2},

	{"key", (getter)Lattice_getKey, NULL,
	 "lattice key", NULL},
	{"mode", (getter)V24_Lattice_getMode, NULL,
	 "lattice key", NULL},
	 {NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python V24_Lattice_Type structure definition:		*/
/*****************************************************************************/
PyTypeObject V24_Lattice_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"Blender Lattice",	/* tp_name */
	sizeof( V24_BPy_Lattice ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	0,		/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	( cmpfunc ) V24_Lattice_compare,			/* tp_compare */
	( reprfunc ) Lattice_repr,	/* tp_repr */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	( hashfunc ) V24_GenericLib_hash,	/* hashfunc tp_hash; */
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
	V24_BPy_Lattice_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Lattice_getseters,         /* struct PyGetSetDef *tp_getset; */
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
