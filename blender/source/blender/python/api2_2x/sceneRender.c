/* 
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can Redistribute it and/or
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
 * Contributor(s): Joseph Gilbert, Dietrich Bollmann
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/
struct View3D; /* keep me up here */

#include "sceneRender.h" /*This must come first*/

#include "DNA_image_types.h"

#include "BKE_image.h"
#include "BKE_global.h"
#include "BKE_screen.h"
#include "BKE_scene.h"

#include "BIF_drawscene.h"
#include "BIF_renderwin.h"
#include "BIF_writeimage.h"

#include "BLI_blenlib.h"

#include "RE_pipeline.h"

#include "mydevice.h"
#include "butspace.h"
#include "blendef.h"
#include "gen_utils.h"

#include "Scene.h"

/* local defines */
#define PY_NONE		     0
#define PY_LOW		     1
#define PY_MEDIUM	     2
#define PY_HIGH		     3
#define PY_HIGHER	     4
#define PY_BEST		     5
#define PY_USEAOSETTINGS 6
#define PY_SKYDOME	     1
#define PY_FULL	         2

enum rend_constants {
	EXPP_RENDER_ATTR_XPARTS = 0,
	EXPP_RENDER_ATTR_YPARTS,
	EXPP_RENDER_ATTR_ASPECTX,
	EXPP_RENDER_ATTR_ASPECTY,
	EXPP_RENDER_ATTR_CFRAME,
	EXPP_RENDER_ATTR_SFRAME,
	EXPP_RENDER_ATTR_EFRAME,
	EXPP_RENDER_ATTR_FPS,
	EXPP_RENDER_ATTR_FPS_BASE,
	EXPP_RENDER_ATTR_SIZEX,
	EXPP_RENDER_ATTR_SIZEY,
	EXPP_RENDER_ATTR_GAUSSFILTER,
	EXPP_RENDER_ATTR_MBLURFACTOR,
};

#define EXPP_RENDER_ATTR_CFRA                 2
#define EXPP_RENDER_ATTR_ANTISHIFT            3
#define EXPP_RENDER_ATTR_EDGEINT              4
#define EXPP_RENDER_ATTR_EFRA                 5
#define EXPP_RENDER_ATTR_QUALITY             11
#define EXPP_RENDER_ATTR_GAUSS               13
#define EXPP_RENDER_ATTR_BLURFAC             14

#define EXPP_RENDER_ATTR_YF_EXPOSURE         20
#define EXPP_RENDER_ATTR_YF_GAMMA            21
#define EXPP_RENDER_ATTR_YF_GIDEPTH          22
#define EXPP_RENDER_ATTR_YF_GICDEPTH         23
#define EXPP_RENDER_ATTR_YF_GIPHOTONCOUNT    24
#define EXPP_RENDER_ATTR_YF_GIPHOTONMIXCOUNT 25
#define EXPP_RENDER_ATTR_YF_GIPHOTONRADIUS   26
#define EXPP_RENDER_ATTR_YF_GIPIXPERSAMPLE   27
#define EXPP_RENDER_ATTR_YF_GIPOWER          28
#define EXPP_RENDER_ATTR_YF_GIREFINE         29
#define EXPP_RENDER_ATTR_YF_GISHADOWQUAL     30
#define EXPP_RENDER_ATTR_YF_RAYBIAS          31
#define EXPP_RENDER_ATTR_YF_PROCCOUNT        32
#define EXPP_RENDER_ATTR_YF_RAYDEPTH         33
#define EXPP_RENDER_ATTR_YF_GIMETHOD         34
#define EXPP_RENDER_ATTR_YF_GIQUALITY        35


/* Render doc strings */
static char V24_M_Render_doc[] = "The Blender Render module";

/* deprecated callbacks */
static PyObject *V24_RenderData_SetRenderPath( V24_BPy_RenderData *self,
		PyObject *args );
static PyObject *V24_RenderData_SetBackbufPath( V24_BPy_RenderData *self,
		PyObject *args );
static PyObject *V24_RenderData_SetFtypePath( V24_BPy_RenderData *self,
		PyObject *args );
static PyObject *V24_RenderData_SetOversamplingLevel( V24_BPy_RenderData * self,
		PyObject * args );
static PyObject *V24_RenderData_SetRenderWinSize( V24_BPy_RenderData * self,
		PyObject * args );
static PyObject *V24_RenderData_SetBorder( V24_BPy_RenderData * self,
		PyObject * args );
static PyObject *V24_RenderData_SetRenderer( V24_BPy_RenderData * self,
		PyObject * args );
static PyObject *V24_RenderData_SetImageType( V24_BPy_RenderData * self,
		PyObject * args );
static PyObject *V24_RenderData_Render( V24_BPy_RenderData * self );

/* V24_BPy_RenderData Internal Protocols */

static PyObject *V24_RenderData_repr( V24_BPy_RenderData * self )
{
	if( self->renderContext )
		return PyString_FromFormat( "[RenderData \"%s\"]",
					    self->scene->id.name + 2 );
	else
		return PyString_FromString( "NULL" );
}

/***************************************************************************/
/* local utility routines for manipulating data                            */
/***************************************************************************/
static PyObject *V24_M_Render_BitToggleInt( PyObject * args, int setting,
					int *structure )
{
	int flag;

	if( !PyArg_ParseTuple( args, "i", &flag ) )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected TRUE or FALSE (1 or 0)" ) );

	if( flag < 0 || flag > 1 )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected TRUE or FALSE (1 or 0)" ) );

	if( flag )
		*structure |= setting;
	else
		*structure &= ~setting;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;

}

static PyObject *V24_M_Render_BitToggleShort( PyObject * args, short setting,
					  short *structure )
{
	int flag;

	if( !PyArg_ParseTuple( args, "i", &flag ) )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected TRUE or FALSE (1 or 0)" ) );

	if( flag < 0 || flag > 1 )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected TRUE or FALSE (1 or 0)" ) );

	if( flag )
		*structure |= setting;
	else
		*structure &= ~setting;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;

}

static PyObject *V24_M_Render_GetSetAttributeFloat( PyObject * args,
						float *structure, float min,
						float max )
{
	float property = -10.0f;
	char error[48];

	if( !PyArg_ParseTuple( args, "|f", &property ) )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError, "expected float" ) );

	if( property != -10.0f ) {
		if( property < min || property > max ) {
			sprintf( error, "out of range - expected %f to %f",
				 min, max );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_AttributeError, error ) );
		}

		*structure = property;
		V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
		Py_RETURN_NONE;
	} else
		return PyFloat_FromDouble( *structure );
}

static PyObject *V24_M_Render_GetSetAttributeShort( PyObject * args,
						short *structure, int min,
						int max )
{
	short property = -10;
	char error[48];

	if( !PyArg_ParseTuple( args, "|h", &property ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int" ) );

	if( property != -10 ) {
		if( property < min || property > max ) {
			sprintf( error, "out of range - expected %d to %d",
				 min, max );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_AttributeError, error ) );
		}

		*structure = property;
		V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
		Py_RETURN_NONE;
	} else
		return PyInt_FromLong( (long int)*structure );
}

static PyObject *V24_M_Render_GetSetAttributeInt( PyObject * args, int *structure,
					      int min, int max )
{
	int property = -10;
	char error[48];

	if( !PyArg_ParseTuple( args, "|i", &property ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int" ) );

	if( property != -10 ) {
		if( property < min || property > max ) {
			sprintf( error, "out of range - expected %d to %d",
				 min, max );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_AttributeError, error ) );
		}

		*structure = property;
		
		/* compare memory locations, not values */
		if (&G.scene->r.cfra == structure) {
			/* are we changing the current frame?
			update all objects, so python scripts can export all objects
			in a scene without worrying about the view layers */
			scene_update_for_newframe(G.scene, (1<<20) - 1);
		}
		
		/*I dont think this should be here, whatif the scene is not the current scene - campbell*/
		V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
		
		Py_RETURN_NONE;
	} else
		return  PyInt_FromLong( *structure );
}


static void V24_M_Render_DoSizePreset( V24_BPy_RenderData * self, short xsch,
				   short ysch, short xasp, short yasp,
				   short size, short xparts, short yparts,
				   short fps, float fps_base,
				   float a, float b, float c,
				   float d )
{
	self->renderContext->xsch = xsch;
	self->renderContext->ysch = ysch;
	self->renderContext->xasp = xasp;
	self->renderContext->yasp = yasp;
	self->renderContext->size = size;
	self->renderContext->frs_sec = fps;
	self->renderContext->frs_sec_base = fps_base;
	self->renderContext->xparts = xparts;
	self->renderContext->yparts = yparts;

	BLI_init_rctf( &self->renderContext->safety, a, b, c, d );
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	V24_EXPP_allqueue( REDRAWVIEWCAM, 0 );
}

/** set / get boolean */

static int V24_M_Render_setBooleanShort( V24_BPy_RenderData * self, PyObject *value, short* var )
{
	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
									"expected boolean value" );

	*var = (PyInt_AsLong( value )) ? 1 : 0;

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

static PyObject *V24_M_Render_getBooleanShort( V24_BPy_RenderData * self, short var )
{
	return PyInt_FromLong( (long) var );
}

/** set / get float */

static int V24_M_Render_setFloat( V24_BPy_RenderData *self, PyObject *value, float *var, float min, float max )
{
	float val;
	char error[48];

	if( !PyFloat_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
									"expected float value" );

	val = (float) PyFloat_AsDouble( value );

	/* check range */
	if ( val < min || val > max ) {
		sprintf( error, "out of range - expected %f to %f", min, max );
		return V24_EXPP_ReturnIntError( PyExc_TypeError,error );
	}

	*var = val;

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

static PyObject *V24_M_Render_getFloat( V24_BPy_RenderData *self, float var )
{
	return PyFloat_FromDouble( (double) var );
}

/** set / get integer */

static int V24_M_Render_setInt( V24_BPy_RenderData *self, PyObject *value, int *var, int min, int max )
{
	int val;
	char error[48];

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
									"expected integer value" );

	val = (int) PyInt_AsLong( value );

	/* check range */
	if ( val < min || val > max ) {
		sprintf( error, "out of range - expected %d to %d", min, max );
		return V24_EXPP_ReturnIntError( PyExc_TypeError,error );
	}

	*var = val;

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

static PyObject *V24_M_Render_getInt( V24_BPy_RenderData *self, int var )
{
	return PyInt_FromLong( (long) var );
}

/***************************************************************************/
/* Render Module Function Definitions                                      */
/***************************************************************************/

PyObject *V24_M_Render_CloseRenderWindow( PyObject * self )
{
	BIF_close_render_display(  );
	Py_RETURN_NONE;
}

PyObject *V24_M_Render_SetRenderWinPos( PyObject * self, PyObject * args )
{
	PyObject *list = NULL;
	char *loc = NULL;
	int x;

	if( !PyArg_ParseTuple( args, "O!", &PyList_Type, &list ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected a list" ) );

	G.winpos = 0;
	for( x = 0; x < PyList_Size( list ); x++ ) {
		if( !PyArg_Parse( PyList_GetItem( list, x ), "s", &loc ) ) {
			return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						      "python list not parseable" );
		}
		if( strcmp( loc, "SW" ) == 0 || strcmp( loc, "sw" ) == 0 )
			G.winpos |= 1;
		else if( strcmp( loc, "S" ) == 0 || strcmp( loc, "s" ) == 0 )
			G.winpos |= 2;
		else if( strcmp( loc, "SE" ) == 0 || strcmp( loc, "se" ) == 0 )
			G.winpos |= 4;
		else if( strcmp( loc, "W" ) == 0 || strcmp( loc, "w" ) == 0 )
			G.winpos |= 8;
		else if( strcmp( loc, "C" ) == 0 || strcmp( loc, "c" ) == 0 )
			G.winpos |= 16;
		else if( strcmp( loc, "E" ) == 0 || strcmp( loc, "e" ) == 0 )
			G.winpos |= 32;
		else if( strcmp( loc, "NW" ) == 0 || strcmp( loc, "nw" ) == 0 )
			G.winpos |= 64;
		else if( strcmp( loc, "N" ) == 0 || strcmp( loc, "n" ) == 0 )
			G.winpos |= 128;
		else if( strcmp( loc, "NE" ) == 0 || strcmp( loc, "ne" ) == 0 )
			G.winpos |= 256;
		else
			return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "list contains unknown string" );
	}
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_M_Render_EnableDispView( PyObject * self )
{
	G.displaymode = R_DISPLAYIMAGE;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_M_Render_EnableDispWin( PyObject * self )
{
	G.displaymode = R_DISPLAYWIN;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}


/***************************************************************************/
/* V24_BPy_RenderData Function Definitions                                     */
/***************************************************************************/

PyObject *V24_RenderData_Render( V24_BPy_RenderData * self )
{
	Scene *oldsce;

	if (!G.background) {
		oldsce = G.scene;
		set_scene( self->scene );
		BIF_do_render( 0 );
		set_scene( oldsce );
	}

	else { /* background mode (blender -b file.blend -P script) */
		Render *re= RE_NewRender("Render");

		int end_frame = G.scene->r.efra; /* is of type short currently */

		if (G.scene != self->scene)
			return V24_EXPP_ReturnPyObjError (PyExc_RuntimeError,
				"scene to render in bg mode must be the active scene");

		G.scene->r.efra = G.scene->r.sfra;

		RE_BlenderAnim(re, G.scene, G.scene->r.sfra, G.scene->r.efra);

		G.scene->r.efra = (short)end_frame;
	}

	Py_RETURN_NONE;
}

/* 
 * This will save the rendered image to an output file path already defined.
 */
PyObject *V24_RenderData_SaveRenderedImage ( V24_BPy_RenderData * self, PyObject *args )
{
	char dir[FILE_MAXDIR * 2], str[FILE_MAXFILE * 2];
	char *name_str, filepath[FILE_MAXDIR+FILE_MAXFILE];
	RenderResult *rr = NULL;
	int zbuff;
	
	if( !PyArg_ParseTuple( args, "s|i", &name_str, &zbuff ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a filename (string) and optional int" );

	if( strlen(self->renderContext->pic) + strlen(name_str)
			>= sizeof(filepath) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
				"full filename too long" );

	if (zbuff !=0	) zbuff = 1; /*required 1/0 */ /* removed! (ton) */

	BLI_strncpy( filepath, self->renderContext->pic, sizeof(filepath) );
	strcat(filepath, name_str);

	rr = RE_GetResult(RE_GetRender(G.scene->id.name));
	if(!rr) {
		return V24_EXPP_ReturnPyObjError (PyExc_ValueError, "No image rendered");
	} else {
		if(G.ima[0]==0) {
			strcpy(dir, G.sce);
			BLI_splitdirstring(dir, str);
			strcpy(G.ima, dir);
		}
		BIF_save_rendered_image(filepath);
	}
	Py_RETURN_NONE;
}

PyObject *V24_RenderData_RenderAnim( V24_BPy_RenderData * self )
{
	Scene *oldsce;

	if (!G.background) {
		oldsce = G.scene;
		set_scene( self->scene );
		BIF_do_render( 1 );
		set_scene( oldsce );
	}
	else { /* background mode (blender -b file.blend -P script) */
		Render *re= RE_NewRender("Render");
		
		if (G.scene != self->scene)
			return V24_EXPP_ReturnPyObjError (PyExc_RuntimeError,
				"scene to render in bg mode must be the active scene");

		if (G.scene->r.sfra > G.scene->r.efra)
			return V24_EXPP_ReturnPyObjError (PyExc_RuntimeError,
				"start frame must be less or equal to end frame");
		
		RE_BlenderAnim(re, G.scene, G.scene->r.sfra, G.scene->r.efra);
	}
	Py_RETURN_NONE;
}

PyObject *V24_RenderData_Play( V24_BPy_RenderData * self )
{
	char file[FILE_MAXDIR + FILE_MAXFILE];
	extern char bprogname[];
	char str[FILE_MAXDIR + FILE_MAXFILE];
	int pos[2], size[2];
	char txt[64];

#ifdef WITH_QUICKTIME
	if( self->renderContext->imtype == R_QUICKTIME ) {

		strcpy( file, self->renderContext->pic );
		BLI_convertstringcode( file, (char *) self->scene,
				       self->renderContext->cfra );
		BLI_make_existing_file( file );
		if( BLI_strcasecmp( file + strlen( file ) - 4, ".mov" ) ) {
			sprintf( txt, "%04d_%04d.mov",
				 ( self->renderContext->sfra ),
				 ( self->renderContext->efra ) );
			strcat( file, txt );
		}
	} else
#endif
	{

		strcpy( file, self->renderContext->pic );
		BLI_convertstringcode( file, G.sce,
				       self->renderContext->cfra );
		BLI_make_existing_file( file );
		if( BLI_strcasecmp( file + strlen( file ) - 4, ".avi" ) ) {
			sprintf( txt, "%04d_%04d.avi",
				 ( self->renderContext->sfra ),
				 ( self->renderContext->efra ) );
			strcat( file, txt );
		}
	}
	if( BLI_exist( file ) ) {
		calc_renderwin_rectangle(640, 480, G.winpos, pos, size);
		sprintf( str, "%s -a -p %d %d \"%s\"", bprogname, pos[0],
			 pos[1], file );
		system( str );
	} else {
		BKE_makepicstring( file, G.scene->r.pic, self->renderContext->sfra, G.scene->r.imtype);
		if( BLI_exist( file ) ) {
			calc_renderwin_rectangle(640, 480, G.winpos, pos, size);
#ifdef WIN32
			sprintf( str, "%s -a -p %d %d \"%s\"", bprogname,
				 pos[0], pos[1], file );
#else
			sprintf( str, "\"%s\" -a -p %d %d \"%s\"", bprogname,
				 pos[0], pos[1], file );
#endif
			system( str );
		} else
			sprintf( "Can't find image: %s", file );
	}

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableBackbuf( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleShort( args, 1,
					&self->renderContext->bufflag );
}

PyObject *V24_RenderData_EnableExtensions( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleShort( args, R_EXTENSION,
					&self->renderContext->scemode );
}

PyObject *V24_RenderData_EnableSequencer( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleShort( args, R_DOSEQ,
					&self->renderContext->scemode );
}

PyObject *V24_RenderData_EnableRenderDaemon( V24_BPy_RenderData * self,
					 PyObject * args )
{
	return V24_M_Render_BitToggleShort( args, R_BG_RENDER,
					&self->renderContext->scemode );
}

PyObject *V24_RenderData_EnableToonShading( V24_BPy_RenderData * self,
					PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_EDGE,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EdgeIntensity( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args,
					      &self->renderContext->edgeint, 0,
					      255 );
}

PyObject *V24_RenderData_SetEdgeColor( V24_BPy_RenderData * self, PyObject * args )
{
	float red, green, blue;

	if( !PyArg_ParseTuple( args, "fff", &red, &green, &blue ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected three floats" ) );

	if( red < 0 || red > 1 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value out of range 0.000 - 1.000 (red)" ) );
	if( green < 0 || green > 1 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value out of range 0.000 - 1.000 (green)" ) );
	if( blue < 0 || blue > 1 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value out of range 0.000 - 1.000 (blue)" ) );

	self->renderContext->edgeR = red;
	self->renderContext->edgeG = green;
	self->renderContext->edgeB = blue;

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_GetEdgeColor( V24_BPy_RenderData * self )
{
	char rgb[24];

	sprintf( rgb, "[%.3f,%.3f,%.3f]", self->renderContext->edgeR,
		 self->renderContext->edgeG, self->renderContext->edgeB );
	return PyString_FromString( rgb );
}

PyObject *V24_RenderData_EnableOversampling( V24_BPy_RenderData * self,
					 PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_OSA,
				      &self->renderContext->mode );
}

static int V24_RenderData_setOSALevel( V24_BPy_RenderData * self,
		PyObject * value )
{
	int level;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	level = PyInt_AsLong( value );
	if( level != 5 && level != 8 && level != 11 && level != 16 )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"expected 5, 8, 11, or 16" );

	self->renderContext->osa = (short)level;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

PyObject *V24_RenderData_EnableMotionBlur( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_MBLUR,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_MotionBlurLevel( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeFloat( args,
					      &self->renderContext->blurfac,
					      0.01f, 5.0f );
}

PyObject *V24_RenderData_PartsX( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args,
					      &self->renderContext->xparts, 1,
					      512 );
}

PyObject *V24_RenderData_PartsY( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args,
					      &self->renderContext->yparts, 1,
					      64 );
}

PyObject *V24_RenderData_EnableSky( V24_BPy_RenderData * self )
{
	self->renderContext->alphamode = R_ADDSKY;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnablePremultiply( V24_BPy_RenderData * self )
{
	self->renderContext->alphamode = R_ALPHAPREMUL;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableKey( V24_BPy_RenderData * self )
{
	self->renderContext->alphamode = R_ALPHAKEY;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableShadow( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_SHADOW,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnableEnvironmentMap( V24_BPy_RenderData * self,
					   PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_ENVMAP,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnablePanorama( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_PANORAMA,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnableRayTracing( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_RAYTRACE,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnableRadiosityRender( V24_BPy_RenderData * self,
					    PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_RADIO,
				      &self->renderContext->mode );
}
PyObject *V24_RenderData_EnableFieldRendering( V24_BPy_RenderData * self,
					   PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_FIELDS,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnableOddFieldFirst( V24_BPy_RenderData * self,
					  PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_ODDFIELD,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnableFieldTimeDisable( V24_BPy_RenderData * self,
					     PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_FIELDSTILL,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_EnableGaussFilter( V24_BPy_RenderData * self,
					PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_GAUSS,
				      &self->renderContext->mode );
	
	/* note, this now is obsolete (ton) */
	/* we now need a call like RenderData_SetFilter() or so */
	/* choices are listed in DNA_scene_types.h (search filtertype) */
}

PyObject *V24_RenderData_EnableBorderRender( V24_BPy_RenderData * self,
					 PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_BORDER,
				      &self->renderContext->mode );
}

static int V24_RenderData_setBorder( V24_BPy_RenderData * self, PyObject * args )
{
	float xmin, ymin, xmax, ymax;

	if( PyList_Check( args ) )
		args = PySequence_Tuple( args );
	else
		Py_INCREF( args );

	if( !PyArg_ParseTuple( args, "ffff", &xmin, &ymin, &xmax, &ymax ) ) {
		Py_DECREF( args );
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected four floats" );
	}

	self->renderContext->border.xmin = V24_EXPP_ClampFloat( xmin, 0.0, 1.0 );
	self->renderContext->border.xmax = V24_EXPP_ClampFloat( xmax, 0.0, 1.0 );
	self->renderContext->border.ymin = V24_EXPP_ClampFloat( ymin, 0.0, 1.0 );
	self->renderContext->border.ymax = V24_EXPP_ClampFloat( ymax, 0.0, 1.0 );

	V24_EXPP_allqueue( REDRAWVIEWCAM, 1 );

	Py_DECREF( args );
	return 0;
}

static PyObject *V24_RenderData_getBorder( V24_BPy_RenderData * self )
{
	return Py_BuildValue( "[ffff]", 
			self->renderContext->border.xmin,
			self->renderContext->border.ymin,
			self->renderContext->border.xmax,
			self->renderContext->border.ymax );
}

PyObject *V24_RenderData_EnableGammaCorrection( V24_BPy_RenderData * self,
					    PyObject * args )
{
	return V24_M_Render_BitToggleInt( args, R_GAMMA,
				      &self->renderContext->mode );
}

PyObject *V24_RenderData_GaussFilterSize( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeFloat( args,
					      &self->renderContext->gauss,
					      0.5f, 1.5f );
}

PyObject *V24_RenderData_StartFrame( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeInt( args, &self->renderContext->sfra,
					    1, MAXFRAME );
}

PyObject *V24_RenderData_CurrentFrame( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeInt( args, &self->renderContext->cfra,
					    1, MAXFRAME );
}

PyObject *V24_RenderData_EndFrame( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeInt( args, &self->renderContext->efra,
					    1, MAXFRAME );
}

PyObject *V24_RenderData_ImageSizeX( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args, &self->renderContext->xsch,
					      4, 10000 );
}

PyObject *V24_RenderData_ImageSizeY( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args, &self->renderContext->ysch,
					      4, 10000 );
}

PyObject *V24_RenderData_AspectRatioX( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args, &self->renderContext->xasp,
					      1, 200 );
}

PyObject *V24_RenderData_AspectRatioY( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args, &self->renderContext->yasp,
					      1, 200 );
}

static int V24_RenderData_setRenderer( V24_BPy_RenderData * self, PyObject * value )
{
	int type;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected constant INTERNAL or YAFRAY" );

	type = PyInt_AsLong( value );
	if( type == R_INTERN )
		self->renderContext->renderer = R_INTERN;
	else if( type == R_YAFRAY )
		self->renderContext->renderer = R_YAFRAY;
	else
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"expected constant INTERNAL or YAFRAY" );

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

PyObject *V24_RenderData_EnableCropping( void )
{
/*	return V24_M_Render_BitToggleInt( args, R_MOVIECROP,
				      &self->renderContext->mode );
*/
	printf("cropping option is now default, obsolete\n");
	Py_RETURN_NONE;
}


static int V24_RenderData_setImageType( V24_BPy_RenderData *self, PyObject *value )
{
	int type;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected int constant" );

	type = PyInt_AS_LONG( value );

	/*
	 * this same logic and more is in buttons_scene.c imagetype_pup code but
	 * only in generating strings for the popup menu, no way to reuse that :(
	 */

	switch( type ) {
	case R_AVIRAW :
	case R_AVIJPEG :
	case R_TARGA :
	case R_RAWTGA :
	case R_RADHDR :
	case R_PNG :
	case R_BMP :
	case R_JPEG90 :
	case R_HAMX :
	case R_IRIS :
	case R_IRIZ :
	case R_FTYPE :
	case R_TIFF :
	case R_CINEON :
	case R_DPX :
#ifdef _WIN32
	case R_AVICODEC :
#endif
#ifdef WITH_OPENEXR
	case R_OPENEXR :
#endif
#ifdef WITH_FFMPEG
	case R_FFMPEG :
#endif
		self->renderContext->imtype = type;
		break;
	case R_QUICKTIME :
		if( G.have_quicktime ) {
			self->renderContext->imtype = R_QUICKTIME;
			break;
		}
	default:
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"unknown constant - see modules dict for help" );
	}

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

PyObject *V24_RenderData_Quality( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args,
					      &self->renderContext->quality,
					      10, 100 );
}

PyObject *V24_RenderData_FramesPerSec( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args,
					      &self->renderContext->frs_sec, 1,
					      120 );
}

PyObject *V24_RenderData_EnableGrayscale( V24_BPy_RenderData * self )
{
	self->renderContext->planes = R_PLANESBW;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableRGBColor( V24_BPy_RenderData * self )
{
	self->renderContext->planes = R_PLANES24;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableRGBAColor( V24_BPy_RenderData * self )
{
	self->renderContext->planes = R_PLANES32;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_SizePreset( V24_BPy_RenderData * self, PyObject * args )
{
	int type;

	if( !PyArg_ParseTuple( args, "i", &type ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected constant" ) );

	if( type == B_PR_PAL ) {
		V24_M_Render_DoSizePreset( self, 720, 576, 54, 51, 100,
				       self->renderContext->xparts,
				       self->renderContext->yparts, 25, 1.0f,
				       0.1f,
				       0.9f, 0.1f, 0.9f );
		self->renderContext->mode &= ~R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_NTSC ) {
		V24_M_Render_DoSizePreset( self, 720, 480, 10, 11, 100, 1, 1,
				       30, 1.001f, 
				       0.1f, 0.9f, 0.1f, 0.9f );
		self->renderContext->mode &= ~R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PRESET ) {
		V24_M_Render_DoSizePreset( self, 720, 576, 54, 51, 100, 1, 1,
				       self->renderContext->frs_sec, 
				       self->renderContext->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		self->renderContext->mode = R_OSA + R_SHADOW + R_FIELDS;
		self->renderContext->imtype = R_TARGA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PRV ) {
		V24_M_Render_DoSizePreset( self, 640, 512, 1, 1, 50, 1, 1,
				       self->renderContext->frs_sec, 
				       self->renderContext->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		self->renderContext->mode &= ~R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PC ) {
		V24_M_Render_DoSizePreset( self, 640, 480, 100, 100, 100, 1, 1,
				       self->renderContext->frs_sec, 
				       self->renderContext->frs_sec_base, 
				       0.0f, 1.0f,
				       0.0f, 1.0f );
		self->renderContext->mode &= ~R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.0f, 1.0f, 0.0f,
			       1.0f );
	} else if( type == B_PR_PAL169 ) {
		V24_M_Render_DoSizePreset( self, 720, 576, 64, 45, 100, 1, 1,
				       25, 1.0f, 0.1f, 0.9f, 0.1f, 0.9f );
		self->renderContext->mode &= ~R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PANO ) {
		V24_M_Render_DoSizePreset( self, 36, 176, 115, 100, 100, 16, 1,
				       self->renderContext->frs_sec, 
				       self->renderContext->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		self->renderContext->mode |= R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_FULL ) {
		V24_M_Render_DoSizePreset( self, 1280, 1024, 1, 1, 100, 1, 1,
				       self->renderContext->frs_sec, 
				       self->renderContext->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		self->renderContext->mode &= ~R_PANORAMA;
		BLI_init_rctf( &self->renderContext->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"unknown constant - see modules dict for help" ) );

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	Py_RETURN_NONE;
}

/*

PyObject *V24_RenderData_SetYafrayGIQuality( V24_BPy_RenderData * self,
					 PyObject * args )
{
	int type;

	if( !PyArg_ParseTuple( args, "i", &type ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected constant" ) );

	if( type == PY_NONE || type == PY_LOW ||
	    type == PY_MEDIUM || type == PY_HIGH ||
	    type == PY_HIGHER || type == PY_BEST ) {
		self->renderContext->GIquality = (short)type;
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"unknown constant - see modules dict for help" ) );

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	Py_RETURN_NONE;
}

PyObject *V24_RenderData_SetYafrayGIMethod( V24_BPy_RenderData * self,
					PyObject * args )
{
	int type;

	if( !PyArg_ParseTuple( args, "i", &type ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected constant" ) );

	if( type == PY_NONE || type == PY_SKYDOME || type == PY_FULL ) {
		self->renderContext->GImethod = (short)type;
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"unknown constant - see modules dict for help" ) );

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	Py_RETURN_NONE;
}
*/

/* (die) beg */

/* YafRay - Yafray GI Method */

static int V24_RenderData_setYafrayGIQuality( V24_BPy_RenderData * self, PyObject * value )
{
	long type;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
									"expected constant" );

	type = PyInt_AsLong( value );

	if( type == PY_NONE   || type == PY_LOW  ||
	    type == PY_MEDIUM || type == PY_HIGH ||
	    type == PY_HIGHER || type == PY_BEST ||
	    type == PY_USEAOSETTINGS
		) {
		self->renderContext->GIquality = (short)type;
	} else {
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
									"expected constant NONE, LOW, MEDIUM, HIGHER or BEST" );
	}

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

static PyObject *V24_RenderData_getYafrayGIQuality( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long) self->renderContext->GIquality );
}

static PyObject *V24_RenderData_SetYafrayGIQuality( V24_BPy_RenderData * self,
												PyObject * args )
{
	return V24_EXPP_setterWrapper( (void*) self, args,
							   (setter) V24_RenderData_setYafrayGIQuality );
}

static PyObject *V24_RenderData_GetYafrayGIQuality( V24_BPy_RenderData * self )
{
	return V24_RenderData_getYafrayGIQuality(self);
}

/* YafRay - Yafray GI Method */

static int V24_RenderData_setYafrayGIMethod( V24_BPy_RenderData * self, PyObject * value )
{
	int type;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected constant NONE, SKYDOME or FULL" );

	type = PyInt_AsLong( value );
	if( type == PY_NONE || type == PY_SKYDOME || type == PY_FULL ) {
		self->renderContext->GImethod = (short)type;
	} else {
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
									"expected constant NONE, SKYDOME or FULL" );
	}

	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

static PyObject *V24_RenderData_getYafrayGIMethod( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long)self->renderContext->GImethod );
}

static PyObject *V24_RenderData_GetYafrayGIMethod( V24_BPy_RenderData * self )
{
	return V24_RenderData_getYafrayGIMethod(self);
}

static PyObject *V24_RenderData_SetYafrayGIMethod( V24_BPy_RenderData * self,
											   PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setYafrayGIMethod );
}


/* YafRay - Export to XML */

static int V24_RenderData_setYafrayExportToXML( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->YFexportxml );
}

static PyObject *V24_RenderData_getYafrayExportToXML( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->YFexportxml );
}

/** Auto AA */

static int V24_RenderData_setYafrayAutoAntiAliasing( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->YF_AA );
}

static PyObject *V24_RenderData_getYafrayAutoAntiAliasing( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->YF_AA );
}

/** Clamp RGB */

static int V24_RenderData_setYafrayClampRGB( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->YF_clamprgb );
}

static PyObject *V24_RenderData_getYafrayClampRGB( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->YF_clamprgb );
}

/** YafRay - Anti-Aliasing Passes */

static int V24_RenderData_setYafrayAntiAliasingPasses( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setInt( self, value, &self->renderContext->YF_AApasses, 0, 64 );
}

static PyObject *V24_RenderData_getYafrayAntiAliasingPasses( V24_BPy_RenderData * self )
{
	return V24_M_Render_getInt( self, self->renderContext->YF_AApasses );
}

/** YafRay - Anti-Aliasing Samples */

static int V24_RenderData_setYafrayAntiAliasingSamples( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setInt( self, value, &self->renderContext->YF_AAsamples, 0, 2048 );
}

static PyObject *V24_RenderData_getYafrayAntiAliasingSamples( V24_BPy_RenderData * self )
{
	return V24_M_Render_getInt( self, self->renderContext->YF_AAsamples );
}

/* YafRay - Anti-Aliasing Pixel Filter Size */

static int V24_RenderData_setYafrayAntiAliasingPixelSize( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setFloat( self, value, &self->renderContext->YF_AApixelsize, 1.0, 2.0 );
}

static PyObject *V24_RenderData_getYafrayAntiAliasingPixelSize( V24_BPy_RenderData * self )
{
	return V24_M_Render_getFloat( self, self->renderContext->YF_AApixelsize );
}

/* YafRay - Anti-Aliasing threshold */

static int V24_RenderData_setYafrayAntiAliasingThreshold( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setFloat( self, value, &self->renderContext->YF_AAthreshold, 0.05, 1.0 );
}

static PyObject *V24_RenderData_getYafrayAntiAliasingThreshold( V24_BPy_RenderData * self )
{
	return V24_M_Render_getFloat( self, self->renderContext->YF_AAthreshold );
}

/** YafRay - Number of processors to use */

static int V24_RenderData_setYafrayNumberOfProcessors( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setInt( self, value, &self->renderContext->YF_numprocs, 1, 8 );
}

static PyObject *V24_RenderData_getYafrayNumberOfProcessors( V24_BPy_RenderData * self )
{
	return V24_M_Render_getInt( self, self->renderContext->YF_numprocs );
}

/* YafRay - Cache occlusion/irradiance samples (faster) */

static int V24_RenderData_setYafrayGICache( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->GIcache );
}

static PyObject *V24_RenderData_getYafrayGICache( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->GIcache );
}

/* YafRay - Enable/disable bumpnormals for cache
   (faster, but no bumpmapping in total indirectly lit areas) */

static int V24_RenderData_setYafrayGICacheBumpNormals( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->YF_nobump );
}

static PyObject *V24_RenderData_getYafrayGICacheBumpNormals( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->YF_nobump );
}

/* YafRay - Shadow quality, keep it under 0.95 :-) */

static int V24_RenderData_setYafrayGICacheShadowQuality( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setFloat( self, value, &self->renderContext->GIshadowquality, 0.01, 1.0 );
}

static PyObject *V24_RenderData_getYafrayGICacheShadowQuality( V24_BPy_RenderData * self )
{
	return V24_M_Render_getFloat( self, self->renderContext->GIshadowquality );
}

/* YafRay - Threshold to refine shadows EXPERIMENTAL. 1 = no refinement */

static int V24_RenderData_setYafrayGICacheRefinement( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setFloat( self, value, &self->renderContext->GIrefinement, 0.001, 1.0 );
}

static PyObject *V24_RenderData_getYafrayGICacheRefinement( V24_BPy_RenderData * self )
{
	return V24_M_Render_getFloat( self, self->renderContext->GIrefinement );
}

/* YafRay - Maximum number of pixels without samples, the lower the better and slower */

static int V24_RenderData_setYafrayGICachePixelsPerSample( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setInt( self, value, &self->renderContext->GIpixelspersample, 1, 50 );
}

static PyObject *V24_RenderData_getYafrayGICachePixelsPerSample( V24_BPy_RenderData * self )
{
	return V24_M_Render_getInt( self, self->renderContext->GIpixelspersample );
}

/** YafRay - Enable/disable use of global photons to help in GI */

static int V24_RenderData_setYafrayGIPhotons( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->GIphotons );
}

static PyObject *V24_RenderData_getYafrayGIPhotons( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->GIphotons );
}

/** YafRay - If true the photonmap is shown directly in the render for tuning */

static int V24_RenderData_setYafrayGITunePhotons( V24_BPy_RenderData * self, PyObject * value )
{
	return V24_M_Render_setBooleanShort( self, value, &self->renderContext->GIdirect );
}

static PyObject *V24_RenderData_getYafrayGITunePhotons( V24_BPy_RenderData * self )
{
	return V24_M_Render_getBooleanShort( self, self->renderContext->GIdirect );
}

/* (die) end */

PyObject *V24_RenderData_YafrayGIPower( V24_BPy_RenderData * self, PyObject * args )
{
	if( self->renderContext->GImethod > 0 ) {
		return V24_M_Render_GetSetAttributeFloat( args,
						      &self->renderContext->
						      GIpower, 0.01f,
						      100.00f );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'SKYDOME' or 'FULL'" ) );
}

PyObject *V24_RenderData_YafrayGIIndirPower( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeFloat( args,
						  &self->renderContext->
						  GIindirpower, 0.01f,
						  100.00f );
}

PyObject *V24_RenderData_YafrayGIDepth( V24_BPy_RenderData * self, PyObject * args )
{
	if( self->renderContext->GImethod == 2 ) {
		return V24_M_Render_GetSetAttributeInt( args,
						    &self->renderContext->
						    GIdepth, 1, 100 );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL'" ) );
}

PyObject *V24_RenderData_YafrayGICDepth( V24_BPy_RenderData * self, PyObject * args )
{
	if( self->renderContext->GImethod == 2 ) {
		return V24_M_Render_GetSetAttributeInt( args,
						    &self->renderContext->
						    GIcausdepth, 1, 100 );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL'" ) );
}

PyObject *V24_RenderData_EnableYafrayGICache( V24_BPy_RenderData * self,
					  PyObject * args )
{
	if( self->renderContext->GImethod == 2 ) {
		return V24_M_Render_BitToggleShort( args, 1,
						&self->renderContext->
						GIcache );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL'" ) );
}

PyObject *V24_RenderData_EnableYafrayGIPhotons( V24_BPy_RenderData * self,
					    PyObject * args )
{
	if( self->renderContext->GImethod == 2 ) {
		return V24_M_Render_BitToggleShort( args, 1,
						&self->renderContext->
						GIphotons );;
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL'" ) );
}

PyObject *V24_RenderData_YafrayGIPhotonCount( V24_BPy_RenderData * self,
					  PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIphotons == 1 ) {
		return V24_M_Render_GetSetAttributeInt( args,
						    &self->renderContext->
						    GIphotoncount, 100000,
						    10000000 );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GIPhotons must be enabled" ) );
}

PyObject *V24_RenderData_YafrayGIPhotonRadius( V24_BPy_RenderData * self,
					   PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIphotons == 1 ) {
		return V24_M_Render_GetSetAttributeFloat( args,
						      &self->renderContext->
						      GIphotonradius, 0.00001f,
						      100.0f );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GIPhotons must be enabled" ) );
}

PyObject *V24_RenderData_YafrayGIPhotonMixCount( V24_BPy_RenderData * self,
					     PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIphotons == 1 ) {
		return V24_M_Render_GetSetAttributeInt( args,
						    &self->renderContext->
						    GImixphotons, 100, 1000 );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GIPhotons must be enabled" ) );
}

PyObject *V24_RenderData_EnableYafrayGITunePhotons( V24_BPy_RenderData * self,
						PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIphotons == 1 ) {
		return V24_M_Render_BitToggleShort( args, 1,
						&self->renderContext->
						GIdirect );;
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GIPhotons must be enabled" ) );
}

PyObject *V24_RenderData_YafrayGIShadowQuality( V24_BPy_RenderData * self,
					    PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIcache == 1 ) {
		return V24_M_Render_GetSetAttributeFloat( args,
						      &self->renderContext->
						      GIshadowquality, 0.01f,
						      1.0f );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GICache must be enabled" ) );
}

PyObject *V24_RenderData_YafrayGIPixelsPerSample( V24_BPy_RenderData * self,
					      PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIcache == 1 ) {
		return V24_M_Render_GetSetAttributeInt( args,
						    &self->renderContext->
						    GIpixelspersample, 1, 50 );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GICache must be enabled" ) );
}

PyObject *V24_RenderData_YafrayGIRefinement( V24_BPy_RenderData * self,
					 PyObject * args )
{
	if( self->renderContext->GImethod == 2
	    && self->renderContext->GIcache == 1 ) {
		return V24_M_Render_GetSetAttributeFloat( args,
						      &self->renderContext->
						      GIrefinement, 0.001f,
						      1.0f );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_StandardError,
						"YafrayGIMethod must be set to 'FULL' and GICache must be enabled" ) );
}

PyObject *V24_RenderData_YafrayRayBias( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeFloat( args,
					      &self->renderContext->YF_raybias,
					      0.0f, 10.0f );
}

PyObject *V24_RenderData_YafrayRayDepth( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeInt( args,
					    &self->renderContext->YF_raydepth,
					    1, 80 );
}

PyObject *V24_RenderData_YafrayGamma( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeFloat( args,
					      &self->renderContext->YF_gamma,
					      0.001f, 5.0f );
}

PyObject *V24_RenderData_YafrayExposure( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeFloat( args,
					      &self->renderContext->
					      YF_exposure, 0.0f, 10.0f );
}

PyObject *V24_RenderData_YafrayProcessorCount( V24_BPy_RenderData * self,
					   PyObject * args )
{
	return V24_M_Render_GetSetAttributeInt( args,
					    &self->renderContext->YF_numprocs,
					    1, 8 );
}

PyObject *V24_RenderData_EnableGameFrameStretch( V24_BPy_RenderData * self )
{
	self->scene->framing.type = SCE_GAMEFRAMING_SCALE;
	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableGameFrameExpose( V24_BPy_RenderData * self )
{
	self->scene->framing.type = SCE_GAMEFRAMING_EXTEND;
	Py_RETURN_NONE;
}

PyObject *V24_RenderData_EnableGameFrameBars( V24_BPy_RenderData * self )
{
	self->scene->framing.type = SCE_GAMEFRAMING_BARS;
	Py_RETURN_NONE;
}

PyObject *V24_RenderData_SetGameFrameColor( V24_BPy_RenderData * self,
					PyObject * args )
{
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;

	if( !PyArg_ParseTuple( args, "fff", &red, &green, &blue ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected three floats" ) );

	if( red < 0 || red > 1 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value out of range 0.000 - 1.000 (red)" ) );
	if( green < 0 || green > 1 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value out of range 0.000 - 1.000 (green)" ) );
	if( blue < 0 || blue > 1 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value out of range 0.000 - 1.000 (blue)" ) );

	self->scene->framing.col[0] = red;
	self->scene->framing.col[1] = green;
	self->scene->framing.col[2] = blue;

	Py_RETURN_NONE;
}

PyObject *V24_RenderData_GetGameFrameColor( V24_BPy_RenderData * self )
{
	char rgb[24];

	sprintf( rgb, "[%.3f,%.3f,%.3f]", self->scene->framing.col[0],
		 self->scene->framing.col[1], self->scene->framing.col[2] );
	return PyString_FromString( rgb );
}

#ifdef __sgi
PyObject *V24_RenderData_SGIMaxsize( V24_BPy_RenderData * self, PyObject * args )
{
	return V24_M_Render_GetSetAttributeShort( args,
					      &self->renderContext->maximsize,
					      0, 500 );
}

PyObject *V24_RenderData_EnableSGICosmo( V24_BPy_RenderData *self, PyObject *args )
{
	return V24_M_Render_BitToggleInt( args, R_COSMO,
				      &self->renderContext->mode );
}
#else
PyObject *V24_RenderData_SGIMaxsize( void )
{
	return V24_EXPP_ReturnPyObjError( PyExc_StandardError,
			"SGI is not defined on this machine" );
}

PyObject *V24_RenderData_EnableSGICosmo( void )
{
	return V24_EXPP_ReturnPyObjError( PyExc_StandardError,
			"SGI is not defined on this machine" );
}
#endif

PyObject *V24_RenderData_OldMapValue( V24_BPy_RenderData * self, PyObject * args )
{
	PyObject *tmp = V24_M_Render_GetSetAttributeInt(args,
		&self->renderContext->framapto, 1, 900);
	self->renderContext->framelen =
		(float)self->renderContext->framapto / self->renderContext->images;
	return tmp;
}

PyObject *V24_RenderData_NewMapValue( V24_BPy_RenderData * self, PyObject * args )
{
	PyObject *tmp = V24_M_Render_GetSetAttributeInt(args,
			&self->renderContext->images, 1, 900);
	self->renderContext->framelen =
		(float)self->renderContext->framapto / self->renderContext->images;
	return tmp;
}

static PyObject *V24_RenderData_getTimeCode( V24_BPy_RenderData * self) {
    char tc[12];
    int h, m, s, cfa;
    double fps;
    
    fps = (double) self->renderContext->frs_sec / 
	    self->renderContext->frs_sec_base;
    cfa = self->renderContext->cfra-1;
	s = cfa / fps;
	m = s / 60;
	h = m / 60;
    if( h > 99 )
        return PyString_FromString("Time Greater than 99 Hours!");	

	sprintf( tc, "%02d:%02d:%02d:%02d", h%60, m%60, s%60, 
		 (int) (cfa - ((int) (cfa / fps) * fps)));
	return PyString_FromString(tc);
}            

/***************************************************************************/
/* generic handlers for getting/setting attributes                         */
/***************************************************************************/

/*
 * get floating point attributes
 */

static PyObject *V24_RenderData_getFloatAttr( V24_BPy_RenderData *self, void *type )
{
	float param;

	switch( (int)type ) {
	case EXPP_RENDER_ATTR_GAUSSFILTER:
		param = self->renderContext->gauss;
		break;
	case EXPP_RENDER_ATTR_MBLURFACTOR:
		param = self->renderContext->blurfac;
		break;
	case EXPP_RENDER_ATTR_FPS_BASE:
		param = self->renderContext->frs_sec_base;
		break;
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type constant in V24_RenderData_getFloatAttr" );
	}
	return PyFloat_FromDouble( param );
}

/*
 * set floating point attributes which require clamping
 */

static int V24_RenderData_setFloatAttrClamp( V24_BPy_RenderData *self, PyObject *value,
		void *type )
{
	float *param;
	float min, max;

	switch( (int)type ) {
	case EXPP_RENDER_ATTR_GAUSSFILTER:
		min = 0.5f;
		max = 1.5f;
		param = &self->renderContext->gauss;
		break;
	case EXPP_RENDER_ATTR_MBLURFACTOR:
	    min = 0.01f;
		max = 5.0f;
		param = &self->renderContext->blurfac;
		break;
	case EXPP_RENDER_ATTR_FPS_BASE:
		min = 1.0f;
		max = 120.0f;
		param = &self->renderContext->frs_sec_base;
		break;
	default:
		return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type constant in V24_RenderData_setFloatAttrClamp" );
	}
	return V24_EXPP_setFloatClamped( value, param, min, max );
}

/*
 * get integer attributes
 */

static PyObject *V24_RenderData_getIValueAttr( V24_BPy_RenderData *self, void *type )
{
	long param;

	switch( (int)type ) {
	case EXPP_RENDER_ATTR_XPARTS:
		param = (long)self->renderContext->xparts;
		break;
	case EXPP_RENDER_ATTR_YPARTS:
		param = (long)self->renderContext->yparts;
		break;
	case EXPP_RENDER_ATTR_ASPECTX:
		param = (long)self->renderContext->xasp;
		break;
	case EXPP_RENDER_ATTR_ASPECTY:
		param = (long)self->renderContext->yasp;
		break;
	case EXPP_RENDER_ATTR_CFRAME:
		param = (long)self->renderContext->cfra;
		break;
	case EXPP_RENDER_ATTR_EFRAME:
		param = (long)self->renderContext->efra;
		break;
	case EXPP_RENDER_ATTR_SFRAME:
		param = (long)self->renderContext->sfra;
		break;
	case EXPP_RENDER_ATTR_FPS:
		param = self->renderContext->frs_sec;
		break;
	case EXPP_RENDER_ATTR_SIZEX:
		param = self->renderContext->xsch;
		break;
	case EXPP_RENDER_ATTR_SIZEY:
		param = self->renderContext->ysch;
		break;
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type constant in V24_RenderData_setIValueAttrClamp" );
	}
	return PyInt_FromLong( param );
}

/*
 * set integer attributes which require clamping
 */

static int V24_RenderData_setIValueAttrClamp( V24_BPy_RenderData *self, PyObject *value,
		void *type )
{
	void *param;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_RENDER_ATTR_XPARTS:
		min = 1;
		max = 512;
		size = 'h';
		param = &self->renderContext->xparts;
		break;
	case EXPP_RENDER_ATTR_YPARTS:
		min = 1;
		max = 64;
		size = 'h';
		param = &self->renderContext->yparts;
		break;
	case EXPP_RENDER_ATTR_ASPECTX:
		min = 1;
		max = 200;
	   	size = 'h';
		param = &self->renderContext->xasp;
		break;
	case EXPP_RENDER_ATTR_ASPECTY:
		min = 1;
		max = 200;
	   	size = 'h';
		param = &self->renderContext->yasp;
		break;
	case EXPP_RENDER_ATTR_CFRAME:
		min = 1;
		max = MAXFRAME;
	   	size = 'h';
		param = &self->renderContext->cfra;
		break;
	case EXPP_RENDER_ATTR_EFRAME:
		min = 1;
		max = MAXFRAME;
		size = 'h';
		param = &self->renderContext->efra;
		break;
	case EXPP_RENDER_ATTR_SFRAME:
		min = 1;
	    max = MAXFRAME;
		size = 'h';
		param = &self->renderContext->sfra;
		break;
	case EXPP_RENDER_ATTR_FPS:
		min = 1;
		max = 120;
		size = 'h';
		param = &self->renderContext->frs_sec;
		break;
	case EXPP_RENDER_ATTR_SIZEX:
		min = 4;
		max = 10000;
		size = 'h';
		param = &self->renderContext->xsch;
		break;
	case EXPP_RENDER_ATTR_SIZEY:
		min = 4;
		max = 10000;
		size = 'h';
		param = &self->renderContext->ysch;
		break;
	default:
		return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type constant in V24_RenderData_setIValueAttrClamp" );
	}
	return V24_EXPP_setIValueClamped( value, param, min, max, size );
}

/***************************************************************************/
/* handlers for other getting/setting attributes                           */
/***************************************************************************/

static PyObject *V24_RenderData_getModeBit( V24_BPy_RenderData *self, void* type )
{
	return V24_EXPP_getBitfield( &self->renderContext->mode,
			(int)type, 'i' );
}

static int V24_RenderData_setModeBit( V24_BPy_RenderData* self, PyObject *value,
		void* type )
{
	return V24_EXPP_setBitfield( value, &self->renderContext->mode,
			(int)type, 'i' );
}

#define MODE_MASK ( R_OSA | R_SHADOW | R_GAMMA | R_ENVMAP | R_EDGE | \
	R_FIELDS | R_FIELDSTILL | R_RADIO | R_BORDER | R_PANORAMA | R_CROP | \
	R_ODDFIELD | R_MBLUR | R_RAYTRACE | R_THREADS )

static PyObject *V24_RenderData_getMode( V24_BPy_RenderData *self )
{
	return PyInt_FromLong( (long)(self->renderContext->mode & MODE_MASK) );
}

static int V24_RenderData_setMode( V24_BPy_RenderData* self, PyObject *arg )
{
	int value;

	if( !PyInt_Check( arg ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	value = PyInt_AsLong( arg );
	if( value & ~MODE_MASK )
		return V24_EXPP_ReturnIntError( PyExc_ValueError, 
				"unexpected bits set in argument" );

	self->renderContext->mode = (short)value;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

static PyObject *V24_RenderData_getSceModeBits( V24_BPy_RenderData *self, void* type )
{
	return V24_EXPP_getBitfield( &self->renderContext->scemode, (int)type, 'h' );
}

static int V24_RenderData_setSceModeBits( V24_BPy_RenderData* self, PyObject *value,
		void* type )
{
	return V24_EXPP_setBitfield( value, &self->renderContext->scemode,
			(int)type, 'h' );
}

static PyObject *V24_RenderData_getSceMode( V24_BPy_RenderData *self )
{
	return PyInt_FromLong ( (long)self->renderContext->scemode );
}

static int V24_RenderData_setSceMode( V24_BPy_RenderData* self, PyObject *arg )
{
	int value;

	if( !PyInt_Check( arg ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	value = PyInt_AsLong( arg );
	if( value & ~( R_EXTENSION | R_DOSEQ ) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError, 
				"unexpected bits set in argument" );

	self->renderContext->scemode = (short)value;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}
 
static PyObject *V24_RenderData_getFramingType( V24_BPy_RenderData *self )
{
	return PyInt_FromLong( (long)self->scene->framing.type );
}

static int V24_RenderData_setFramingType( V24_BPy_RenderData *self, PyObject *value )
{
	return V24_EXPP_setIValueRange( value, &self->scene->framing.type,
			SCE_GAMEFRAMING_BARS, SCE_GAMEFRAMING_SCALE, 'b' );
}

static PyObject *V24_RenderData_getEdgeColor( V24_BPy_RenderData * self )
{
	return Py_BuildValue( "[fff]", self->renderContext->edgeR,
			self->renderContext->edgeG, self->renderContext->edgeB );
}

static int V24_RenderData_setEdgeColor( V24_BPy_RenderData * self, PyObject * args )
{
	float red, green, blue;

	/* if we get a list, convert to a tuple; otherwise hope for the best */
	if( PyList_Check( args ) )
		args = PySequence_Tuple( args );
	else
		Py_INCREF( args );

	if( !PyArg_ParseTuple( args, "fff", &red, &green, &blue ) ) {
		Py_DECREF( args );
		return V24_EXPP_ReturnIntError( PyExc_TypeError, "expected three floats" );
	}
	Py_DECREF( args );

	self->renderContext->edgeR = V24_EXPP_ClampFloat( red, 0.0, 1.0 );
	self->renderContext->edgeG = V24_EXPP_ClampFloat( green, 0.0, 1.0 );
	self->renderContext->edgeB = V24_EXPP_ClampFloat( blue, 0.0, 1.0 );
	return 0;
}

static PyObject *V24_RenderData_getOSALevel( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long)self->renderContext->osa );
}

static PyObject *V24_RenderData_getRenderer( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long)self->renderContext->renderer ); 
}

static PyObject *V24_RenderData_getImageType( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long) self->renderContext->imtype );
}

static int V24_RenderData_setGameFrameColor( V24_BPy_RenderData * self,
		PyObject * args )
{
	float red, green, blue;

	/* if we get a list, convert to a tuple; otherwise hope for the best */
	if( PyList_Check( args ) )
		args = PySequence_Tuple( args );
	else
		Py_INCREF( args );

	if( !PyArg_ParseTuple( args, "fff", &red, &green, &blue ) ) {
		Py_DECREF( args );
		return V24_EXPP_ReturnIntError( PyExc_TypeError, "expected three floats" );
	}
	Py_DECREF( args );

	self->scene->framing.col[0] = V24_EXPP_ClampFloat( red, 0.0, 1.0 );
	self->scene->framing.col[1] = V24_EXPP_ClampFloat( green, 0.0, 1.0 );
	self->scene->framing.col[2] = V24_EXPP_ClampFloat( blue, 0.0, 1.0 );
	return 0;
}

static PyObject *V24_RenderData_getGameFrameColor( V24_BPy_RenderData * self )
{
	return Py_BuildValue( "[fff]", self->scene->framing.col[0],
		 self->scene->framing.col[1], self->scene->framing.col[2] );
}

static PyObject *V24_RenderData_getBackbuf( V24_BPy_RenderData * self )
{
	return V24_EXPP_getBitfield( &self->renderContext->bufflag,
			R_BACKBUF, 'h' );
}

static int V24_RenderData_setBackbuf( V24_BPy_RenderData* self, PyObject *value )
{
	return V24_EXPP_setBitfield( value, &self->renderContext->bufflag,
			R_BACKBUF, 'h' );
}

static int V24_RenderData_setImagePlanes( V24_BPy_RenderData *self, PyObject *value )
{
	int depth;
	char *errstr = "expected int argument of 8, 24, or 32";

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, errstr );

	depth = PyInt_AsLong( value );
	if( depth != 8 && depth != 24 && depth != 32 )
		return V24_EXPP_ReturnIntError( PyExc_ValueError, errstr );

	self->renderContext->planes = (short)depth;

	return 0;
}

static PyObject *V24_RenderData_getImagePlanes( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long) self->renderContext->planes );
}

static int V24_RenderData_setAlphaMode( V24_BPy_RenderData *self, PyObject *value )
{
	return V24_EXPP_setIValueRange( value, &self->renderContext->alphamode,
			R_ADDSKY, R_ALPHAKEY, 'h' );
}

static PyObject *V24_RenderData_getAlphaMode( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long) self->renderContext->alphamode );
}

static PyObject *V24_RenderData_getDisplayMode( void )
{
	return PyInt_FromLong( (long) G.displaymode );
}

static int V24_RenderData_setDisplayMode( V24_BPy_RenderData *self,
		PyObject *value )
{
	return V24_EXPP_setIValueRange( value, &G.displaymode,
			R_DISPLAYIMAGE, R_DISPLAYSCREEN, 'h' );
}

static PyObject *V24_RenderData_getRenderPath( V24_BPy_RenderData * self )
{
	return PyString_FromString( self->renderContext->pic );
}

static int V24_RenderData_setRenderPath( V24_BPy_RenderData * self, PyObject * value )
{
	char *name;

	name = PyString_AsString( value );
	if( !name )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected a string" );

	if( strlen( name ) >= sizeof(self->renderContext->pic) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
						"render path is too long" );

	strcpy( self->renderContext->pic, name );
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

PyObject *V24_RenderData_getBackbufPath( V24_BPy_RenderData * self )
{
	return PyString_FromString( self->renderContext->backbuf );
}

static int V24_RenderData_setBackbufPath( V24_BPy_RenderData *self, PyObject *value )
{
	char *name;
	Image *ima;

	name = PyString_AsString( value );
	if( !name )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, "expected a string" );

	if( strlen( name ) >= sizeof(self->renderContext->backbuf) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"backbuf path is too long" );

	strcpy( self->renderContext->backbuf, name );
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	ima = BKE_add_image_file( name );
	if( ima )
		BKE_image_signal( ima, NULL, IMA_SIGNAL_RELOAD );

	return 0;
}

PyObject *V24_RenderData_getFtypePath( V24_BPy_RenderData * self )
{
	return PyString_FromString( self->renderContext->ftype );
}

static int V24_RenderData_setFtypePath( V24_BPy_RenderData *self, PyObject *value )
{
	char *name;

	name = PyString_AsString( value );
	if( !name )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, "expected a string" );

	if( strlen( name ) >= sizeof(self->renderContext->ftype) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"ftype path is too long" );

	strcpy( self->renderContext->ftype, name );
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

PyObject *V24_RenderData_getRenderWinSize( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long) self->renderContext->size );
}

static int V24_RenderData_setRenderWinSize( V24_BPy_RenderData *self, PyObject *value )
{
	int size;
	char *errstr = "expected int argument of 25, 50, 75, or 100";

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, errstr );

	size = PyInt_AsLong( value );
	if( size != 25 && size != 50 && size != 75 && size != 100 )
		return V24_EXPP_ReturnIntError( PyExc_ValueError, errstr );

	self->renderContext->size = (short)size;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

static PyObject *V24_RenderData_getMapOld( V24_BPy_RenderData *self )
{
	return PyInt_FromLong( (long)self->renderContext->framapto );
}

static int V24_RenderData_setMapOld( V24_BPy_RenderData *self, PyObject *value )
{
	int result = V24_EXPP_setIValueClamped( value, &self->renderContext->framapto,
			1, 900, 'h' );
	self->renderContext->framelen =
		(float)self->renderContext->framapto / self->renderContext->images;
	return result;
}

static PyObject *V24_RenderData_getMapNew( V24_BPy_RenderData *self )
{
	return PyInt_FromLong( (long)self->renderContext->images );
}

static int V24_RenderData_setMapNew( V24_BPy_RenderData *self, PyObject *value )
{
	int result = V24_EXPP_setIValueClamped( value, &self->renderContext->images,
			1, 900, 'h' );
	self->renderContext->framelen =
		(float)self->renderContext->framapto / self->renderContext->images;
	return result;
}

static PyObject *V24_RenderData_getSet( V24_BPy_RenderData *self )
{
	if( self->scene->set )
		return V24_Scene_CreatePyObject( self->scene->set );
	Py_RETURN_NONE;
}

static int V24_RenderData_setSet( V24_BPy_RenderData *self, PyObject *value )
{
	V24_BPy_Scene *sc;

	/* if "None", delete the link to the scene */
	if( value == Py_None ) {
		self->scene->set = NULL;
		return 0;
	}

	/* be sure argument is a Scene */
	if( !V24_BPy_Scene_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected Scene as argument" );

	/* check for attempt to link to ourselves */
	sc = (V24_BPy_Scene *)value;
	if( self->scene == sc->scene )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"cannot link a scene to itself" );

	/*
	 * Accept the set link, then check for a circular link.  If circular link
	 * exists, scene_check_setscene() sets self->scene->set to NULL.
	 */

	self->scene->set = sc->scene;
	if( !scene_check_setscene( self->scene ) )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"linking scene would create a cycle" );

	return 0;
}


PyObject *V24_RenderData_getThreads( V24_BPy_RenderData * self )
{
	return PyInt_FromLong( (long) self->renderContext->threads );
}

static int V24_RenderData_setThreads( V24_BPy_RenderData *self, PyObject *value )
{
	int threads;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, "Error, threads must be an int" );

	threads = PyInt_AsLong( value );
	if (threads<1) threads = 1;
	else if (threads>8) threads = 8;
	
	self->renderContext->threads = (short)threads;
	V24_EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

/***************************************************************************/
/* V24_BPy_RenderData attribute def                                            */
/***************************************************************************/
static PyGetSetDef V24_BPy_RenderData_getseters[] = {
	{"oversampling",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Oversampling (anti-aliasing) enabled",
	 (void *)R_OSA},
	{"shadow",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Shadow calculation enabled",
	 (void *)R_SHADOW},
	{"gammaCorrection",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Gamma correction enabled",
	 (void *)R_GAMMA},
/* R_ORTHO	unused */
	{"environmentMap",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Environment map rendering enabled",
	 (void *)R_ENVMAP},
	{"toonShading",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Toon edge shading enabled",
	 (void *)R_EDGE},
	{"fieldRendering", 
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Field rendering enabled",
	 (void *)R_FIELDS},
	{"fieldTimeDisable",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Time difference in field calculations disabled ('X' in UI)",
	 (void *)R_FIELDSTILL},
	{"radiosityRender",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Radiosity rendering enabled",
	 (void *)R_RADIO},
	{"borderRender",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Small cut-out rendering enabled",
	 (void *)R_BORDER},
	{"panorama",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Panorama rendering enabled",
	 (void *)R_PANORAMA},
	{"crop",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Crop image during border renders",
	 (void *)R_CROP},
/* R_COSMO	unsupported */
	{"oddFieldFirst",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Odd field first rendering enabled",
	 (void *)R_ODDFIELD},
	{"motionBlur",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Motion blur enabled",
	 (void *)R_MBLUR},
	{"rayTracing",
	 (getter)V24_RenderData_getModeBit, (setter)V24_RenderData_setModeBit,
	 "Ray tracing enabled",
	 (void *)R_RAYTRACE},
/* R_GAUSS unused */
/* R_FBUF unused */
/* R_THREADS unused */
	{"threads",
	 (getter)V24_RenderData_getThreads, (setter)V24_RenderData_setThreads,
	 "Number of threads used to render",
	 NULL},
/* R_SPEED unused */
	{"mode",
	 (getter)V24_RenderData_getMode, (setter)V24_RenderData_setMode,
	 "Mode bitfield",
	 NULL},

	/* scene modes */
	{"sequencer",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "'Do Sequence' enabled",
     (void *)R_DOSEQ},
	{"extensions",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "Add extensions to output (when rendering animations) enabled",
     (void *)R_EXTENSION},
	{"compositor",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "'Do Compositor' enabled.",
     (void *)R_DOCOMP},
	{"freeImages",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "Free texture images after render.",
     (void *)R_FREE_IMAGE},
	{"singleLayer",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "Only render the active layer.",
     (void *)R_SINGLE_LAYER},
	{"saveBuffers",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "Save render buffers to disk while rendering, saves memory.",
     (void *)R_EXR_TILE_FILE},
	{"compositeFree",
     (getter)V24_RenderData_getSceModeBits, (setter)V24_RenderData_setSceModeBits,
     "Free nodes that are not used while composite.",
     (void *)R_COMP_FREE},

	{"sceneMode",
     (getter)V24_RenderData_getSceMode, (setter)V24_RenderData_setSceMode,
     "Scene mode bitfield",
     NULL},
/* R_BG_RENDER unused */

	{"gameFrame",
	 (getter)V24_RenderData_getFramingType, (setter)V24_RenderData_setFramingType,
	 "Game framing type",
	 NULL},

	{"renderPath",
	 (getter)V24_RenderData_getRenderPath, (setter)V24_RenderData_setRenderPath,
	 "The path to output the rendered images to",
	 NULL},
	{"backbufPath",
	 (getter)V24_RenderData_getBackbufPath, (setter)V24_RenderData_setBackbufPath,
	 "Path to a background image (setting loads image)",
	 NULL},
	{"ftypePath",
	 (getter)V24_RenderData_getFtypePath, (setter)V24_RenderData_setFtypePath,
	 "The path to Ftype file",
	 NULL},
	{"edgeColor",
	 (getter)V24_RenderData_getEdgeColor, (setter)V24_RenderData_setEdgeColor,
	 "RGB color triplet for edges in Toon shading",
	 NULL},
	{"OSALevel",
	 (getter)V24_RenderData_getOSALevel, (setter)V24_RenderData_setOSALevel,
	 "Oversampling (anti-aliasing) level",
	 NULL},
	{"renderwinSize",
	 (getter)V24_RenderData_getRenderWinSize, (setter)V24_RenderData_setRenderWinSize,
	 "Size of the rendering window (25, 50, 75, or 100)",
	 NULL},
	{"border",
	 (getter)V24_RenderData_getBorder, (setter)V24_RenderData_setBorder,
	 "The border for border rendering",
	 NULL},
	{"timeCode",
	 (getter)V24_RenderData_getTimeCode, (setter)NULL,
	 "Get the current frame in HH:MM:SS:FF format",
	 NULL},
	{"renderer",
	 (getter)V24_RenderData_getRenderer, (setter)V24_RenderData_setRenderer,
	 "Rendering engine choice",
	 NULL},
	{"imageType",
	 (getter)V24_RenderData_getImageType, (setter)V24_RenderData_setImageType,
	 "File format for saving images",
	 NULL},
	{"gameFrameColor",
	 (getter)V24_RenderData_getGameFrameColor,(setter)V24_RenderData_setGameFrameColor,
	 "RGB color triplet for bars",
	 NULL},
	{"backbuf",
	 (getter)V24_RenderData_getBackbuf, (setter)V24_RenderData_setBackbuf,
	 "Backbuffer image enabled",
	 NULL},
	{"imagePlanes",
	 (getter)V24_RenderData_getImagePlanes, (setter)V24_RenderData_setImagePlanes,
	 "Image depth (8, 24, or 32 bits)",
	 NULL},
	{"alphaMode",
	 (getter)V24_RenderData_getAlphaMode, (setter)V24_RenderData_setAlphaMode,
	 "Setting for sky/background.",
	 NULL},
	{"displayMode",
	 (getter)V24_RenderData_getDisplayMode, (setter)V24_RenderData_setDisplayMode,
	 "Render output in separate window or 3D view",
	 NULL},

	{"xParts",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Number of horizontal parts for image render",
	 (void *)EXPP_RENDER_ATTR_XPARTS},
	{"yParts",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Number of vertical parts for image render",
	 (void *)EXPP_RENDER_ATTR_YPARTS},
	{"aspectX",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Horizontal aspect ratio",
	 (void *)EXPP_RENDER_ATTR_ASPECTX},
	{"aspectY",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Vertical aspect ratio",
	 (void *)EXPP_RENDER_ATTR_ASPECTY},
	{"cFrame",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "The current frame for rendering",
	 (void *)EXPP_RENDER_ATTR_CFRAME},
	{"sFrame",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Starting frame for rendering",
	 (void *)EXPP_RENDER_ATTR_SFRAME},
	{"eFrame",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Ending frame for rendering",
	 (void *)EXPP_RENDER_ATTR_EFRAME},
	{"fps",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Frames per second",
	 (void *)EXPP_RENDER_ATTR_FPS},
	{"fpsBase",
	 (getter)V24_RenderData_getFloatAttr, (setter)V24_RenderData_setFloatAttrClamp,
	 "Frames per second base",
	 (void *)EXPP_RENDER_ATTR_FPS_BASE},
	{"sizeX",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Image width (in pixels)",
	 (void *)EXPP_RENDER_ATTR_SIZEX},
	{"sizeY",
	 (getter)V24_RenderData_getIValueAttr, (setter)V24_RenderData_setIValueAttrClamp,
	 "Image height (in pixels)",
	 (void *)EXPP_RENDER_ATTR_SIZEY},

	{"gaussFilter",
	 (getter)V24_RenderData_getFloatAttr, (setter)V24_RenderData_setFloatAttrClamp,
	 "Gauss filter size",
	 (void *)EXPP_RENDER_ATTR_GAUSSFILTER},
	{"mblurFactor",
	 (getter)V24_RenderData_getFloatAttr, (setter)V24_RenderData_setFloatAttrClamp,
	 "Motion blur factor",
	 (void *)EXPP_RENDER_ATTR_MBLURFACTOR},
	{"mapOld",
	 (getter)V24_RenderData_getMapOld, (setter)V24_RenderData_setMapOld,
	 "Number of frames the Map Old will last",
	 NULL},
	{"mapNew",
	 (getter)V24_RenderData_getMapNew, (setter)V24_RenderData_setMapNew,
	 "New mapping value (in frames)",
	 NULL},
	{"set",
	 (getter)V24_RenderData_getSet, (setter)V24_RenderData_setSet,
	 "Scene link 'set' value",
	 NULL},

	{"yafrayGIMethod",
	 (getter)V24_RenderData_getYafrayGIMethod, (setter)V24_RenderData_setYafrayGIMethod,
	 "Global illumination method",
	 NULL},
	{"yafrayGIQuality",
	 (getter)V24_RenderData_getYafrayGIQuality, (setter)V24_RenderData_setYafrayGIQuality,
	 "Global Illumination quality",
	 NULL},
	{"yafrayExportToXML",
	 (getter)V24_RenderData_getYafrayExportToXML, (setter)V24_RenderData_setYafrayExportToXML,
	 "If true export to an xml file and call yafray instead of plugin",
	 NULL},
	{"yafrayAutoAntiAliasing",
	 (getter)V24_RenderData_getYafrayAutoAntiAliasing, (setter)V24_RenderData_setYafrayAutoAntiAliasing,
	 "Automatic anti-aliasing enabled/disabled",
	 NULL},
	{"yafrayClampRGB",
	 (getter)V24_RenderData_getYafrayClampRGB, (setter)V24_RenderData_setYafrayClampRGB,
	 "Clamp RGB enabled/disabled",
	 NULL},
	{"yafrayAntiAliasingPasses",
	 (getter)V24_RenderData_getYafrayAntiAliasingPasses, (setter)V24_RenderData_setYafrayAntiAliasingPasses,
	 "Number of anti-aliasing passes (0 is no anti-aliasing)",
	 NULL},
	{"yafrayAntiAliasingSamples",
	 (getter)V24_RenderData_getYafrayAntiAliasingSamples, (setter)V24_RenderData_setYafrayAntiAliasingSamples,
	 "Number of samples per pass",
	 NULL},
	{"yafrayAntiAliasingPixelSize",
	 (getter)V24_RenderData_getYafrayAntiAliasingPixelSize, (setter)V24_RenderData_setYafrayAntiAliasingPixelSize,
	 "Anti-aliasing pixel filter size",
	 NULL},
	{"yafrayAntiAliasingThreshold",
	 (getter)V24_RenderData_getYafrayAntiAliasingThreshold, (setter)V24_RenderData_setYafrayAntiAliasingThreshold,
	 "Anti-aliasing threshold",
	 NULL},
	{"yafrayNumberOfProcessors",
	 (getter)V24_RenderData_getYafrayNumberOfProcessors, (setter)V24_RenderData_setYafrayNumberOfProcessors,
	 "Number of processors to use",
	 NULL},
	{"yafrayGICache",
	 (getter)V24_RenderData_getYafrayGICache, (setter)V24_RenderData_setYafrayGICache,
	 "Cache occlusion/irradiance samples (faster)",
	 NULL},
	{"yafrayGICacheBumpNormals",
	 (getter)V24_RenderData_getYafrayGICacheBumpNormals, (setter)V24_RenderData_setYafrayGICacheBumpNormals,
	 "Enable/disable bumpnormals for cache",
	 NULL},
	{"yafrayGICacheShadowQuality",
	 (getter)V24_RenderData_getYafrayGICacheShadowQuality, (setter)V24_RenderData_setYafrayGICacheShadowQuality,
	 "Shadow quality, keep it under 0.95 :-)",
	 NULL},
	{"yafrayGICachePixelsPerSample",
	 (getter)V24_RenderData_getYafrayGICachePixelsPerSample, (setter)V24_RenderData_setYafrayGICachePixelsPerSample,
	 "Maximum number of pixels without samples, the lower the better and slower",
	 NULL},
	{"yafrayGICacheRefinement",
	 (getter)V24_RenderData_getYafrayGICacheRefinement, (setter)V24_RenderData_setYafrayGICacheRefinement,
	 "Threshold to refine shadows EXPERIMENTAL. 1 = no refinement",
	 NULL},
	{"yafrayGIPhotons",
	 (getter)V24_RenderData_getYafrayGIPhotons, (setter)V24_RenderData_setYafrayGIPhotons,
	 "Enable/disable use of global photons to help in GI",
	 NULL},
	{"yafrayGITunePhotons",
	 (getter)V24_RenderData_getYafrayGITunePhotons, (setter)V24_RenderData_setYafrayGITunePhotons,
	 "If true the photonmap is shown directly in the render for tuning",
	 NULL},

	{NULL,NULL,NULL,NULL,NULL}
};

/***************************************************************************/
/* V24_BPy_RenderData method def                                               */
/***************************************************************************/
static PyMethodDef V24_BPy_RenderData_methods[] = {
	{"render", ( PyCFunction ) V24_RenderData_Render, METH_NOARGS,
	 "() - render the scene"},
	{"saveRenderedImage", (PyCFunction)V24_RenderData_SaveRenderedImage, METH_VARARGS,
	 "(filename) - save an image generated by a call to render() (set output path first)"},
	{"renderAnim", ( PyCFunction ) V24_RenderData_RenderAnim, METH_NOARGS,
	 "() - render a sequence from start frame to end frame"},
	{"play", ( PyCFunction ) V24_RenderData_Play, METH_NOARGS,
	 "() - play animation of rendered images/avi (searches Pics: field)"},
	{"setRenderPath", ( PyCFunction ) V24_RenderData_SetRenderPath,
	 METH_VARARGS,
	 "(string) - get/set the path to output the rendered images to"},
	{"getRenderPath", ( PyCFunction ) V24_RenderData_getRenderPath,
	 METH_NOARGS,
	 "() - get the path to directory where rendered images will go"},
	{"setBackbufPath", ( PyCFunction ) V24_RenderData_SetBackbufPath,
	 METH_VARARGS,
	 "(string) - get/set the path to a background image and load it"},
	{"getBackbufPath", ( PyCFunction ) V24_RenderData_getBackbufPath,
	 METH_NOARGS,
	 "() - get the path to background image file"},
	{"enableBackbuf", ( PyCFunction ) V24_RenderData_EnableBackbuf,
	 METH_VARARGS,
	 "(bool) - enable/disable the backbuf image"},
	{"setFtypePath", ( PyCFunction ) V24_RenderData_SetFtypePath, METH_VARARGS,
	 "(string) - get/set the path to output the Ftype file"},
	{"getFtypePath", ( PyCFunction ) V24_RenderData_getFtypePath, METH_NOARGS,
	 "() - get the path to Ftype file"},
	{"enableExtensions", ( PyCFunction ) V24_RenderData_EnableExtensions,
	 METH_VARARGS,
	 "(bool) - enable/disable windows extensions for output files"},
	{"enableSequencer", ( PyCFunction ) V24_RenderData_EnableSequencer,
	 METH_VARARGS,
	 "(bool) - enable/disable Do Sequence"},
	{"enableRenderDaemon", ( PyCFunction ) V24_RenderData_EnableRenderDaemon,
	 METH_VARARGS,
	 "(bool) - enable/disable Scene daemon"},
	{"enableToonShading", ( PyCFunction ) V24_RenderData_EnableToonShading,
	 METH_VARARGS,
	 "(bool) - enable/disable Edge rendering"},
	{"edgeIntensity", ( PyCFunction ) V24_RenderData_EdgeIntensity,
	 METH_VARARGS,
	 "(int) - get/set edge intensity for toon shading"},
	{"setEdgeColor", ( PyCFunction ) V24_RenderData_SetEdgeColor, METH_VARARGS,
	 "(f,f,f) - set the edge color for toon shading - Red,Green,Blue expected."},
	{"getEdgeColor", ( PyCFunction ) V24_RenderData_GetEdgeColor, METH_NOARGS,
	 "() - get the edge color for toon shading - Red,Green,Blue expected."},
	{"enableOversampling", ( PyCFunction ) V24_RenderData_EnableOversampling,
	 METH_VARARGS,
	 "(bool) - enable/disable oversampling (anit-aliasing)."},
	{"setOversamplingLevel",
	 ( PyCFunction ) V24_RenderData_SetOversamplingLevel, METH_VARARGS,
	 "(enum) - get/set the level of oversampling (anit-aliasing)."},
	{"enableMotionBlur", ( PyCFunction ) V24_RenderData_EnableMotionBlur,
	 METH_VARARGS,
	 "(bool) - enable/disable MBlur."},
	{"motionBlurLevel", ( PyCFunction ) V24_RenderData_MotionBlurLevel,
	 METH_VARARGS,
	 "(float) - get/set the length of shutter time for motion blur."},
	{"partsX", ( PyCFunction ) V24_RenderData_PartsX, METH_VARARGS,
	 "(int) - get/set the number of parts to divide the render in the X direction"},
	{"partsY", ( PyCFunction ) V24_RenderData_PartsY, METH_VARARGS,
	 "(int) - get/set the number of parts to divide the render in the Y direction"},
	{"enableSky", ( PyCFunction ) V24_RenderData_EnableSky, METH_NOARGS,
	 "() - enable render background with sky"},
	{"enablePremultiply", ( PyCFunction ) V24_RenderData_EnablePremultiply,
	 METH_NOARGS,
	 "() - enable premultiply alpha"},
	{"enableKey", ( PyCFunction ) V24_RenderData_EnableKey, METH_NOARGS,
	 "() - enable alpha and color values remain unchanged"},
	{"enableShadow", ( PyCFunction ) V24_RenderData_EnableShadow, METH_VARARGS,
	 "(bool) - enable/disable shadow calculation"},
	{"enablePanorama", ( PyCFunction ) V24_RenderData_EnablePanorama,
	 METH_VARARGS,
	 "(bool) - enable/disable panorama rendering (output width is multiplied by Xparts)"},
	{"enableEnvironmentMap",
	 ( PyCFunction ) V24_RenderData_EnableEnvironmentMap, METH_VARARGS,
	 "(bool) - enable/disable environment map rendering"},
	{"enableRayTracing", ( PyCFunction ) V24_RenderData_EnableRayTracing,
	 METH_VARARGS,
	 "(bool) - enable/disable ray tracing"},
	{"enableRadiosityRender",
	 ( PyCFunction ) V24_RenderData_EnableRadiosityRender, METH_VARARGS,
	 "(bool) - enable/disable radiosity rendering"},
	{"getRenderWinSize", ( PyCFunction ) V24_RenderData_getRenderWinSize,
	 METH_NOARGS,
	 "() - get the size of the render window"},
	{"setRenderWinSize", ( PyCFunction ) V24_RenderData_SetRenderWinSize,
	 METH_VARARGS,
	 "(int) - set the size of the render window"},
	{"enableFieldRendering",
	 ( PyCFunction ) V24_RenderData_EnableFieldRendering, METH_VARARGS,
	 "(bool) - enable/disable field rendering"},
	{"enableOddFieldFirst", ( PyCFunction ) V24_RenderData_EnableOddFieldFirst,
	 METH_VARARGS,
	 "(bool) - enable/disable Odd field first rendering (Default: Even field)"},
	{"enableFieldTimeDisable",
	 ( PyCFunction ) V24_RenderData_EnableFieldTimeDisable, METH_VARARGS,
	 "(bool) - enable/disable time difference in field calculations"},
	{"enableGaussFilter", ( PyCFunction ) V24_RenderData_EnableGaussFilter,
	 METH_VARARGS,
	 "(bool) - enable/disable Gauss sampling filter for antialiasing"},
	{"enableBorderRender", ( PyCFunction ) V24_RenderData_EnableBorderRender,
	 METH_VARARGS,
	 "(bool) - enable/disable small cut-out rendering"},
	{"setBorder", ( PyCFunction ) V24_RenderData_SetBorder, METH_VARARGS,
	 "(f,f,f,f) - set the border for border rendering"},
	{"gaussFilterSize", ( PyCFunction ) V24_RenderData_GaussFilterSize,
	 METH_VARARGS,
	 "(float) - get/sets the Gauss filter size"},
	{"startFrame", ( PyCFunction ) V24_RenderData_StartFrame, METH_VARARGS,
	 "(int) - get/set the starting frame for rendering"},
	{"currentFrame", ( PyCFunction ) V24_RenderData_CurrentFrame, METH_VARARGS,
	 "(int) - get/set the current frame for rendering"},
	{"endFrame", ( PyCFunction ) V24_RenderData_EndFrame, METH_VARARGS,
	 "(int) - get/set the ending frame for rendering"},
	{"getTimeCode", ( PyCFunction ) V24_RenderData_getTimeCode, METH_NOARGS,
	 "get the current frame in HH:MM:SS:FF format"},
	{"imageSizeX", ( PyCFunction ) V24_RenderData_ImageSizeX, METH_VARARGS,
	 "(int) - get/set the image width in pixels"},
	{"imageSizeY", ( PyCFunction ) V24_RenderData_ImageSizeY, METH_VARARGS,
	 "(int) - get/set the image height in pixels"},
	{"aspectRatioX", ( PyCFunction ) V24_RenderData_AspectRatioX, METH_VARARGS,
	 "(int) - get/set the horizontal aspect ratio"},
	{"aspectRatioY", ( PyCFunction ) V24_RenderData_AspectRatioY, METH_VARARGS,
	 "(int) - get/set the vertical aspect ratio"},
	{"setRenderer", ( PyCFunction ) V24_RenderData_SetRenderer, METH_VARARGS,
	 "(enum) - get/set which renderer to render the output"},
	{"enableCropping", ( PyCFunction ) V24_RenderData_EnableCropping,
	 METH_VARARGS,
	 "(bool) - enable/disable exclusion of border rendering from total image"},
	{"setImageType", ( PyCFunction ) V24_RenderData_SetImageType, METH_VARARGS,
	 "(enum) - get/set the type of image to output from the render"},
	{"quality", ( PyCFunction ) V24_RenderData_Quality, METH_VARARGS,
	 "(int) - get/set quality get/setting for JPEG images, AVI Jpeg and SGI movies"},
	{"framesPerSec", ( PyCFunction ) V24_RenderData_FramesPerSec, METH_VARARGS,
	 "(int) - get/set frames per second"},
	{"enableGrayscale", ( PyCFunction ) V24_RenderData_EnableGrayscale,
	 METH_NOARGS,
	 "() - images are saved with BW (grayscale) data"},
	{"enableRGBColor", ( PyCFunction ) V24_RenderData_EnableRGBColor,
	 METH_NOARGS,
	 "() - images are saved with RGB (color) data"},
	{"enableRGBAColor", ( PyCFunction ) V24_RenderData_EnableRGBAColor,
	 METH_NOARGS,
	 "() - images are saved with RGB and Alpha data (if supported)"},
	{"sizePreset", ( PyCFunction ) V24_RenderData_SizePreset, METH_VARARGS,
	 "(enum) - get/set the render to one of a few preget/sets"},
	{"setYafrayGIQuality", ( PyCFunction ) V24_RenderData_SetYafrayGIQuality,
	 METH_VARARGS,
	 "(enum) - set yafray global Illumination quality"},
	{"getYafrayGIQuality", ( PyCFunction ) V24_RenderData_GetYafrayGIQuality,
	 METH_VARARGS,
	 "(enum) - get yafray global Illumination quality"},
	{"setYafrayGIMethod", ( PyCFunction ) V24_RenderData_SetYafrayGIMethod,
	 METH_VARARGS,
	 "(enum) - set yafray global Illumination method"},
	{"getYafrayGIMethod", ( PyCFunction ) V24_RenderData_GetYafrayGIMethod,
	 METH_VARARGS,
	 "(enum) - get yafray global Illumination method"},
	{"yafrayGIPower", ( PyCFunction ) V24_RenderData_YafrayGIPower,
	 METH_VARARGS,
	 "(float) - get/set GI lighting intensity scale"},
	{"yafrayGIIndirPower", ( PyCFunction ) V24_RenderData_YafrayGIIndirPower,
	 METH_VARARGS,
	 "(float) - get/set GI indifect lighting intensity scale"},
	{"yafrayGIDepth", ( PyCFunction ) V24_RenderData_YafrayGIDepth,
	 METH_VARARGS,
	 "(int) - get/set number of bounces of the indirect light"},
	{"yafrayGICDepth", ( PyCFunction ) V24_RenderData_YafrayGICDepth,
	 METH_VARARGS,
	 "(int) - get/set number of bounces inside objects (for caustics)"},
	{"enableYafrayGICache", ( PyCFunction ) V24_RenderData_EnableYafrayGICache,
	 METH_VARARGS,
	 "(bool) - enable/disable cache irradiance samples (faster)"},
	{"enableYafrayGIPhotons",
	 ( PyCFunction ) V24_RenderData_EnableYafrayGIPhotons, METH_VARARGS,
	 "(bool) - enable/disable use global photons to help in GI"},
	{"yafrayGIPhotonCount", ( PyCFunction ) V24_RenderData_YafrayGIPhotonCount,
	 METH_VARARGS,
	 "(int) - get/set number of photons to shoot"},
	{"yafrayGIPhotonRadius",
	 ( PyCFunction ) V24_RenderData_YafrayGIPhotonRadius, METH_VARARGS,
	 "(float) - get/set radius to search for photons to mix (blur)"},
	{"yafrayGIPhotonMixCount",
	 ( PyCFunction ) V24_RenderData_YafrayGIPhotonMixCount, METH_VARARGS,
	 "(int) - get/set number of photons to mix"},
	{"enableYafrayGITunePhotons",
	 ( PyCFunction ) V24_RenderData_EnableYafrayGITunePhotons, METH_VARARGS,
	 "(bool) - enable/disable show the photonmap directly in the render for tuning"},
	{"yafrayGIShadowQuality",
	 ( PyCFunction ) V24_RenderData_YafrayGIShadowQuality, METH_VARARGS,
	 "(float) - get/set the shadow quality, keep it under 0.95"},
	{"yafrayGIPixelsPerSample",
	 ( PyCFunction ) V24_RenderData_YafrayGIPixelsPerSample, METH_VARARGS,
	 "(int) - get/set maximum number of pixels without samples, the lower the better and slower"},
	{"yafrayGIRefinement", ( PyCFunction ) V24_RenderData_YafrayGIRefinement,
	 METH_VARARGS,
	 "(float) - get/setthreshold to refine shadows EXPERIMENTAL. 1 = no refinement"},
	{"yafrayRayBias", ( PyCFunction ) V24_RenderData_YafrayRayBias,
	 METH_VARARGS,
	 "(float) - get/set shadow ray bias to avoid self shadowing"},
	{"yafrayRayDepth", ( PyCFunction ) V24_RenderData_YafrayRayDepth,
	 METH_VARARGS,
	 "(int) - get/set maximum render ray depth from the camera"},
	{"yafrayGamma", ( PyCFunction ) V24_RenderData_YafrayGamma, METH_VARARGS,
	 "(float) - get/set gamma correction, 1 is off"},
	{"yafrayExposure", ( PyCFunction ) V24_RenderData_YafrayExposure,
	 METH_VARARGS,
	 "(float) - get/set exposure adjustment, 0 is off"},
	{"yafrayProcessorCount",
	 ( PyCFunction ) V24_RenderData_YafrayProcessorCount, METH_VARARGS,
	 "(int) - get/set number of processors to use"},
	{"enableGameFrameStretch",
	 ( PyCFunction ) V24_RenderData_EnableGameFrameStretch, METH_NOARGS,
	 "(l) - enble stretch or squeeze the viewport to fill the display window"},
	{"enableGameFrameExpose",
	 ( PyCFunction ) V24_RenderData_EnableGameFrameExpose, METH_NOARGS,
	 "(l) - enable show the entire viewport in the display window, viewing more horizontally or vertically"},
	{"enableGameFrameBars", ( PyCFunction ) V24_RenderData_EnableGameFrameBars,
	 METH_NOARGS,
	 "() - enable show the entire viewport in the display window, using bar horizontally or vertically"},
	{"setGameFrameColor", ( PyCFunction ) V24_RenderData_SetGameFrameColor,
	 METH_VARARGS,
	 "(f,f,f) - set the red, green, blue component of the bars"},
	{"getGameFrameColor", ( PyCFunction ) V24_RenderData_GetGameFrameColor,
	 METH_NOARGS,
	 "() - get the red, green, blue component of the bars"},
	{"SGIMaxsize", ( PyCFunction ) V24_RenderData_SGIMaxsize, METH_VARARGS,
	 "(int) - get/set maximum size per frame to save in an SGI movie"},
	{"enableSGICosmo", ( PyCFunction ) V24_RenderData_EnableSGICosmo,
	 METH_VARARGS,
	 "(bool) - enable/disable attempt to save SGI movies using Cosmo hardware"},
	{"oldMapValue", ( PyCFunction ) V24_RenderData_OldMapValue, METH_VARARGS,
	 "(int) - get/set specify old map value in frames"},
	{"newMapValue", ( PyCFunction ) V24_RenderData_NewMapValue, METH_VARARGS,
	 "(int) - get/set specify new map value in frames"},
	{NULL, NULL, 0, NULL}
};
 
/*------------------------------------V24_BPy_RenderData Type defintion------ */
PyTypeObject V24_RenderData_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender RenderData",       /* char *tp_name; */
	sizeof( V24_BPy_RenderData ),   /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,						/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) V24_RenderData_repr,     /* reprfunc tp_repr; */

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
	NULL,                       /* getiterfunc tp_iter; */
	NULL,                       /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	V24_BPy_RenderData_methods,     /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_RenderData_getseters,   /* struct PyGetSetDef *tp_getset; */
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

/***************************************************************************/
/* Render method def                                                       */
/***************************************************************************/
struct PyMethodDef M_Render_methods[] = {
	{"CloseRenderWindow", ( PyCFunction ) V24_M_Render_CloseRenderWindow,
	 METH_NOARGS,
	 "() - close the rendering window"},
	{"EnableDispView", ( PyCFunction ) V24_M_Render_EnableDispView,
	 METH_NOARGS,
	 "(bool) - enable Sceneing in view"},
	{"EnableDispWin", ( PyCFunction ) V24_M_Render_EnableDispWin, METH_NOARGS,
	 "(bool) - enable Sceneing in new window"},
	{"SetRenderWinPos", ( PyCFunction ) V24_M_Render_SetRenderWinPos,
	 METH_VARARGS,
	 "([string list]) - position the rendering window in around the edge of the screen"},
	{NULL, NULL, 0, NULL}
};

static PyObject *V24_M_Render_ModesDict( void )
{
	PyObject *M = V24_PyConstant_New(  );

	if( M ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) M;
		V24_PyConstant_Insert( d, "OSA", PyInt_FromLong( R_OSA ) );
		V24_PyConstant_Insert( d, "SHADOW", PyInt_FromLong( R_SHADOW ) );
		V24_PyConstant_Insert( d, "GAMMA", PyInt_FromLong( R_GAMMA ) );
		V24_PyConstant_Insert( d, "ENVMAP", PyInt_FromLong( R_ENVMAP ) );
		V24_PyConstant_Insert( d, "TOONSHADING", PyInt_FromLong( R_EDGE ) );
		V24_PyConstant_Insert( d, "FIELDRENDER", PyInt_FromLong( R_FIELDS ) );
		V24_PyConstant_Insert( d, "FIELDTIME", PyInt_FromLong( R_FIELDSTILL ) );
		V24_PyConstant_Insert( d, "RADIOSITY", PyInt_FromLong( R_RADIO ) );
		V24_PyConstant_Insert( d, "BORDER_RENDER", PyInt_FromLong( R_BORDER ) );
		V24_PyConstant_Insert( d, "PANORAMA", PyInt_FromLong( R_PANORAMA ) );
		V24_PyConstant_Insert( d, "CROP", PyInt_FromLong( R_CROP ) );
		V24_PyConstant_Insert( d, "ODDFIELD", PyInt_FromLong( R_ODDFIELD ) );
		V24_PyConstant_Insert( d, "MBLUR", PyInt_FromLong( R_MBLUR ) );
		V24_PyConstant_Insert( d, "RAYTRACING", PyInt_FromLong( R_RAYTRACE ) );
		V24_PyConstant_Insert( d, "THREADS", PyInt_FromLong( R_THREADS ) );
	}
	return M;
}

static PyObject *V24_M_Render_SceModesDict( void )
{
	PyObject *M = V24_PyConstant_New(  );

	if( M ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) M;
		V24_PyConstant_Insert( d, "SEQUENCER", PyInt_FromLong( R_DOSEQ ) );
		V24_PyConstant_Insert( d, "EXTENSION", PyInt_FromLong( R_EXTENSION ) );
	}
	return M;
}

static PyObject *V24_M_Render_GameFramingDict( void )
{
	PyObject *M = V24_PyConstant_New(  );

	if( M ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) M;
		V24_PyConstant_Insert( d, "BARS",
				PyInt_FromLong( SCE_GAMEFRAMING_BARS ) );
		V24_PyConstant_Insert( d, "EXTEND",
				PyInt_FromLong( SCE_GAMEFRAMING_EXTEND ) );
		V24_PyConstant_Insert( d, "SCALE",
				PyInt_FromLong( SCE_GAMEFRAMING_SCALE ) );
	}
	return M;
}

/***************************************************************************/
/* Render Module Init                                                      */
/***************************************************************************/
PyObject *V24_Render_Init( void )
{
	PyObject *submodule;
	PyObject *ModesDict = V24_M_Render_ModesDict( );
	PyObject *SceModesDict = V24_M_Render_SceModesDict( );
	PyObject *GFramingDict = V24_M_Render_GameFramingDict( );

	if( PyType_Ready( &V24_RenderData_Type ) < 0 )
		return NULL;

	submodule = Py_InitModule3( "Blender.Scene.Render",
				    M_Render_methods, V24_M_Render_doc );

	if( ModesDict )
		PyModule_AddObject( submodule, "Modes", ModesDict );
	if( SceModesDict )
		PyModule_AddObject( submodule, "SceModes", SceModesDict );
	if( GFramingDict )
		PyModule_AddObject( submodule, "FramingModes", GFramingDict );

	/* ugh: why aren't these in a constant dict? */

	PyModule_AddIntConstant( submodule, "INTERNAL", R_INTERN );
	PyModule_AddIntConstant( submodule, "YAFRAY", R_YAFRAY );
	PyModule_AddIntConstant( submodule, "AVIRAW", R_AVIRAW );
	PyModule_AddIntConstant( submodule, "AVIJPEG", R_AVIJPEG );
	PyModule_AddIntConstant( submodule, "AVICODEC", R_AVICODEC );
	PyModule_AddIntConstant( submodule, "QUICKTIME", R_QUICKTIME );
	PyModule_AddIntConstant( submodule, "TARGA", R_TARGA );
	PyModule_AddIntConstant( submodule, "RAWTGA", R_RAWTGA );
	PyModule_AddIntConstant( submodule, "HDR", R_RADHDR );
	PyModule_AddIntConstant( submodule, "PNG", R_PNG );
	PyModule_AddIntConstant( submodule, "BMP", R_BMP );
	PyModule_AddIntConstant( submodule, "JPEG", R_JPEG90 );
	PyModule_AddIntConstant( submodule, "HAMX", R_HAMX );
	PyModule_AddIntConstant( submodule, "IRIS", R_IRIS );
	PyModule_AddIntConstant( submodule, "IRISZ", R_IRIZ );
	PyModule_AddIntConstant( submodule, "FTYPE", R_FTYPE );
	PyModule_AddIntConstant( submodule, "PAL", B_PR_PAL );
	PyModule_AddIntConstant( submodule, "NTSC", B_PR_NTSC );
	PyModule_AddIntConstant( submodule, "DEFAULT", B_PR_PRESET );
	PyModule_AddIntConstant( submodule, "PREVIEW", B_PR_PRV );
	PyModule_AddIntConstant( submodule, "PC", B_PR_PC );
	PyModule_AddIntConstant( submodule, "PAL169", B_PR_PAL169 );
	PyModule_AddIntConstant( submodule, "PANO", B_PR_PANO );
	PyModule_AddIntConstant( submodule, "FULL", B_PR_FULL );
	PyModule_AddIntConstant( submodule, "NONE", PY_NONE );
	PyModule_AddIntConstant( submodule, "LOW", PY_LOW );
	PyModule_AddIntConstant( submodule, "MEDIUM", PY_MEDIUM );
	PyModule_AddIntConstant( submodule, "HIGH", PY_HIGH );
	PyModule_AddIntConstant( submodule, "HIGHER", PY_HIGHER );
	PyModule_AddIntConstant( submodule, "BEST", PY_BEST );
	PyModule_AddIntConstant( submodule, "USEAOSETTINGS", PY_USEAOSETTINGS );
	PyModule_AddIntConstant( submodule, "SKYDOME", PY_SKYDOME );
	PyModule_AddIntConstant( submodule, "GIFULL", PY_FULL );
	PyModule_AddIntConstant( submodule, "OPENEXR", R_OPENEXR );
	PyModule_AddIntConstant( submodule, "TIFF", R_TIFF );
	PyModule_AddIntConstant( submodule, "FFMPEG", R_FFMPEG );
	PyModule_AddIntConstant( submodule, "CINEON", R_CINEON );
	PyModule_AddIntConstant( submodule, "DPX", R_DPX );

	return ( submodule );
}

/***************************************************************************/
/* V24_BPy_RenderData Callbacks                                                */
/***************************************************************************/

PyObject *V24_RenderData_CreatePyObject( struct Scene * scene )
{
	V24_BPy_RenderData *py_renderdata;

	py_renderdata =
		( V24_BPy_RenderData * ) PyObject_NEW( V24_BPy_RenderData,
						   &V24_RenderData_Type );

	if( py_renderdata == NULL ) {
		return ( NULL );
	}
	py_renderdata->renderContext = &scene->r;
	py_renderdata->scene = scene;

	return ( ( PyObject * ) py_renderdata );
}

/* #####DEPRECATED###### */

static PyObject *V24_RenderData_SetRenderPath( V24_BPy_RenderData *self,
		PyObject *args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setRenderPath );
}

static PyObject *V24_RenderData_SetBackbufPath( V24_BPy_RenderData *self,
		PyObject *args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setBackbufPath );
}

static PyObject *V24_RenderData_SetFtypePath( V24_BPy_RenderData *self,
		PyObject *args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_RenderData_setFtypePath );
}

static PyObject *V24_RenderData_SetOversamplingLevel( V24_BPy_RenderData * self,
		PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setOSALevel );
}

static PyObject *V24_RenderData_SetRenderWinSize( V24_BPy_RenderData * self,
		PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setRenderWinSize );
}

static PyObject *V24_RenderData_SetBorder( V24_BPy_RenderData * self,
		PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_RenderData_setBorder );
}

static PyObject *V24_RenderData_SetRenderer( V24_BPy_RenderData * self,
		PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setRenderer );
}

static PyObject *V24_RenderData_SetImageType( V24_BPy_RenderData * self,
		PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_RenderData_setImageType );
}
