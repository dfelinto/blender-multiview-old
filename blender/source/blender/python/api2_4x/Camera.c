/* 
 * $Id: Camera.c 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Willian P. Germano, Johnny Matthews, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Camera.h" /*This must come first */

#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_object.h"
#include "BKE_library.h"
#include "BLI_blenlib.h"
#include "BLI_arithb.h" /* for M_PI */
#include "BSE_editipo.h"
#include "BIF_space.h"
#include "mydevice.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "Ipo.h"


#define IPOKEY_LENS 0
#define IPOKEY_CLIPPING 1



enum cam_consts {
	EXPP_CAM_ATTR_LENS = 0,
	EXPP_CAM_ATTR_ANGLE,
	EXPP_CAM_ATTR_DOFDIST,
	EXPP_CAM_ATTR_CLIPEND,
	EXPP_CAM_ATTR_CLIPSTART,
	EXPP_CAM_ATTR_SCALE,
	EXPP_CAM_ATTR_DRAWSIZE,
	EXPP_CAM_ATTR_SHIFTX,
	EXPP_CAM_ATTR_SHIFTY,
	EXPP_CAM_ATTR_ALPHA,
};

/*****************************************************************************/
/* Python API function prototypes for the Camera module.                     */
/*****************************************************************************/
static PyObject *V24_M_Camera_New( PyObject * self, PyObject * args,
			       PyObject * keywords );
static PyObject *V24_M_Camera_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* Blender.Camera.__doc__                                                    */
/*****************************************************************************/
static char V24_M_Camera_doc[] = "The Blender Camera module\n\
\n\
This module provides access to **Camera Data** objects in Blender\n\
\n\
Example::\n\
\n\
  from Blender import Camera, Object, Scene\n\
  c = Camera.New('ortho')      # create new ortho camera data\n\
  c.scale = 6.0                # set scale value\n\
  scn = Scene.GetCurrent()     # get current Scene\n\
  ob = scn.objects.new(c)      # Make an object from this data in the scene\n\
  cur.setCurrentCamera(ob)     # make this camera the active";

static char V24_M_Camera_New_doc[] =
	"Camera.New (type = 'persp', name = 'CamData'):\n\
        Return a new Camera Data object with the given type and name.";

static char V24_M_Camera_Get_doc[] = "Camera.Get (name = None):\n\
        Return the camera data with the given 'name', None if not found, or\n\
        Return a list with all Camera Data objects in the current scene,\n\
        if no argument was given.";

/*****************************************************************************/
/* Python method structure definition for Blender.Camera module:             */
/*****************************************************************************/
struct PyMethodDef V24_M_Camera_methods[] = {
	{"New", ( PyCFunction ) V24_M_Camera_New, METH_VARARGS | METH_KEYWORDS,
	 V24_M_Camera_New_doc},
	{"Get", V24_M_Camera_Get, METH_VARARGS, V24_M_Camera_Get_doc},
	{"get", V24_M_Camera_Get, METH_VARARGS, V24_M_Camera_Get_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_BPy_Camera methods declarations:                                   */
/*****************************************************************************/
static PyObject *V24_Camera_oldgetType( V24_BPy_Camera * self );
static PyObject *V24_Camera_oldgetMode( V24_BPy_Camera * self );
static PyObject *V24_Camera_oldgetLens( V24_BPy_Camera * self );
static PyObject *V24_Camera_oldgetClipStart( V24_BPy_Camera * self );
static PyObject *V24_Camera_oldgetClipEnd( V24_BPy_Camera * self );
static PyObject *V24_Camera_oldgetDrawSize( V24_BPy_Camera * self );
static PyObject *V24_Camera_oldgetScale( V24_BPy_Camera * self );
static PyObject *V24_Camera_getIpo( V24_BPy_Camera * self );
static int V24_Camera_setIpo( V24_BPy_Camera * self, PyObject * value );
static PyObject *V24_Camera_oldsetIpo( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetType( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetMode( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetLens( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetClipStart( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetClipEnd( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetDrawSize( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldsetScale( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldgetScriptLinks( V24_BPy_Camera * self, PyObject * value );
static PyObject *V24_Camera_addScriptLink( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_oldclearIpo( V24_BPy_Camera * self );
static PyObject *V24_Camera_clearScriptLinks( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_insertIpoKey( V24_BPy_Camera * self, PyObject * args );
static PyObject *V24_Camera_copy( V24_BPy_Camera * self );


/*****************************************************************************/
/* Python V24_BPy_Camera methods table:                                          */
/*****************************************************************************/
static PyMethodDef V24_BPy_Camera_methods[] = {
	/* name, method, flags, doc */
	{"getIpo", ( PyCFunction ) V24_Camera_getIpo, METH_NOARGS,
	 "() - Return Camera Data Ipo"},
	{"getName", ( PyCFunction ) V24_GenericLib_getName, METH_NOARGS,
	 "() - Return Camera Data name"},
	{"getType", ( PyCFunction ) V24_Camera_oldgetType, METH_NOARGS,
	 "() - Return Camera type - 'persp':0, 'ortho':1"},
	{"getMode", ( PyCFunction ) V24_Camera_oldgetMode, METH_NOARGS,
	 "() - Return Camera mode flags (or'ed value) -\n"
	 "     'showLimits':1, 'showMist':2"},
	{"getLens", ( PyCFunction ) V24_Camera_oldgetLens, METH_NOARGS,
	 "() - Return *perspective* Camera lens value"},
	{"getScale", ( PyCFunction ) V24_Camera_oldgetScale, METH_NOARGS,
	 "() - Return *ortho* Camera scale value"},
	{"getClipStart", ( PyCFunction ) V24_Camera_oldgetClipStart, METH_NOARGS,
	 "() - Return Camera clip start value"},
	{"getClipEnd", ( PyCFunction ) V24_Camera_oldgetClipEnd, METH_NOARGS,
	 "() - Return Camera clip end value"},
	{"getDrawSize", ( PyCFunction ) V24_Camera_oldgetDrawSize, METH_NOARGS,
	 "() - Return Camera draw size value"},
	{"setIpo", ( PyCFunction ) V24_Camera_oldsetIpo, METH_VARARGS,
	 "(Blender Ipo) - Set Camera Ipo"},
	{"clearIpo", ( PyCFunction ) V24_Camera_oldclearIpo, METH_NOARGS,
	 "() - Unlink Ipo from this Camera."},
	 {"insertIpoKey", ( PyCFunction ) V24_Camera_insertIpoKey, METH_VARARGS,
	 "( Camera IPO type ) - Inserts a key into IPO"},
	{"setName", ( PyCFunction ) V24_GenericLib_setName_with_method, METH_VARARGS,
	 "(s) - Set Camera Data name"},
	{"setType", ( PyCFunction ) V24_Camera_oldsetType, METH_O,
	 "(s) - Set Camera type, which can be 'persp' or 'ortho'"},
	{"setMode", ( PyCFunction ) V24_Camera_oldsetMode, METH_VARARGS,
	 "(<s<,s>>) - Set Camera mode flag(s): 'showLimits' and 'showMist'"},
	{"setLens", ( PyCFunction ) V24_Camera_oldsetLens, METH_O,
	 "(f) - Set *perpective* Camera lens value"},
	{"setScale", ( PyCFunction ) V24_Camera_oldsetScale, METH_O,
	 "(f) - Set *ortho* Camera scale value"},
	{"setClipStart", ( PyCFunction ) V24_Camera_oldsetClipStart, METH_O,
	 "(f) - Set Camera clip start value"},
	{"setClipEnd", ( PyCFunction ) V24_Camera_oldsetClipEnd, METH_O,
	 "(f) - Set Camera clip end value"},
	{"setDrawSize", ( PyCFunction ) V24_Camera_oldsetDrawSize, METH_O,
	 "(f) - Set Camera draw size value"},
	{"getScriptLinks", ( PyCFunction ) V24_Camera_oldgetScriptLinks, METH_O,
	 "(eventname) - Get a list of this camera's scriptlinks (Text names) "
	 "of the given type\n"
	 "(eventname) - string: FrameChanged, Redraw or Render."},
	{"addScriptLink", ( PyCFunction ) V24_Camera_addScriptLink, METH_VARARGS,
	 "(text, evt) - Add a new camera scriptlink.\n"
	 "(text) - string: an existing Blender Text name;\n"
	 "(evt) string: FrameChanged, Redraw or Render."},
	{"clearScriptLinks", ( PyCFunction ) V24_Camera_clearScriptLinks,
	 METH_NOARGS,
	 "() - Delete all scriptlinks from this camera.\n"
	 "([s1<,s2,s3...>]) - Delete specified scriptlinks from this camera."},
	{"__copy__", ( PyCFunction ) V24_Camera_copy, METH_NOARGS,
	 "() - Return a copy of the camera."},
	{"copy", ( PyCFunction ) V24_Camera_copy, METH_NOARGS,
	 "() - Return a copy of the camera."},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_Camera_Type callback function prototypes:                          */
/*****************************************************************************/
static int V24_Camera_compare( V24_BPy_Camera * a, V24_BPy_Camera * b );
static PyObject *V24_Camera_repr( V24_BPy_Camera * self );

static PyObject *V24_M_Camera_New( PyObject * self, PyObject * args,
			       PyObject * kwords )
{
	char *type_str = "persp";	/* "persp" is type 0, "ortho" is type 1 */
	char *name_str = "Camera";
	static char *kwlist[] = { "type_str", "name_str", NULL };
	PyObject *pycam;	/* for Camera Data object wrapper in Python */
	Camera *blcam;		/* for actual Camera Data we create in Blender */

	/* Parse the arguments passed in by the Python interpreter */
	if( !PyArg_ParseTupleAndKeywords( args, kwords, "|ss", kwlist,
					  &type_str, &name_str ) )
		/* We expected string(s) (or nothing) as argument, but we didn't get that. */
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "expected zero, one or two strings as arguments" );

	blcam = add_camera( name_str );	/* first create the Camera Data in Blender */

	if( blcam )		/* now create the wrapper obj in Python */
		pycam = V24_Camera_CreatePyObject( blcam );
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Camera Data in Blender" );

	/* let's return user count to zero, because ... */
	blcam->id.us = 0;	/* ... add_camera() incref'ed it */
	/* XXX XXX Do this in other modules, too */

	if( pycam == NULL )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Camera PyObject" );

	if( strcmp( type_str, "persp" ) == 0 )
		/* default, no need to set, so */
		/*blcam->type = (short)EXPP_CAM_TYPE_PERSP */
		;
	/* we comment this line */
	else if( strcmp( type_str, "ortho" ) == 0 )
		blcam->type = ( short ) EXPP_CAM_TYPE_ORTHO;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "unknown camera type" );
	
	return pycam;
}

static PyObject *V24_M_Camera_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Camera *cam_iter;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument (or nothing)" );

	cam_iter = G.main->camera.first;

	if( name ) {		/* (name) - Search camera by name */

		PyObject *wanted_cam = NULL;

		while( cam_iter && !wanted_cam ) {

			if( strcmp( name, cam_iter->id.name + 2 ) == 0 ) {
				wanted_cam = V24_Camera_CreatePyObject( cam_iter );
				break;
			}

			cam_iter = cam_iter->id.next;
		}

		if( !wanted_cam ) {	/* Requested camera doesn't exist */
			char error_msg[64];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Camera \"%s\" not found", name );
			return V24_EXPP_ReturnPyObjError( PyExc_NameError,
						      error_msg );
		}

		return wanted_cam;
	}

	else {			/* () - return a list of wrappers for all cameras in the scene */
		int index = 0;
		PyObject *cam_pylist, *pyobj;

		cam_pylist =
			PyList_New( BLI_countlist( &( G.main->camera ) ) );

		if( !cam_pylist )
			return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						      "couldn't create PyList" );

		while( cam_iter ) {
			pyobj = V24_Camera_CreatePyObject( cam_iter );

			if( !pyobj ) {
				Py_DECREF(cam_pylist);
				return V24_EXPP_ReturnPyObjError
					( PyExc_MemoryError,
					  "couldn't create Camera PyObject" );
			}
			PyList_SET_ITEM( cam_pylist, index, pyobj );

			cam_iter = cam_iter->id.next;
			index++;
		}

		return cam_pylist;
	}
}

PyObject *V24_Camera_Init( void )
{
	PyObject *V24_submodule;

	if( PyType_Ready( &V24_Camera_Type ) < 0 )
		return NULL;
	
	V24_submodule = Py_InitModule3( "Blender.Camera",
				    V24_M_Camera_methods, V24_M_Camera_doc );

	PyModule_AddIntConstant( V24_submodule, "LENS",     IPOKEY_LENS );
	PyModule_AddIntConstant( V24_submodule, "CLIPPING", IPOKEY_CLIPPING );

	return V24_submodule;
}

/* Three Python V24_Camera_Type helper functions needed by the Object module: */

PyObject *V24_Camera_CreatePyObject( Camera * cam )
{
	V24_BPy_Camera *pycam;

	pycam = ( V24_BPy_Camera * ) PyObject_NEW( V24_BPy_Camera, &V24_Camera_Type );

	if( !pycam )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create V24_BPy_Camera PyObject" );

	pycam->camera = cam;
	return ( PyObject * ) pycam;
}

Camera *V24_Camera_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Camera * ) pyobj )->camera;
}

/*****************************************************************************/
/* Python V24_BPy_Camera methods:                                               */
/*****************************************************************************/

static PyObject *V24_Camera_oldgetType( V24_BPy_Camera * self )
{
	return PyInt_FromLong( self->camera->type );
}

static PyObject *V24_Camera_oldgetMode( V24_BPy_Camera * self )
{
	return PyInt_FromLong( self->camera->flag );
}

static PyObject *V24_Camera_oldgetLens( V24_BPy_Camera * self )
{
	return PyFloat_FromDouble( self->camera->lens );
}

static PyObject *V24_Camera_oldgetScale( V24_BPy_Camera * self )
{
	return PyFloat_FromDouble( self->camera->ortho_scale );
}

static PyObject *V24_Camera_oldgetClipStart( V24_BPy_Camera * self )
{
	return PyFloat_FromDouble( self->camera->clipsta );
}

static PyObject *V24_Camera_oldgetClipEnd( V24_BPy_Camera * self )
{
	return PyFloat_FromDouble( self->camera->clipend );
}

static PyObject *V24_Camera_oldgetDrawSize( V24_BPy_Camera * self )
{
	return PyFloat_FromDouble( self->camera->drawsize );
}



static PyObject *V24_Camera_oldsetIpo( V24_BPy_Camera * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Camera_setIpo );
}

static PyObject *V24_Camera_oldclearIpo( V24_BPy_Camera * self )
{
	Camera *cam = self->camera;
	Ipo *ipo = ( Ipo * ) cam->ipo;

	if( ipo ) {
		ID *id = &ipo->id;
		if( id->us > 0 )
			id->us--;
		cam->ipo = NULL;

		return V24_EXPP_incr_ret_True();
	}

	return V24_EXPP_incr_ret_False(); /* no ipo found */
}

static PyObject *V24_Camera_oldsetType( V24_BPy_Camera * self, PyObject * value )
{
	char *type = PyString_AsString(value);

	if(!value)
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );

	if( strcmp( type, "persp" ) == 0 )
		self->camera->type = ( short ) EXPP_CAM_TYPE_PERSP;
	else if( strcmp( type, "ortho" ) == 0 )
		self->camera->type = ( short ) EXPP_CAM_TYPE_ORTHO;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "unknown camera type" );

	Py_RETURN_NONE;
}

static PyObject *V24_Camera_oldsetMode( V24_BPy_Camera * self, PyObject * args )
{
	char *mode_str1 = NULL, *mode_str2 = NULL;
	short flag = 0;

	if( !PyArg_ParseTuple( args, "|ss", &mode_str1, &mode_str2 ) )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "expected one or two strings as arguments" );

	if( mode_str1 != NULL ) {
		if( strcmp( mode_str1, "showLimits" ) == 0 )
			flag |= ( short ) EXPP_CAM_MODE_SHOWLIMITS;
		else if( strcmp( mode_str1, "showMist" ) == 0 )
			flag |= ( short ) EXPP_CAM_MODE_SHOWMIST;
		else
			return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "first argument is an unknown camera flag" );

		if( mode_str2 != NULL ) {
			if( strcmp( mode_str2, "showLimits" ) == 0 )
				flag |= ( short ) EXPP_CAM_MODE_SHOWLIMITS;
			else if( strcmp( mode_str2, "showMist" ) == 0 )
				flag |= ( short ) EXPP_CAM_MODE_SHOWMIST;
			else
				return V24_EXPP_ReturnPyObjError
					( PyExc_AttributeError,
					  "second argument is an unknown camera flag" );
		}
	}

	self->camera->flag = flag;

	Py_RETURN_NONE;
}

static PyObject *V24_Camera_oldsetLens( V24_BPy_Camera * self, PyObject * value )
{
	float param = PyFloat_AsDouble(value);

	if( !PyFloat_Check(value) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected float argument" );

	self->camera->lens = V24_EXPP_ClampFloat( param,
					      EXPP_CAM_LENS_MIN,
					      EXPP_CAM_LENS_MAX );

	Py_RETURN_NONE;
}

static PyObject *V24_Camera_oldsetScale( V24_BPy_Camera * self, PyObject * value )
{
	float param = PyFloat_AsDouble(value);

	if( !PyFloat_Check(value) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected float argument" );

	self->camera->ortho_scale = V24_EXPP_ClampFloat( param,
					      EXPP_CAM_SCALE_MIN,
					      EXPP_CAM_SCALE_MAX );

	Py_RETURN_NONE;
}

static PyObject *V24_Camera_oldsetClipStart( V24_BPy_Camera * self, PyObject * value )
{
	float param = PyFloat_AsDouble(value);

	if( !PyFloat_Check(value) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected float argument" );

	self->camera->clipsta = V24_EXPP_ClampFloat( param,
						 EXPP_CAM_CLIPSTART_MIN,
						 EXPP_CAM_CLIPSTART_MAX );

	Py_RETURN_NONE;
}

static PyObject *V24_Camera_oldsetClipEnd( V24_BPy_Camera * self, PyObject * value )
{
	float param = PyFloat_AsDouble(value);

	if( !PyFloat_Check(value) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected float argument" );

	self->camera->clipend = V24_EXPP_ClampFloat( param,
						 EXPP_CAM_CLIPEND_MIN,
						 EXPP_CAM_CLIPEND_MAX );

	Py_RETURN_NONE;
}

static PyObject *V24_Camera_oldsetDrawSize( V24_BPy_Camera * self, PyObject * value )
{
	float param = PyFloat_AsDouble(value);

	if( !PyFloat_Check(value) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected float argument" );

	self->camera->drawsize = V24_EXPP_ClampFloat( param,
						  EXPP_CAM_DRAWSIZE_MIN,
						  EXPP_CAM_DRAWSIZE_MAX );

	Py_RETURN_NONE;
}

/* cam.addScriptLink */
static PyObject *V24_Camera_addScriptLink( V24_BPy_Camera * self, PyObject * args )
{
	Camera *cam = self->camera;
	ScriptLink *slink = NULL;

	slink = &( cam )->scriptlink;

	return V24_EXPP_addScriptLink( slink, args, 0 );
}

/* cam.clearScriptLinks */
static PyObject *V24_Camera_clearScriptLinks( V24_BPy_Camera * self, PyObject * args )
{
	Camera *cam = self->camera;
	ScriptLink *slink = NULL;

	slink = &( cam )->scriptlink;

	return V24_EXPP_clearScriptLinks( slink, args );
}

/* cam.getScriptLinks */
static PyObject *V24_Camera_oldgetScriptLinks( V24_BPy_Camera * self, PyObject * value )
{
	Camera *cam = self->camera;
	ScriptLink *slink = NULL;
	PyObject *ret = NULL;

	slink = &( cam )->scriptlink;

	ret = V24_EXPP_getScriptLinks( slink, value, 0 );

	if( ret )
		return ret;
	else
		return NULL;
}

/* cam.__copy__ */
static PyObject *V24_Camera_copy( V24_BPy_Camera * self )
{
	PyObject *pycam;	/* for Camera Data object wrapper in Python */
	Camera *blcam;		/* for actual Camera Data we create in Blender */

	blcam = copy_camera( self->camera );	/* first create the Camera Data in Blender */

	if( blcam )		/* now create the wrapper obj in Python */
		pycam = V24_Camera_CreatePyObject( blcam );
	else
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Camera Data in Blender" );

	/* let's return user count to zero, because ... */
	blcam->id.us = 0;	/* ... copy_camera() incref'ed it */
	/* XXX XXX Do this in other modules, too */

	if( pycam == NULL )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Camera PyObject" );

	return pycam;
}

static PyObject *V24_Camera_getType( V24_BPy_Camera * self )
{
	if (self->camera->type == EXPP_CAM_TYPE_PERSP)
		return PyString_FromString("persp");
	else /* must be EXPP_CAM_TYPE_ORTHO */
		return PyString_FromString("ortho");
}

static int V24_Camera_setType( V24_BPy_Camera * self, PyObject * value )
{
	char *type = NULL;
	type = PyString_AsString(value);
	
	if (!type)
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
					      "expected a string" );
	if (strcmp("persp", type)==0) {
		self->camera->type = EXPP_CAM_TYPE_PERSP;
		return 0;
	} else if (strcmp("ortho", type)==0) {
		self->camera->type = EXPP_CAM_TYPE_ORTHO;
		return 0;
	}
	
	return V24_EXPP_ReturnIntError( PyExc_ValueError,
		"expected a string \"ortho\" or \"persp\"" );
}



static PyObject *V24_Camera_getMode( V24_BPy_Camera * self )
{
	return PyInt_FromLong(self->camera->flag);
}

static int V24_Camera_setMode( V24_BPy_Camera * self, PyObject * value )
{
	unsigned int flag = 0;
	
	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
			"expected an integer (bitmask) as argument" );
	
	flag = ( unsigned int )PyInt_AS_LONG( value );
	
	self->camera->flag = flag;
	return 0;
}

static PyObject *V24_Camera_getIpo( V24_BPy_Camera * self )
{
	struct Ipo *ipo = self->camera->ipo;

	if( ipo )
		return V24_Ipo_CreatePyObject( ipo );
	Py_RETURN_NONE;
}

static int V24_Camera_setIpo( V24_BPy_Camera * self, PyObject * value )
{
	return V24_GenericLib_assignData(value, (void **) &self->camera->ipo, 0, 1, ID_IP, ID_CA);
}

/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( V24_BPy_Camera *self, void *type )
{
	float param;
	struct Camera *cam= self->camera;

	switch( (int)type ) {
	case EXPP_CAM_ATTR_LENS: 
		param = cam->lens;
		break;
	case EXPP_CAM_ATTR_ANGLE: 
		param = 360.0f * atan(16.0f/cam->lens) / M_PI;
		break;
	case EXPP_CAM_ATTR_DOFDIST: 
		param = cam->YF_dofdist;
		break;
	case EXPP_CAM_ATTR_CLIPSTART: 
		param = cam->clipsta;
		break;
	case EXPP_CAM_ATTR_CLIPEND: 
		param = cam->clipend;
		break;
	case EXPP_CAM_ATTR_DRAWSIZE: 
		param = cam->drawsize;
		break;
	case EXPP_CAM_ATTR_SCALE: 
		param = cam->ortho_scale;
		break;
	case EXPP_CAM_ATTR_ALPHA: 
		param = cam->passepartalpha;
		break;
	case EXPP_CAM_ATTR_SHIFTX: 
		param = cam->shiftx;
		break;
	case EXPP_CAM_ATTR_SHIFTY:
		param = cam->shifty;
		break;
	
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getFloatAttr" );
	}

	return PyFloat_FromDouble( param );
}



/*
 * set floating point attributes which require clamping
 */

static int setFloatAttrClamp( V24_BPy_Camera *self, PyObject *value, void *type )
{
	float *param;
	struct Camera *cam = self->camera;
	float min, max;
	int ret;
 
	switch( (int)type ) {
	case EXPP_CAM_ATTR_LENS:
		min = 1.0;
		max = 250.0;
		param = &cam->lens;
		break;
	case EXPP_CAM_ATTR_ANGLE:
		min = 7.323871;
		max = 172.847331;
		param = &cam->lens;
		break;
	case EXPP_CAM_ATTR_DOFDIST:
		min = 0.0;
		max = 5000.0;
		param = &cam->YF_dofdist;
		break;
	case EXPP_CAM_ATTR_CLIPSTART:
		min = 0.0;
		max = 100.0;
		param = &cam->clipsta;
		break;
	case EXPP_CAM_ATTR_CLIPEND:
		min = 1.0;
		max = 5000.0;
		param = &cam->clipend;
		break;
	case EXPP_CAM_ATTR_DRAWSIZE:
		min = 0.1f;
		max = 10.0;
		param = &cam->drawsize;
		break;
	case EXPP_CAM_ATTR_SCALE:
		min = 0.01f;
		max = 1000.0;
		param = &cam->ortho_scale;
		break;
	case EXPP_CAM_ATTR_ALPHA:
		min = 0.0;
		max = 1.0;
		param = &cam->passepartalpha;
		break;
	case EXPP_CAM_ATTR_SHIFTX:
		min = -2.0;
		max =  2.0;
		param = &cam->shiftx;
		break;
	case EXPP_CAM_ATTR_SHIFTY:
		min = -2.0;
		max =  2.0;
		param = &cam->shifty;
		break;
	
	default:
		return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	ret = V24_EXPP_setFloatClamped( value, param, min, max );
	
	if (ret==0) {
		if ((int)type == EXPP_CAM_ATTR_ANGLE) {
			cam->lens = 16.0f / tan(M_PI*cam->lens/360.0f);
		}
	}
	return ret;
}


/*
 * get floating point attributes
 */

static PyObject *getFlagAttr( V24_BPy_Camera *self, void *type )
{
	if (self->camera->flag & (int)type)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/*
 * set floating point attributes which require clamping
 */

static int setFlagAttr( V24_BPy_Camera *self, PyObject *value, void *type )
{
	int param = PyObject_IsTrue( value );
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if (param)
		self->camera->flag |= (int)type;
	else
		self->camera->flag &= ~(int)type;
	return 0;
}


/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Camera_getseters[] = {
	GENERIC_LIB_GETSETATTR,
	{"type",
	 (getter)V24_Camera_getType, (setter)V24_Camera_setType,
	 "camera type \"persp\" or \"ortho\"",
	 NULL},
	{"mode",
	 (getter)V24_Camera_getMode, (setter)V24_Camera_setMode,
	 "Cameras mode",
	 NULL},
	{"ipo",
	 (getter)V24_Camera_getIpo, (setter)V24_Camera_setIpo,
	 "Cameras ipo",
	 NULL},
	
	/* float settings */
	{"lens",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "lens angle for perspective cameras",
	 (void *)EXPP_CAM_ATTR_LENS},
	{"angle",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "lens angle for perspective cameras",
	 (void *)EXPP_CAM_ATTR_ANGLE},
	 
	{"scale",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "scale for ortho cameras",
	 (void *)EXPP_CAM_ATTR_SCALE},
	{"clipStart",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the cameras clip start",
	 (void *)EXPP_CAM_ATTR_CLIPSTART},
	{"clipEnd",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the cameras clip end",
	 (void *)EXPP_CAM_ATTR_CLIPEND},
	{"shiftX",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the cameras X perspective shift",
	 (void *)EXPP_CAM_ATTR_SHIFTX},
	{"shiftY",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the cameras Y perspective shift",
	 (void *)EXPP_CAM_ATTR_SHIFTY},
	{"dofDist",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "cameras dof distance",
	 (void *)EXPP_CAM_ATTR_DOFDIST},
	{"drawSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "the cameras display size",
	 (void *)EXPP_CAM_ATTR_DRAWSIZE},
	{"alpha",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "passepart alpha value for display",
	 (void *)EXPP_CAM_ATTR_ALPHA},
	 
	/* flags - use flags as defined in DNA_camera_types.h */
	{"drawLimits",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "toggle the draw limits display flag",
	 (void *)CAM_SHOWLIMITS},
	{"drawMist",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "toggle the draw mist display flag",
	 (void *)CAM_SHOWMIST},
	{"drawName",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "toggle the draw name display flag",
	 (void *)CAM_SHOWNAME},
	{"drawTileSafe",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "toggle the tile safe display flag",
	 (void *)CAM_SHOWTITLESAFE},
	{"drawPassepartout",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "toggle the passPartOut display flag",
	 (void *)CAM_SHOWPASSEPARTOUT},
	{"angleToggle",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "toggle the camera input unit flag",
	 (void *)CAM_ANGLETOGGLE},
	{NULL,NULL,NULL,NULL}  /* Sentinel */
};


/*****************************************************************************/
/* Python V24_Camera_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject V24_Camera_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Camera",           /* char *tp_name; */
	sizeof( V24_BPy_Camera ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,						/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Camera_compare, /* cmpfunc tp_compare; */
	( reprfunc ) V24_Camera_repr,   /* reprfunc tp_repr; */

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
	NULL, /* getiterfunc tp_iter; */
	NULL, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	V24_BPy_Camera_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Camera_getseters,                       /* struct PyGetSetDef *tp_getset; */
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



static int V24_Camera_compare( V24_BPy_Camera * a, V24_BPy_Camera * b )
{
	Camera *pa = a->camera, *pb = b->camera;
	return ( pa == pb ) ? 0 : -1;
}

static PyObject *V24_Camera_repr( V24_BPy_Camera * self )
{
	return PyString_FromFormat( "[Camera \"%s\"]",
				    self->camera->id.name + 2 );
}

/*
 * V24_Camera_insertIpoKey()
 *  inserts Camera IPO key for LENS and CLIPPING
 */

static PyObject *V24_Camera_insertIpoKey( V24_BPy_Camera * self, PyObject * args )
{
	int key = 0;

	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
										"expected int argument" ) );

	if (key == IPOKEY_LENS){
		insertkey((ID *)self->camera, ID_CA, NULL, NULL, CAM_LENS, 0);     
	}
	else if (key == IPOKEY_CLIPPING){
		insertkey((ID *)self->camera, ID_CA, NULL, NULL, CAM_STA, 0);
		insertkey((ID *)self->camera, ID_CA, NULL, NULL, CAM_END, 0);   
	}

	allspace(REMAKEIPO, 0);
	V24_EXPP_allqueue(REDRAWIPO, 0);
	V24_EXPP_allqueue(REDRAWVIEW3D, 0);
	V24_EXPP_allqueue(REDRAWACTION, 0);
	V24_EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}
