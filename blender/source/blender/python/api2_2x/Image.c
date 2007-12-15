/* 
 * $Id$
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
 * Contributor(s): Willian P. Germano, Campbell Barton, Joilnen B. Leite,
 * Austin Benesh
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/
#include "Image.h"		/*This must come first */

#include "BDR_drawmesh.h"	/* free_realtime_image */
#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_image.h"
#include "BKE_idprop.h"
#include "BIF_drawimage.h"
#include "BLI_blenlib.h"
#include "DNA_space_types.h"	/* FILE_MAXDIR = 160 */
#include "IMB_imbuf_types.h"	/* for the IB_rect define */
#include "BIF_gl.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "BKE_packedFile.h"
#include "DNA_packedFile_types.h"
#include "BKE_icons.h"
#include "IMB_imbuf.h"
#include "IDProp.h"

/* used so we can get G.scene->r.cfra for getting the
current image frame, some images change frame if they are a sequence */
#include "DNA_scene_types.h"

/*****************************************************************************/
/* Python V24_BPy_Image defaults:																								 */
/*****************************************************************************/
#define EXPP_IMAGE_REP			1
#define EXPP_IMAGE_REP_MIN	1
#define EXPP_IMAGE_REP_MAX 16


enum img_consts {
	EXPP_IMAGE_ATTR_XREP = 0,
	EXPP_IMAGE_ATTR_YREP,
	EXPP_IMAGE_ATTR_START,
	EXPP_IMAGE_ATTR_END,
	EXPP_IMAGE_ATTR_SPEED,
	EXPP_IMAGE_ATTR_BINDCODE,
	EXPP_IMAGE_ATTR_SOURCE,
};

/************************/
/*** The Image Module ***/
/************************/

/*****************************************************************************/
/* Python API function prototypes for the Image module.	 */
/*****************************************************************************/
static PyObject *V24_M_Image_New( PyObject * self, PyObject * args );
static PyObject *V24_M_Image_Get( PyObject * self, PyObject * args );
static PyObject *V24_M_Image_GetCurrent( PyObject * self );
static PyObject *V24_M_Image_Load( PyObject * self, PyObject * value );


/*****************************************************************************/
/* Python V24_BPy_Image methods declarations:	 */
/*****************************************************************************/
static PyObject *V24_Image_getFilename( V24_BPy_Image * self );
static PyObject *V24_Image_getSize( V24_BPy_Image * self );
static PyObject *V24_Image_getDepth( V24_BPy_Image * self );
static PyObject *V24_Image_getXRep( V24_BPy_Image * self );
static PyObject *V24_Image_getYRep( V24_BPy_Image * self );
static PyObject *V24_Image_getBindCode( V24_BPy_Image * self );
static PyObject *V24_Image_getStart( V24_BPy_Image * self );
static PyObject *V24_Image_getEnd( V24_BPy_Image * self );
static PyObject *V24_Image_getSpeed( V24_BPy_Image * self );
static int V24_Image_setFilename( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_oldsetFilename( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_setXRep( V24_BPy_Image * self, PyObject * value );
static PyObject *V24_Image_setYRep( V24_BPy_Image * self, PyObject * value );
static PyObject *V24_Image_setStart( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_setEnd( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_setSpeed( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_reload( V24_BPy_Image * self );
static PyObject *V24_Image_glLoad( V24_BPy_Image * self );
static PyObject *V24_Image_glFree( V24_BPy_Image * self );
static PyObject *V24_Image_getPixelF( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_getPixelI( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_setPixelF( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_setPixelI( V24_BPy_Image * self, PyObject * args );
static PyObject *V24_Image_getMaxXY( V24_BPy_Image * self );
static PyObject *V24_Image_getMinXY( V24_BPy_Image * self );
static PyObject *V24_Image_save( V24_BPy_Image * self );
static PyObject *V24_Image_unpack( V24_BPy_Image * self, PyObject * value );
static PyObject *V24_Image_pack( V24_BPy_Image * self );
static PyObject *V24_Image_makeCurrent( V24_BPy_Image * self );


/*****************************************************************************/
/* Python V24_BPy_Image methods table:	 */
/*****************************************************************************/
static PyMethodDef V24_BPy_Image_methods[] = {
	/* name, method, flags, doc */
	{"getPixelF", ( PyCFunction ) V24_Image_getPixelF, METH_VARARGS,
	 "(int, int) - Get pixel color as floats 0.0-1.0 returns [r,g,b,a]"},
	{"getPixelI", ( PyCFunction ) V24_Image_getPixelI, METH_VARARGS,
	 "(int, int) - Get pixel color as ints 0-255 returns [r,g,b,a]"},
	{"setPixelF", ( PyCFunction ) V24_Image_setPixelF, METH_VARARGS,
	 "(int, int, [f r,f g,f b,f a]) - Set pixel color using floats 0.0-1.0"},
	{"setPixelI", ( PyCFunction ) V24_Image_setPixelI, METH_VARARGS,
	 "(int, int, [i r, i g, i b, i a]) - Set pixel color using ints 0-255"},
	{"getMaxXY", ( PyCFunction ) V24_Image_getMaxXY, METH_NOARGS,
	 "() - Get maximum x & y coordinates of current image as [x, y]"},
	{"getMinXY", ( PyCFunction ) V24_Image_getMinXY, METH_NOARGS,
	 "() - Get minimun x & y coordinates of image as [x, y]"},
	{"getName", ( PyCFunction ) V24_GenericLib_getName, METH_NOARGS,
	 "() - Return Image object name"},
	{"getFilename", ( PyCFunction ) V24_Image_getFilename, METH_NOARGS,
	 "() - Return Image object filename"},
	{"getSize", ( PyCFunction ) V24_Image_getSize, METH_NOARGS,
	 "() - Return Image object [width, height] dimension in pixels"},
	{"getDepth", ( PyCFunction ) V24_Image_getDepth, METH_NOARGS,
	 "() - Return Image object pixel depth"},
	{"getXRep", ( PyCFunction ) V24_Image_getXRep, METH_NOARGS,
	 "() - Return Image object x repetition value"},
	{"getYRep", ( PyCFunction ) V24_Image_getYRep, METH_NOARGS,
	 "() - Return Image object y repetition value"},
	{"getStart", ( PyCFunction ) V24_Image_getStart, METH_NOARGS,
	 "() - Return Image object start frame."},
	{"getEnd", ( PyCFunction ) V24_Image_getEnd, METH_NOARGS,
	 "() - Return Image object end frame."},
	{"getSpeed", ( PyCFunction ) V24_Image_getSpeed, METH_NOARGS,
	 "() - Return Image object speed (fps)."},
	{"getBindCode", ( PyCFunction ) V24_Image_getBindCode, METH_NOARGS,
	 "() - Return Image object's bind code value"},
	{"reload", ( PyCFunction ) V24_Image_reload, METH_NOARGS,
	 "() - Reload the image from the filesystem"},
	{"glLoad", ( PyCFunction ) V24_Image_glLoad, METH_NOARGS,
	 "() - Load the image data in OpenGL texture memory.\n\
	The bindcode (int) is returned."},
	{"glFree", ( PyCFunction ) V24_Image_glFree, METH_NOARGS,
	 "() - Free the image data from OpenGL texture memory only,\n\
		see also image.glLoad()."},
	{"setName", ( PyCFunction ) V24_GenericLib_setName_with_method, METH_VARARGS,
	 "(str) - Change Image object name"},
	{"setFilename", ( PyCFunction ) V24_Image_oldsetFilename, METH_VARARGS,
	 "(str) - Change Image file name"},
	{"setXRep", ( PyCFunction ) V24_Image_setXRep, METH_O,
	 "(int) - Change Image object x repetition value"},
	{"setYRep", ( PyCFunction ) V24_Image_setYRep, METH_O,
	 "(int) - Change Image object y repetition value"},
	{"setStart", ( PyCFunction ) V24_Image_setStart, METH_VARARGS,
	 "(int) - Change Image object animation start value"},
	{"setEnd", ( PyCFunction ) V24_Image_setEnd, METH_VARARGS,
	 "(int) - Change Image object animation end value"},
	{"setSpeed", ( PyCFunction ) V24_Image_setSpeed, METH_VARARGS,
	 "(int) - Change Image object animation speed (fps)"},
	{"save", ( PyCFunction ) V24_Image_save, METH_NOARGS,
	 "() - Write image buffer to file"},
	{"unpack", ( PyCFunction ) V24_Image_unpack, METH_VARARGS,
	 "(int) - Unpack image. Uses the values defined in Blender.UnpackModes."},
	{"pack", ( PyCFunction ) V24_Image_pack, METH_NOARGS,
	 "() - Pack the image"},
	{"makeCurrent", ( PyCFunction ) V24_Image_makeCurrent, METH_NOARGS,
	 "() - Make this the currently displayed image"},
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/* The following string definitions are used for documentation strings.	 */
/* In Python these will be written to the console when doing a		 */
/* Blender.Image.__doc__		 */
/*****************************************************************************/
static char V24_M_Image_doc[] = "The Blender Image module\n\n";

static char V24_M_Image_New_doc[] =
	"() - return a new Image object";

static char V24_M_Image_Get_doc[] =
	"(name) - return the image with the name 'name', \
returns None if not found.\n If 'name' is not specified, \
it returns a list of all images in the\ncurrent scene.";

static char V24_M_Image_GetCurrent_doc[] =
	"() - return the current image, from last active the uv/image view, \
returns None no image is in the view.\n";

static char V24_M_Image_Load_doc[] =
	"(filename) - return image from file filename as Image Object, \
returns None if not found.\n";

/*****************************************************************************/
/* Python method structure definition for Blender.Image module:		 */
/*****************************************************************************/
struct PyMethodDef M_Image_methods[] = {
	{"New", V24_M_Image_New, METH_VARARGS, V24_M_Image_New_doc},
	{"Get", V24_M_Image_Get, METH_VARARGS, V24_M_Image_Get_doc},
	{"GetCurrent", ( PyCFunction ) V24_M_Image_GetCurrent, METH_NOARGS, V24_M_Image_GetCurrent_doc},	
	{"get", V24_M_Image_Get, METH_VARARGS, V24_M_Image_Get_doc},
	{"Load", V24_M_Image_Load, METH_O, V24_M_Image_Load_doc},
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/* Function:    V24_M_Image_New     */
/* Python equivalent:        Blender.Image.New    */
/*****************************************************************************/
static PyObject *V24_M_Image_New( PyObject * self, PyObject * args)
{
	int width, height, depth;
	char *name;
	float color[] = {0, 0, 0, 1};
	Image *image;
	if( !PyArg_ParseTuple( args, "siii", &name, &width, &height, &depth ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected 1 string and 3 ints" ) );
	if (width > 5000 || height > 5000 || width < 1 || height < 1)
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					"Image width and height must be between 1 and 5000" ) );
	image = BKE_add_image_size(width, height, name, 0, color);
	if( !image )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create PyObject V24_Image_Type" ) );

	/* reset usage count, since BKE_add_image_size() incremented it */
	/* image->id.us--; */
	/* Strange, new images have a user count of one???, otherwise it messes up */
	
	return V24_Image_CreatePyObject( image );
}



/*****************************************************************************/
/* Function:		V24_M_Image_Get	 */
/* Python equivalent:	Blender.Image.Get   */
/* Description:		Receives a string and returns the image object	 */
/*			whose name matches the string.	If no argument is  */
/*			passed in, a list of all image names in the	 */
/*			current scene is returned.			 */
/*****************************************************************************/
static PyObject *V24_M_Image_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Image *img_iter;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected string argument (or nothing)" ) );

	img_iter = G.main->image.first;

	if( name ) {		/* (name) - Search image by name */

		V24_BPy_Image *wanted_image = NULL;

		while( ( img_iter ) && ( wanted_image == NULL ) ) {
			if( strcmp( name, img_iter->id.name + 2 ) == 0 ) {
				wanted_image = ( V24_BPy_Image * )
					PyObject_NEW( V24_BPy_Image, &V24_Image_Type );
				if( wanted_image )
					wanted_image->image = img_iter;
			}
			img_iter = img_iter->id.next;
		}

		if( wanted_image == NULL ) {	/* Requested image doesn't exist */
			char error_msg[64];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Image \"%s\" not found", name );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_NameError, error_msg ) );
		}

		return ( PyObject * ) wanted_image;
	}

	else {			/* () - return a list of all images in the scene */
		int index = 0;
		PyObject *img_list, *pyobj;

		img_list = PyList_New( BLI_countlist( &( G.main->image ) ) );

		if( img_list == NULL )
			return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
							"couldn't create PyList" ) );

		while( img_iter ) {
			pyobj = V24_Image_CreatePyObject( img_iter );

			PyList_SET_ITEM( img_list, index, pyobj );

			img_iter = img_iter->id.next;
			index++;
		}

		return ( img_list );
	}
}



/*****************************************************************************/
/* Function:		V24_M_Image_GetCurrent*/
/* Python equivalent:	Blender.Image.GetCurrent   */
/* Description:		Returns the active current (G.sima)	 */
/*			This will be the image last under the mouse cursor */
/*			None if there is no Image.			 */
/*****************************************************************************/
static PyObject *V24_M_Image_GetCurrent( PyObject * self )
{
	if (!G.sima || !G.sima->image) {
		Py_RETURN_NONE;
	}
	what_image( G.sima );	/* make sure image data exists */
	return V24_Image_CreatePyObject( G.sima->image );
}


/*****************************************************************************/
/* Function:	V24_M_Image_Load		 */
/* Python equivalent:	Blender.Image.Load   */
/* Description:		Receives a string and returns the image object	 */
/*			whose filename matches the string.		 */
/*****************************************************************************/
static PyObject *V24_M_Image_Load( PyObject * self, PyObject * value )
{
	char *fname = PyString_AsString(value);
	Image *img_ptr;
	V24_BPy_Image *image;

	if( !value )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected string argument" ) );

	image = ( V24_BPy_Image * ) PyObject_NEW( V24_BPy_Image, &V24_Image_Type );

	if( !image )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create PyObject V24_Image_Type" ) );

	img_ptr = BKE_add_image_file( fname );
	if( !img_ptr )
		return ( V24_EXPP_ReturnPyObjError( PyExc_IOError,
						"couldn't load image" ) );

	/* force a load the image buffers*/
	BKE_image_get_ibuf(img_ptr, NULL);

	image->image = img_ptr;

	return ( PyObject * ) image;
}


/**
 * getPixelF( x, y )
 *  returns float list of pixel colors in rgba order.
 *  returned values are floats normalized to 0.0 - 1.0.
 *  blender images are all 4x8 bit at the moment apr-2005
 */

static PyObject *V24_Image_getPixelF( V24_BPy_Image * self, PyObject * args )
{

	PyObject *attr;
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	char *pixel;		/* image data */
	int index;		/* offset into image data */
	int x = 0;
	int y = 0;
	int pixel_size = 4;	/* each pixel is 4 x 8-bits packed in unsigned int */
	int i;
	
	if( !PyArg_ParseTuple( args, "ii", &x, &y ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers" );

	if( !ibuf || !ibuf->rect )	/* loading didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	if( ibuf->type == 1 )	/* bitplane image */
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of range" );

	/* 
	   assumption: from looking at source, skipx is often not set,
	   so we calc ourselves
	 */

	attr = PyList_New(4);
	
	if (!attr)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't allocate memory for color list" );
	
	index = ( x + y * ibuf->x ) * pixel_size;

	pixel = ( char * ) ibuf->rect;
	for (i=0; i<4; i++) {
		PyList_SetItem( attr, i, PyFloat_FromDouble( ( ( double ) pixel[index+i] ) / 255.0 ));
	}
	return attr;
}


/**
 * getPixelI( x, y )
 *  returns integer list of pixel colors in rgba order.
 *  returned values are ints normalized to 0-255.
 *  blender images are all 4x8 bit at the moment apr-2005
 */

static PyObject *V24_Image_getPixelI( V24_BPy_Image * self, PyObject * args )
{
	PyObject *attr = PyList_New(4);
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	char *pixel;		/* image data */
	int index;		/* offset into image data */
	int x = 0;
	int y = 0;
	int pixel_size = 4;	/* each pixel is 4 x 8-bits packed in unsigned int */
	int i;

	if (!attr)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't allocate memory for color list" );
	
	if( !PyArg_ParseTuple( args, "ii", &x, &y ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers" );

	if( !ibuf || !ibuf->rect )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	if( ibuf->type == 1 )	/* bitplane image */
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of range" );

	/* 
	   assumption: from looking at source, skipx is often not set,
	   so we calc ourselves
	 */

	index = ( x + y * ibuf->x ) * pixel_size;
	pixel = ( char * ) ibuf->rect;
	
	for (i=0; i<4; i++) {
		PyList_SetItem( attr, i, PyInt_FromLong( pixel[index+i] ));
	}
	return attr;
}


/* set pixel as floats */

static PyObject *V24_Image_setPixelF( V24_BPy_Image * self, PyObject * args )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	char *pixel;		/* image data */
	int index;		/* offset into image data */
	int x = 0;
	int y = 0;
	int a = 0;
	int pixel_size = 4;	/* each pixel is 4 x 8-bits packed in unsigned int */
	float p[4];

	if( !PyArg_ParseTuple
	    ( args, "ii(ffff)", &x, &y, &p[0], &p[1], &p[2], &p[3] ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers and an array of 4 floats" );

	if( !ibuf || !ibuf->rect )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	if( ibuf->type == 1 )	/* bitplane image */
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of ruange" );

	for( a = 0; a < 4; a++ ) {
		if( p[a] > 1.0 || p[a] < 0.0 )
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "r, g, b, or a is out of range" );
	}


	/* 
	   assumption: from looking at source, skipx is often not set,
	   so we calc ourselves
	 */

	index = ( x + y * ibuf->x ) * pixel_size;

	pixel = ( char * ) ibuf->rect;

	pixel[index] = ( char ) ( p[0] * 255.0 );
	pixel[index + 1] = ( char ) ( p[1] * 255.0 );
	pixel[index + 2] = ( char ) ( p[2] * 255.0 );
	pixel[index + 3] = ( char ) ( p[3] * 255.0 );

	ibuf->userflags |= IB_BITMAPDIRTY;
	Py_RETURN_NONE;
}


/* set pixel as ints */

static PyObject *V24_Image_setPixelI( V24_BPy_Image * self, PyObject * args )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	char *pixel;		/* image data */
	int index;		/* offset into image data */
	int x = 0;
	int y = 0;
	int a = 0;
	int pixel_size = 4;	/* each pixel is 4 x 8-bits packed in unsigned int */
	int p[4];

	if( !PyArg_ParseTuple
	    ( args, "ii(iiii)", &x, &y, &p[0], &p[1], &p[2], &p[3] ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers and an list of 4 ints" );

	if( !ibuf || !ibuf->rect )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	if( ibuf->type == 1 )	/* bitplane image */
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of range" );

	for( a = 0; a < 4; a++ ) {
		if( p[a] > 255 || p[a] < 0 )
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "r, g, b, or a is out of range" );
	}

	/* 
	   assumption: from looking at source, skipx is often not set,
	   so we calc ourselves
	 */

	index = ( x + y * ibuf->x ) * pixel_size;

	pixel = ( char * ) ibuf->rect;

	pixel[index] = ( char ) p[0];
	pixel[index + 1] = ( char ) p[1];
	pixel[index + 2] = ( char ) p[2];
	pixel[index + 3] = ( char ) p[3];
	
	ibuf->userflags |= IB_BITMAPDIRTY;
	Py_RETURN_NONE;
}


/* get max extent of image */

static PyObject *V24_Image_getMaxXY( V24_BPy_Image * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

	if( !ibuf || !ibuf->rect )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	return Py_BuildValue( "[i,i]", ibuf->x, ibuf->y );
}


/* get min extent of image */

static PyObject *V24_Image_getMinXY( V24_BPy_Image * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

	if( !ibuf || !ibuf->rect )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	return Py_BuildValue( "[i,i]", ibuf->xorig, ibuf->yorig );
}

/* unpack image */

static PyObject *V24_Image_unpack( V24_BPy_Image * self, PyObject * value )
{
	Image *image = self->image;
	int mode = (int)PyInt_AsLong(value);
	
	/*get the absolute path */
	if( mode==-1 )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 1 integer from Blender.UnpackModes" );
	
	if (image->packedfile==NULL)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "image not packed" );
	
	if (unpackImage(image, mode) == RET_ERROR)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"error unpacking image" );
	Py_RETURN_NONE;
}

/* pack image */

static PyObject *V24_Image_pack( V24_BPy_Image * self )
{
	Image *image = self->image;	
	ImBuf *ibuf = BKE_image_get_ibuf(image, NULL);
	
	if( !ibuf || !ibuf->rect )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't load image data in Blender" );
	
	if (image->packedfile ) { /* RePack? */
		if (ibuf->userflags & IB_BITMAPDIRTY)
			BKE_image_memorypack(image);
	} else { /* Pack for the first time */
		if (ibuf && (ibuf->userflags & IB_BITMAPDIRTY))
			BKE_image_memorypack(image);
		else
			image->packedfile = newPackedFile(image->name);
	}
	Py_RETURN_NONE;
}


static PyObject *V24_Image_makeCurrent( V24_BPy_Image * self )
{
#if 0	/* add back in when bpy becomes "official" */
	static char warning = 1;
	if( warning ) {
		printf("image.makeCurrent() deprecated!\n\t use 'bpy.images.active = image instead'\n");
		--warning;
	}
#endif
	
	if (!G.sima)
		Py_RETURN_FALSE;
	
	G.sima->image= self->image;
	Py_RETURN_TRUE;
}


/* save image to file */

static PyObject *V24_Image_save( V24_BPy_Image * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

	if(!ibuf)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "could not save image (no image buffer)" );
	
	/* If this is a packed file, write using writePackedFile
	 * because IMB_saveiff wont save to a file */
	if (self->image->packedfile) {
		if (writePackedFile(self->image->name, self->image->packedfile, 0) != RET_OK) {
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
		      "could not save image (writing image from packedfile failed)" );
		}
	} else if (!IMB_saveiff( ibuf, self->image->name, ibuf->flags))
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "could not save image (writing the image buffer failed)" );

	Py_RETURN_NONE;		/*  normal return, image saved */
}

static PyObject *V24_M_Image_SourceDict( void )
{
	PyObject *Dict = V24_PyConstant_New(  );
	if( Dict ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Dict;
		V24_PyConstant_Insert(d, "STILL", PyInt_FromLong(IMA_SRC_FILE));
		V24_PyConstant_Insert(d, "MOVIE", PyInt_FromLong(IMA_SRC_MOVIE));
		V24_PyConstant_Insert(d, "SEQUENCE", PyInt_FromLong(IMA_SRC_SEQUENCE));
		V24_PyConstant_Insert(d, "GENERATED", PyInt_FromLong(IMA_SRC_GENERATED));
	}
	return Dict;
}

/*****************************************************************************/
/* Function:		V24_Image_Init	 */
/*****************************************************************************/
PyObject *V24_Image_Init( void )
{
	PyObject *submodule;
	PyObject *Sources = V24_M_Image_SourceDict( );

	if( PyType_Ready( &V24_Image_Type ) < 0 )
		return NULL;

	submodule =
		Py_InitModule3( "Blender.Image", M_Image_methods,
				V24_M_Image_doc );

	if( Sources )
		PyModule_AddObject( submodule, "Sources", Sources );

	return submodule;
}

/*****************************************************************************/
/* Python V24_Image_Type callback function prototypes:	 */
/*****************************************************************************/
static int V24_Image_compare( V24_BPy_Image * a, V24_BPy_Image * b );
static PyObject *V24_Image_repr( V24_BPy_Image * self );

/*****************************************************************************/
/* Function:		V24_Image_CreatePyObject	 */
/* Description: This function will create a new V24_BPy_Image from an existing  */
/*		Blender image structure.	 */
/*****************************************************************************/
PyObject *V24_Image_CreatePyObject( Image * image )
{
	V24_BPy_Image *py_img;
	py_img = ( V24_BPy_Image * ) PyObject_NEW( V24_BPy_Image, &V24_Image_Type );
	
	if( !py_img )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create V24_BPy_Image object" );
	
	py_img->image = image;
	return ( PyObject * ) py_img;
}

/*****************************************************************************/
/* Function:	V24_Image_FromPyObject	 */
/* Description: Returns the Blender Image associated with this object  	 */
/*****************************************************************************/
Image *V24_Image_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Image * ) pyobj )->image;
}

static PyObject *V24_Image_getFilename( V24_BPy_Image * self )
{
	return PyString_FromString( self->image->name );
}

static PyObject *V24_Image_getSize( V24_BPy_Image * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	PyObject *attr;
	
	if( !ibuf )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	attr = PyList_New(2);
	
	if( !attr )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't get Image.size attribute" );
	
	PyList_SetItem( attr, 0, PyInt_FromLong(ibuf->x));
	PyList_SetItem( attr, 1, PyInt_FromLong(ibuf->y));
	return attr;
}

static PyObject *V24_Image_getDepth( V24_BPy_Image * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

	if( !ibuf )	/* didn't work */
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't load image data in Blender" );

	return PyInt_FromLong( (long)ibuf->depth );
}


static PyObject *V24_Image_getXRep( V24_BPy_Image * self )
{
	return PyInt_FromLong( self->image->xrep );
}

static PyObject *V24_Image_getYRep( V24_BPy_Image * self )
{
	return PyInt_FromLong( self->image->yrep );
}

static PyObject *V24_Image_getStart( V24_BPy_Image * self )
{
	return PyInt_FromLong( self->image->twsta );
}

static PyObject *V24_Image_getEnd( V24_BPy_Image * self )
{
	return PyInt_FromLong( self->image->twend );
}

static PyObject *V24_Image_getSpeed( V24_BPy_Image * self )
{
	return PyInt_FromLong( self->image->animspeed );
}

static PyObject *V24_Image_getBindCode( V24_BPy_Image * self )
{
	return PyLong_FromUnsignedLong( self->image->bindcode );
}

static PyObject *V24_Image_reload( V24_BPy_Image * self )
{
	Image *image = self->image;

	BKE_image_signal(image, NULL, IMA_SIGNAL_RELOAD);

	Py_RETURN_NONE;
}

static PyObject *V24_Image_glFree( V24_BPy_Image * self )
{
	Image *image = self->image;

	free_realtime_image( image );
	/* remove the nocollect flag, image is available for garbage collection again */
	image->flag &= ~IMA_NOCOLLECT;
	Py_RETURN_NONE;
}

static PyObject *V24_Image_glLoad( V24_BPy_Image * self )
{
	Image *image = self->image;
	unsigned int *bind = &image->bindcode;

	if( *bind == 0 ) {
		ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

		if( !ibuf )	/* didn't work */
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "couldn't load image data in Blender" );

		glGenTextures( 1, ( GLuint * ) bind );
		glBindTexture( GL_TEXTURE_2D, *bind );

		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, ibuf->x,
				   ibuf->y, GL_RGBA, GL_UNSIGNED_BYTE,
				   ibuf->rect );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				 GL_LINEAR_MIPMAP_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				 GL_LINEAR );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ibuf->x,
			      ibuf->y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			      ibuf->rect );

		/* raise the nocollect flag, 
		   image is not available for garbage collection 
		   (python GL might use it directly)
		 */
		image->flag |= IMA_NOCOLLECT;
	}

	return PyLong_FromUnsignedLong( image->bindcode );
}

static int V24_Image_setFilename( V24_BPy_Image * self, PyObject * value )
{
	char *name;

	name = PyString_AsString(value);

	if( !name )
		return ( V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected a string argument" ) );

	/* max len is FILE_MAXDIR == 160, FILE_MAXFILE == 80 chars like done in DNA_image_types.h */
	if( strlen(name) >= FILE_MAXDIR + FILE_MAXFILE )
		return ( V24_EXPP_ReturnIntError( PyExc_TypeError,
						"string argument is limited to 240 chars at most" ) );

	strcpy( self->image->name, name );
	return 0;
}

static PyObject *V24_Image_oldsetFilename( V24_BPy_Image * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Image_setFilename );
}

static PyObject *V24_Image_setXRep( V24_BPy_Image * self, PyObject * value )
{
	short param = (short)PyInt_AsLong(value);

	if( param !=-1 && param >= EXPP_IMAGE_REP_MIN && param <= EXPP_IMAGE_REP_MAX)
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument in [1,16]" ) );

	self->image->xrep = param;
	Py_RETURN_NONE;
}

static PyObject *V24_Image_setYRep( V24_BPy_Image * self, PyObject * value )
{
	short param = (short)PyInt_AsLong(value);

	if( param !=-1 && param >= EXPP_IMAGE_REP_MIN && param <= EXPP_IMAGE_REP_MAX)
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument in [1,16]" ) );

	self->image->yrep = param;
	Py_RETURN_NONE;
}


static PyObject *V24_Image_setStart( V24_BPy_Image * self, PyObject * args )
{
	short value;

	if( !PyArg_ParseTuple( args, "h", &value ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument in [0,128]" ) );

	if( value >= 0 && value <= 128 )
		self->image->twsta = value;
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_ValueError,
						"expected int argument in [0,128]" ) );

	Py_RETURN_NONE;
}


static PyObject *V24_Image_setEnd( V24_BPy_Image * self, PyObject * args )
{
	short value;

	if( !PyArg_ParseTuple( args, "h", &value ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument in [0,128]" ) );

	if( value >= 0 && value <= 128 )
		self->image->twend = value;
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_ValueError,
						"expected int argument in [0,128]" ) );

	Py_RETURN_NONE;
}

static PyObject *V24_Image_setSpeed( V24_BPy_Image * self, PyObject * args )
{
	short value;

	if( !PyArg_ParseTuple( args, "h", &value ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument in [0,128]" ) );

	if( value >= 1 && value <= 100 )
		self->image->animspeed = value;
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_ValueError,
						"expected int argument in [0,128]" ) );

	Py_RETURN_NONE;
}


/*****************************************************************************/
/* Function:	V24_Image_compare			 */
/* Description: This is a callback function for the V24_BPy_Image type. It	 */
/*		compares two V24_Image_Type objects. Only the "==" and "!="	 */
/*		comparisons are meaninful. Returns 0 for equality and -1 if  */
/*		they don't point to the same Blender Image struct.	 */
/*		In Python it becomes 1 if they are equal, 0 otherwise.	 */
/*****************************************************************************/
static int V24_Image_compare( V24_BPy_Image * a, V24_BPy_Image * b )
{
	return ( a->image == b->image ) ? 0 : -1;
}

/*****************************************************************************/
/* Function:	V24_Image_repr	 */
/* Description: This is a callback function for the V24_BPy_Image type. It	 */
/*		builds a meaninful string to represent image objects.	 */
/*****************************************************************************/
static PyObject *V24_Image_repr( V24_BPy_Image * self )
{
	return PyString_FromFormat( "[Image \"%s\"]",
				    self->image->id.name + 2 );
}

static PyObject *V24_Image_getPacked(V24_BPy_Image *self, void *closure)
{
	if (self->image->packedfile)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject *V24_Image_hasData(V24_BPy_Image *self, void *closure)
{
	if (self->image->ibufs.first)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject *V24_Image_getFlag(V24_BPy_Image *self, void *flag)
{
	if (self->image->flag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static PyObject *V24_Image_getFlagTpage(V24_BPy_Image *self, void *flag)
{
	if (self->image->tpageflag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int V24_Image_setSource( V24_BPy_Image *self, PyObject *args)
{
    PyObject* integer = PyNumber_Int( args );
	short value;

	if( !integer )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	value = ( short )PyInt_AS_LONG( integer );
	Py_DECREF( integer );

	if( value < IMA_SRC_FILE || value > IMA_SRC_GENERATED )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"expected integer argument in range 1-4" );

	self->image->source = value;
	return 0;
}

static int V24_Image_setFlag(V24_BPy_Image *self, PyObject *value, void *flag)
{
	int param = PyObject_IsTrue( value );
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if ( param )
		self->image->flag |= (int)flag;
	else
		self->image->flag &= ~(int)flag;
	return 0;
}

static int V24_Image_setFlagTpage(V24_BPy_Image *self, PyObject *value, void *flag)
{
	int param = PyObject_IsTrue( value );
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if ( param )
		self->image->tpageflag |= (int)flag;
	else
		self->image->tpageflag &= ~(int)flag;
	return 0;
}

/*
 * get integer attributes
 */
static PyObject *getIntAttr( V24_BPy_Image *self, void *type )
{
	int param;
	struct Image *image = self->image;

	switch( (int)type ) {
	case EXPP_IMAGE_ATTR_XREP:
		param = image->xrep;
		break;
	case EXPP_IMAGE_ATTR_YREP:
		param = image->xrep;
		break;
	case EXPP_IMAGE_ATTR_START:
		param = image->twsta;
		break;	
	case EXPP_IMAGE_ATTR_END:
		param = image->twend;
		break;
	case EXPP_IMAGE_ATTR_SPEED:
		param = image->animspeed;
		break;
	case EXPP_IMAGE_ATTR_BINDCODE:
		param = image->bindcode;
		break;
	case EXPP_IMAGE_ATTR_SOURCE:
		param = image->source;
		break;
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getIntAttr" );
	}

	return PyInt_FromLong( param );
}


/*
 * set integer attributes which require clamping
 */

static int setIntAttrClamp( V24_BPy_Image *self, PyObject *value, void *type )
{
	void *param;
	struct Image *image = self->image;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_IMAGE_ATTR_XREP:
		min = EXPP_IMAGE_REP_MIN;
		max = EXPP_IMAGE_REP_MAX;
		size = 'h';
		param = (void *)&image->xrep;
		break;
	case EXPP_IMAGE_ATTR_YREP:
		min = EXPP_IMAGE_REP_MIN;
		max = EXPP_IMAGE_REP_MAX;
		size = 'h';
		param = (void *)&image->yrep;
		break;
	case EXPP_IMAGE_ATTR_START:
		min = 0;
		max = 128;
		size = 'h';
		param = (void *)&image->twsta;
		break;
	case EXPP_IMAGE_ATTR_END:
		min = 0;
		max = 128;
		size = 'h';
		param = (void *)&image->twend;
		break;
	case EXPP_IMAGE_ATTR_SPEED:
		min = 0;
		max = 100;
		size = 'h';
		param = (void *)&image->animspeed;
		break;
	default:
		return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}
	return V24_EXPP_setIValueClamped( value, param, min, max, size );
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Image_getseters[] = {
	V24_GENERIC_LIB_GETSETATTR,
	{"filename", (getter)V24_Image_getFilename, (setter)V24_Image_setFilename,
	 "image path", NULL},
	/* readonly */
	{"depth", (getter)V24_Image_getDepth, (setter)NULL,
	 "image depth", NULL},
	{"size", (getter)V24_Image_getSize, (setter)NULL,
	 "image size", NULL},
	{"packed", (getter)V24_Image_getPacked, (setter)NULL,
	 "image packed state", NULL },
	{"has_data", (getter)V24_Image_hasData, (setter)NULL,
	 "is image data loaded", NULL },
	/* ints */
	{"xrep", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "image xrep", (void *)EXPP_IMAGE_ATTR_XREP },
	{"yrep", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "image yrep", (void *)EXPP_IMAGE_ATTR_YREP },
	{"start", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "image start frame", (void *)EXPP_IMAGE_ATTR_START },
	{"end", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "image end frame", (void *)EXPP_IMAGE_ATTR_END },
	{"speed", (getter)getIntAttr, (setter)setIntAttrClamp,
	 "image end frame", (void *)EXPP_IMAGE_ATTR_SPEED },
	{"bindcode", (getter)getIntAttr, (setter)NULL,
	 "openGL bindcode", (void *)EXPP_IMAGE_ATTR_BINDCODE },
	{"source", (getter)getIntAttr, (setter)V24_Image_setSource,
	 "image source type", (void *)EXPP_IMAGE_ATTR_SOURCE },
	/* flags */
	{"fields", (getter)V24_Image_getFlag, (setter)V24_Image_setFlag,
	 "image fields toggle", (void *)IMA_FIELDS },
	{"fields_odd", (getter)V24_Image_getFlag, (setter)V24_Image_setFlag,
	 "image odd fields toggle", (void *)IMA_STD_FIELD },
	{"antialias", (getter)V24_Image_getFlag, (setter)V24_Image_setFlag,
	 "image antialiasing toggle", (void *)IMA_ANTIALI },
	{"reflect", (getter)V24_Image_getFlag, (setter)V24_Image_setFlag,
	 "image reflect toggle", (void *)IMA_REFLECT },
	{"clampX", (getter)V24_Image_getFlagTpage, (setter)V24_Image_setFlagTpage,
	 "disable tiling on the X axis", (void *)IMA_CLAMP_U },
	{"clampY", (getter)V24_Image_getFlagTpage, (setter)V24_Image_setFlagTpage,
	 "disable tiling on the Y axis", (void *)IMA_CLAMP_V },
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};


/*****************************************************************************/
/* Python V24_Image_Type structure definition:   */
/*****************************************************************************/
PyTypeObject V24_Image_Type = {
	PyObject_HEAD_INIT( NULL ) /*     required macro. ( no comma needed )  */ 
	0,	/* ob_size */
	"Blender Image",	/* tp_name */
	sizeof( V24_BPy_Image ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	0,		/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	( cmpfunc ) V24_Image_compare,	/* tp_compare */
	( reprfunc ) V24_Image_repr,	/* tp_repr */

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
	V24_BPy_Image_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Image_getseters,         /* struct PyGetSetDef *tp_getset; */
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
