/* 
 * $Id: sceneRender.c 12814 2007-12-07 10:23:00Z campbellbarton $
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
#include "BKE_ipo.h"

#include "BIF_drawscene.h"
#include "BIF_renderwin.h"
#include "BIF_writeimage.h"

#include "BLI_blenlib.h"

#include "RE_pipeline.h"

#include "mydevice.h"
#include "butspace.h"
#include "blendef.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "Const.h"
#include "color.h"
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
	EXPP_RENDER_ATTR_EDGEINT,
	EXPP_RENDER_ATTR_THREADS,
	EXPP_RENDER_ATTR_QUALITY,
	EXPP_RENDER_ATTR_OLDMAPVAL,
	EXPP_RENDER_ATTR_NEWMAPVAL,
	EXPP_RENDER_ATTR_YF_RAYDEPTH,
	EXPP_RENDER_ATTR_YF_NUMPROCS,
	EXPP_RENDER_ATTR_YF_AAPASSES,
	EXPP_RENDER_ATTR_YF_AASAMPLES,

	
	EXPP_RENDER_ATTR_YF_GICACHE,
	EXPP_RENDER_ATTR_YF_GIDEPTH,
	EXPP_RENDER_ATTR_YF_GICDEPTH,
	EXPP_RENDER_ATTR_YF_GIPHOTONS,
	EXPP_RENDER_ATTR_YF_GIDIRECT,
	EXPP_RENDER_ATTR_YF_EXPORTXML,
	EXPP_RENDER_ATTR_YF_AA,
	EXPP_RENDER_ATTR_YF_CLAMPRGB,
	EXPP_RENDER_ATTR_YF_NOBUMP,
	EXPP_RENDER_ATTR_YF_PXPERSAMPLE,
	EXPP_RENDER_ATTR_YF_GIPHOTONCOUNT,
	EXPP_RENDER_ATTR_YF_GIMIXPHOTONS,
	
	EXPP_RENDER_ATTR_GAUSSFILTER,
	EXPP_RENDER_ATTR_MBLURFACTOR,
	EXPP_RENDER_ATTR_CTIME,
	EXPP_RENDER_ATTR_YF_GIPOWER,
	EXPP_RENDER_ATTR_YF_GIINDIRPOWER,
	EXPP_RENDER_ATTR_YF_GIPHOTONRADIUS,
	EXPP_RENDER_ATTR_YF_GISHADOWQUALITY,
	EXPP_RENDER_ATTR_YF_GIREFINEMENT,
	EXPP_RENDER_ATTR_YF_RAYBIAS,
	EXPP_RENDER_ATTR_YF_GAMMA,
	EXPP_RENDER_ATTR_YF_EXPOSURE,
	EXPP_RENDER_ATTR_YF_AAPXSIZE,
	EXPP_RENDER_ATTR_YF_AATHRESH,
	EXPP_RENDER_ATTR_OSA,		/* get only */
	EXPP_RENDER_ATTR_PLANES,	/* get only */
	EXPP_RENDER_ATTR_WINSIZE,	/* get only */
};

enum rend_const_constants {
	EXPP_RENDER_ATTR_FORMAT = 0,
	EXPP_RENDER_ATTR_PRESET,
	EXPP_RENDER_ATTR_GITYPE,
	EXPP_RENDER_ATTR_GIQUALITY,
	EXPP_RENDER_ATTR_WINPOS,
	EXPP_RENDER_ATTR_GAMEFRAME,
	EXPP_RENDER_ATTR_RENDERER,
	EXPP_RENDER_ATTR_ALPHA,
	EXPP_RENDER_ATTR_DISPLAY,
};

/* checks for the scene being removed */
#define REND_DEL_CHECK_PY(self) if (!(self->bpysce->scene)) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "Scene has been removed" ) )
#define REND_DEL_CHECK_INT(self) if (!(self->bpysce->scene)) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "Scene has been removed" ) )

static constIdents formatTypesIdents[] = {
	{"TARGA",			{(int)R_TARGA}},	
	{"IRIS",			{(int)R_IRIS}},
	{"HAMX",			{(int)R_HAMX}},
	{"FTYPE",			{(int)R_FTYPE}},
	{"JPEG",			{(int)R_JPEG90}},
	{"MOVIE",			{(int)R_MOVIE}},
	{"IRISZ",			{(int)R_IRIZ}},
	{"RAWTGA",			{(int)R_RAWTGA}},
	{"AVIRAW",			{(int)R_AVIRAW}},
	{"AVIJPEG",			{(int)R_AVIJPEG}},
	{"PNG",				{(int)R_PNG}},
	{"AVICODEC",		{(int)R_AVICODEC}},
	{"QUICKTIME",		{(int)R_QUICKTIME}},
	{"BMP",				{(int)R_BMP}},
	{"HDR",				{(int)R_RADHDR}},
	{"TIFF",			{(int)R_TIFF}},
	{"OPENEXR",			{(int)R_OPENEXR}},
	{"FFMPEG",			{(int)R_FFMPEG}},
	{"FRAMESERVER",		{(int)R_FRAMESERVER}},
	{"CINEON",			{(int)R_CINEON}},
	{"DPX",				{(int)R_DPX}},
	{"MULTILAYER",		{(int)R_MULTILAYER}},
	{"DDS",				{(int)R_DDS}},
};
static constDefinition formatTypes = {
	EXPP_CONST_INT, "Formats",
		sizeof(formatTypesIdents)/sizeof(constIdents), formatTypesIdents
};

static constIdents sizePresetsIdents[] = {
	{"PAL",				{(int)B_PR_PAL}},
	{"NTSC",			{(int)B_PR_NTSC}},
	{"DEFAULT",			{(int)B_PR_PRESET}},
	{"PREVIEW",			{(int)B_PR_PRV}},
	{"PC",				{(int)B_PR_PC}},
	{"PAL169",			{(int)B_PR_PAL169}},
	{"PANO",			{(int)B_PR_PANO}},
	{"FULL",			{(int)B_PR_FULL}},
};
static constDefinition sizePresets = {
	EXPP_CONST_INT, "SizePresets",
		sizeof(sizePresetsIdents)/sizeof(constIdents), sizePresetsIdents
};

static constIdents yafrayGITypesIdents[] = {
	{"NONE",			{(int)PY_NONE}},
	{"SKYDOME",			{(int)PY_SKYDOME}},
	{"FULL",			{(int)PY_FULL}},
};
static constDefinition yafrayGITypes = {
	EXPP_CONST_INT, "YafratGITypes",
		sizeof(yafrayGITypesIdents)/sizeof(constIdents), yafrayGITypesIdents
};

static constIdents yafrayGIQualityIdents[] = {
	{"NONE",			{(int)PY_NONE}},
	{"LOW",				{(int)PY_LOW}},
	{"MEDIUM",			{(int)PY_MEDIUM}},
	{"HIGH",			{(int)PY_HIGH}},
	{"HIGHER",			{(int)PY_HIGHER}},
	{"BEST",			{(int)PY_BEST}},
	{"USEAOSETTINGS",	{(int)PY_USEAOSETTINGS}},
};
static constDefinition yafrayGIQuality = {
	EXPP_CONST_INT, "YafrayGIQuality",
		sizeof(yafrayGIQualityIdents)/sizeof(constIdents), yafrayGIQualityIdents
};

static constIdents gameFramingIdents[] = {
	{"SCALE",			{(int)SCE_GAMEFRAMING_SCALE}},
	{"EXTEND",			{(int)SCE_GAMEFRAMING_EXTEND}},
	{"BARS",			{(int)SCE_GAMEFRAMING_BARS}},
};
static constDefinition gameFraming = {
	EXPP_CONST_INT, "GameFraming",
		sizeof(gameFramingIdents)/sizeof(constIdents), gameFramingIdents
};

static constIdents renderTypesIdents[] = {
	{"INTERNAL",		{(int)R_INTERN}},
	{"YAFRAY",			{(int)R_YAFRAY}},
};
static constDefinition renderTypes = {
	EXPP_CONST_INT, "Renderers",
		sizeof(renderTypesIdents)/sizeof(constIdents), renderTypesIdents
};

static constIdents alphaTypesIdents[] = {
	{"SKY",				{(int)R_ADDSKY}},
	{"PREMULTIPLY",		{(int)R_ALPHAPREMUL}},
	{"KEY",				{(int)R_ALPHAKEY}},
};
static constDefinition alphaTypes = {
	EXPP_CONST_INT, "AlphaTypes",
		sizeof(alphaTypesIdents)/sizeof(constIdents), alphaTypesIdents
};

static constIdents displayTypesIdents[] = {
	{"IMAGE",			{(int)R_DISPLAYIMAGE}},
	{"WIN",				{(int)R_DISPLAYWIN}},
	{"SCREEN",			{(int)R_DISPLAYSCREEN}},
};
static constDefinition displayTypes = {
	EXPP_CONST_INT, "displayTypes",
		sizeof(displayTypesIdents)/sizeof(constIdents), displayTypesIdents
};

static constIdents winPosIdents[] = {
	{"SW",				{(int)1}},
	{"S",				{(int)2}},
	{"SE",				{(int)4}},
	{"W",				{(int)8}},
	{"C",				{(int)16}},
	{"E",				{(int)32}},
	{"NW",				{(int)64}},
	{"N",				{(int)128}},
	{"NE",				{(int)256}},
};
static constDefinition winPos = {
	EXPP_CONST_INT, "WindowPos",
		sizeof(winPosIdents)/sizeof(constIdents), winPosIdents
};


RenderData * RenderData_get_pointer( BPyRenderObject * self )
{
	if (self->bpysce && self->bpysce->scene )
		return &self->bpysce->scene->r;
	
	return NULL;
}

/* deprecated callbacks */
static PyObject *RenderData_Render( BPyRenderObject * self );

/* BPyRenderObject Internal Protocols */

static PyObject *RenderData_repr( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	if( r )
		return PyString_FromFormat( "[RenderData \"%s\"]",
					    self->bpysce->scene->id.name + 2 );
	else
		return PyString_FromString( "[RenderData <deleted>]" );
}

static void RenderData_dealloc( BPyRenderObject * self )
{
	Py_DECREF( self->bpysce );
	PyObject_DEL( self );
}


static void Render_DoSizePreset( RenderData * r, short xsch,
				   short ysch, short xasp, short yasp,
				   short size, short xparts, short yparts,
				   short fps, float fps_base,
				   float a, float b, float c,
				   float d )
{
	r->xsch = xsch;
	r->ysch = ysch;
	r->xasp = xasp;
	r->yasp = yasp;
	r->size = size;
	r->frs_sec = fps;
	r->frs_sec_base = fps_base;
	r->xparts = xparts;
	r->yparts = yparts;

	BLI_init_rctf( &r->safety, a, b, c, d );
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	EXPP_allqueue( REDRAWVIEWCAM, 0 );
}

/***************************************************************************/
/* Render Module Function Definitions                                      */
/***************************************************************************/

PyObject * Render_getRenderWinPos( BPyRenderObject * self )
{
	int x = 1;
	PyObject * pyls;
	PyObject * pyob;
	
	REND_DEL_CHECK_PY(self);
	
	pyls = PyList_New(0);
	
	while (x <= 256) {
		/* we know this wont fail */
		pyob = PyConst_NewInt( &winPos, x );
		PyList_Append(pyls, pyob);
		Py_DECREF(pyob);
		x*=2;
	}
	return pyls;
}


int Render_setRenderWinPos( BPyRenderObject * self, PyObject * value )
{
	int x, origpos = G.winpos;
	constValue *c;
	
	REND_DEL_CHECK_INT(self);
	
	if( !PyList_Check(value) )
		return ( EXPP_ReturnIntError( PyExc_AttributeError,
						"expected a list" ) );
	
	G.winpos = 0;
	for( x = 0; x < PyList_Size( value ); x++ ) {
		c = Const_FromPyObject( &winPos, PyList_GetItem( value, x ));
		if( !c ) {
			G.winpos = origpos;
			return EXPP_ReturnIntError( PyExc_TypeError,
						      "python list not parseable" );
		}
		
		G.winpos |= c->i;
	}
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

/***************************************************************************/
/* BPyRenderObject Function Definitions                                     */
/***************************************************************************/

PyObject *RenderData_Render( BPyRenderObject * self )
{
	Scene *oldsce;
	
	REND_DEL_CHECK_PY(self);
	
	if (!G.background) {
		oldsce = G.scene;
		set_scene( self->bpysce->scene );
		BIF_do_render( 0 );
		set_scene( oldsce );
	}

	else { /* background mode (blender -b file.blend -P script) */
		Render *re= RE_NewRender("Render");

		int end_frame = G.scene->r.efra; /* is of type short currently */

		if (G.scene != self->bpysce->scene)
			return EXPP_ReturnPyObjError (PyExc_RuntimeError,
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
PyObject *RenderData_SaveRenderedImage ( BPyRenderObject * self, PyObject *args )
{
	RenderData *r = RenderData_get_pointer( self );
	char dir[FILE_MAXDIR * 2], str[FILE_MAXFILE * 2];
	char *name_str, filepath[FILE_MAXDIR+FILE_MAXFILE];
	RenderResult *rr = NULL;
	int zbuff;
	
	REND_DEL_CHECK_PY(self);
	
	if( !PyArg_ParseTuple( args, "s|i", &name_str, &zbuff ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a filename (string) and optional int" );

	if( strlen(r->pic) + strlen(name_str)
			>= sizeof(filepath) )
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"full filename too long" );

	if (zbuff !=0	) zbuff = 1; /*required 1/0 */ /* removed! (ton) */

	BLI_strncpy( filepath, r->pic, sizeof(filepath) );
	strcat(filepath, name_str);

	rr = RE_GetResult(RE_GetRender(G.scene->id.name));
	if(!rr) {
		return EXPP_ReturnPyObjError (PyExc_ValueError, "No image rendered");
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

PyObject *RenderData_RenderAnim( BPyRenderObject * self )
{
	Scene *oldsce;

	REND_DEL_CHECK_PY(self);
	
	if (!G.background) {
		oldsce = G.scene;
		set_scene( self->bpysce->scene );
		BIF_do_render( 1 );
		set_scene( oldsce );
	}
	else { /* background mode (blender -b file.blend -P script) */
		Render *re= RE_NewRender("Render");
		
		if (G.scene != self->bpysce->scene)
			return EXPP_ReturnPyObjError (PyExc_RuntimeError,
				"scene to render in bg mode must be the active scene");

		if (G.scene->r.sfra > G.scene->r.efra)
			return EXPP_ReturnPyObjError (PyExc_RuntimeError,
				"start frame must be less or equal to end frame");
		
		RE_BlenderAnim(re, G.scene, G.scene->r.sfra, G.scene->r.efra);
	}
	Py_RETURN_NONE;
}

PyObject *RenderData_Play( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	char file[FILE_MAXDIR + FILE_MAXFILE];
	extern char bprogname[];
	char str[FILE_MAXDIR + FILE_MAXFILE];
	int pos[2], size[2];
	char txt[64];
	
	REND_DEL_CHECK_PY(self);

#ifdef WITH_QUICKTIME
	if( r->imtype == R_QUICKTIME ) {

		strcpy( file, r->pic );
		BLI_convertstringcode( file, (char *) self->bpysce->scene,
				       r->cfra );
		BLI_make_existing_file( file );
		if( BLI_strcasecmp( file + strlen( file ) - 4, ".mov" ) ) {
			sprintf( txt, "%04d_%04d.mov",
				 ( r->sfra ),
				 ( r->efra ) );
			strcat( file, txt );
		}
	} else
#endif
	{

		strcpy( file, r->pic );
		BLI_convertstringcode( file, G.sce,
				       r->cfra );
		BLI_make_existing_file( file );
		if( BLI_strcasecmp( file + strlen( file ) - 4, ".avi" ) ) {
			sprintf( txt, "%04d_%04d.avi",
				 ( r->sfra ),
				 ( r->efra ) );
			strcat( file, txt );
		}
	}
	if( BLI_exist( file ) ) {
		calc_renderwin_rectangle(640, 480, G.winpos, pos, size);
		sprintf( str, "%s -a -p %d %d \"%s\"", bprogname, pos[0],
			 pos[1], file );
		system( str );
	} else {
		BKE_makepicstring( file, G.scene->r.pic, r->sfra, G.scene->r.imtype);
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

static int RenderData_setOSALevel( BPyRenderObject * self, PyObject * value )
{
	RenderData *r = RenderData_get_pointer( self );
	int level = PyInt_AsLong( value );
	
	REND_DEL_CHECK_INT(self);
	
	if( level == -1 && PyErr_Occurred() )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	if( level != 5 && level != 8 && level != 11 && level != 16 )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"expected 5, 8, 11, or 16" );

	r->osa = (short)level;
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

static int RenderData_setBorder( BPyRenderObject * self, PyObject * value )
{
	RenderData *r = RenderData_get_pointer( self );
	int ret;
	
	REND_DEL_CHECK_INT(self);
	
	ret = EXPP_setVec4(value, &r->border.xmin, &r->border.xmax, &r->border.ymin, &r->border.ymax);
	
	if (ret == -1)
		return -1;
	
	CLAMP(r->border.xmin, 0.0, 1.0);
	CLAMP(r->border.xmax, 0.0, 1.0);
	CLAMP(r->border.ymin, 0.0, 1.0);
	CLAMP(r->border.ymax, 0.0, 1.0);

	EXPP_allqueue( REDRAWVIEWCAM, 1 );
	return 0;
}

static PyObject *RenderData_getBorder( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	
	return Py_BuildValue( "[ffff]", 
			r->border.xmin,
			r->border.ymin,
			r->border.xmax,
			r->border.ymax );
}

void RenderData_SizePreset( RenderData * r, int type )
{
	/* Checking done */
	if( type == B_PR_PAL ) {
		Render_DoSizePreset( r, 720, 576, 54, 51, 100,
				       r->xparts,
				       r->yparts, 25, 1.0f,
				       0.1f,
				       0.9f, 0.1f, 0.9f );
		r->mode &= ~R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_NTSC ) {
		Render_DoSizePreset( r, 720, 480, 10, 11, 100, 1, 1,
				       30, 1.001f, 
				       0.1f, 0.9f, 0.1f, 0.9f );
		r->mode &= ~R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PRESET ) {
		Render_DoSizePreset( r, 720, 576, 54, 51, 100, 1, 1,
				       r->frs_sec, 
				       r->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		r->mode = R_OSA + R_SHADOW + R_FIELDS;
		r->imtype = R_TARGA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PRV ) {
		Render_DoSizePreset( r, 640, 512, 1, 1, 50, 1, 1,
				       r->frs_sec, 
				       r->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		r->mode &= ~R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PC ) {
		Render_DoSizePreset( r, 640, 480, 100, 100, 100, 1, 1,
				       r->frs_sec, 
				       r->frs_sec_base, 
				       0.0f, 1.0f,
				       0.0f, 1.0f );
		r->mode &= ~R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.0f, 1.0f, 0.0f,
			       1.0f );
	} else if( type == B_PR_PAL169 ) {
		Render_DoSizePreset( r, 720, 576, 64, 45, 100, 1, 1,
				       25, 1.0f, 0.1f, 0.9f, 0.1f, 0.9f );
		r->mode &= ~R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_PANO ) {
		Render_DoSizePreset( r, 36, 176, 115, 100, 100, 16, 1,
				       r->frs_sec, 
				       r->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		r->mode |= R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	} else if( type == B_PR_FULL ) {
		Render_DoSizePreset( r, 1280, 1024, 1, 1, 100, 1, 1,
				       r->frs_sec, 
				       r->frs_sec_base, 
				       0.1f, 0.9f,
				       0.1f, 0.9f );
		r->mode &= ~R_PANORAMA;
		BLI_init_rctf( &r->safety, 0.1f, 0.9f, 0.1f,
			       0.9f );
	}
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );
}

static PyObject *RenderData_getTimeCode( BPyRenderObject * self)
{
	RenderData *r = RenderData_get_pointer( self );
    char tc[12];
    int h, m, s, cfa;
    double fps;
    
    REND_DEL_CHECK_PY(self);

    fps = (double) r->frs_sec / 
	    r->frs_sec_base;
    cfa = r->cfra-1;
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

static PyObject *getFloatAttr( BPyRenderObject *self, void *type )
{
	RenderData *r = RenderData_get_pointer( self );
	float param;

	REND_DEL_CHECK_PY(self);
	
	switch( (int)type ) {
	case EXPP_RENDER_ATTR_GAUSSFILTER:
		param = r->gauss;
		break;
	case EXPP_RENDER_ATTR_MBLURFACTOR:
		param = r->blurfac;
		break;
	case EXPP_RENDER_ATTR_CTIME:
		param = frame_to_float( r->cfra );
		break;
	case EXPP_RENDER_ATTR_FPS_BASE:
		param = r->frs_sec_base;
		break;
	case EXPP_RENDER_ATTR_YF_GIPOWER:
		param = r->GIpower;
		break;
	case EXPP_RENDER_ATTR_YF_GIINDIRPOWER:
		param = r->GIindirpower;
		break;
	case EXPP_RENDER_ATTR_YF_GIPHOTONRADIUS:
		param = r->GIphotonradius; 
		break;
	case EXPP_RENDER_ATTR_YF_GISHADOWQUALITY:
		param = r->GIshadowquality; 
		break;
	case EXPP_RENDER_ATTR_YF_GIREFINEMENT:
		param = r->GIrefinement; 
		break;
	case EXPP_RENDER_ATTR_YF_RAYBIAS:
		param = r->YF_raybias; 
		break;
	case EXPP_RENDER_ATTR_YF_GAMMA:
		param = r->YF_gamma; 
		break;
	case EXPP_RENDER_ATTR_YF_EXPOSURE:
		param = r->YF_exposure; 
		break;
	case EXPP_RENDER_ATTR_YF_AAPXSIZE:
		param = r->YF_AApixelsize; 
		break;
	case EXPP_RENDER_ATTR_YF_AATHRESH:
		param = r->YF_AAthreshold; 
		break;		
	
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type constant in getFloatAttr" );
	}
	return PyFloat_FromDouble( param );
}

/*
 * set floating point attributes which require clamping
 */

static int setFloatAttrClamp( BPyRenderObject *self, PyObject *value, void *type )
{
	RenderData *r = RenderData_get_pointer( self );
	float *param;
	float min, max;

	REND_DEL_CHECK_INT(self);
	
	switch( (int)type ) {
	case EXPP_RENDER_ATTR_GAUSSFILTER:
		min = 0.5f;		max = 1.5f;
		param = &r->gauss;
		break;
	case EXPP_RENDER_ATTR_MBLURFACTOR:
	    min = 0.01f;	max = 5.0f;
		param = &r->blurfac;
		break;
	case EXPP_RENDER_ATTR_FPS_BASE:
		min = 1.0f;
		max = 120.0f;
		param = &r->frs_sec_base;
		break;
	/* TODO - add this setting */
	/*case EXPP_RENDER_ATTR_CTIME:	
		int to= (int) floor(0.5 + frame/G.scene->r.framelen );*/
	case EXPP_RENDER_ATTR_YF_GIPOWER:
	    min = 0.01f;	max = 100.0f;
		param = &r->GIpower;
		break;
	case EXPP_RENDER_ATTR_YF_GIINDIRPOWER:
	    min = 0.01f;	max = 100.0f;
		param = &r->GIindirpower;
		break;
	case EXPP_RENDER_ATTR_YF_GIPHOTONRADIUS:
	    min = 0.0001f;	max = 100.0f;
		param = &r->GIphotonradius;
		break;
	case EXPP_RENDER_ATTR_YF_GISHADOWQUALITY:
	    min = 0.01f;	max = 1.0f;
		param = &r->GIshadowquality;
		break;
	case EXPP_RENDER_ATTR_YF_GIREFINEMENT:
	    min = 0.001f;	max = 1.0f;
		param = &r->GIrefinement;
		break;
	case EXPP_RENDER_ATTR_YF_RAYBIAS:
	    min = 0.0f;		max = 10.0f;
		param = &r->YF_raybias;
		break;
	case EXPP_RENDER_ATTR_YF_GAMMA:
	    min = 0.001f;	max = 5.0f;
		param = &r->YF_gamma;
		break;
	case EXPP_RENDER_ATTR_YF_EXPOSURE:
	    min = 0.0f;		max = 10.0f;
		param = &r->YF_exposure;
		break;
	case EXPP_RENDER_ATTR_YF_AAPXSIZE:
	    min = 1.0f;		max = 2.0f;
		param = &r->YF_AApixelsize;
		break;
	case EXPP_RENDER_ATTR_YF_AATHRESH:
	    min = 0.05f;	max = 1.0f;
		param = &r->YF_AAthreshold;
		break;		
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type constant in setFloatAttrClamp" );
	}
	return EXPP_setFloatClamped( value, param, min, max );
}

/*
 * get integer attributes
 */

static PyObject *getIntAttr( BPyRenderObject *self, void *type )
{
	RenderData *r = RenderData_get_pointer( self );
	long param;

	REND_DEL_CHECK_PY(self);
	
	switch( (int)type ) {
	case EXPP_RENDER_ATTR_XPARTS:
		param = (long)r->xparts;
		break;
	case EXPP_RENDER_ATTR_YPARTS:
		param = (long)r->yparts;
		break;
	case EXPP_RENDER_ATTR_ASPECTX:
		param = (long)r->xasp;
		break;
	case EXPP_RENDER_ATTR_ASPECTY:
		param = (long)r->yasp;
		break;
	case EXPP_RENDER_ATTR_CFRAME:
		param = (long)r->cfra;
		break;
	case EXPP_RENDER_ATTR_EFRAME:
		param = (long)r->efra;
		break;
	case EXPP_RENDER_ATTR_SFRAME:
		param = (long)r->sfra;
		break;
	case EXPP_RENDER_ATTR_FPS:
		param = r->frs_sec;
		break;
	case EXPP_RENDER_ATTR_SIZEX:
		param = r->xsch;
		break;
	case EXPP_RENDER_ATTR_SIZEY:
		param = r->ysch;
		break;
	case EXPP_RENDER_ATTR_EDGEINT:
		param = (long)r->edgeint;
		break;
	case EXPP_RENDER_ATTR_THREADS:
		param = (long)r->threads;
		break;
	case EXPP_RENDER_ATTR_QUALITY:
		param = (long)r->quality;
		break;
	case EXPP_RENDER_ATTR_OLDMAPVAL:
		param = (long)r->framapto;
		break;
	case EXPP_RENDER_ATTR_NEWMAPVAL:
		param = (long)r->images;
		break;
	case EXPP_RENDER_ATTR_YF_RAYDEPTH:
		param = (long)r->YF_raydepth;
		break;
	case EXPP_RENDER_ATTR_YF_GICACHE:
		param = (long)r->GIcache;
		break;
	case EXPP_RENDER_ATTR_YF_GIDEPTH:
		param = (long)r->GIdepth;
		break;
	case EXPP_RENDER_ATTR_YF_GICDEPTH:
		param = (long)r->GIcausdepth;
		break;
	case EXPP_RENDER_ATTR_YF_GIPHOTONS:
		param = (long)r->GIphotons;
		break;
	case EXPP_RENDER_ATTR_YF_GIDIRECT:
		param = (long)r->GIdirect;
		break;
	case EXPP_RENDER_ATTR_YF_EXPORTXML:
		param = (long)r->YFexportxml;
		break;
	case EXPP_RENDER_ATTR_YF_AA:
		param = (long)r->YF_AA;
		break;
	case EXPP_RENDER_ATTR_YF_CLAMPRGB:
		param = (long)r->YF_clamprgb;
		break;
	case EXPP_RENDER_ATTR_YF_NOBUMP:
		param = (long)r->YF_nobump;
		break;
	case EXPP_RENDER_ATTR_YF_NUMPROCS:
		param = (long)r->YF_numprocs;
		break;
	case EXPP_RENDER_ATTR_YF_AAPASSES:
		param = (long)r->YF_AApasses;
		break;		
	case EXPP_RENDER_ATTR_YF_AASAMPLES:
		param = (long)r->YF_AAsamples;
		break;
	case EXPP_RENDER_ATTR_YF_PXPERSAMPLE:
		param = (long)r->GIpixelspersample;
		break;
	case EXPP_RENDER_ATTR_YF_GIPHOTONCOUNT:
		param = (long)r->GIphotoncount;
		break;
	case EXPP_RENDER_ATTR_YF_GIMIXPHOTONS:
		param = (long)r->GImixphotons;
		break;
	case EXPP_RENDER_ATTR_OSA:
		param = (long)r->osa;
		break;
	case EXPP_RENDER_ATTR_PLANES:
		param = (long)r->planes;
		break;
	case EXPP_RENDER_ATTR_WINSIZE:
		param = (long)r->size;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type constant in getIntAttrClamp" );
	}
	return PyInt_FromLong( param );
}

/*
 * set integer attributes which require clamping
 */
static int setIntAttrClamp( BPyRenderObject *self, PyObject *value, void *type )
{
	RenderData *r = RenderData_get_pointer( self );	
	void *param;
	int min, max, size, ret;

	REND_DEL_CHECK_INT(self);
	
	switch( (int)type ) {
	case EXPP_RENDER_ATTR_XPARTS:
		min = 1;		max = 512;		size = 'h';
		param = &r->xparts;
		break;
	case EXPP_RENDER_ATTR_YPARTS:
		min = 1;		max = 64;		size = 'h';
		param = &r->yparts;
		break;
	case EXPP_RENDER_ATTR_ASPECTX:
		min = 1;		max = 200;	   	size = 'h';
		param = &r->xasp;
		break;
	case EXPP_RENDER_ATTR_ASPECTY:
		min = 1;		max = 200;	   	size = 'h';
		param = &r->yasp;
		break;
	case EXPP_RENDER_ATTR_CFRAME:
		min = 1;		max = MAXFRAME;	size = 'h';
		param = &r->cfra;
		break;
	case EXPP_RENDER_ATTR_EFRAME:
		min = 1;		max = MAXFRAME;	size = 'h';
		param = &r->efra;
		break;
	case EXPP_RENDER_ATTR_SFRAME:
		min = 1;	    max = MAXFRAME;	size = 'h';
		param = &r->sfra;
		break;
	case EXPP_RENDER_ATTR_FPS:
		min = 1;		max = 120;		size = 'h';
		param = &r->frs_sec;
		break;
	case EXPP_RENDER_ATTR_SIZEX:
		min = 4;		max = 10000;	size = 'h';
		param = &r->xsch;
		break;
	case EXPP_RENDER_ATTR_SIZEY:
		min = 4;		max = 10000;	size = 'h';
		param = &r->ysch;
		break;
	case EXPP_RENDER_ATTR_EDGEINT:
		min = 0;		max = 255;		size = 'h';
		param = &r->edgeint;
		break;
	case EXPP_RENDER_ATTR_THREADS:
		min = 1;		max = 8;		size = 'h';
		param = &r->threads;
		break;
	case EXPP_RENDER_ATTR_QUALITY:
		min = 10;		max = 100;		size = 'h';
		param = &r->quality;
		break;		
	case EXPP_RENDER_ATTR_OLDMAPVAL:
		min = 1;		max = 900;		size = 'i';
		param = &r->framapto;
		break;
	case EXPP_RENDER_ATTR_NEWMAPVAL:
		min = 1;		max = 900;		size = 'i';
		param = &r->images;
		break;
	case EXPP_RENDER_ATTR_YF_RAYDEPTH:
		min = 1;		max = 80;		size = 'i';
		param = &r->YF_raydepth;
		break;
	case EXPP_RENDER_ATTR_YF_GICACHE:
		min = 0;		max = 1;		size = 'h';
		param = &r->GIcache;
		break;
	case EXPP_RENDER_ATTR_YF_GIDEPTH:
		min = 1;		max = 100;		size = 'i';
		param = &r->GIdepth;
		break;
	case EXPP_RENDER_ATTR_YF_GICDEPTH:
		min = 1;		max = 100;		size = 'i';
		param = &r->GIcausdepth;
		break;
	case EXPP_RENDER_ATTR_YF_GIPHOTONS:
		min = 0;		max = 1;		size = 'h';
		param = &r->GIphotons;
		break;
	case EXPP_RENDER_ATTR_YF_GIDIRECT:
		min = 0;		max = 1;		size = 'h';
		param = &r->GIdirect;
		break;
	case EXPP_RENDER_ATTR_YF_EXPORTXML:
		min = 0;		max = 1;		size = 'h';
		param = &r->YFexportxml;
		break;
	case EXPP_RENDER_ATTR_YF_AA:
		min = 0;		max = 1;		size = 'h';
		param = &r->YF_AA;
		break;
	case EXPP_RENDER_ATTR_YF_CLAMPRGB:
		min = 0;		max = 1;		size = 'h';
		param = &r->YF_clamprgb;
		break;
	case EXPP_RENDER_ATTR_YF_NOBUMP:
		min = 0;		max = 1;		size = 'h';
		param = &r->YF_nobump;
		break;
	case EXPP_RENDER_ATTR_YF_NUMPROCS:
		min = 1;		max = 8;		size = 'i';
		param = &r->YF_numprocs;
		break;
	case EXPP_RENDER_ATTR_YF_AAPASSES:
		min = 0;		max = 64;		size = 'i';
		param = &r->YF_AApasses;
		break;	
	case EXPP_RENDER_ATTR_YF_AASAMPLES:
		min = 0;		max = 2048;		size = 'i';
		param = &r->YF_AAsamples;
		break;
	case EXPP_RENDER_ATTR_YF_PXPERSAMPLE:
		min = 1;		max = 50;		size = 'i';
		param = &r->GIpixelspersample;
		break;
	case EXPP_RENDER_ATTR_YF_GIPHOTONCOUNT:
		min = 100000;	max = 10000000;	size = 'i';
		param = &r->GIphotoncount;
		break;
	case EXPP_RENDER_ATTR_YF_GIMIXPHOTONS:
		min = 100;		max = 1000;		size = 'i';
		param = &r->GImixphotons;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type constant in setIntAttrClamp" );
	}
	ret = EXPP_setIValueClamped( value, param, min, max, size );
	if (ret != -1) { /* no errors */ 
		if (((int)type==EXPP_RENDER_ATTR_OLDMAPVAL) || ((int)type==EXPP_RENDER_ATTR_NEWMAPVAL)) {
			r->framelen =
					(float)r->framapto / r->images;
		}
	}
	return ret;
}


/*
 * get constant attributes
 */

static PyObject *getConstAttr( BPyRenderObject *self, void *type )
{
	RenderData *r = RenderData_get_pointer( self );
	constDefinition *constType;
	constValue param;

	REND_DEL_CHECK_PY(self);
	
	switch( (int)type ) {
	case EXPP_RENDER_ATTR_FORMAT:
		param.i = r->imtype;
		constType = &formatTypes;
		break;
	/*case EXPP_RENDER_ATTR_PRESET:
		param.i = r->;
		constType = &sizePresets;
		break; // Can only SET */
	case EXPP_RENDER_ATTR_GITYPE:
		param.i = r->GImethod;
		constType = &yafrayGITypes;
		break;
	case EXPP_RENDER_ATTR_GIQUALITY:
		param.i = r->GIquality;
		constType = &yafrayGIQuality;
		break;
	case EXPP_RENDER_ATTR_GAMEFRAME:
		param.i = self->bpysce->scene->framing.type;
		constType = &gameFraming;
		break;
	case EXPP_RENDER_ATTR_RENDERER:
		param.i = r->renderer;
		constType = &renderTypes;
		break;
	case EXPP_RENDER_ATTR_ALPHA:
		param.i = r->alphamode;
		constType = &alphaTypes;
		break;
	case EXPP_RENDER_ATTR_DISPLAY:
		param.i = G.displaymode; /* WARNING - should this be in render? */
		constType = &displayTypes;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getConstAttr" );
	}
   	return PyConst_NewInt( constType, param.i );
}

/*
 * set constant attributes
 */

static int setConstAttr( BPyRenderObject *self, PyObject *value, void *type )
{
	RenderData *r = RenderData_get_pointer( self );
	void *param;
	constDefinition *constType;
	constValue *c;
	int size_preset = -1; /* used for windowsize */
	
	REND_DEL_CHECK_INT(self);
	
	/* for each constant, assign pointer to its storage and set the type
 	 * of constant class we should match */
	switch( (int)type ) {
	case EXPP_RENDER_ATTR_FORMAT:
		param = (void *)&r->imtype;
		constType = &formatTypes;
		break;
	case EXPP_RENDER_ATTR_PRESET:
		param = (void *)&size_preset;
		constType = &sizePresets;
		break; // Can only SET */
	case EXPP_RENDER_ATTR_GITYPE:
		param = (void *)&r->GImethod;
		constType = &yafrayGITypes;
		break;
	case EXPP_RENDER_ATTR_GIQUALITY:
		param = (void *)&r->GIquality;
		constType = &yafrayGIQuality;
		break;
	case EXPP_RENDER_ATTR_GAMEFRAME:
		param = (void *)&self->bpysce->scene->framing.type;
		constType = &gameFraming;
		break;
	case EXPP_RENDER_ATTR_RENDERER:
		param = (void *)&r->renderer;
		constType = &renderTypes;
		break;
	case EXPP_RENDER_ATTR_ALPHA:
		param = (void *)&r->alphamode;
		constType = &alphaTypes;
		break;
	case EXPP_RENDER_ATTR_DISPLAY:
		param = (void *)&G.displaymode; /* WARNING - should this be in render? */
		constType = &displayTypes;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in getConstAttr" );
	}
	
	/*
	 * this checks for a constant of the correct type, and if so use the
	 * value
	 */
	/* check that we have a valid constant */
	c = Const_FromPyObject( constType, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected  constant type/string not found" );
	
	*(int *)param = c->i;  
	
	if (size_preset != -1)
		RenderData_SizePreset(r, size_preset);
	
	return 0;
}


/***************************************************************************/
/* handlers for other getting/setting attributes                           */
/***************************************************************************/

static PyObject *RenderData_getModeBit(BPyRenderObject *self, void *flag)
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	
	if (r->mode & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int RenderData_setModeBit(BPyRenderObject *self, PyObject *value, void *flag)
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_INT(self);
	
	if ( PyObject_IsTrue(value) )
		r->mode |= (int)flag;
	else
		r->mode &= ~(int)flag;
	return 0;
}

static PyObject *RenderData_getSceModeBit(BPyRenderObject *self, void *flag)
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	
	if (r->scemode & (int)flag)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
		
}

static int RenderData_setSceModeBits(BPyRenderObject *self, PyObject *value, void *flag)
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_INT(self);
	
	if ( PyObject_IsTrue(value) )
		r->scemode |= (int)flag;
	else
		r->scemode &= ~(int)flag;
	return 0;
}

static PyObject *RenderData_getEdgeColor( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	float col[3];
	
	REND_DEL_CHECK_PY(self);
	
	col[0] = r->edgeR;
	col[1] = r->edgeG;
	col[2] = r->edgeB;	
	return Color_CreatePyObject(col, 3, 0, 0, (PyObject *)NULL);
}

static int RenderData_setEdgeColor( BPyRenderObject * self, PyObject * value )
{
	RenderData *r = RenderData_get_pointer( self );
	int ret;
	REND_DEL_CHECK_INT(self);
	
	ret = EXPP_setVec3(value, &r->edgeR, &r->edgeG, &r->edgeB);
	if (ret == -1)
		return -1;
	
	CLAMP(r->edgeR, 0.0, 1.0);
	CLAMP(r->edgeG, 0.0, 1.0);
	CLAMP(r->edgeB, 0.0, 1.0);
	return 0;
}

static PyObject *RenderData_getGameFrameColor( BPyRenderObject * self )
{
	REND_DEL_CHECK_PY(self);
	return Color_CreatePyObject(self->bpysce->scene->framing.col, 3, 0, 0, (PyObject *)NULL);
}

static int RenderData_setGameFrameColor( BPyRenderObject * self, PyObject * value )
{
	
	int ret; 
	REND_DEL_CHECK_INT(self);
	
	ret = EXPP_setVec3(value,	&self->bpysce->scene->framing.col[0],
								&self->bpysce->scene->framing.col[1],
								&self->bpysce->scene->framing.col[2]);
	
	if (ret == -1)
		return -1;
	
	CLAMP(self->bpysce->scene->framing.col[0], 0.0, 1.0);
	CLAMP(self->bpysce->scene->framing.col[1], 0.0, 1.0);
	CLAMP(self->bpysce->scene->framing.col[2], 0.0, 1.0);
	return 0;
}

static PyObject *RenderData_getBackbuf( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	return EXPP_getBitfield( &r->bufflag,
			R_BACKBUF, 'h' );
}

static int RenderData_setBackbuf( BPyRenderObject* self, PyObject *value )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_INT(self);
	return EXPP_setBitfield( value, &r->bufflag,
			R_BACKBUF, 'h' );
}

static int RenderData_setImagePlanes( BPyRenderObject *self, PyObject *value )
{
	RenderData *r = RenderData_get_pointer( self );
	int depth = PyInt_AsLong( value );
	REND_DEL_CHECK_INT(self);
	if( (depth != 8 && depth != 24 && depth != 32) )
		return EXPP_ReturnIntError( PyExc_ValueError, "value must be 8, 24 or 32" );

	r->planes = (short)depth;
	return 0;
}

static PyObject *RenderData_getRenderPath( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	return PyString_FromString( r->pic );
}

static int RenderData_setRenderPath( BPyRenderObject * self, PyObject * value )
{
	RenderData *r = RenderData_get_pointer( self );
	char *name = PyString_AsString( value );
	REND_DEL_CHECK_INT(self);
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError,
						"expected a string" );

	if( strlen( name ) >= sizeof(r->pic) )
		return EXPP_ReturnIntError( PyExc_ValueError,
						"render path is too long" );

	strcpy( r->pic, name );
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

PyObject *RenderData_getBackbufPath( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	return PyString_FromString( r->backbuf );
}

static int RenderData_setBackbufPath( BPyRenderObject *self, PyObject *value )
{
	RenderData *r = RenderData_get_pointer( self );
	Image *ima;
	char *name = PyString_AsString( value );
	REND_DEL_CHECK_INT(self);
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError, "expected a string" );

	if( strlen( name ) >= sizeof(r->backbuf) )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"backbuf path is too long" );

	strcpy( r->backbuf, name );
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	ima = BKE_add_image_file( name );
	if( ima )
		BKE_image_signal( ima, NULL, IMA_SIGNAL_RELOAD );

	return 0;
}

PyObject *RenderData_getFtypePath( BPyRenderObject * self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	return PyString_FromString( r->ftype );
}

static int RenderData_setFtypePath( BPyRenderObject *self, PyObject *value )
{
	RenderData *r = RenderData_get_pointer( self );
	char *name = PyString_AsString( value );
	REND_DEL_CHECK_INT(self);
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError, "expected a string" );

	if( strlen( name ) >= sizeof(r->ftype) )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"ftype path is too long" );

	strcpy( r->ftype, name );
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );

	return 0;
}

static int RenderData_setRenderWinSize( BPyRenderObject *self, PyObject *value )
{	
	RenderData *r = RenderData_get_pointer( self );
	int size = PyInt_AsLong( value );
	REND_DEL_CHECK_INT(self);
	if( size != 25 && size != 50 && size != 75 && size != 100 )
		return EXPP_ReturnIntError( PyExc_ValueError, "expected int argument of 25, 50, 75, or 100" );

	r->size = (short)size;
	EXPP_allqueue( REDRAWBUTSSCENE, 0 );
	return 0;
}

static PyObject *RenderData_getMapOld( BPyRenderObject *self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	return PyInt_FromLong( (long)r->framapto );
}

static int RenderData_setMapOld( BPyRenderObject *self, PyObject *value )
{
	RenderData *r = RenderData_get_pointer( self );
	int ret;
	REND_DEL_CHECK_INT(self);
	
	ret = EXPP_setIValueClamped( value, &r->framapto, 1, 900, 'h' );
	if (ret != -1)
		r->framelen = (float)r->framapto / r->images;
	
	return ret;
}

static PyObject *RenderData_getMapNew( BPyRenderObject *self )
{
	RenderData *r = RenderData_get_pointer( self );
	REND_DEL_CHECK_PY(self);
	return PyInt_FromLong( (long)r->images );
}

static int RenderData_setMapNew( BPyRenderObject *self, PyObject *value )
{
	RenderData *r = RenderData_get_pointer( self );
	int ret;
	REND_DEL_CHECK_INT(self);
	
	ret = EXPP_setIValueClamped( value, &r->images, 1, 900, 'h' );
	if (ret != -1)
		r->framelen = (float)r->framapto / r->images;
	
	return ret;
}

static PyObject *RenderData_getSet( BPyRenderObject *self )
{	/* set==NULL -> None is ok */
	REND_DEL_CHECK_PY(self);
	return Scene_CreatePyObject( self->bpysce->scene->set );
}

static int RenderData_setSet( BPyRenderObject *self, PyObject *value )
{
	int ret;
	Scene *orig_sce;	
	REND_DEL_CHECK_INT(self);

	orig_sce = self->bpysce->scene->set;
	ret = GenericLib_assignData(value, (void **) &self->bpysce->scene->set, (void **) &self->bpysce->scene, 0, ID_SCE, 0);
	if (ret == -1)
		return ret;
	
	if( self->bpysce->scene->set && !scene_check_setscene( self->bpysce->scene ) ) {
		self->bpysce->scene->set = orig_sce;
		return EXPP_ReturnIntError( PyExc_ValueError,
				"linking scene would create a cycle" );
	}
	return 0;
}

/***************************************************************************/
/* BPyRenderObject attribute def                                            */
/***************************************************************************/
static PyGetSetDef BPyRender_getset[] = {
	/* custom */
	{"windowPos",
	 (getter)Render_setRenderWinPos, (setter)Render_getRenderWinPos,
	 "Oversampling (anti-aliasing) enabled",
	 NULL},
	{"mapOld",
	 (getter)RenderData_getMapOld, (setter)RenderData_setMapOld,
	 "Number of frames the Map Old will last",
	 NULL},
	{"mapNew",
	 (getter)RenderData_getMapNew, (setter)RenderData_setMapNew,
	 "New mapping value (in frames)",
	 NULL},
	{"set",
	 (getter)RenderData_getSet, (setter)RenderData_setSet,
	 "Scene link 'set' value",
	 NULL},
	{"edgeColor",
	 (getter)RenderData_getEdgeColor, (setter)RenderData_setEdgeColor,
	 "RGB color triplet for edges in Toon shading",
	 NULL},
	{"gameFrameColor",
	 (getter)RenderData_getGameFrameColor,(setter)RenderData_setGameFrameColor,
	 "RGB color triplet for bars",
	 NULL},
	 
	{"oversampling",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Oversampling (anti-aliasing) enabled",
	 (void *)R_OSA},
	{"shadow",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Shadow calculation enabled",
	 (void *)R_SHADOW},
	{"gammaCorrection",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Gamma correction enabled",
	 (void *)R_GAMMA},
/* R_ORTHO	unused */
	{"environmentMap",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Environment map rendering enabled",
	 (void *)R_ENVMAP},
	{"toonShading",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Toon edge shading enabled",
	 (void *)R_EDGE},
	{"fieldRendering", 
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Field rendering enabled",
	 (void *)R_FIELDS},
	{"fieldTimeDisable",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Time difference in field calculations disabled ('X' in UI)",
	 (void *)R_FIELDSTILL},
	{"radiosityRender",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Radiosity rendering enabled",
	 (void *)R_RADIO},
	{"borderRender",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Small cut-out rendering enabled",
	 (void *)R_BORDER},
	{"panorama",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Panorama rendering enabled",
	 (void *)R_PANORAMA},
	{"crop",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Crop image during border renders",
	 (void *)R_CROP},
/* R_COSMO	unsupported */
	{"oddFieldFirst",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Odd field first rendering enabled",
	 (void *)R_ODDFIELD},
	{"motionBlur",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Motion blur enabled",
	 (void *)R_MBLUR},
	{"rayTracing",
	 (getter)RenderData_getModeBit, (setter)RenderData_setModeBit,
	 "Ray tracing enabled",
	 (void *)R_RAYTRACE},
/* R_GAUSS unused */
/* R_FBUF unused */
/* R_THREADS unused */
/* R_SPEED unused */
	/* scene modes */
	{"sequencer",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "'Do Sequence' enabled",
     (void *)R_DOSEQ},
	{"extensions",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "Add extensions to output (when rendering animations) enabled",
     (void *)R_EXTENSION},
	{"compositor",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "'Do Compositor' enabled.",
     (void *)R_DOCOMP},
	{"freeImages",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "Free texture images after render.",
     (void *)R_FREE_IMAGE},
	{"singleLayer",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "Only render the active layer.",
     (void *)R_SINGLE_LAYER},
	{"saveBuffers",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "Save render buffers to disk while rendering, saves memory.",
     (void *)R_EXR_TILE_FILE},
	{"compositeFree",
     (getter)RenderData_getSceModeBit, (setter)RenderData_setSceModeBits,
     "Free nodes that are not used while composite.",
     (void *)R_COMP_FREE},
/* R_BG_RENDER unused */

	{"renderPath",
	 (getter)RenderData_getRenderPath, (setter)RenderData_setRenderPath,
	 "The path to output the rendered images to",
	 NULL},
	{"backbufPath",
	 (getter)RenderData_getBackbufPath, (setter)RenderData_setBackbufPath,
	 "Path to a background image (setting loads image)",
	 NULL},
	{"ftypePath",
	 (getter)RenderData_getFtypePath, (setter)RenderData_setFtypePath,
	 "The path to Ftype file",
	 NULL},

	{"border",
	 (getter)RenderData_getBorder, (setter)RenderData_setBorder,
	 "The border for border rendering",
	 NULL},
	{"timeCode",
	 (getter)RenderData_getTimeCode, (setter)NULL,
	 "Get the current frame in HH:MM:SS:FF format",
	 NULL},
	{"backbuf",
	 (getter)RenderData_getBackbuf, (setter)RenderData_setBackbuf,
	 "Backbuffer image enabled",
	 NULL},
	{"imagePlanes",
	 (getter)getIntAttr, (setter)RenderData_setImagePlanes,
	 "Image depth (8, 24, or 32 bits)",
	 NULL},
	
	{"imageType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_RENDER_ATTR_FORMAT},
	{"yafrayGIQuality",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_RENDER_ATTR_GIQUALITY},
	{"yafrayGIType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_RENDER_ATTR_GITYPE},
	{"preset",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "(enum) - (set only) the render to one of a presets",
	 (void *)EXPP_RENDER_ATTR_PRESET},
	{"gameFrame",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "",
	 (void *)EXPP_RENDER_ATTR_GAMEFRAME},
	{"alphaMode",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Setting for sky/background.",
	 (void *)EXPP_RENDER_ATTR_ALPHA},
	{"displayMode",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Render output in separate window or 3D view",
	 (void *)EXPP_RENDER_ATTR_DISPLAY},
	{"renderer",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "Rendering engine choice",
	 (void *)EXPP_RENDER_ATTR_RENDERER},
	
	/* ints */
	{"xParts",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of horizontal parts for image render",
	 (void *)EXPP_RENDER_ATTR_XPARTS},
	{"yParts",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of vertical parts for image render",
	 (void *)EXPP_RENDER_ATTR_YPARTS},
	{"aspectX",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Horizontal aspect ratio",
	 (void *)EXPP_RENDER_ATTR_ASPECTX},
	{"aspectY",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Vertical aspect ratio",
	 (void *)EXPP_RENDER_ATTR_ASPECTY},
	{"cFrame",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "The current frame for rendering",
	 (void *)EXPP_RENDER_ATTR_CFRAME},
	{"sFrame",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Starting frame for rendering",
	 (void *)EXPP_RENDER_ATTR_SFRAME},
	{"eFrame",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Ending frame for rendering",
	 (void *)EXPP_RENDER_ATTR_EFRAME},
	{"fps",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Frames per second",
	 (void *)EXPP_RENDER_ATTR_FPS},
    {"fpsBase",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Frames per second base",
	 (void *)EXPP_RENDER_ATTR_FPS_BASE},
	{"sizeX",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Image width (in pixels)",
	 (void *)EXPP_RENDER_ATTR_SIZEX},
	{"sizeY",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Image height (in pixels)",
	 (void *)EXPP_RENDER_ATTR_SIZEY},
	{"edgeIntensity",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "edge intensity for toon shading",
	 (void *)EXPP_RENDER_ATTR_EDGEINT},
	{"threads",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of threads used to render",
	 (void *)EXPP_RENDER_ATTR_THREADS},
	{"quality",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "quality for JPEG images, AVI Jpeg and SGI movies",
	 (void *)EXPP_RENDER_ATTR_QUALITY},
	{"yafrayRayDepth",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "maximum render ray depth from the camera",
	 (void *)EXPP_RENDER_ATTR_YF_RAYDEPTH},
	{"yafrayGICache",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "(bool) - Cache occlusion/irradiance samples (faster)",
	 (void *)EXPP_RENDER_ATTR_YF_GICACHE},
	{"yafrayGIDepth",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "number of bounces of the indirect light",
	 (void *)EXPP_RENDER_ATTR_YF_GIDEPTH},
	{"yafrayGICDepth",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "number of bounces inside objects (for caustics)",
	 (void *)EXPP_RENDER_ATTR_YF_GICDEPTH},
	{"yafrayGIPhotons",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "(bool) - enable/disable use global photons to help in GI",
	 (void *)EXPP_RENDER_ATTR_YF_GIPHOTONS},
	{"yafrayGITunePhotons",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "(bool) - enable/disable show the photonmap directly in the render for tuning",
	 (void *)EXPP_RENDER_ATTR_YF_GIDIRECT},
	{"yafrayExportToXML",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "If true export to an xml file and call yafray instead of plugin",
	 (void *)EXPP_RENDER_ATTR_YF_EXPORTXML},
	{"yafrayAutoAntiAliasing",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Automatic anti-aliasing enabled/disabled",
	 (void *)EXPP_RENDER_ATTR_YF_AA},
	{"yafrayClampRGB",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Clamp RGB enabled/disabled",
	 (void *)EXPP_RENDER_ATTR_YF_CLAMPRGB},
	{"yafrayGICacheBumpNormals",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Enable/disable bumpnormals for cache",
	 (void *)EXPP_RENDER_ATTR_YF_NOBUMP},
	{"yafrayNumberOfProcessors",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of processors to use",
	 (void *)EXPP_RENDER_ATTR_YF_NUMPROCS},
	{"yafrayAntiAliasingPasses",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of anti-aliasing passes (0 is no anti-aliasing)",
	 (void *)EXPP_RENDER_ATTR_YF_AAPASSES},
	{"yafrayAntiAliasingSamples",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Number of samples per pass",
	 (void *)EXPP_RENDER_ATTR_YF_AASAMPLES},
	{"yafrayGIPixelsPerSample",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "maximum number of pixels without samples, the lower the better and slower",
	 (void *)EXPP_RENDER_ATTR_YF_PXPERSAMPLE},
	{"yafrayGIPhotonCount",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "number of photons to shoot",
	 (void *)EXPP_RENDER_ATTR_YF_GIPHOTONCOUNT},
	{"yafrayGIPhotonMixCount",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "number of photons to mix",
	 (void *)EXPP_RENDER_ATTR_YF_GIMIXPHOTONS},
	{"oldMapValue",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "old map value in frames",
	 (void *)EXPP_RENDER_ATTR_OLDMAPVAL},
	{"newMapValue",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "new map value in frames",
	 (void *)EXPP_RENDER_ATTR_NEWMAPVAL},
	{"OSALevel",
	 (getter)getIntAttr, (setter)RenderData_setOSALevel,
	 "Oversampling (anti-aliasing) level",
	 NULL},
	{"renderwinSize",
	 (getter)getIntAttr, (setter)RenderData_setRenderWinSize,
	 "Size of the rendering window (25, 50, 75, or 100)",
	 (void *)EXPP_RENDER_ATTR_WINSIZE},
	
	 /* floats */
	{"gaussFilter",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Gauss filter size",
	 (void *)EXPP_RENDER_ATTR_GAUSSFILTER},
	{"mblurFactor",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Motion blur factor",
	 (void *)EXPP_RENDER_ATTR_MBLURFACTOR},
	{"yafrayGIPower",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "GI lighting intensity scale",
	(void *)EXPP_RENDER_ATTR_YF_GIINDIRPOWER},	 
	{"yafrayGIIndirPower",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "GI indifect lighting intensity scale",
	(void *)EXPP_RENDER_ATTR_YF_GIINDIRPOWER},
	{"yafrayGIPhotonRadius",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "radius to search for photons to mix (blur)",
	(void *)EXPP_RENDER_ATTR_YF_GIPHOTONRADIUS},
	{"yafrayGIShadowQuality",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "shadow quality, keep it under 0.95",
	(void *)EXPP_RENDER_ATTR_YF_GISHADOWQUALITY},	
	{"yafrayGICacheRefinement",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "threshold to refine shadows EXPERIMENTAL. 1 = no refinement",
	(void *)EXPP_RENDER_ATTR_YF_GIREFINEMENT},
	{"yafrayRayBias",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "shadow ray bias to avoid self shadowing",
	(void *)EXPP_RENDER_ATTR_YF_RAYBIAS},
	{"yafrayGamma",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "gamma correction, 1 is off",
	(void *)EXPP_RENDER_ATTR_YF_GAMMA},
	{"yafrayExposure",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "exposure adjustment, 0 is off",
	(void *)EXPP_RENDER_ATTR_YF_EXPOSURE},	
	{"yafrayAntiAliasingPixelSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Anti-aliasing pixel filter size",
	 (void *)EXPP_RENDER_ATTR_YF_AAPXSIZE},
	{"yafrayAntiAliasingThreshold",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Anti-aliasing threshold",
	 (void *)EXPP_RENDER_ATTR_YF_AATHRESH},
	{NULL}
};

/***************************************************************************/
/* BPyRenderObject method def                                               */
/***************************************************************************/
static PyMethodDef BPyRender_methods[] = {
	{"render", ( PyCFunction ) RenderData_Render, METH_NOARGS,
	 "() - render the scene"},
	{"saveRenderedImage", (PyCFunction)RenderData_SaveRenderedImage, METH_VARARGS,
	 "(filename) - save an image generated by a call to render() (set output path first)"},
	{"renderAnim", ( PyCFunction ) RenderData_RenderAnim, METH_NOARGS,
	 "() - render a sequence from start frame to end frame"},
	{"play", ( PyCFunction ) RenderData_Play, METH_NOARGS,
	 "() - play animation of rendered images/avi (searches Pics: field)"},
	{NULL, NULL, 0, NULL}
};
 
/*------------------------------------BPyRenderObject Type defintion------ */
PyTypeObject BPyRender_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender RenderData",       /* char *tp_name; */
	sizeof( BPyRenderObject ),   /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) RenderData_dealloc,	/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) RenderData_repr,     /* reprfunc tp_repr; */

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
	BPyRender_methods,     /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyRender_getset,   /* struct PyGetSetDef *tp_getset; */
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
/* BPyRenderObject Callbacks                                                */
/***************************************************************************/

PyObject *RenderData_CreatePyObject( BPySceneObject * bpysce )
{
	BPyRenderObject *py_renderdata;

	py_renderdata =
		( BPyRenderObject * ) PyObject_NEW( BPyRenderObject,
						   &BPyRender_Type );

	if( py_renderdata == NULL ) {
		return ( NULL );
	}
	
	Py_INCREF(bpysce);
	py_renderdata->bpysce = bpysce;
	return ( ( PyObject * ) py_renderdata );
}

PyObject *RenderType_Init( void )
{
	/* only allocate new dictionary once  constDefinition */
	if( BPyRender_Type.tp_dict == NULL ) {
		BPyRender_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &formatTypes );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &sizePresets );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &yafrayGITypes );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &winPos );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &gameFraming );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &renderTypes );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &alphaTypes );
		PyConstCategory_AddObjectToDict( BPyRender_Type.tp_dict, &displayTypes );
		PyType_Ready( &BPyRender_Type );
	}
	return (PyObject *) &BPyRender_Type;
}
