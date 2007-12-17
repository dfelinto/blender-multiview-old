/* 
 * $Id: Image.c 11467 2007-08-01 20:26:24Z campbellbarton $
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
#include "BKE_utildefines.h"
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
#include "Const.h"

/* used so we can get G.scene->r.cfra for getting the
current image frame, some images change frame if they are a sequence */
#include "DNA_scene_types.h"

#define IMBUF_DEL_CHECK_INT(ibuf) if (!ibuf) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "couldn't load image data in Blender" ) )
#define IMBUF_DEL_CHECK_PY(ibuf) if (!ibuf) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "couldn't load image data in Blender" ) )

#define IMBUF_RECT_DEL_CHECK_INT(ibuf) if (!ibuf || !ibuf->rect) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "couldn't load image data in Blender" ) )
#define IMBUF_RECT_DEL_CHECK_PY(ibuf) if (!ibuf || !ibuf->rect) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "couldn't load image data in Blender" ) )

/*****************************************************************************/
/* Python BPyImageObject defaults:																								 */
/*****************************************************************************/
#define EXPP_IMAGE_REP			1

enum img_consts {
	EXPP_IMAGE_ATTR_XREP = 0,
	EXPP_IMAGE_ATTR_YREP,
	EXPP_IMAGE_ATTR_START,
	EXPP_IMAGE_ATTR_END,
	EXPP_IMAGE_ATTR_SPEED,
	EXPP_IMAGE_ATTR_BINDCODE,
};

/*****************************************************************************/
/* Python BPyImageObject methods declarations:	 */
/*****************************************************************************/
static PyObject *Image_getFilename( BPyImageObject * self );
static PyObject *Image_getSource( BPyImageObject * self );
static int Image_setSource( BPyImageObject * self, PyObject *value  );
static PyObject *Image_getSize( BPyImageObject * self );
static PyObject *Image_getDepth( BPyImageObject * self );
static int Image_setFilename( BPyImageObject * self, PyObject * args );
static PyObject *Image_reload( BPyImageObject * self );
static PyObject *Image_glLoad( BPyImageObject * self );
static PyObject *Image_glFree( BPyImageObject * self );
static PyObject *Image_getPixelF( BPyImageObject * self, PyObject * args );
static PyObject *Image_getPixelI( BPyImageObject * self, PyObject * args );
static PyObject *Image_setPixelF( BPyImageObject * self, PyObject * args );
static PyObject *Image_setPixelI( BPyImageObject * self, PyObject * args );
static PyObject *Image_getMaxXY( BPyImageObject * self );
static PyObject *Image_getMinXY( BPyImageObject * self );
static PyObject *Image_save( BPyImageObject * self );
static PyObject *Image_unpack( BPyImageObject * self, PyObject * args );
static PyObject *Image_pack( BPyImageObject * self );

/*****************************************************************************/
/* Python BPyImageObject methods table:	 */
/*****************************************************************************/
static PyMethodDef BPyImage_methods[] = {
	/* name, method, flags, doc */
	{"getPixelF", ( PyCFunction ) Image_getPixelF, METH_VARARGS,
	 "(int, int) - Get pixel color as floats 0.0-1.0 returns [r,g,b,a]"},
	{"getPixelI", ( PyCFunction ) Image_getPixelI, METH_VARARGS,
	 "(int, int) - Get pixel color as ints 0-255 returns [r,g,b,a]"},
	{"setPixelF", ( PyCFunction ) Image_setPixelF, METH_VARARGS,
	 "(int, int, [f r,f g,f b,f a]) - Set pixel color using floats 0.0-1.0"},
	{"setPixelI", ( PyCFunction ) Image_setPixelI, METH_VARARGS,
	 "(int, int, [i r, i g, i b, i a]) - Set pixel color using ints 0-255"},
	{"reload", ( PyCFunction ) Image_reload, METH_NOARGS,
	 "() - Reload the image from the filesystem"},
	{"glLoad", ( PyCFunction ) Image_glLoad, METH_NOARGS,
	 "() - Load the image data in OpenGL texture memory.\n\
	The bindcode (int) is returned."},
	{"glFree", ( PyCFunction ) Image_glFree, METH_NOARGS,
	 "() - Free the image data from OpenGL texture memory only,\n\
		see also image.glLoad()."},
	{"save", ( PyCFunction ) Image_save, METH_NOARGS,
	 "() - Write image buffer to file"},
	{"unpack", ( PyCFunction ) Image_unpack, METH_VARARGS,
	 "(int) - Unpack image. Uses the values defined in Blender.UnpackModes."},
	{"pack", ( PyCFunction ) Image_pack, METH_NOARGS,
	 "() - Pack the image"},
	{NULL, NULL, 0, NULL}
};

/**
 * getPixelF( x, y )
 *  returns float list of pixel colors in rgba order.
 *  returned values are floats normalized to 0.0 - 1.0.
 *  blender images are all 4x8 bit at the moment apr-2005
 */

static PyObject *Image_getPixelF( BPyImageObject * self, PyObject * args )
{

	PyObject *attr;
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	char *pixel;		/* image data */
	int index;		/* offset into image data */
	int x = 0;
	int y = 0;
	int pixel_size = 4;	/* each pixel is 4 x 8-bits packed in unsigned int */
	int i;
	
	IMBUF_RECT_DEL_CHECK_PY(ibuf);
	
	if( !PyArg_ParseTuple( args, "ii", &x, &y ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers" );

	if( ibuf->type == 1 )	/* bitplane image */
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of range" );

	/* 
	   assumption: from looking at source, skipx is often not set,
	   so we calc ourselves
	 */

	attr = PyList_New(4);
	
	if (!attr)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
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

static PyObject *Image_getPixelI( BPyImageObject * self, PyObject * args )
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
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't allocate memory for color list" );
	
	if( !PyArg_ParseTuple( args, "ii", &x, &y ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers" );

	IMBUF_RECT_DEL_CHECK_PY(ibuf);
	
	if( ibuf->type == 1 )	/* bitplane image */
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
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

static PyObject *Image_setPixelF( BPyImageObject * self, PyObject * args )
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
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers and an array of 4 floats" );

	IMBUF_RECT_DEL_CHECK_PY(ibuf);

	if( ibuf->type == 1 )	/* bitplane image */
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of ruange" );

	for( a = 0; a < 4; a++ ) {
		if( p[a] > 1.0 || p[a] < 0.0 )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
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

static PyObject *Image_setPixelI( BPyImageObject * self, PyObject * args )
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
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 2 integers and an list of 4 ints" );

	IMBUF_RECT_DEL_CHECK_PY(ibuf);

	if( ibuf->type == 1 )	/* bitplane image */
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "unsupported bitplane image format" );

	if( x > ( ibuf->x - 1 )
	    || y > ( ibuf->y - 1 )
	    || x < ibuf->xorig || y < ibuf->yorig )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "x or y is out of range" );

	for( a = 0; a < 4; a++ ) {
		if( p[a] > 255 || p[a] < 0 )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
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

static PyObject *Image_getMaxXY( BPyImageObject * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	IMBUF_RECT_DEL_CHECK_PY(ibuf);
	return Py_BuildValue( "(i,i)", ibuf->x, ibuf->y );
}


/* get min extent of image */

static PyObject *Image_getMinXY( BPyImageObject * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	IMBUF_RECT_DEL_CHECK_PY(ibuf);
	return Py_BuildValue( "(i,i)", ibuf->xorig, ibuf->yorig );
}

/* unpack image */

static PyObject *Image_unpack( BPyImageObject * self, PyObject * args )
{
	Image *image = self->image;
	int mode;
	
	/*get the absolute path */
	if( !PyArg_ParseTuple( args, "i", &mode ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 1 integer from Blender.UnpackModes" );
	
	if (image->packedfile==NULL)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "image not packed" );
	
	if (unpackImage(image, mode) == RET_ERROR)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"error unpacking image" );
	Py_RETURN_NONE;
}

/* pack image */

static PyObject *Image_pack( BPyImageObject * self )
{
	Image *image = self->image;	
	ImBuf *ibuf = BKE_image_get_ibuf(image, NULL);
	IMBUF_RECT_DEL_CHECK_PY(ibuf);
	
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

/* save image to file */

static PyObject *Image_save( BPyImageObject * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

	IMBUF_DEL_CHECK_PY(ibuf);
	
	/* If this is a packed file, write using writePackedFile
	 * because IMB_saveiff wont save to a file */
	if (self->image->packedfile) {
		if (writePackedFile(self->image->name, self->image->packedfile, 0) != RET_OK) {
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
		      "could not save image (writing image from packedfile failed)" );
		}
	} else if (!IMB_saveiff( ibuf, self->image->name, ibuf->flags))
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "could not save image (writing the image buffer failed)" );

	Py_RETURN_NONE;		/*  normal return, image saved */
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

static constIdents sourceTypesIdents[] = {
	{"STILL",   	{(int)IMA_SRC_FILE}},
	{"MOVIE",		{(int)IMA_SRC_MOVIE}},
	{"SEQUENCE",	{(int)IMA_SRC_SEQUENCE}},
	{"GENERATED",	{(int)IMA_SRC_GENERATED}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition sourceTypes = {
	EXPP_CONST_INT, "SourceTypes",
		sizeof(sourceTypesIdents)/sizeof(constIdents), sourceTypesIdents
};

/*****************************************************************************/
/* Function:		Image_CreatePyObject	 */
/* Description: This function will create a new BPyImageObject from an existing  */
/*		Blender image structure.	 */
/*****************************************************************************/
PyObject *Image_CreatePyObject( Image * image )
{
	BPyImageObject *py_img;
	
	if (!image)
		Py_RETURN_NONE;
	
	py_img = ( BPyImageObject * ) PyObject_NEW( BPyImageObject, &BPyImage_Type );
	
	if( !py_img )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyImageObject object" );
	
	py_img->image = image;
	return ( PyObject * ) py_img;
}

static PyObject *Image_getFilename( BPyImageObject * self )
{
	return PyString_FromString( self->image->name );
}

static PyObject *Image_getSize( BPyImageObject * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	IMBUF_DEL_CHECK_PY(ibuf);
	return Py_BuildValue( "(i,i)", ibuf->x, ibuf->y );
}

static PyObject *Image_getDepth( BPyImageObject * self )
{
	ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);
	IMBUF_DEL_CHECK_PY(ibuf);
	return PyInt_FromLong( (long)ibuf->depth );
}

static PyObject *Image_reload( BPyImageObject * self )
{
	Image *image = self->image;

	BKE_image_signal(image, NULL, IMA_SIGNAL_RELOAD);

	Py_RETURN_NONE;
}

static PyObject *Image_glFree( BPyImageObject * self )
{
	Image *image = self->image;

	free_realtime_image( image );
	/* remove the nocollect flag, image is available for garbage collection again */
	image->flag &= ~IMA_NOCOLLECT;
	Py_RETURN_NONE;
}

static PyObject *Image_glLoad( BPyImageObject * self )
{
	Image *image = self->image;
	unsigned int *bind = &image->bindcode;

	if( *bind == 0 ) {
		ImBuf *ibuf= BKE_image_get_ibuf(self->image, NULL);

		IMBUF_DEL_CHECK_PY(ibuf);

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

static int Image_setFilename( BPyImageObject * self, PyObject * value )
{
	char *name = PyString_AsString(value);
	if( !name )
		return ( EXPP_ReturnIntError( PyExc_TypeError,
						"expected a string argument" ) );

	/* max len is FILE_MAXDIR == 160, FILE_MAXFILE == 80 chars like done in DNA_image_types.h */
	if( strlen(name) >= FILE_MAXDIR + FILE_MAXFILE )
		return ( EXPP_ReturnIntError( PyExc_TypeError,
						"string argument is limited to 240 chars at most" ) );

	strcpy( self->image->name, name );
	return 0;
}

static PyObject *Image_getPacked(BPyImageObject *self, void *closure)
{
	if (self->image->packedfile)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject *Image_hasData(BPyImageObject *self, void *closure)
{
	if (self->image->ibufs.first)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject *Image_getFlag(BPyImageObject *self, void *flag)
{
	if (self->image->flag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static PyObject *Image_getFlagTpage(BPyImageObject *self, void *flag)
{
	if (self->image->tpageflag & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static PyObject *Image_getSource( BPyImageObject * self )
{
	return PyConst_NewInt( &sourceTypes, self->image->source );	
}

static int Image_setSource( BPyImageObject * self, PyObject *value  )
{
	constValue *c = Const_FromPyObject( &sourceTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected SourceType constant or string" );
	
	self->image->source = c->i;
	return 0;
}


static int Image_setFlag(BPyImageObject *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->image->flag |= (int)flag;
	else
		self->image->flag &= ~(int)flag;
	return 0;
}

static int Image_setFlagTpage(BPyImageObject *self, PyObject *value, void *flag)
{
	if ( PyObject_IsTrue(value) )
		self->image->tpageflag |= (int)flag;
	else
		self->image->tpageflag &= ~(int)flag;
	return 0;
}

/*
 * get integer attributes
 */
static PyObject *getIntAttr( BPyImageObject *self, void *type )
{
	PyObject *attr = NULL;
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
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getIntAttr" );
	}

	attr = PyInt_FromLong( param );
	
	if( attr )
		return attr;

	return EXPP_ReturnPyObjError( PyExc_MemoryError,
				"PyInt_FromLong() failed!" );
}


/*
 * set integer attributes which require clamping
 */

static int setIntAttrClamp( BPyImageObject *self, PyObject *value, void *type )
{
	void *param;
	struct Image *image = self->image;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_IMAGE_ATTR_XREP:
		min = 1; max = 16; size = 'h';
		param = (void *)&image->xrep;
		break;
	case EXPP_IMAGE_ATTR_YREP:
		min = 1; max = 16; size = 'h';
		param = (void *)&image->yrep;
		break;
	case EXPP_IMAGE_ATTR_START:
		min = 0; max = 128; size = 'h';
		param = (void *)&image->twsta;
		break;
	case EXPP_IMAGE_ATTR_END:
		min = 0; max = 128; size = 'h';
		param = (void *)&image->twend;
		break;
	case EXPP_IMAGE_ATTR_SPEED:
		min = 0; max = 100; size = 'h';
		param = (void *)&image->animspeed;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}
	return EXPP_setIValueClamped( value, param, min, max, size );
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyImage_getset[] = {
	{"filename", (getter)Image_getFilename, (setter)Image_setFilename,
	 "image path", NULL},
	{"source", (getter)Image_getSource, (setter)Image_setSource,
	 "image source type", NULL },

	/* readonly */
	{"depth", (getter)Image_getDepth, (setter)NULL,
	 "image depth", NULL},
	{"size", (getter)Image_getSize, (setter)NULL,
	 "image size", NULL},
	{"packed", (getter)Image_getPacked, (setter)NULL,
	 "image packed state", NULL },
	{"has_data", (getter)Image_hasData, (setter)NULL,
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
	/* flags */
	{"fields", (getter)Image_getFlag, (setter)Image_setFlag,
	 "image fields toggle", (void *)IMA_FIELDS },
	{"fields_odd", (getter)Image_getFlag, (setter)Image_setFlag,
	 "image odd fields toggle", (void *)IMA_STD_FIELD },
	{"antialias", (getter)Image_getFlag, (setter)Image_setFlag,
	 "image antialiasing toggle", (void *)IMA_ANTIALI },
	{"reflect", (getter)Image_getFlag, (setter)Image_setFlag,
	 "image reflect toggle", (void *)IMA_REFLECT },
	{"clampX", (getter)Image_getFlagTpage, (setter)Image_setFlagTpage,
	 "disable tiling on the X axis", (void *)IMA_CLAMP_U },
	{"clampY", (getter)Image_getFlagTpage, (setter)Image_setFlagTpage,
	 "disable tiling on the Y axis", (void *)IMA_CLAMP_V },
	{"maxXY", (getter)Image_getMaxXY, (setter)NULL,
	 "", (void *)IMA_CLAMP_U },
	{"minXY", (getter)Image_getMinXY, (setter)NULL,
	 "", (void *)NULL },	 
	{NULL}  /* Sentinel */
};

/* this types constructor, new and load */
static PyObject *Image_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"name", "filename", NULL};
	char *name="Image", *filename=NULL;
	int img_width=256, img_height=256;
	ID *id;
	
	if(
		!PyArg_ParseTupleAndKeywords(args, kwds, "|ss", kwlist, &name, &filename) &&
		!PyArg_ParseTuple( args, "|sii", &name, &img_width, &img_height )
	)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"one string and two ints expected as arguments for a new image or two strings to load an image" );
	
	PyErr_Clear(); /* one of the above would set an error */

	
	if (filename) { /* Load Image */
		if (strlen(filename) > FILE_MAXDIR + FILE_MAXFILE - 1)
				return ( EXPP_ReturnPyObjError( PyExc_IOError,
							"filename too long" ) );
		
		id = (ID *)BKE_add_image_file( filename );
		
		if (!id)
			return ( EXPP_ReturnPyObjError( PyExc_IOError,
					"couldn't load the image" ) );
		
		if (name)
			rename_id( id, name );
	} else { /* New Image*/
		int img_width=256, img_height=256;
		float color[] = {0, 0, 0, 1};
		CLAMP(img_width,  4, 5000);
		CLAMP(img_height, 4, 5000);
		id = (ID *)BKE_add_image_size(img_width, img_height, name, 0, color);
		
		if( !id )
			return ( EXPP_ReturnPyObjError( PyExc_MemoryError,
				"couldn't create new image data" ) );
	}
	
	/* new images have zero user count */
	return Image_CreatePyObject((Image *)id);
}

/*****************************************************************************/
/* Python BPyImage_Type structure definition:   */
/*****************************************************************************/
PyTypeObject BPyImage_Type = {
	PyObject_HEAD_INIT( NULL ) /*     required macro. ( no comma needed )  */ 
	0,	/* ob_size */
	"Blender Image",	/* tp_name */
	sizeof( BPyImageObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	0,		/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	NULL,	/* tp_compare */
	NULL,	/* tp_repr */

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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* long tp_flags; */

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
	BPyImage_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyImage_getset,         /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Image_new,                       /* newfunc tp_new; */
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

PyObject *ImageType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyImage_Type.tp_dict == NULL ) {
		BPyImage_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyImage_Type.tp_dict, &sourceTypes );
		PyType_Ready( &BPyImage_Type ) ;
	}
	return (PyObject *) &BPyImage_Type ;
}
