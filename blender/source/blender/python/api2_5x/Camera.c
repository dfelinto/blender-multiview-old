/* 
 * $Id: Camera.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
#include "bpy_list.h"
#include "bpy_data.h"


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

static char BPyCamera_Type_doc[] ="\
class Camera:\n\
	'''\n\
	The Camera Data object\n\
	======================\n\
		This object gives access to Camera-specific data in Blender.\n\
		\n\
		Example::\n\
		\n\
			import bpy\n\
			cam = bpy.data.cameras.new()	# create new ortho camera data\n\
			cam.orthographic = True			# create new ortho camera data\n\
			cam.scale = 6.0					# set scale value for ortho view\n\
			sce = bpy.data.scenes.active	# get current scene\n\
			ob = scn.objects.new(cam)		# add a new camera object from the data\n\
			scn.camera = ob					# make this camera the active\n\
";

static char Camera_copy_doc[] ="\
():\n\
	'''\n\
	Make a copy of this camera\n\
	@rtype: Camera\n\
	@return: a copy of this camera\n\
	'''\n\
";

static char Camera_insertkey_doc[] ="\
(ipo):\n\
	'''\n\
	Inserts a key into IPO\n\
	@type ipo: Blender Constant\n\
	@param ipo: Constant for the IPO key, not yet implimented.\n\
	'''\n\
";



/*****************************************************************************/
/* Python BPyCameraObject methods:                                               */
/*****************************************************************************/

/*
 * Camera_insertKey()
 *  inserts Camera IPO key for LENS and CLIPPING
 * - TODO- Replce constants.
 */

static PyObject *Camera_insertKey( BPyCameraObject * self, PyObject * args )
{
	int key = 0;

	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( EXPP_ReturnPyObjError( PyExc_AttributeError,
										"expected int argument" ) );

	if (key == IPOKEY_LENS){
		insertkey((ID *)self->camera, ID_CA, NULL, NULL, CAM_LENS, 0);     
	}
	else if (key == IPOKEY_CLIPPING){
		insertkey((ID *)self->camera, ID_CA, NULL, NULL, CAM_STA, 0);
		insertkey((ID *)self->camera, ID_CA, NULL, NULL, CAM_END, 0);   
	}

	allspace(REMAKEIPO, 0);
	EXPP_allqueue(REDRAWIPO, 0);
	EXPP_allqueue(REDRAWVIEW3D, 0);
	EXPP_allqueue(REDRAWACTION, 0);
	EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}


/* cam.__copy__ */
static PyObject *Camera_copy( BPyCameraObject * self )
{
	PyObject *pycam;	/* for Camera Data object wrapper in Python */
	Camera *blcam;		/* for actual Camera Data we create in Blender */

	blcam = copy_camera( self->camera );	/* first create the Camera Data in Blender */

	if( blcam )		/* now create the wrapper obj in Python */
		pycam = Camera_CreatePyObject( blcam );
	else
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Camera Data in Blender" );

	/* let's return user count to zero, because ... */
	blcam->id.us = 0;	/* ... copy_camera() incref'ed it */
	/* XXX XXX Do this in other modules, too */

	if( pycam == NULL )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Camera PyObject" );

	return pycam;
}


/*****************************************************************************/
/* Python BPyCameraObject methods table:                                          */
/*****************************************************************************/
static PyMethodDef BPyCamera_methods[] = {
	/* name, method, flags, doc */
    {"insertKey", ( PyCFunction ) Camera_insertKey, METH_VARARGS,
	 Camera_insertkey_doc},
	{"__copy__", ( PyCFunction ) Camera_copy, METH_NOARGS,
	 Camera_copy_doc},
	{"copy", ( PyCFunction ) Camera_copy, METH_NOARGS,
	 Camera_copy_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyCamera_Type callback function prototypes:                          */
/*****************************************************************************/

/* Three Python BPyCamera_Type helper functions needed by the Object module: */

PyObject *Camera_CreatePyObject( Camera * cam )
{
	BPyCameraObject *pycam;

	pycam = ( BPyCameraObject * ) PyObject_NEW( BPyCameraObject, &BPyCamera_Type );

	if( !pycam )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyCameraObject PyObject" );

	pycam->camera = cam;
	return ( PyObject * ) pycam;
}

static PyObject *Camera_getOrtho( BPyCameraObject * self )
{
	if (self->camera->type == EXPP_CAM_TYPE_PERSP)
		Py_RETURN_FALSE;
	else /* must be EXPP_CAM_TYPE_ORTHO */
		Py_RETURN_TRUE;
}

static int Camera_setOrtho( BPyCameraObject * self, PyObject * value )
{
	if (PyObject_IsTrue( value ))
		self->camera->type = EXPP_CAM_TYPE_ORTHO;
	else
		self->camera->type = EXPP_CAM_TYPE_PERSP;
	
	return 0;
}

static PyObject *Camera_getIpo( BPyCameraObject * self )
{
	return Ipo_CreatePyObject( self->camera->ipo );
}

static int Camera_setIpo( BPyCameraObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->camera->ipo, 0, 1, ID_IP, ID_CA);
}

/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( BPyCameraObject *self, void *type )
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
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getFloatAttr" );
	}

	return PyFloat_FromDouble( param );
}



/*
 * set floating point attributes which require clamping
 */

static int setFloatAttrClamp( BPyCameraObject *self, PyObject *value, void *type )
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
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	ret = EXPP_setFloatClamped( value, param, min, max );
	
	if (ret==0) {
		if ((int)type == EXPP_CAM_ATTR_ANGLE) {
			cam->lens = 16.0f / tan(M_PI*cam->lens/360.0f);
		}
	}
	return ret;
}


/* get floating point attributes */
static PyObject *getFlagAttr( BPyCameraObject *self, void *type )
{
	if (self->camera->flag & (int)type)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/* set floating point attributes which require clamping */
static int setFlagAttr( BPyCameraObject *self, PyObject *value, void *type )
{
	if (PyObject_IsTrue(value))
		self->camera->flag |= (int)type;
	else
		self->camera->flag &= ~(int)type;
	return 0;
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyCamera_getset[] = {
	GENERIC_LIB_GETSETATTR_SCRIPTLINK,
	{"orthographic",
	 (getter)Camera_getOrtho, (setter)Camera_setOrtho, "\
@ivar orthographic: When true this camera will render with no perspective.\n\
@type orthographic: bool",
	 NULL},
	{"ipo",
	 (getter)Camera_getIpo, (setter)Camera_setIpo, "\
@ivar ipo: The ipo linked to this camera data object.\n\
	 Assign None to clear the ipo from this camera.\n\
@type ipo: Blender Ipo",
	 NULL},
	 
	/* float settings */
	{"lens",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar lens: The lens value in [1.0, 250.0], only relevant to cameras with their orthographic is set to False.\n\
@type lens: float",
	 (void *)EXPP_CAM_ATTR_LENS},
	{"angle",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar angle: The lens value in degrees [7.323871, 172.847331], only relevant to cameras with their orthographic is set to False.\n\
@type angle: float",
	 (void *)EXPP_CAM_ATTR_ANGLE},
	 
	{"scale",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar scale: The scale value in [0.01, 1000.00], only relevant to cameras with their orthographic is set to True.\n\
@type scale: float",
	 (void *)EXPP_CAM_ATTR_SCALE},
	{"clipStart",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar clipStart: The clip start value in [0.0, 100.0].\n\
@type clipStart: float",
	 (void *)EXPP_CAM_ATTR_CLIPSTART},
	{"clipEnd",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar clipEnd: The clip end value in [1.0, 5000.0].\n\
@type clipEnd: float",
	 (void *)EXPP_CAM_ATTR_CLIPEND},
	{"shiftX",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar shiftX: The horizontal offset of the camera [-2.0, 2.0].\n\
@type shiftX: float",
	 (void *)EXPP_CAM_ATTR_SHIFTX},
	{"shiftY",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar shiftY: The vertical offset of the camera [-2.0, 2.0].\n\
@type shiftY: float",
	 (void *)EXPP_CAM_ATTR_SHIFTY},
	{"dofDist",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar dofDist: The focal point of the camera in [0.0, 5000.0].\n\
@type dofDist: float",
	 (void *)EXPP_CAM_ATTR_DOFDIST},
	{"drawSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar drawSize: The display size for the camera an the 3d view [0.1, 10.0].\n\
@type drawSize: float",
	 (void *)EXPP_CAM_ATTR_DRAWSIZE},
	{"alpha",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, "\
@ivar alpha: The PassePart alpha [0.0, 1.0].\n\
@type alpha: float",
	 (void *)EXPP_CAM_ATTR_ALPHA},
	 
	/* flags - use flags as defined in DNA_camera_types.h */
	{"drawLimits",
	 (getter)getFlagAttr, (setter)setFlagAttr, "\
@ivar drawLimits: Display drawlimits in the 3D view for this camera.\n\
@type drawLimits: bool",
	 (void *)CAM_SHOWLIMITS},
	{"drawMist",
	 (getter)getFlagAttr, (setter)setFlagAttr, "\
@ivar drawMist: Display mist limits in the 3D view for this camera.\n\
@type drawMist: bool",
	 (void *)CAM_SHOWMIST},
	{"drawName",
	 (getter)getFlagAttr, (setter)setFlagAttr, "\
@ivar drawName: Display camera name in the 3D view.\n\
@type drawName: bool",
	 (void *)CAM_SHOWNAME},
	{"drawTitleSafe",
	 (getter)getFlagAttr, (setter)setFlagAttr, "\
@ivar drawTitleSafe: Display title safe bounds in the 3D view for this camera.\n\
@type drawTitleSafe: bool",
	 (void *)CAM_SHOWTITLESAFE},
	{"drawPassepartout",
	 (getter)getFlagAttr, (setter)setFlagAttr, "\
@ivar drawPassepartout: Mask areas outside the camera view.\n\
@type drawPassepartout: bool",
	 (void *)CAM_SHOWPASSEPARTOUT},
/* Why would python need to modify this flag? */
#if 0
	{"angleToggle",
	 (getter)getFlagAttr, (setter)setFlagAttr, "\
@ivar angleToggle: Display the \n\
@type angleToggle: bool",
	 (void *)CAM_ANGLETOGGLE},
#endif
	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Camera";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_camera( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new camera data" );
	
	id->us = 0;
	return Camera_CreatePyObject((Camera *)id);
}

/*****************************************************************************/
/* Python BPyCamera_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject BPyCamera_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Camera",           /* char *tp_name; */
	sizeof( BPyCameraObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,						/* cmpfunc tp_compare; */
	NULL,						/* reprfunc tp_repr; */

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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* long tp_flags; */

	BPyCamera_Type_doc,                       /*  char *tp_doc;  Documentation string */
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
	BPyCamera_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,						/* struct PyMemberDef *tp_members; */
	BPyCamera_getset,			/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,						/* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	(newfunc)Camera_new,					/* newfunc tp_new; */
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

PyObject * CameraType_Init( void )
{	
	PyType_Ready( &BPyCamera_Type );
	BPyCamera_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &BPyCamera_Type;
}
