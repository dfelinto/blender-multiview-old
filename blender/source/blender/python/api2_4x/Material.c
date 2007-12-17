/* 
 * $Id: Material.c 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Willian P. Germano, Michel Selten, Alex Mole,
 * Alexander Szakaly, Campbell Barton, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Material.h" /*This must come first*/

#include "DNA_space_types.h"
#include "DNA_material_types.h"
#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_material.h"
#include "BKE_texture.h"
#include "BKE_idprop.h"
#include "BKE_utildefines.h" /* for CLAMP */
#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"
#include "BSE_editipo.h"
#include "BIF_space.h"
#include "mydevice.h"
#include "constant.h"
#include "MTex.h"
#include "Texture.h"
#include "Ipo.h"
#include "Group.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "IDProp.h"

/*****************************************************************************/
/* Python V24_BPy_Material defaults: */
/*****************************************************************************/
/* Material MIN, MAX values */
#define EXPP_MAT_ADD_MIN			 0.0f
#define EXPP_MAT_ADD_MAX			 1.0f
#define EXPP_MAT_ALPHA_MIN	   0.0f
#define EXPP_MAT_ALPHA_MAX		 1.0f
#define EXPP_MAT_AMB_MIN			 0.0f
#define EXPP_MAT_AMB_MAX			 1.0f
#define EXPP_MAT_COL_MIN			 0.0f /* min/max for all ... */
#define EXPP_MAT_COL_MAX			 1.0f /* ... color triplets  */
#define EXPP_MAT_EMIT_MIN			 0.0f
#define EXPP_MAT_EMIT_MAX			 1.0f
#define EXPP_MAT_REF_MIN			 0.0f
#define EXPP_MAT_REF_MAX			 1.0f
#define EXPP_MAT_SPEC_MIN			 0.0f
#define EXPP_MAT_SPEC_MAX			 2.0f
#define EXPP_MAT_SPECTRA_MIN	 0.0f
#define EXPP_MAT_SPECTRA_MAX	 1.0f

/* Shader specific settings */

#define EXPP_MAT_ROUGHNESS_MIN			 0.0f
#define EXPP_MAT_ROUGHNESS_MAX			 3.140f
#define EXPP_MAT_SPECSIZE_MIN			 0.0f
#define EXPP_MAT_SPECSIZE_MAX			 1.530f
#define EXPP_MAT_DIFFUSESIZE_MIN		 0.0f
#define EXPP_MAT_DIFFUSESIZE_MAX			 3.140f
#define EXPP_MAT_SPECSMOOTH_MIN			 0.0f
#define EXPP_MAT_SPECSMOOTH_MAX			 1.0f
#define EXPP_MAT_DIFFUSESMOOTH_MIN			 0.0f
#define EXPP_MAT_DIFFUSESMOOTH_MAX			 1.0f
#define EXPP_MAT_DIFFUSE_DARKNESS_MIN			 0.0f
#define EXPP_MAT_DIFFUSE_DARKNESS_MAX			 2.0f
#define EXPP_MAT_REFRACINDEX_MIN			 1.0f
#define EXPP_MAT_REFRACINDEX_MAX			 10.0f
#define EXPP_MAT_RMS_MIN			 0.0f
#define EXPP_MAT_RMS_MAX			 0.4f
/* End shader settings */

#define EXPP_MAT_ZOFFS_MIN			 0.0
#define EXPP_MAT_ZOFFS_MAX			10.0
#define EXPP_MAT_HALOSIZE_MIN			 0.0
#define EXPP_MAT_HALOSIZE_MAX		 100.0
#define EXPP_MAT_FLARESIZE_MIN		 0.1f
#define EXPP_MAT_FLARESIZE_MAX		25.0
#define EXPP_MAT_FLAREBOOST_MIN		 0.1f
#define EXPP_MAT_FLAREBOOST_MAX		10.0
#define EXPP_MAT_SUBSIZE_MIN			 0.1f
#define EXPP_MAT_SUBSIZE_MAX			25.0

#define EXPP_MAT_HARD_MIN				 1
#define EXPP_MAT_HARD_MAX		 255	/* 127 with MODE HALO ON */
#define EXPP_MAT_HALOSEED_MIN		 0
#define EXPP_MAT_HALOSEED_MAX    255
#define EXPP_MAT_NFLARES_MIN		 1
#define EXPP_MAT_NFLARES_MAX		32
#define EXPP_MAT_FLARESEED_MIN	 0
#define EXPP_MAT_FLARESEED_MAX 255
#define EXPP_MAT_NSTARS_MIN			 3
#define EXPP_MAT_NSTARS_MAX			50
#define EXPP_MAT_NLINES_MIN			 0
#define EXPP_MAT_NLINES_MAX		 250
#define EXPP_MAT_NRINGS_MIN			 0
#define EXPP_MAT_NRINGS_MAX			24

#define EXPP_MAT_RAYMIRR_MIN			 0.0
#define EXPP_MAT_RAYMIRR_MAX			 1.0
#define EXPP_MAT_MIRRDEPTH_MIN			 0
#define EXPP_MAT_MIRRDEPTH_MAX			 10
#define EXPP_MAT_FRESNELMIRR_MIN			0.0
#define EXPP_MAT_FRESNELMIRR_MAX			5.0
#define EXPP_MAT_FRESNELMIRRFAC_MIN			1.0
#define EXPP_MAT_FRESNELMIRRFAC_MAX			5.0
#define EXPP_MAT_FILTER_MIN			0.0
#define EXPP_MAT_FILTER_MAX			1.0
#define EXPP_MAT_TRANSLUCENCY_MIN			0.0
#define EXPP_MAT_TRANSLUCENCY_MAX			1.0
#define EXPP_MAT_ZOFFS_MIN				0.0
#define EXPP_MAT_ZOFFS_MAX				10.0
#define EXPP_MAT_IOR_MIN				1.0
#define EXPP_MAT_IOR_MAX				3.0
#define EXPP_MAT_TRANSDEPTH_MIN				0
#define EXPP_MAT_TRANSDEPTH_MAX				10
#define EXPP_MAT_FRESNELTRANS_MIN			0.0
#define EXPP_MAT_FRESNELTRANS_MAX			5.0
#define EXPP_MAT_FRESNELTRANSFAC_MIN			1.0
#define EXPP_MAT_FRESNELTRANSFAC_MAX			5.0
#define EXPP_MAT_SPECTRANS_MIN				0.0
#define EXPP_MAT_SPECTRANS_MAX				1.0
#define EXPP_MAT_MIRRTRANSADD_MIN			0.0
#define EXPP_MAT_MIRRTRANSADD_MAX			1.0

/* closure values for getColorComponent()/setColorComponent() */

#define	EXPP_MAT_COMP_R		0
#define	EXPP_MAT_COMP_G		1
#define	EXPP_MAT_COMP_B		2
#define	EXPP_MAT_COMP_SPECR	3
#define	EXPP_MAT_COMP_SPECG	4
#define	EXPP_MAT_COMP_SPECB	5
#define	EXPP_MAT_COMP_MIRR	6
#define	EXPP_MAT_COMP_MIRG	7
#define	EXPP_MAT_COMP_MIRB	8
#define	EXPP_MAT_COMP_SSSR	9
#define	EXPP_MAT_COMP_SSSG	10
#define	EXPP_MAT_COMP_SSSB	11


#define IPOKEY_RGB          0
#define IPOKEY_ALPHA        1 
#define IPOKEY_HALOSIZE     2 
#define IPOKEY_MODE         3
#define IPOKEY_ALLCOLOR     10
#define IPOKEY_ALLMIRROR    14
#define IPOKEY_OFS          12
#define IPOKEY_SIZE         13
#define IPOKEY_ALLMAPPING   11

/* SSS Settings */
#define EXPP_MAT_SSS_SCALE_MIN			0.001
#define EXPP_MAT_SSS_SCALE_MAX			1000.0
#define EXPP_MAT_SSS_RADIUS_MIN			0.0
#define EXPP_MAT_SSS_RADIUS_MAX			10000.0
#define EXPP_MAT_SSS_IOR_MIN			0.1
#define EXPP_MAT_SSS_IOR_MAX			2.0
#define EXPP_MAT_SSS_ERROR_MIN			0.0
#define EXPP_MAT_SSS_ERROR_MAX			1.0
#define EXPP_MAT_SSS_FRONT_MIN			0.0
#define EXPP_MAT_SSS_FRONT_MAX			2.0
#define EXPP_MAT_SSS_BACK_MIN			0.0
#define EXPP_MAT_SSS_BACK_MAX			10.0


/*****************************************************************************/
/* Python API function prototypes for the Material module.                   */
/*****************************************************************************/
static PyObject *V24_M_Material_New( PyObject * self, PyObject * args,
				 PyObject * keywords );
static PyObject *V24_M_Material_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* The following string definitions are used for documentation strings.  In  */
/* Python these will be written to the console when doing a                  */
/* Blender.Material.__doc__                                                  */
/*****************************************************************************/
static char V24_M_Material_doc[] = "The Blender Material module";

static char V24_M_Material_New_doc[] =
	"(name) - return a new material called 'name'\n\
() - return a new material called 'Mat'";

static char V24_M_Material_Get_doc[] =
	"(name) - return the material called 'name', None if not found.\n\
() - return a list of all materials in the current scene.";

/*****************************************************************************/
/* Python method structure definition for Blender.Material module:           */
/*****************************************************************************/
struct PyMethodDef V24_M_Material_methods[] = {
	{"New", ( PyCFunction ) V24_M_Material_New, METH_VARARGS | METH_KEYWORDS,
	 V24_M_Material_New_doc},
	{"Get", V24_M_Material_Get, METH_VARARGS, V24_M_Material_Get_doc},
	{"get", V24_M_Material_Get, METH_VARARGS, V24_M_Material_Get_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Function:	V24_M_Material_New                                               */
/* Python equivalent:		Blender.Material.New                             */
/*****************************************************************************/
static PyObject *V24_M_Material_New( PyObject * self, PyObject * args,
				 PyObject * keywords )
{
	char *name = "Mat";
	static char *kwlist[] = { "name", NULL };
	V24_BPy_Material *pymat; /* for Material Data object wrapper in Python */
	Material *blmat; /* for actual Material Data we create in Blender */
	char buf[21];

	if( !PyArg_ParseTupleAndKeywords
	    ( args, keywords, "|s", kwlist, &name ) )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected string or nothing as argument" ) );

	if( strcmp( name, "Mat" ) != 0 )	/* use gave us a name ? */
		PyOS_snprintf( buf, sizeof( buf ), "%s", name );

	blmat = add_material( name );	/* first create the Material Data in Blender */

	if( blmat )		/* now create the wrapper obj in Python */
		pymat = ( V24_BPy_Material * ) V24_Material_CreatePyObject( blmat );
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"couldn't create Material Data in Blender" ) );

	blmat->id.us = 0;	/* was incref'ed by add_material() above */

	if( pymat == NULL )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create Material Data object" ) );

	return ( PyObject * ) pymat;
}

/*****************************************************************************/
/* Function:	V24_M_Material_Get	 */
/* Python equivalent:	Blender.Material.Get */
/* Description:		Receives a string and returns the material whose */
/*			name matches the string.	If no argument is */
/*			passed in, a list with all materials in the	 */
/*			current scene is returned.			 */
/*****************************************************************************/
static PyObject *V24_M_Material_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Material *mat_iter;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected string argument (or nothing)" ) );

	if( name ) {		/* (name) - Search material by name */

		mat_iter = ( Material * ) V24_GetIdFromList( &( G.main->mat ), name );

		if( mat_iter == NULL ) { /* Requested material doesn't exist */
			char error_msg[64];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Material \"%s\" not found", name );
			return V24_EXPP_ReturnPyObjError( PyExc_NameError,
						      error_msg );
		}

		return V24_Material_CreatePyObject( mat_iter );
	}

	else {			/* () - return a list with all materials in the scene */
		int index = 0;
		PyObject *matlist, *pyobj;
	
		matlist = PyList_New( BLI_countlist( &( G.main->mat ) ) );

		if( !matlist )
			return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
							"couldn't create PyList" ) );
		
		mat_iter = G.main->mat.first;
		while( mat_iter ) {
			pyobj = V24_Material_CreatePyObject( mat_iter );

			if( !pyobj ) {
				Py_DECREF(matlist);
				return ( V24_EXPP_ReturnPyObjError
					 ( PyExc_MemoryError,
					   "couldn't create PyObject" ) );
			}
			PyList_SET_ITEM( matlist, index, pyobj );

			mat_iter = mat_iter->id.next;
			index++;
		}

		return matlist;
	}
}

static PyObject *V24_Material_ModesDict( void )
{
	PyObject *Modes = V24_PyConstant_New(  );

	if( Modes ) {
		V24_BPy_constant *c = ( V24_BPy_constant * ) Modes;

		V24_PyConstant_Insert(c, "TRACEABLE", PyInt_FromLong(MA_TRACEBLE));
		V24_PyConstant_Insert(c, "SHADOW", PyInt_FromLong(MA_SHADOW));
		V24_PyConstant_Insert(c, "SHADOWBUF", PyInt_FromLong(MA_SHADBUF));
		V24_PyConstant_Insert(c, "TANGENTSTR", PyInt_FromLong(MA_TANGENT_STR));
		V24_PyConstant_Insert(c, "FULLOSA", PyInt_FromLong(MA_FULL_OSA));
		V24_PyConstant_Insert(c, "RAYBIAS", PyInt_FromLong(MA_RAYBIAS));
		V24_PyConstant_Insert(c, "TRANSPSHADOW", PyInt_FromLong(MA_SHADOW_TRA));
		V24_PyConstant_Insert(c, "RAMPCOL", PyInt_FromLong(MA_RAMP_COL));
		V24_PyConstant_Insert(c, "RAMPSPEC", PyInt_FromLong(MA_RAMP_SPEC));
		V24_PyConstant_Insert(c, "SHADELESS", PyInt_FromLong(MA_SHLESS));
		V24_PyConstant_Insert(c, "WIRE", PyInt_FromLong(MA_WIRE));
		V24_PyConstant_Insert(c, "VCOL_LIGHT", PyInt_FromLong(MA_VERTEXCOL));
		V24_PyConstant_Insert(c, "HALO", PyInt_FromLong(MA_HALO));
		V24_PyConstant_Insert(c, "ZTRANSP", PyInt_FromLong(MA_ZTRA));
		V24_PyConstant_Insert(c, "VCOL_PAINT", PyInt_FromLong(MA_VERTEXCOLP));
		V24_PyConstant_Insert(c, "ZINVERT", PyInt_FromLong(MA_ZINV));
		V24_PyConstant_Insert(c, "HALORINGS", PyInt_FromLong(MA_HALO_RINGS));
		V24_PyConstant_Insert(c, "ENV", PyInt_FromLong(MA_ENV));
		V24_PyConstant_Insert(c, "HALOLINES", PyInt_FromLong(MA_HALO_LINES));
		V24_PyConstant_Insert(c, "ONLYSHADOW", PyInt_FromLong(MA_ONLYSHADOW));
		V24_PyConstant_Insert(c, "HALOXALPHA", PyInt_FromLong(MA_HALO_XALPHA));
		V24_PyConstant_Insert(c, "HALOSTAR", PyInt_FromLong(MA_STAR));
		V24_PyConstant_Insert(c, "TEXFACE", PyInt_FromLong(MA_FACETEXTURE));
		V24_PyConstant_Insert(c, "HALOTEX", PyInt_FromLong(MA_HALOTEX));
		V24_PyConstant_Insert(c, "HALOPUNO", PyInt_FromLong(MA_HALOPUNO));
		V24_PyConstant_Insert(c, "NOMIST", PyInt_FromLong(MA_NOMIST));
		V24_PyConstant_Insert(c, "HALOSHADE", PyInt_FromLong(MA_HALO_SHADE));
		V24_PyConstant_Insert(c, "HALOFLARE", PyInt_FromLong(MA_HALO_FLARE));
		V24_PyConstant_Insert(c, "RADIO", PyInt_FromLong(MA_RADIO));
		V24_PyConstant_Insert(c, "RAYMIRROR", PyInt_FromLong(MA_RAYMIRROR));
		V24_PyConstant_Insert(c, "ZTRA", PyInt_FromLong(MA_ZTRA));
		V24_PyConstant_Insert(c, "RAYTRANSP", PyInt_FromLong(MA_RAYTRANSP));
		V24_PyConstant_Insert(c, "TANGENT_V", PyInt_FromLong(MA_TANGENT_V));
		V24_PyConstant_Insert(c, "NMAP_TS", PyInt_FromLong(MA_NORMAP_TANG));
		V24_PyConstant_Insert(c, "GROUP_EXCLUSIVE", PyInt_FromLong(MA_GROUP_NOLAY));
	}

	return Modes;
}


static PyObject *V24_Material_ShadersDict( void )
{
	PyObject *Shaders = V24_PyConstant_New(  );

	if( Shaders ) {
		V24_BPy_constant *c = ( V24_BPy_constant * ) Shaders;

		V24_PyConstant_Insert(c, "DIFFUSE_LAMBERT", PyInt_FromLong(MA_DIFF_LAMBERT));
		V24_PyConstant_Insert(c, "DIFFUSE_ORENNAYAR", PyInt_FromLong(MA_DIFF_ORENNAYAR));
		V24_PyConstant_Insert(c, "DIFFUSE_TOON", PyInt_FromLong(MA_DIFF_TOON));
		V24_PyConstant_Insert(c, "DIFFUSE_MINNAERT", PyInt_FromLong(MA_DIFF_MINNAERT));
		V24_PyConstant_Insert(c, "SPEC_COOKTORR", PyInt_FromLong(MA_SPEC_COOKTORR));
		V24_PyConstant_Insert(c, "SPEC_PHONG", PyInt_FromLong(MA_SPEC_PHONG));
		V24_PyConstant_Insert(c, "SPEC_BLINN", PyInt_FromLong(MA_SPEC_BLINN));
		V24_PyConstant_Insert(c, "SPEC_TOON", PyInt_FromLong(MA_SPEC_TOON));
		V24_PyConstant_Insert(c, "SPEC_WARDISO", PyInt_FromLong(MA_SPEC_WARDISO));

	}

	return Shaders;
}


/*****************************************************************************/
/* Function:	V24_Material_Init */
/*****************************************************************************/
PyObject *V24_Material_Init( void )
{
	PyObject *V24_submodule, *Modes, *Shaders;

	if( PyType_Ready( &V24_Material_Type ) < 0)
		return NULL;

	Modes = V24_Material_ModesDict(  );
	Shaders = V24_Material_ShadersDict(  );

	V24_submodule = Py_InitModule3( "Blender.Material",
				    V24_M_Material_methods, V24_M_Material_doc );

	if( Modes )
		PyModule_AddObject( V24_submodule, "Modes", Modes );
	if( Shaders )
		PyModule_AddObject( V24_submodule, "Shaders", Shaders );
	
	PyModule_AddIntConstant( V24_submodule, "RGB", IPOKEY_RGB );
	PyModule_AddIntConstant( V24_submodule, "ALPHA", IPOKEY_ALPHA );
	PyModule_AddIntConstant( V24_submodule, "HALOSIZE", IPOKEY_HALOSIZE );
	PyModule_AddIntConstant( V24_submodule, "MODE", IPOKEY_MODE );
	PyModule_AddIntConstant( V24_submodule, "ALLCOLOR", IPOKEY_ALLCOLOR );
	PyModule_AddIntConstant( V24_submodule, "ALLMIRROR", IPOKEY_ALLMIRROR );
	PyModule_AddIntConstant( V24_submodule, "OFS", IPOKEY_OFS );
	PyModule_AddIntConstant( V24_submodule, "SIZE", IPOKEY_SIZE );
	PyModule_AddIntConstant( V24_submodule, "ALLMAPPING", IPOKEY_ALLMAPPING );

	return ( V24_submodule );
}

/***************************/
/*** The Material PyType ***/
/***************************/

static PyObject *V24_Matr_oldsetAdd( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetAlpha( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetAmb( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetEmit( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFilter( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFlareBoost( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFlareSeed( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFlareSize( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFresnelMirr( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFresnelMirrFac( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFresnelTrans( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetFresnelTransFac( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetHaloSeed( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetHaloSize( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetHardness( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetIOR( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetNFlares( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetNLines( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetNRings( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetNStars( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetRayMirr( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetMirrDepth( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetRef( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSpec( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSpecTransp( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSubSize( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetTransDepth( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetZOffset( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetMode( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetIpo( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetRGBCol( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSpecCol( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSpecShader( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetMirCol( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetDiffuseShader( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetRoughness( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSpecSize( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetDiffuseSize( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetSpecSmooth( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetDiffuseSmooth( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetDiffuseDarkness( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetRefracIndex( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetRms( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Matr_oldsetTranslucency( V24_BPy_Material * self, PyObject * args );

static int V24_Material_setIpo( V24_BPy_Material * self, PyObject * value );

static int V24_Material_setMode( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRGBCol( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSpecCol( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setMirCol( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssCol( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setColorComponent( V24_BPy_Material * self, PyObject * value,
							void * closure );
static int V24_Material_setAmb( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setEmit( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSpecTransp( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setAlpha( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setShadAlpha( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRef( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSpec( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setZOffset( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setLightGroup( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setAdd( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setHaloSize( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFlareSize( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFlareBoost( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSubSize( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setHaloSeed( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFlareSeed( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setHardness( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setNFlares( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setNStars( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setNLines( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setNRings( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRayMirr( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setMirrDepth( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFresnelMirr( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFresnelMirrFac( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setIOR( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setTransDepth( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFresnelTrans( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFresnelTransFac( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRigidBodyFriction( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRigidBodyRestitution( V24_BPy_Material * self, PyObject * value );

static int V24_Material_setSpecShader( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setDiffuseShader( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRoughness( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSpecSize( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setDiffuseSize( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSpecSmooth( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setDiffuseSmooth( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setDiffuseDarkness( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRefracIndex( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setRms( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setFilter( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setTranslucency( V24_BPy_Material * self, PyObject * value );

static int V24_Material_setSssEnable( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssScale( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssRadius( V24_BPy_Material * self, PyObject * value, void * type );
static int V24_Material_setSssIOR( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssError( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssColorBlend( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssTexScatter( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssFront( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssBack( V24_BPy_Material * self, PyObject * value );
static int V24_Material_setSssBack( V24_BPy_Material * self, PyObject * value );

static PyObject *V24_Material_getColorComponent( V24_BPy_Material * self,
							void * closure );

/*static int Material_setSeptex( V24_BPy_Material * self, PyObject * value );
  static PyObject *Material_getSeptex( V24_BPy_Material * self );*/

/*****************************************************************************/
/* Python V24_BPy_Material methods declarations: */
/*****************************************************************************/
static PyObject *V24_Material_getIpo( V24_BPy_Material * self );
static PyObject *V24_Material_getMode( V24_BPy_Material * self );
static PyObject *V24_Material_getRGBCol( V24_BPy_Material * self );
/*static PyObject *Material_getAmbCol(V24_BPy_Material *self);*/
static PyObject *V24_Material_getSpecCol( V24_BPy_Material * self );
static PyObject *V24_Material_getMirCol( V24_BPy_Material * self );
static PyObject *V24_Material_getSssCol( V24_BPy_Material * self );
static PyObject *V24_Material_getAmb( V24_BPy_Material * self );
static PyObject *V24_Material_getEmit( V24_BPy_Material * self );
static PyObject *V24_Material_getAlpha( V24_BPy_Material * self );
static PyObject *V24_Material_getShadAlpha( V24_BPy_Material * self );
static PyObject *V24_Material_getRef( V24_BPy_Material * self );
static PyObject *V24_Material_getSpec( V24_BPy_Material * self );
static PyObject *V24_Material_getSpecTransp( V24_BPy_Material * self );
static PyObject *V24_Material_getAdd( V24_BPy_Material * self );
static PyObject *V24_Material_getZOffset( V24_BPy_Material * self );
static PyObject *V24_Material_getLightGroup( V24_BPy_Material * self );
static PyObject *V24_Material_getHaloSize( V24_BPy_Material * self );
static PyObject *V24_Material_getHaloSeed( V24_BPy_Material * self );
static PyObject *V24_Material_getFlareSize( V24_BPy_Material * self );
static PyObject *V24_Material_getFlareSeed( V24_BPy_Material * self );
static PyObject *V24_Material_getFlareBoost( V24_BPy_Material * self );
static PyObject *V24_Material_getSubSize( V24_BPy_Material * self );
static PyObject *V24_Material_getHardness( V24_BPy_Material * self );
static PyObject *V24_Material_getNFlares( V24_BPy_Material * self );
static PyObject *V24_Material_getNStars( V24_BPy_Material * self );
static PyObject *V24_Material_getNLines( V24_BPy_Material * self );
static PyObject *V24_Material_getNRings( V24_BPy_Material * self );
/* Shader settings */
static PyObject *V24_Material_getSpecShader( V24_BPy_Material * self );
static PyObject *V24_Material_getDiffuseShader( V24_BPy_Material * self );
static PyObject *V24_Material_getRoughness( V24_BPy_Material * self );
static PyObject *V24_Material_getSpecSize( V24_BPy_Material * self );
static PyObject *V24_Material_getDiffuseSize( V24_BPy_Material * self );
static PyObject *V24_Material_getSpecSmooth( V24_BPy_Material * self );
static PyObject *V24_Material_getDiffuseSmooth( V24_BPy_Material * self );
static PyObject *V24_Material_getDiffuseDarkness( V24_BPy_Material * self );
static PyObject *V24_Material_getRefracIndex( V24_BPy_Material * self );
static PyObject *V24_Material_getRms( V24_BPy_Material * self );

static PyObject *V24_Material_getRayMirr( V24_BPy_Material * self );
static PyObject *V24_Material_getMirrDepth( V24_BPy_Material * self );
static PyObject *V24_Material_getFresnelMirr( V24_BPy_Material * self );
static PyObject *V24_Material_getFresnelMirrFac( V24_BPy_Material * self );
static PyObject *V24_Material_getIOR( V24_BPy_Material * self );
static PyObject *V24_Material_getTransDepth( V24_BPy_Material * self );
static PyObject *V24_Material_getFresnelTrans( V24_BPy_Material * self );
static PyObject *V24_Material_getFresnelTransFac( V24_BPy_Material * self );
static PyObject *V24_Material_getRigidBodyFriction( V24_BPy_Material * self );
static PyObject *V24_Material_getRigidBodyRestitution( V24_BPy_Material * self );

static PyObject *V24_Material_getSssEnable( V24_BPy_Material * self );
static PyObject *V24_Material_getSssScale( V24_BPy_Material * self );
static PyObject *V24_Material_getSssRadius( V24_BPy_Material * self, void * type );
static PyObject *V24_Material_getSssIOR( V24_BPy_Material * self );
static PyObject *V24_Material_getSssError( V24_BPy_Material * self );
static PyObject *V24_Material_getSssColorBlend( V24_BPy_Material * self );
static PyObject *V24_Material_getSssTexScatter( V24_BPy_Material * self );
static PyObject *V24_Material_getSssFront( V24_BPy_Material * self );
static PyObject *V24_Material_getSssBack( V24_BPy_Material * self );
static PyObject *V24_Material_getSssBack( V24_BPy_Material * self );

static PyObject *V24_Material_getFilter( V24_BPy_Material * self );
static PyObject *V24_Material_getTranslucency( V24_BPy_Material * self );
static PyObject *V24_Material_getTextures( V24_BPy_Material * self );
static PyObject *V24_Material_clearIpo( V24_BPy_Material * self );

static PyObject *V24_Material_setTexture( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Material_clearTexture( V24_BPy_Material * self, PyObject * value );

static PyObject *V24_Material_getScriptLinks(V24_BPy_Material *self, PyObject * value );
static PyObject *V24_Material_addScriptLink(V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Material_clearScriptLinks(V24_BPy_Material *self, PyObject *args);

static PyObject *V24_Material_insertIpoKey( V24_BPy_Material * self, PyObject * args );
static PyObject *V24_Material_getColorband( V24_BPy_Material * self, void * type);
int V24_Material_setColorband( V24_BPy_Material * self, PyObject * value, void * type);
static PyObject *V24_Material_copy( V24_BPy_Material * self );


/*****************************************************************************/
/* Python V24_BPy_Material methods table: */
/*****************************************************************************/
static PyMethodDef V24_BPy_Material_methods[] = {
	/* name, method, flags, doc */
	{"getName", ( PyCFunction ) V24_GenericLib_getName, METH_NOARGS,
	 "() - Return Material's name"},
	{"getIpo", ( PyCFunction ) V24_Material_getIpo, METH_NOARGS,
	 "() - Return Material's ipo or None if not found"},
	{"getMode", ( PyCFunction ) V24_Material_getMode, METH_NOARGS,
	 "() - Return Material's mode flags"},
	{"getRGBCol", ( PyCFunction ) V24_Material_getRGBCol, METH_NOARGS,
	 "() - Return Material's rgb color triplet"},
/*	{"getAmbCol", (PyCFunction)Material_getAmbCol, METH_NOARGS,
			"() - Return Material's ambient color"},*/
	{"getSpecCol", ( PyCFunction ) V24_Material_getSpecCol, METH_NOARGS,
	 "() - Return Material's specular color"},
	{"getMirCol", ( PyCFunction ) V24_Material_getMirCol, METH_NOARGS,
	 "() - Return Material's mirror color"},
	{"getAmb", ( PyCFunction ) V24_Material_getAmb, METH_NOARGS,
	 "() - Return Material's ambient color blend factor"},
	{"getEmit", ( PyCFunction ) V24_Material_getEmit, METH_NOARGS,
	 "() - Return Material's emitting light intensity"},
	{"getAlpha", ( PyCFunction ) V24_Material_getAlpha, METH_NOARGS,
	 "() - Return Material's alpha (transparency) value"},
	{"getRef", ( PyCFunction ) V24_Material_getRef, METH_NOARGS,
	 "() - Return Material's reflectivity"},
	{"getSpec", ( PyCFunction ) V24_Material_getSpec, METH_NOARGS,
	 "() - Return Material's specularity"},
	/* Shader specific settings */
	{"getSpecShader", ( PyCFunction ) V24_Material_getSpecShader, METH_NOARGS,
	 "() - Returns Material's specular shader" },
	{"getDiffuseShader", ( PyCFunction ) V24_Material_getDiffuseShader, METH_NOARGS,
	 "() - Returns Material's diffuse shader" },
	 {"getRoughness", ( PyCFunction ) V24_Material_getRoughness, METH_NOARGS,
	 "() - Returns Material's Roughness (applies to the \"Oren Nayar\" Diffuse Shader only)" },
	{"getSpecSize", ( PyCFunction ) V24_Material_getSpecSize, METH_NOARGS,
	 "() - Returns Material's size of specular area (applies to the \"Toon\" Specular Shader only)" },
	{"getDiffuseSize", ( PyCFunction ) V24_Material_getDiffuseSize, METH_NOARGS,
	 "() - Returns Material's size of diffuse area (applies to the \"Toon\" Diffuse Shader only)" },
	{"getSpecSmooth", ( PyCFunction ) V24_Material_getSpecSmooth, METH_NOARGS,
	 "() - Returns Material's smoothing of specular area (applies to the \"Toon\" Diffuse Shader only)" },
	{"getDiffuseSmooth", ( PyCFunction ) V24_Material_getDiffuseSmooth, METH_NOARGS,
	 "() - Returns Material's smoothing of diffuse area (applies to the \"Toon\" Diffuse Shader only)" },
	{"getDiffuseDarkness", ( PyCFunction ) V24_Material_getDiffuseDarkness, METH_NOARGS,
	 "() - Returns Material's diffuse darkness (applies to the \"Minnaert\" Diffuse Shader only)" },
	{"getRefracIndex", ( PyCFunction ) V24_Material_getRefracIndex, METH_NOARGS,
	 "() - Returns Material's Index of Refraction (applies to the \"Blinn\" Specular Shader only)" },	 
	{"getRms", ( PyCFunction ) V24_Material_getRms, METH_NOARGS,
	 "() - Returns Material's standard deviation of surface slope (applies to the \"WardIso\" Specular Shader only)" },
	/* End shader settings */
	{"getSpecTransp", ( PyCFunction ) V24_Material_getSpecTransp, METH_NOARGS,
	 "() - Return Material's specular transparency"},
	{"getAdd", ( PyCFunction ) V24_Material_getAdd, METH_NOARGS,
	 "() - Return Material's glow factor"},
	{"getZOffset", ( PyCFunction ) V24_Material_getZOffset, METH_NOARGS,
	 "() - Return Material's artificial offset for faces"},
	{"getHaloSize", ( PyCFunction ) V24_Material_getHaloSize, METH_NOARGS,
	 "() - Return Material's halo size"},
	{"getHaloSeed", ( PyCFunction ) V24_Material_getHaloSeed, METH_NOARGS,
	 "() - Return Material's seed for random ring dimension and line "
	 "location in halos"},
	{"getFlareSize", ( PyCFunction ) V24_Material_getFlareSize, METH_NOARGS,
	 "() - Return Material's (flare size)/(halo size) factor"},
	{"getFlareSeed", ( PyCFunction ) V24_Material_getFlareSeed, METH_NOARGS,
	 "() - Return Material's flare offset in the seed table"},
	{"getFlareBoost", ( PyCFunction ) V24_Material_getFlareBoost, METH_NOARGS,
	 "() - Return Material's flare boost"},
	{"getSubSize", ( PyCFunction ) V24_Material_getSubSize, METH_NOARGS,
	 "() - Return Material's dimension of subflare, dots and circles"},
	{"getHardness", ( PyCFunction ) V24_Material_getHardness, METH_NOARGS,
	 "() - Return Material's specular hardness"},
	{"getNFlares", ( PyCFunction ) V24_Material_getNFlares, METH_NOARGS,
	 "() - Return Material's number of flares in halo"},
	{"getNStars", ( PyCFunction ) V24_Material_getNStars, METH_NOARGS,
	 "() - Return Material's number of points in the halo stars"},
	{"getNLines", ( PyCFunction ) V24_Material_getNLines, METH_NOARGS,
	 "() - Return Material's number of lines in halo"},
	{"getNRings", ( PyCFunction ) V24_Material_getNRings, METH_NOARGS,
	 "() - Return Material's number of rings in halo"},
	{"getRayMirr", ( PyCFunction ) V24_Material_getRayMirr, METH_NOARGS,
	 "() - Return mount mirror"},
	{"getMirrDepth", ( PyCFunction ) V24_Material_getMirrDepth, METH_NOARGS,
	 "() - Return amount mirror depth"},
	{"getFresnelMirr", ( PyCFunction ) V24_Material_getFresnelMirr, METH_NOARGS,
	 "() - Return fresnel power for refractions"},
	{"getFresnelMirrFac", ( PyCFunction ) V24_Material_getFresnelMirrFac, METH_NOARGS,
	 "() - Return fresnel power for refractions factor"},
	{"getFilter", ( PyCFunction ) V24_Material_getFilter, METH_NOARGS,
	 "() - Return the amount of filtering when transparent raytrace is enabled"},
	{"getTranslucency", ( PyCFunction ) V24_Material_getTranslucency, METH_NOARGS,
	 "() - Return the Translucency, the amount of diffuse shading of the back side"},
	{"getIOR", ( PyCFunction ) V24_Material_getIOR, METH_NOARGS,
	 "() - Return IOR"},
	{"getTransDepth", ( PyCFunction ) V24_Material_getTransDepth, METH_NOARGS,
	 "() - Return amount inter-refractions"},
	{"getFresnelTrans", ( PyCFunction ) V24_Material_getFresnelTrans, METH_NOARGS,
	 "() - Return fresnel power for refractions"},
	{"getFresnelTransFac", ( PyCFunction ) V24_Material_getFresnelTransFac, METH_NOARGS,
	 "() - Return fresnel power for refractions factor"},

	{"getTextures", ( PyCFunction ) V24_Material_getTextures, METH_NOARGS,
	 "() - Return Material's texture list as a tuple"},
	{"setName", ( PyCFunction ) V24_GenericLib_setName_with_method, METH_VARARGS,
	 "(s) - Change Material's name"},
	{"setIpo", ( PyCFunction ) V24_Matr_oldsetIpo, METH_VARARGS,
	 "(Blender Ipo) - Change Material's Ipo"},
	{"clearIpo", ( PyCFunction ) V24_Material_clearIpo, METH_NOARGS,
	 "(Blender Ipo) - Unlink Ipo from this Material"},
	{"insertIpoKey", ( PyCFunction ) V24_Material_insertIpoKey, METH_VARARGS,
	 "(Material Ipo Constant) - Insert IPO Key at current frame"},	 
	{"setMode", ( PyCFunction ) V24_Matr_oldsetMode, METH_VARARGS,
	 "([s[,s]]) - Set Material's mode flag(s)"},
	{"setRGBCol", ( PyCFunction ) V24_Matr_oldsetRGBCol, METH_VARARGS,
	 "(f,f,f or [f,f,f]) - Set Material's rgb color triplet"},
/*	{"setAmbCol", (PyCFunction)Matr_oldsetAmbCol, METH_VARARGS,
			"(f,f,f or [f,f,f]) - Set Material's ambient color"},*/
	{"setSpecCol", ( PyCFunction ) V24_Matr_oldsetSpecCol, METH_VARARGS,
	 "(f,f,f or [f,f,f]) - Set Material's specular color"},
	 
	/* Shader spesific settings */
	{"setSpecShader", ( PyCFunction ) V24_Matr_oldsetSpecShader, METH_VARARGS,
	 "(i) - Set the Material's specular shader" },
	{"setDiffuseShader", ( PyCFunction ) V24_Matr_oldsetDiffuseShader, METH_VARARGS,
	 "(i) - Set the Material's diffuse shader" },
	 {"setRoughness", ( PyCFunction ) V24_Matr_oldsetRoughness, METH_VARARGS,
	 "(f) - Set the Material's Roughness (applies to the \"Oren Nayar\" Diffuse Shader only)" },
	{"setSpecSize", ( PyCFunction ) V24_Matr_oldsetSpecSize, METH_VARARGS,
	 "(f) - Set the Material's size of specular area (applies to the \"Toon\" Specular Shader only)" },
	{"setDiffuseSize", ( PyCFunction ) V24_Matr_oldsetDiffuseSize, METH_VARARGS,
	 "(f) - Set the Material's size of diffuse area (applies to the \"Toon\" Diffuse Shader only)" },
	{"setSpecSmooth", ( PyCFunction ) V24_Matr_oldsetSpecSmooth, METH_VARARGS,
	 "(f) - Set the Material's smoothing of specular area (applies to the \"Toon\" Specular Shader only)" },
	{"setDiffuseSmooth", ( PyCFunction ) V24_Matr_oldsetDiffuseSmooth, METH_VARARGS,
	 "(f) - Set the Material's smoothing of diffuse area (applies to the \"Toon\" Diffuse Shader only)" },
	{"setDiffuseDarkness", ( PyCFunction ) V24_Matr_oldsetDiffuseDarkness, METH_VARARGS,
	 "(f) - Set the Material's diffuse darkness (applies to the \"Minnaert\" Diffuse Shader only)" },
	{"setRefracIndex", ( PyCFunction ) V24_Matr_oldsetRefracIndex, METH_VARARGS,
	 "(f) - Set the Material's Index of Refraction (applies to the \"Blinn\" Specular Shader only)" },	 
	{"setRms", ( PyCFunction ) V24_Matr_oldsetRms, METH_VARARGS,
	 "(f) - Set the Material's standard deviation of surface slope (applies to the \"WardIso\" Specular Shader only)" },
	/* End shader settings */
	 
	{"setMirCol", ( PyCFunction ) V24_Matr_oldsetMirCol, METH_VARARGS,
	 "(f,f,f or [f,f,f]) - Set Material's mirror color"},
	{"setAmb", ( PyCFunction ) V24_Matr_oldsetAmb, METH_VARARGS,
	 "(f) - Set how much the Material's color is affected"
	 " by \nthe global ambient colors - [0.0, 1.0]"},
	{"setEmit", ( PyCFunction ) V24_Matr_oldsetEmit, METH_VARARGS,
	 "(f) - Set Material's emitting light intensity - [0.0, 1.0]"},
	{"setAlpha", ( PyCFunction ) V24_Matr_oldsetAlpha, METH_VARARGS,
	 "(f) - Set Material's alpha (transparency) - [0.0, 1.0]"},
	{"setRef", ( PyCFunction ) V24_Matr_oldsetRef, METH_VARARGS,
	 "(f) - Set Material's reflectivity - [0.0, 1.0]"},
	{"setSpec", ( PyCFunction ) V24_Matr_oldsetSpec, METH_VARARGS,
	 "(f) - Set Material's specularity - [0.0, 2.0]"},
	{"setSpecTransp", ( PyCFunction ) V24_Matr_oldsetSpecTransp, METH_VARARGS,
	 "(f) - Set Material's specular transparency - [0.0, 1.0]"},
	{"setAdd", ( PyCFunction ) V24_Matr_oldsetAdd, METH_VARARGS,
	 "(f) - Set Material's glow factor - [0.0, 1.0]"},
	{"setZOffset", ( PyCFunction ) V24_Matr_oldsetZOffset, METH_VARARGS,
	 "(f) - Set Material's artificial offset - [0.0, 10.0]"},
	{"setHaloSize", ( PyCFunction ) V24_Matr_oldsetHaloSize, METH_VARARGS,
	 "(f) - Set Material's halo size - [0.0, 100.0]"},
	{"setHaloSeed", ( PyCFunction ) V24_Matr_oldsetHaloSeed, METH_VARARGS,
	 "(i) - Set Material's halo seed - [0, 255]"},
	{"setFlareSize", ( PyCFunction ) V24_Matr_oldsetFlareSize, METH_VARARGS,
	 "(f) - Set Material's factor: (flare size)/(halo size) - [0.1, 25.0]"},
	{"setFlareSeed", ( PyCFunction ) V24_Matr_oldsetFlareSeed, METH_VARARGS,
	 "(i) - Set Material's flare seed - [0, 255]"},
	{"setFlareBoost", ( PyCFunction ) V24_Matr_oldsetFlareBoost, METH_VARARGS,
	 "(f) - Set Material's flare boost - [0.1, 10.0]"},
	{"setSubSize", ( PyCFunction ) V24_Matr_oldsetSubSize, METH_VARARGS,
	 "(f) - Set Material's dimension of subflare,"
	 " dots and circles - [0.1, 25.0]"},
	{"setHardness", ( PyCFunction ) V24_Matr_oldsetHardness, METH_VARARGS,
	 "(i) - Set Material's hardness - [1, 255 (127 if halo mode is ON)]"},
	{"setNFlares", ( PyCFunction ) V24_Matr_oldsetNFlares, METH_VARARGS,
	 "(i) - Set Material's number of flares in halo - [1, 32]"},
	{"setNStars", ( PyCFunction ) V24_Matr_oldsetNStars, METH_VARARGS,
	 "(i) - Set Material's number of stars in halo - [3, 50]"},
	{"setNLines", ( PyCFunction ) V24_Matr_oldsetNLines, METH_VARARGS,
	 "(i) - Set Material's number of lines in halo - [0, 250]"},
	{"setNRings", ( PyCFunction ) V24_Matr_oldsetNRings, METH_VARARGS,
	 "(i) - Set Material's number of rings in halo - [0, 24]"},
	{"setRayMirr", ( PyCFunction ) V24_Matr_oldsetRayMirr, METH_VARARGS,
	 "(f) - Set amount mirror - [0.0, 1.0]"},
	{"setMirrDepth", ( PyCFunction ) V24_Matr_oldsetMirrDepth, METH_VARARGS,
	 "(i) - Set amount inter-reflections - [0, 10]"},
	{"setFresnelMirr", ( PyCFunction ) V24_Matr_oldsetFresnelMirr, METH_VARARGS,
	 "(f) - Set fresnel power for mirror - [0.0, 5.0]"},
	{"setFresnelMirrFac", ( PyCFunction ) V24_Matr_oldsetFresnelMirrFac, METH_VARARGS,
	 "(f) - Set blend fac for mirror fresnel - [1.0, 5.0]"},
	{"setFilter", ( PyCFunction ) V24_Matr_oldsetFilter, METH_VARARGS,
	 "(f) - Set the amount of filtering when transparent raytrace is enabled"},
	{"setTranslucency", ( PyCFunction ) V24_Matr_oldsetTranslucency, METH_VARARGS,
	 "(f) - Set the Translucency, the amount of diffuse shading of the back side"},
	{"setIOR", ( PyCFunction ) V24_Matr_oldsetIOR, METH_VARARGS,
	 "(f) - Set IOR - [1.0, 3.0]"},
	{"setTransDepth", ( PyCFunction ) V24_Matr_oldsetTransDepth, METH_VARARGS,
	 "(i) - Set amount inter-refractions - [0, 10]"},
	{"setFresnelTrans", ( PyCFunction ) V24_Matr_oldsetFresnelTrans, METH_VARARGS,
	 "(f) - Set fresnel power for refractions - [0.0, 5.0]"},
	{"setFresnelTransFac", ( PyCFunction ) V24_Matr_oldsetFresnelTransFac, METH_VARARGS,
	 "(f) - Set fresnel power for refractions factor- [0.0, 5.0]"},
	{"setTexture", ( PyCFunction ) V24_Material_setTexture, METH_VARARGS,
	 "(n,tex,texco=0,mapto=0) - Set numbered texture to tex"},
	{"clearTexture", ( PyCFunction ) V24_Material_clearTexture, METH_O,
	 "(n) - Remove texture from numbered slot"},
	{"getScriptLinks", ( PyCFunction ) V24_Material_getScriptLinks, METH_O,
	 "(eventname) - Get a list of this material's scriptlinks (Text names) "
	 "of the given type\n"
	 "(eventname) - string: FrameChanged, Redraw or Render."},
	{"addScriptLink", ( PyCFunction ) V24_Material_addScriptLink, METH_VARARGS,
	 "(text, evt) - Add a new material scriptlink.\n"
	 "(text) - string: an existing Blender Text name;\n"
	 "(evt) string: FrameChanged, Redraw or Render."},
	{"clearScriptLinks", ( PyCFunction ) V24_Material_clearScriptLinks, METH_VARARGS,
	 "() - Delete all scriptlinks from this material.\n"
	 "([s1<,s2,s3...>]) - Delete specified scriptlinks from this material."},
	{"__copy__", ( PyCFunction ) V24_Material_copy, METH_NOARGS,
	 "() - Return a copy of the material."},
	{"copy", ( PyCFunction ) V24_Material_copy, METH_NOARGS,
	 "() - Return a copy of the material."},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/

static PyGetSetDef V24_BPy_Material_getseters[] = {
	GENERIC_LIB_GETSETATTR,
	{"add",
	 (getter)V24_Material_getAdd, (setter)V24_Material_setAdd,
	 "Strength of the add effect",
	 NULL},
	{"alpha",
	 (getter)V24_Material_getAlpha, (setter)V24_Material_setAlpha,
	 "Alpha setting ",
	 NULL},
	{"shadAlpha",
	 (getter)V24_Material_getShadAlpha, (setter)V24_Material_setShadAlpha,
	 "Shadow Alpha setting",
	 NULL},
	{"amb",
	 (getter)V24_Material_getAmb, (setter)V24_Material_setAmb,
	 "Amount of global ambient color material receives",
	 NULL},
	{"diffuseDarkness",
	 (getter)V24_Material_getDiffuseDarkness, (setter)V24_Material_setDiffuseDarkness,
	 "Material's diffuse darkness (\"Minnaert\" diffuse shader only)",
	 NULL},
	{"diffuseShader",
	 (getter)V24_Material_getDiffuseShader, (setter)V24_Material_setDiffuseShader,
	 "Diffuse shader type",
	 NULL},
	{"diffuseSize",
	 (getter)V24_Material_getDiffuseSize, (setter)V24_Material_setDiffuseSize,
	 "Material's diffuse area size (\"Toon\" diffuse shader only)",
	 NULL},
	{"diffuseSmooth",
	 (getter)V24_Material_getDiffuseSmooth, (setter)V24_Material_setDiffuseSmooth,
	 "Material's diffuse area smoothing (\"Toon\" diffuse shader only)",
	 NULL},
	{"emit",
	 (getter)V24_Material_getEmit, (setter)V24_Material_setEmit,
	 "Amount of light the material emits",
	 NULL},
	{"filter",
	 (getter)V24_Material_getFilter, (setter)V24_Material_setFilter,
	 "Amount of filtering when transparent raytrace is enabled",
	 NULL},
	{"flareBoost",
	 (getter)V24_Material_getFlareBoost, (setter)V24_Material_setFlareBoost,
	 "Flare's extra strength",
	 NULL},
	{"flareSeed",
	 (getter)V24_Material_getFlareSeed, (setter)V24_Material_setFlareSeed,
	 "Offset in the flare seed table",
	 NULL},
	{"flareSize",
	 (getter)V24_Material_getFlareSize, (setter)V24_Material_setFlareSize,
	 "Ratio of flare size to halo size",
	 NULL},
	{"fresnelDepth",
	 (getter)V24_Material_getFresnelMirr, (setter)V24_Material_setFresnelMirr,
	 "Power of Fresnel for mirror reflection",
	 NULL},
	{"fresnelDepthFac",
	 (getter)V24_Material_getFresnelMirrFac, (setter)V24_Material_setFresnelMirrFac,
	 "Blending factor for Fresnel mirror",
	 NULL},
	{"fresnelTrans",
	 (getter)V24_Material_getFresnelTrans, (setter)V24_Material_setFresnelTrans,
	 "Power of Fresnel for transparency",
	 NULL},
	{"fresnelTransFac",
	 (getter)V24_Material_getFresnelTransFac, (setter)V24_Material_setFresnelTransFac,
	 "Blending factor for Fresnel transparency",
	 NULL},
	 {"rbFriction",
	 (getter)V24_Material_getRigidBodyFriction, (setter)V24_Material_setRigidBodyFriction,
	 "Rigid Body Friction coefficient",
	 NULL},
	 {"rbRestitution",
	 (getter)V24_Material_getRigidBodyRestitution, (setter)V24_Material_setRigidBodyRestitution,
	 "Rigid Body Restitution coefficient",
	 NULL},

	{"haloSeed",
	 (getter)V24_Material_getHaloSeed, (setter)V24_Material_setHaloSeed,
	 "Randomizes halo ring dimension and line location",
	 NULL},
	{"haloSize",
	 (getter)V24_Material_getHaloSize, (setter)V24_Material_setHaloSize,
	 "Dimension of the halo",
	 NULL},
	{"hard",
	 (getter)V24_Material_getHardness, (setter)V24_Material_setHardness,
	 "Specularity hardness",
	 NULL},
	{"IOR",
	 (getter)V24_Material_getIOR, (setter)V24_Material_setIOR,
	 "Angular index of refraction for raytrace",
	 NULL},
	{"ipo",
	 (getter)V24_Material_getIpo, (setter)V24_Material_setIpo,
	 "Material Ipo data",
	 NULL},
	{"mirCol",
	 (getter)V24_Material_getMirCol, (setter)V24_Material_setMirCol,
	 "Mirror RGB color triplet",
	 NULL},
	{"mirR",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Mirror color red component",
	 (void *) EXPP_MAT_COMP_MIRR },
	{"mirG",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Mirror color green component",
	 (void *) EXPP_MAT_COMP_MIRG },
	{"mirB",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Mirror color blue component",
	 (void *) EXPP_MAT_COMP_MIRB },
	{"sssCol",
	 (getter)V24_Material_getSssCol, (setter)V24_Material_setSssCol,
	 "Sss RGB color triplet",
	 NULL},
	{"sssR",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "SSS color red component",
	 (void *) EXPP_MAT_COMP_SSSR },
	{"sssG",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "SSS color green component",
	 (void *) EXPP_MAT_COMP_SSSG },
	{"sssB",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "SSS color blue component",
	 (void *) EXPP_MAT_COMP_SSSB },
	{"mode",
	 (getter)V24_Material_getMode, (setter)V24_Material_setMode,
	 "Material mode bitmask",
	 NULL},
	{"nFlares",
	 (getter)V24_Material_getNFlares, (setter)V24_Material_setNFlares,
	 "Number of subflares with halo",
	 NULL},
	{"nLines",
	 (getter)V24_Material_getNLines, (setter)V24_Material_setNLines,
	 "Number of star-shaped lines with halo",
	 NULL},
	{"nRings",
	 (getter)V24_Material_getNRings, (setter)V24_Material_setNRings,
	 "Number of rings with halo",
	 NULL},
	{"nStars",
	 (getter)V24_Material_getNStars, (setter)V24_Material_setNStars,
	 "Number of star points with halo",
	 NULL},
	{"rayMirr",
	 (getter)V24_Material_getRayMirr, (setter)V24_Material_setRayMirr,
	 "Mirror reflection amount for raytrace",
	 NULL},
	{"rayMirrDepth",
	 (getter)V24_Material_getMirrDepth, (setter)V24_Material_setMirrDepth,
	 "Amount of raytrace inter-reflections",
	 NULL},
	{"ref",
	 (getter)V24_Material_getRef, (setter)V24_Material_setRef,
	 "Amount of reflections (for shader)",
	 NULL},
	{"refracIndex",
	 (getter)V24_Material_getRefracIndex, (setter)V24_Material_setRefracIndex,
	 "Material's Index of Refraction (applies to the \"Blinn\" Specular Shader only",
	 NULL},
	{"rgbCol",
	 (getter)V24_Material_getRGBCol, (setter)V24_Material_setRGBCol,
	 "Diffuse RGB color triplet",
	 NULL},
	{"rms",
	 (getter)V24_Material_getRms, (setter)V24_Material_setRms,
	 "Material's surface slope standard deviation (\"WardIso\" specular shader only)",
	 NULL},
	{"roughness",
	 (getter)V24_Material_getRoughness, (setter)V24_Material_setRoughness,
	 "Material's roughness (\"Oren Nayar\" diffuse shader only)",
	 NULL},
	{"spec",
	 (getter)V24_Material_getSpec, (setter)V24_Material_setSpec,
	 "Degree of specularity",
	 NULL},
	{"specCol",
	 (getter)V24_Material_getSpecCol, (setter)V24_Material_setSpecCol,
	 "Specular RGB color triplet",
	 NULL},
	{"specR",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Specular color red component",
	 (void *) EXPP_MAT_COMP_SPECR },
	{"specG",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Specular color green component",
	 (void *) EXPP_MAT_COMP_SPECG },
	{"specB",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Specular color blue component",
	 (void *) EXPP_MAT_COMP_SPECB },
	{"specTransp",
	 (getter)V24_Material_getSpecTransp, (setter)V24_Material_setSpecTransp,
	 "Makes specular areas opaque on transparent materials",
	 NULL},
	{"specShader",
	 (getter)V24_Material_getSpecShader, (setter)V24_Material_setSpecShader,
	 "Specular shader type",
	 NULL},
	{"specSize",
	 (getter)V24_Material_getSpecSize, (setter)V24_Material_setSpecSize,
	 "Material's specular area size (\"Toon\" specular shader only)",
	 NULL},
	{"specSmooth",
	 (getter)V24_Material_getSpecSmooth, (setter)V24_Material_setSpecSmooth,
	 "Sets the smoothness of specular toon area",
	 NULL},
	{"subSize",
	 (getter)V24_Material_getSubSize, (setter)V24_Material_setSubSize,
	 "Dimension of subflares, dots and circles",
	 NULL},
	{"transDepth",
	 (getter)V24_Material_getTransDepth, (setter)V24_Material_setTransDepth,
	 "Amount of refractions for raytrace",
	 NULL},
	{"translucency",
	 (getter)V24_Material_getTranslucency, (setter)V24_Material_setTranslucency,
	 "Amount of diffuse shading of the back side",
	 NULL},
	{"zOffset",
	 (getter)V24_Material_getZOffset, (setter)V24_Material_setZOffset,
	 "Artificial offset in the Z buffer (for Ztransp option)",
	 NULL},
	{"lightGroup",
	 (getter)V24_Material_getLightGroup, (setter)V24_Material_setLightGroup,
	 "Set the light group for this material",
	 NULL},
	{"R",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Diffuse color red component",
	 (void *) EXPP_MAT_COMP_R },
	{"G",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Diffuse color green component",
	 (void *) EXPP_MAT_COMP_G },
	{"B",
	 (getter)V24_Material_getColorComponent, (setter)V24_Material_setColorComponent,
	 "Diffuse color blue component",
	 (void *) EXPP_MAT_COMP_B },
	{"colorbandDiffuse",
	 (getter)V24_Material_getColorband, (setter)V24_Material_setColorband,
	 "The diffuse colorband for this material",
	 (void *) 0},
	{"colorbandSpecular",
	 (getter)V24_Material_getColorband, (setter)V24_Material_setColorband,
	 "The specular colorband for this material",
	 (void *) 1},
	
	/* SSS settings */
	{"enableSSS",
	 (getter)V24_Material_getSssEnable, (setter)V24_Material_setSssEnable,
	 "if true, SSS will be rendered for this material",
	 NULL},
	{"sssScale",
	 (getter)V24_Material_getSssScale, (setter)V24_Material_setSssScale,
	 "object scale for sss",
	 NULL},
	{"sssRadiusRed",
	 (getter)V24_Material_getSssRadius, (setter)V24_Material_setSssRadius,
	 "Mean red scattering path length",
	 (void *) 0},
	{"sssRadiusGreen",
	 (getter)V24_Material_getSssRadius, (setter)V24_Material_setSssRadius,
	 "Mean red scattering path length",
	 (void *) 1},
	{"sssRadiusBlue",
	 (getter)V24_Material_getSssRadius, (setter)V24_Material_setSssRadius,
	 "Mean red scattering path length",
	 (void *) 0},
	{"sssIOR",
	 (getter)V24_Material_getSssIOR, (setter)V24_Material_setSssIOR,
	 "index of refraction",
	 NULL},
	{"sssError",
	 (getter)V24_Material_getSssError, (setter)V24_Material_setSssError,
	 "Error",
	 NULL},
	{"sssColorBlend",
	 (getter)V24_Material_getSssColorBlend, (setter)V24_Material_setSssColorBlend,
	 "Blend factor for SSS Colors",
	 NULL},
	{"sssTextureScatter",
	 (getter)V24_Material_getSssTexScatter, (setter)V24_Material_setSssTexScatter,
	 "Texture scattering factor",
	 NULL},
	{"sssFont",
	 (getter)V24_Material_getSssFront, (setter)V24_Material_setSssFront,
	 "Front scattering weight",
	 NULL},
	{"sssBack",
	 (getter)V24_Material_getSssBack, (setter)V24_Material_setSssBack,
	 "Back scattering weight",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python V24_Material_Type callback function prototypes: */
/*****************************************************************************/
static void V24_Material_dealloc( V24_BPy_Material * self );
static int V24_Material_compare( V24_BPy_Material * a, V24_BPy_Material * b);
static PyObject *V24_Material_repr( V24_BPy_Material * self );

/*****************************************************************************/
/* Python V24_Material_Type structure definition:                                */
/*****************************************************************************/
PyTypeObject V24_Material_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Material",         /* char *tp_name; */
	sizeof( V24_BPy_Material ),     /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) V24_Material_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Material_compare,/* cmpfunc tp_compare; */
	( reprfunc ) V24_Material_repr, /* reprfunc tp_repr; */

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
	V24_BPy_Material_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Material_getseters,     /* struct PyGetSetDef *tp_getset; */
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

/*****************************************************************************/
/* Function:	V24_Material_dealloc          */
/* Description: This is a callback function for the V24_BPy_Material type. It is */
/*		the destructor function.				 */
/*****************************************************************************/
static void V24_Material_dealloc( V24_BPy_Material * self )
{
	Py_DECREF( self->col );
	Py_DECREF( self->amb );
	Py_DECREF( self->spec );
	Py_DECREF( self->mir );
	Py_DECREF( self->sss );
	PyObject_DEL( self );
}

/*****************************************************************************/
/* Function:	V24_Material_CreatePyObject		*/
/* Description: Create a new V24_BPy_Material from an  existing */
/*		 Blender material structure.	 */
/*****************************************************************************/
PyObject *V24_Material_CreatePyObject( struct Material *mat )
{
	V24_BPy_Material *pymat;
	float *col[3], *amb[3], *spec[3], *mir[3], *sss[3];

	pymat = ( V24_BPy_Material * ) PyObject_NEW( V24_BPy_Material,
						 &V24_Material_Type );

	if( !pymat )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create V24_BPy_Material object" );

	pymat->material = mat;

	col[0] = &mat->r;
	col[1] = &mat->g;
	col[2] = &mat->b;

	amb[0] = &mat->ambr;
	amb[1] = &mat->ambg;
	amb[2] = &mat->ambb;

	spec[0] = &mat->specr;
	spec[1] = &mat->specg;
	spec[2] = &mat->specb;

	mir[0] = &mat->mirr;
	mir[1] = &mat->mirg;
	mir[2] = &mat->mirb;
	
	sss[0] = &mat->sss_col[0];
	sss[1] = &mat->sss_col[1];
	sss[2] = &mat->sss_col[2];

	pymat->col = ( V24_BPy_rgbTuple * ) V24_rgbTuple_New( col );
	pymat->amb = ( V24_BPy_rgbTuple * ) V24_rgbTuple_New( amb );
	pymat->spec = ( V24_BPy_rgbTuple * ) V24_rgbTuple_New( spec );
	pymat->mir = ( V24_BPy_rgbTuple * ) V24_rgbTuple_New( mir );
	pymat->sss = ( V24_BPy_rgbTuple * ) V24_rgbTuple_New( sss );

	return ( PyObject * ) pymat;
}

/*****************************************************************************/
/* Function:		V24_Material_FromPyObject	 */
/* Description: This function returns the Blender material from the given */
/*		PyObject.	 */
/*****************************************************************************/
Material *V24_Material_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Material * ) pyobj )->material;
}

static PyObject *V24_Material_getIpo( V24_BPy_Material * self )
{
	Ipo *ipo = self->material->ipo;

	if( !ipo )
		Py_RETURN_NONE;

	return V24_Ipo_CreatePyObject( ipo );
}

static PyObject *V24_Material_getMode( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->mode );
}

static PyObject *V24_Material_getRGBCol( V24_BPy_Material * self )
{
	return V24_rgbTuple_getCol( self->col );
}

/*
static PyObject *Material_getAmbCol(V24_BPy_Material *self)
{
	return V24_rgbTuple_getCol(self->amb);
}
*/
static PyObject *V24_Material_getSpecCol( V24_BPy_Material * self )
{
	return V24_rgbTuple_getCol( self->spec );
}

static PyObject *V24_Material_getMirCol( V24_BPy_Material * self )
{
	return V24_rgbTuple_getCol( self->mir );
}

static PyObject *V24_Material_getSssCol( V24_BPy_Material * self )
{
	return V24_rgbTuple_getCol( self->sss );
}

static PyObject *V24_Material_getSpecShader( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->spec_shader );
}

static PyObject *V24_Material_getDiffuseShader( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->diff_shader );
}

static PyObject *V24_Material_getRoughness( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->roughness );
}

static PyObject *V24_Material_getSpecSize( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->param[2] );
}

static PyObject *V24_Material_getDiffuseSize( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->param[0] );
}

static PyObject *V24_Material_getSpecSmooth( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->param[3] );
}

static PyObject *V24_Material_getDiffuseSmooth( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->param[1] );
}

static PyObject *V24_Material_getDiffuseDarkness( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->darkness );
}

static PyObject *V24_Material_getRefracIndex( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->refrac );
}
	
static PyObject *V24_Material_getRms( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->rms );
}

static PyObject *V24_Material_getAmb( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->amb );
}

static PyObject *V24_Material_getEmit( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->emit );
}

static PyObject *V24_Material_getAlpha( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->alpha );
}

static PyObject *V24_Material_getShadAlpha( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->shad_alpha );
}

static PyObject *V24_Material_getRef( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->ref );
}

static PyObject *V24_Material_getSpec( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->spec );
}

static PyObject *V24_Material_getSpecTransp( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->spectra );
}

static PyObject *V24_Material_getAdd( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->add );
}

static PyObject *V24_Material_getZOffset( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->zoffs );
}

static PyObject *V24_Material_getLightGroup( V24_BPy_Material * self )
{
	return V24_Group_CreatePyObject( self->material->group );
}

static PyObject *V24_Material_getHaloSize( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->hasize );
}

static PyObject *V24_Material_getFlareSize( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->flaresize );
}

static PyObject *V24_Material_getFlareBoost( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->flareboost );
}

static PyObject *V24_Material_getSubSize( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->subsize );
}

static PyObject *V24_Material_getHaloSeed( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->seed1 );
}

static PyObject *V24_Material_getFlareSeed( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->seed2 );
}

static PyObject *V24_Material_getHardness( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->har );
}

static PyObject *V24_Material_getNFlares( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->flarec );
}

static PyObject *V24_Material_getNStars( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->starc );
}

static PyObject *V24_Material_getNLines( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->linec );
}

static PyObject *V24_Material_getNRings( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->ringc );
}

static PyObject *V24_Material_getRayMirr( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->ray_mirror );
}

static PyObject *V24_Material_getMirrDepth( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->ray_depth );
}

static PyObject *V24_Material_getFresnelMirr( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->fresnel_mir );
}

static PyObject *V24_Material_getFresnelMirrFac( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->fresnel_mir_i );
}

static PyObject *V24_Material_getFilter( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->filter );
}

static PyObject *V24_Material_getTranslucency( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->translucency );
}

static PyObject *V24_Material_getIOR( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->ang );
}

static PyObject *V24_Material_getTransDepth( V24_BPy_Material * self )
{
	return PyInt_FromLong( ( long ) self->material->ray_depth_tra );
}

static PyObject *V24_Material_getFresnelTrans( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->fresnel_tra );
}

static PyObject *V24_Material_getFresnelTransFac( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->fresnel_tra_i );
}

static PyObject* V24_Material_getRigidBodyFriction( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->friction );
}

static PyObject* V24_Material_getRigidBodyRestitution( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->reflect );
}

/* SSS */
static PyObject* V24_Material_getSssEnable( V24_BPy_Material * self )
{
	return V24_EXPP_getBitfield( &self->material->sss_flag, MA_DIFF_SSS, 'h' );
}

static PyObject* V24_Material_getSssScale( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_scale );
}

static PyObject* V24_Material_getSssRadius( V24_BPy_Material * self, void * type )
{
	return PyFloat_FromDouble( ( double ) (self->material->sss_radius[(int)type]) );
}

static PyObject* V24_Material_getSssIOR( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_ior);
}

static PyObject* V24_Material_getSssError( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_error);
}

static PyObject* V24_Material_getSssColorBlend( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_colfac);
}

static PyObject* V24_Material_getSssTexScatter( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_texfac);
}

static PyObject* V24_Material_getSssFront( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_front);
}

static PyObject* V24_Material_getSssBack( V24_BPy_Material * self )
{
	return PyFloat_FromDouble( ( double ) self->material->sss_back);
}

static PyObject *V24_Material_getTextures( V24_BPy_Material * self )
{
	int i;
	struct MTex *mtex;
	PyObject *t[MAX_MTEX];
	PyObject *tuple;

	/* build a texture list */
	for( i = 0; i < MAX_MTEX; ++i ) {
		mtex = self->material->mtex[i];

		if( mtex ) {
			t[i] = V24_MTex_CreatePyObject( mtex );
		} else {
			Py_INCREF( Py_None );
			t[i] = Py_None;
		}
	}

	/* turn the array into a tuple */
	tuple = Py_BuildValue( "NNNNNNNNNN", t[0], t[1], t[2], t[3],
			       t[4], t[5], t[6], t[7], t[8], t[9] );
	if( !tuple )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "V24_Material_getTextures: couldn't create PyTuple" );

	return tuple;
}

/*
 * this should accept a Py_None argument and just delete the Ipo link
 * (as V24_Lamp_clearIpo() does)
 */

static int V24_Material_setIpo( V24_BPy_Material * self, PyObject * value )
{
	return V24_GenericLib_assignData(value, (void **) &self->material->ipo, 0, 1, ID_IP, ID_MA);
}


/* 
 *  V24_Material_insertIpoKey( key )
 *   inserts Material IPO key at current frame
 */

static PyObject *V24_Material_insertIpoKey( V24_BPy_Material * self, PyObject * args )
{
    int key = 0, map;
    
	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int argument" ) ); 
    				
	map = texchannel_to_adrcode(self->material->texact);
	
	if(key==IPOKEY_RGB || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_COL_R, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_COL_G, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_COL_B, 0);
	}
	if(key==IPOKEY_ALPHA || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_ALPHA, 0);
	}
	if(key==IPOKEY_HALOSIZE || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_HASIZE, 0);
	}
	if(key==IPOKEY_MODE || key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_MODE, 0);
	}
	if(key==IPOKEY_ALLCOLOR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC_R, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC_G, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC_B, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_REF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_EMIT, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_AMB, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_SPEC, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_HARD, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_MODE, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_TRANSLU, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_ADD, 0);
	}
	if(key==IPOKEY_ALLMIRROR) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_RAYM, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESMIR, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESMIRI, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESTRA, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, MA_FRESTRAI, 0);
	}
	if(key==IPOKEY_OFS || key==IPOKEY_ALLMAPPING) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_OFS_X, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_OFS_Y, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_OFS_Z, 0);
	}
	if(key==IPOKEY_SIZE || key==IPOKEY_ALLMAPPING) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_SIZE_X, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_SIZE_Y, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_SIZE_Z, 0);
	}
	if(key==IPOKEY_ALLMAPPING) {
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_R, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_G, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_B, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_DVAR, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_COLF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_NORF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_VARF, 0);
		insertkey((ID *)self->material, ID_MA, NULL, NULL, map+MAP_DISP, 0);
	}

	allspace(REMAKEIPO, 0);
	V24_EXPP_allqueue(REDRAWIPO, 0);
	V24_EXPP_allqueue(REDRAWVIEW3D, 0);
	V24_EXPP_allqueue(REDRAWACTION, 0);
	V24_EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}

static int V24_Material_setMode( V24_BPy_Material * self, PyObject * value )
{
	int param;

	if( !PyInt_Check( value ) ) {
		char errstr[128];
		sprintf ( errstr , "expected int bitmask of 0x%08x", MA_MODE_MASK );
		return V24_EXPP_ReturnIntError( PyExc_TypeError, errstr );
	}
	param = PyInt_AS_LONG ( value );

	if ( ( param & MA_MODE_MASK ) != param )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
						"invalid bit(s) set in mask" );

	self->material->mode &= ( MA_RAMP_COL | MA_RAMP_SPEC );
	self->material->mode |= param & ~( MA_RAMP_COL | MA_RAMP_SPEC );

	return 0;
}

static int V24_Material_setRGBCol( V24_BPy_Material * self, PyObject * value )
{
	return V24_rgbTuple_setCol( self->col, value );
}

/*
static PyObject *Material_setAmbCol (V24_BPy_Material *self, PyObject * value )
{
	return V24_rgbTuple_setCol(self->amb, value);
}
*/

static int V24_Material_setSpecCol( V24_BPy_Material * self, PyObject * value )
{
	return V24_rgbTuple_setCol( self->spec, value );
}

static int V24_Material_setMirCol( V24_BPy_Material * self, PyObject * value )
{
	return V24_rgbTuple_setCol( self->mir, value );
}

static int V24_Material_setSssCol( V24_BPy_Material * self, PyObject * value )
{
	return V24_rgbTuple_setCol( self->sss, value );
}

static int V24_Material_setColorComponent( V24_BPy_Material * self, PyObject * value,
							void * closure )
{
	float param;

	if( !PyNumber_Check ( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected float argument in [0.0,1.0]" );

	param = (float)PyFloat_AsDouble( value );
	param = V24_EXPP_ClampFloat( param, EXPP_MAT_COL_MIN, EXPP_MAT_COL_MAX );

	switch ( (int)closure ) {
	case EXPP_MAT_COMP_R:
		self->material->r = param;
		return 0;
	case EXPP_MAT_COMP_G:
		self->material->g = param;
		return 0;
	case EXPP_MAT_COMP_B:
		self->material->b = param;
		return 0;
	case EXPP_MAT_COMP_SPECR:
		self->material->specr = param;
		return 0;
	case EXPP_MAT_COMP_SPECG:
		self->material->specg = param;
		return 0;
	case EXPP_MAT_COMP_SPECB:
		self->material->specb = param;
		return 0;
	case EXPP_MAT_COMP_MIRR:
		self->material->mirr = param;
		return 0;
	case EXPP_MAT_COMP_MIRG:
		self->material->mirg = param;
		return 0;
	case EXPP_MAT_COMP_MIRB:
		self->material->mirb = param;
		return 0;
	case EXPP_MAT_COMP_SSSR:
		self->material->sss_col[0] = param;
		return 0;
	case EXPP_MAT_COMP_SSSG:
		self->material->sss_col[1] = param;
		return 0;
	case EXPP_MAT_COMP_SSSB:
		self->material->sss_col[2] = param;
		return 0;
	}
	return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"unknown color component specified" );
}

/*#define setFloatWrapper(val, min, max) {return V24_EXPP_setFloatClamped ( value, &self->material->#val, #min, #max}*/

static int V24_Material_setAmb( V24_BPy_Material * self, PyObject * value )
{ 
	return V24_EXPP_setFloatClamped ( value, &self->material->amb,
								EXPP_MAT_AMB_MIN,
					       		EXPP_MAT_AMB_MAX );
}

static int V24_Material_setEmit( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->emit,
								EXPP_MAT_EMIT_MIN,
								EXPP_MAT_EMIT_MAX );
}

static int V24_Material_setSpecTransp( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->spectra,
								EXPP_MAT_SPECTRA_MIN,
								EXPP_MAT_SPECTRA_MAX );
}

static int V24_Material_setAlpha( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->alpha,
								EXPP_MAT_ALPHA_MIN,
								EXPP_MAT_ALPHA_MAX );
}

static int V24_Material_setShadAlpha( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->shad_alpha,
								EXPP_MAT_ALPHA_MIN,
								EXPP_MAT_ALPHA_MAX );
}

static int V24_Material_setRef( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->ref,
								EXPP_MAT_REF_MIN,
								EXPP_MAT_REF_MAX );
}

static int V24_Material_setSpec( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->spec,
								EXPP_MAT_SPEC_MIN,
								EXPP_MAT_SPEC_MAX );
}

static int V24_Material_setZOffset( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->zoffs,
		   						EXPP_MAT_ZOFFS_MIN,
								EXPP_MAT_ZOFFS_MAX );
}

static int V24_Material_setLightGroup( V24_BPy_Material * self, PyObject * value )
{
	return V24_GenericLib_assignData(value, (void **) &self->material->group, NULL, 1, ID_GR, 0);
}

static int V24_Material_setAdd( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->add,
								EXPP_MAT_ADD_MIN,
								EXPP_MAT_ADD_MAX );
}

static int V24_Material_setHaloSize( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->hasize,
		   						EXPP_MAT_HALOSIZE_MIN,
								EXPP_MAT_HALOSIZE_MAX );
}

static int V24_Material_setFlareSize( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->flaresize,
								EXPP_MAT_FLARESIZE_MIN,
								EXPP_MAT_FLARESIZE_MAX );
}

static int V24_Material_setFlareBoost( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->flareboost,
								EXPP_MAT_FLAREBOOST_MIN,
								EXPP_MAT_FLAREBOOST_MAX );
}

static int V24_Material_setSubSize( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->subsize,
								EXPP_MAT_SUBSIZE_MIN,
								EXPP_MAT_SUBSIZE_MAX );
}

static int V24_Material_setHaloSeed( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->seed1,
								EXPP_MAT_HALOSEED_MIN,
								EXPP_MAT_HALOSEED_MAX, 'b' );
}

static int V24_Material_setFlareSeed( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->seed2,
								EXPP_MAT_FLARESEED_MIN,
								EXPP_MAT_FLARESEED_MAX, 'b' );
}

static int V24_Material_setHardness( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->har,
		   						EXPP_MAT_HARD_MIN,
								EXPP_MAT_HARD_MAX, 'h' );
}

static int V24_Material_setNFlares( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->flarec,
								EXPP_MAT_NFLARES_MIN,
								EXPP_MAT_NFLARES_MAX, 'h' );
}

static int V24_Material_setNStars( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->starc,
								EXPP_MAT_NSTARS_MIN,
								EXPP_MAT_NSTARS_MAX, 'h' );
}

static int V24_Material_setNLines( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->linec,
								EXPP_MAT_NLINES_MIN,
								EXPP_MAT_NLINES_MAX, 'h' );
}

static int V24_Material_setNRings( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->ringc,
		   						EXPP_MAT_NRINGS_MIN,
								EXPP_MAT_NRINGS_MAX, 'h' );
}

static int V24_Material_setRayMirr( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->ray_mirror,
								EXPP_MAT_RAYMIRR_MIN,
								EXPP_MAT_RAYMIRR_MAX );
}

static int V24_Material_setMirrDepth( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->ray_depth,
								EXPP_MAT_MIRRDEPTH_MIN,
								EXPP_MAT_MIRRDEPTH_MAX, 'h' );
}

static int V24_Material_setFresnelMirr( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->fresnel_mir,
								EXPP_MAT_FRESNELMIRR_MIN,
								EXPP_MAT_FRESNELMIRR_MAX );
}

static int V24_Material_setFresnelMirrFac( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->fresnel_mir_i,
								EXPP_MAT_FRESNELMIRRFAC_MIN,
								EXPP_MAT_FRESNELMIRRFAC_MAX );
}

static int V24_Material_setIOR( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->ang,
								EXPP_MAT_IOR_MIN,
								EXPP_MAT_IOR_MAX );
}

static int V24_Material_setTransDepth( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->material->ray_depth_tra,
								EXPP_MAT_TRANSDEPTH_MIN,
								EXPP_MAT_TRANSDEPTH_MAX, 'h' );
}

static int V24_Material_setFresnelTrans( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->fresnel_tra,
								EXPP_MAT_FRESNELTRANS_MIN,
								EXPP_MAT_FRESNELTRANS_MAX );
}

static int V24_Material_setFresnelTransFac( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->fresnel_tra_i,
								EXPP_MAT_FRESNELTRANSFAC_MIN,
								EXPP_MAT_FRESNELTRANSFAC_MAX );
}

static int V24_Material_setRigidBodyFriction( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->friction,
								0.f,
								100.f );
}

static int V24_Material_setRigidBodyRestitution( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->reflect,
								0.f,
								1.f );
}




static int V24_Material_setSpecShader( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueRange( value, &self->material->spec_shader,
								MA_SPEC_COOKTORR,
								MA_SPEC_WARDISO, 'h' );
}

static int V24_Material_setDiffuseShader( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setIValueRange( value, &self->material->diff_shader,
								MA_DIFF_LAMBERT,
								MA_DIFF_MINNAERT, 'h' );
}

static int V24_Material_setRoughness( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->roughness,
								EXPP_MAT_ROUGHNESS_MIN,
								EXPP_MAT_ROUGHNESS_MAX );
}

static int V24_Material_setSpecSize( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->param[2],
								EXPP_MAT_SPECSIZE_MIN,
								EXPP_MAT_SPECSIZE_MAX );
}

static int V24_Material_setDiffuseSize( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->param[0],
								EXPP_MAT_DIFFUSESIZE_MIN,
								EXPP_MAT_DIFFUSESIZE_MAX );
}

static int V24_Material_setSpecSmooth( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->param[3],
								EXPP_MAT_SPECSMOOTH_MIN,
								EXPP_MAT_SPECSMOOTH_MAX );
}

static int V24_Material_setDiffuseSmooth( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->param[1],
								EXPP_MAT_DIFFUSESMOOTH_MIN,
								EXPP_MAT_DIFFUSESMOOTH_MAX );
}

static int V24_Material_setDiffuseDarkness( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->darkness,
								EXPP_MAT_DIFFUSE_DARKNESS_MIN,
								EXPP_MAT_DIFFUSE_DARKNESS_MAX );
}

static int V24_Material_setRefracIndex( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->refrac,
								EXPP_MAT_REFRACINDEX_MIN,
								EXPP_MAT_REFRACINDEX_MAX );
}

static int V24_Material_setRms( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->rms,
								EXPP_MAT_RMS_MIN,
								EXPP_MAT_RMS_MAX );
}

static int V24_Material_setFilter( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->filter,
								EXPP_MAT_FILTER_MIN,
								EXPP_MAT_FILTER_MAX );
}

static int V24_Material_setTranslucency( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->translucency,
								EXPP_MAT_TRANSLUCENCY_MIN,
								EXPP_MAT_TRANSLUCENCY_MAX );
}

/* SSS */
static int V24_Material_setSssEnable( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setBitfield( value, &self->material->sss_flag, MA_DIFF_SSS, 'h' );
}

static int V24_Material_setSssScale( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_scale,
								EXPP_MAT_SSS_SCALE_MIN,
								EXPP_MAT_SSS_SCALE_MAX);
}

static int V24_Material_setSssRadius( V24_BPy_Material * self, PyObject * value, void *type )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_radius[(int)type],
								EXPP_MAT_SSS_RADIUS_MIN,
								EXPP_MAT_SSS_RADIUS_MAX);
}

static int V24_Material_setSssIOR( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_ior,
								EXPP_MAT_SSS_IOR_MIN,
								EXPP_MAT_SSS_IOR_MAX);
}

static int V24_Material_setSssError( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_error,
								EXPP_MAT_SSS_IOR_MIN,
								EXPP_MAT_SSS_IOR_MAX);
}

static int V24_Material_setSssColorBlend( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_colfac,
								0.0,
								1.0);
}

static int V24_Material_setSssTexScatter( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_texfac,
								0.0,
								1.0);
}

static int V24_Material_setSssFront( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_front,
								EXPP_MAT_SSS_FRONT_MIN,
								EXPP_MAT_SSS_FRONT_MAX);
}

static int V24_Material_setSssBack( V24_BPy_Material * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->material->sss_back,
								EXPP_MAT_SSS_BACK_MIN,
								EXPP_MAT_SSS_BACK_MAX);
}




static PyObject *V24_Material_setTexture( V24_BPy_Material * self, PyObject * args )
{
	int texnum;
	PyObject *pytex;
	Tex *bltex;
	int texco = TEXCO_ORCO, mapto = MAP_COL;

	if( !PyArg_ParseTuple( args, "iO!|ii", &texnum, &V24_Texture_Type, &pytex,
			       &texco, &mapto ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected int in [0,9] and Texture" );
	if( ( texnum < 0 ) || ( texnum >= MAX_MTEX ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected int in [0,9] and Texture" );

	bltex = V24_Texture_FromPyObject( pytex );

	if( !self->material->mtex[texnum] ) {
		/* there isn't an mtex for this slot so we need to make one */
		self->material->mtex[texnum] = add_mtex(  );
	} else {
		/* we already had a texture here so deal with the old one first */
		self->material->mtex[texnum]->tex->id.us--;
	}

	self->material->mtex[texnum]->tex = bltex;
	id_us_plus( &bltex->id );
	self->material->mtex[texnum]->texco = (short)texco;
	self->material->mtex[texnum]->mapto = (short)mapto;

	Py_RETURN_NONE;
}

static PyObject *V24_Material_clearTexture( V24_BPy_Material * self, PyObject * value )
{
	int texnum = (int)PyInt_AsLong(value);
	struct MTex *mtex;
	/* non ints will be -1 */
	if( ( texnum < 0 ) || ( texnum >= MAX_MTEX ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected int in [0,9]" );

	mtex = self->material->mtex[texnum];
	if( mtex ) {
		if( mtex->tex )
			mtex->tex->id.us--;
		MEM_freeN( mtex );
		self->material->mtex[texnum] = NULL;
	}

	Py_RETURN_NONE;
}

/* mat.addScriptLink */
static PyObject *V24_Material_addScriptLink( V24_BPy_Material * self, PyObject * args )
{
	Material *mat = self->material;
	ScriptLink *slink = NULL;

	slink = &( mat )->scriptlink;

	return V24_EXPP_addScriptLink( slink, args, 0 );
}

/* mat.clearScriptLinks */
static PyObject *V24_Material_clearScriptLinks(V24_BPy_Material *self, PyObject *args )
{
	Material *mat = self->material;
	ScriptLink *slink = NULL;

	slink = &( mat )->scriptlink;

	return V24_EXPP_clearScriptLinks( slink, args );
}

/* mat.getScriptLinks */
static PyObject *V24_Material_getScriptLinks( V24_BPy_Material * self,
					  PyObject * value )
{
	Material *mat = self->material;
	ScriptLink *slink = NULL;
	PyObject *ret = NULL;

	slink = &( mat )->scriptlink;

	/* can't this just return?  EXP_getScriptLinks() returns a PyObject*
	 * or NULL anyway */

	ret = V24_EXPP_getScriptLinks( slink, value, 0 );

	if( ret )
		return ret;
	else
		return NULL;
}

/* mat.__copy__ */
static PyObject *V24_Material_copy( V24_BPy_Material * self )
{
	V24_BPy_Material *pymat; /* for Material Data object wrapper in Python */
	Material *blmat; /* for actual Material Data we create in Blender */
	
	blmat = copy_material( self->material );	/* first copy the Material Data in Blender */

	if( blmat )		/* now create the wrapper obj in Python */
		pymat = ( V24_BPy_Material * ) V24_Material_CreatePyObject( blmat );
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"couldn't create Material Data in Blender" ) );

	blmat->id.us = 0;	/* was incref'ed by add_material() above */

	if( pymat == NULL )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create Material Data object" ) );

	return ( PyObject * ) pymat;
}

/* mat_a==mat_b or mat_a!=mat_b*/
static int V24_Material_compare( V24_BPy_Material * a, V24_BPy_Material * b )
{
	return ( a->material == b->material) ? 0 : -1;
}

/*****************************************************************************/
/* Function:	V24_Material_repr	 */
/* Description: This is a callback function for the V24_BPy_Material type. It  */
/*		 builds a meaninful string to represent material objects.   */
/*****************************************************************************/
static PyObject *V24_Material_repr( V24_BPy_Material * self )
{
	return PyString_FromFormat( "[Material \"%s\"]",
				    self->material->id.name + 2 );
}

/*****************************************************************************/
/* These functions are used here and in in Texture.c						*/
/*****************************************************************************/
PyObject *V24_EXPP_PyList_fromColorband( ColorBand *coba )
{
	short i;
	PyObject *cbls;
	PyObject *colls;
	
	if (!coba)
		return PyList_New( 0 );
	
	cbls = PyList_New( coba->tot );
	
	for (i=0; i < coba->tot; i++) {
		colls = PyList_New( 5 );
		PyList_SET_ITEM( colls, 0, PyFloat_FromDouble(coba->data[i].r) );
		PyList_SET_ITEM( colls, 1, PyFloat_FromDouble(coba->data[i].g) );
		PyList_SET_ITEM( colls, 2, PyFloat_FromDouble(coba->data[i].b) );
		PyList_SET_ITEM( colls, 3, PyFloat_FromDouble(coba->data[i].a) );
		PyList_SET_ITEM( colls, 4, PyFloat_FromDouble(coba->data[i].pos) );
		PyList_SET_ITEM(cbls, i, colls);
	}
	return cbls;
}

/* make sure you coba is not none before calling this */
int V24_EXPP_Colorband_fromPyList( ColorBand **coba, PyObject * value )
{
	short totcol, i;
	PyObject *colseq;
	PyObject *pyflt;
	float f;
	
	if ( !PySequence_Check( value )  )
		return ( V24_EXPP_ReturnIntError( PyExc_TypeError,
				"Colorband must be a sequence" ) );
	
	totcol = PySequence_Size(value);
	if ( totcol > 31)
		return ( V24_EXPP_ReturnIntError( PyExc_ValueError,
				"Colorband must be between 1 and 31 in length" ) );
	
	if (totcol==0) {
		MEM_freeN(*coba);
		*coba = NULL;
		return 0;
	}
	
	if (!*coba)
		*coba = MEM_callocN( sizeof(ColorBand), "colorband");
	
	for (i=0; i<totcol; i++) {
		colseq = PySequence_GetItem( value, i );
		if ( !PySequence_Check( colseq ) || PySequence_Size( colseq ) != 5) {
			Py_DECREF ( colseq );
			return ( V24_EXPP_ReturnIntError( PyExc_ValueError,
				"Colorband colors must be sequences of 5 floats" ) );
		}
		for (i=0; i<5; i++) {
			pyflt = PySequence_GetItem( colseq, i );
			if (!PyNumber_Check(pyflt)) {
				return ( V24_EXPP_ReturnIntError( PyExc_ValueError,
					"Colorband colors must be sequences of 5 floats" ) );
				Py_DECREF ( pyflt );
				Py_DECREF ( colseq );
			}
			Py_DECREF ( pyflt );
		}
		Py_DECREF ( colseq );
	}
	
	/* ok, continue - should check for 5 floats, will ignore non floats for now */
	(*coba)->tot = totcol;
	for (i=0; i<totcol; i++) {
		colseq = PySequence_GetItem( value, i );
		
		pyflt = PySequence_GetItem( colseq, 0 ); 
		f = (float)PyFloat_AsDouble( pyflt );
		CLAMP(f, 0.0, 1.0);
		(*coba)->data[i].r = f;
		Py_DECREF ( pyflt );
		
		pyflt = PySequence_GetItem( colseq, 1 ); 
		f = (float)PyFloat_AsDouble( pyflt );
		CLAMP(f, 0.0, 1.0);
		(*coba)->data[i].g = f;
		Py_DECREF ( pyflt );
		
		pyflt = PySequence_GetItem( colseq, 2 ); 
		f = (float)PyFloat_AsDouble( pyflt );
		CLAMP(f, 0.0, 1.0);
		(*coba)->data[i].b = f;
		Py_DECREF ( pyflt );
		
		pyflt = PySequence_GetItem( colseq, 3 ); 
		f = (float)PyFloat_AsDouble( pyflt );
		CLAMP(f, 0.0, 1.0);
		(*coba)->data[i].a = f;
		Py_DECREF ( pyflt );
		
		pyflt = PySequence_GetItem( colseq, 4 ); 
		f = (float)PyFloat_AsDouble( pyflt );
		CLAMP(f, 0.0, 1.0);
		(*coba)->data[i].pos = f;
		Py_DECREF ( pyflt );
		
		Py_DECREF ( colseq );
	}
	return 0;
}


/*****************************************************************************/
/* These functions are used in NMesh.c and Object.c	 */
/*****************************************************************************/
PyObject *V24_EXPP_PyList_fromMaterialList( Material ** matlist, int len, int all )
{
	PyObject *list;
	int i;

	list = PyList_New( 0 );
	if( !matlist )
		return list;

	for( i = 0; i < len; i++ ) {
		Material *mat = matlist[i];
		PyObject *ob;

		if( mat ) {
			ob = V24_Material_CreatePyObject( mat );
			PyList_Append( list, ob );
			Py_DECREF( ob );	/* because Append increfs */
		} else if( all ) {	/* return NULL mats (empty slots) as Py_None */
			PyList_Append( list, Py_None );
		}
	}

	return list;
}

Material **V24_EXPP_newMaterialList_fromPyList( PyObject * list )
{
	int i, len;
	V24_BPy_Material *pymat = 0;
	Material *mat;
	Material **matlist;

	len = PySequence_Length( list );
	if( len > 16 )
		len = 16;
	else if( len <= 0 )
		return NULL;

	matlist = V24_EXPP_newMaterialList( len );

	for( i = 0; i < len; i++ ) {

		pymat = ( V24_BPy_Material * ) PySequence_GetItem( list, i );

		if( BPy_Material_Check( ( PyObject * ) pymat ) ) {
			mat = pymat->material;
			matlist[i] = mat;
		} else if( ( PyObject * ) pymat == Py_None ) {
			matlist[i] = NULL;
		} else {	/* error; illegal type in material list */
			Py_DECREF( pymat );
			MEM_freeN( matlist );
			return NULL;
		}

		Py_DECREF( pymat );
	}

	return matlist;
}

Material **V24_EXPP_newMaterialList( int len )
{
	Material **matlist =
		( Material ** ) MEM_mallocN( len * sizeof( Material * ),
					     "MaterialList" );

	return matlist;
}

int V24_EXPP_releaseMaterialList( Material ** matlist, int len )
{
	int i;
	Material *mat;

	if( ( len < 0 ) || ( len > MAXMAT ) ) {
		printf( "illegal matindex!\n" );
		return 0;
	}

	for( i = 0; i < len; i++ ) {
		mat = matlist[i];
		if( mat ) {
			if( ( ( ID * ) mat )->us > 0 )
				( ( ID * ) mat )->us--;
			else
				printf( "FATAL: material usage=0: %s",
					( ( ID * ) mat )->name );
		}
	}
	MEM_freeN( matlist );

	return 1;
}

/** expands pointer array of length 'oldsize' to length 'newsize'.
	* A pointer to the (void *) array must be passed as first argument 
	* The array pointer content can be NULL, in this case a new array of length
	* 'newsize' is created.
	*/

static int expandPtrArray( void **p, int oldsize, int newsize )
{
	void *newarray;

	if( newsize < oldsize ) {
		return 0;
	}
	newarray = MEM_callocN( sizeof( void * ) * newsize, "PtrArray" );
	if( *p ) {
		memcpy( newarray, *p, sizeof( void * ) * oldsize );
		MEM_freeN( *p );
	}
	*p = newarray;
	return 1;
}

int V24_EXPP_synchronizeMaterialLists( Object * object )
{
	Material ***p_dataMaterials = give_matarar( object );
	short *nmaterials = give_totcolp( object );
	int result = 0;

	if( object->totcol > *nmaterials ) {
		/* More object mats than data mats */
		result = expandPtrArray( ( void * ) p_dataMaterials,
					 *nmaterials, object->totcol );
		*nmaterials = object->totcol;
	} else {
		if( object->totcol < *nmaterials ) {
			/* More data mats than object mats */
			result = expandPtrArray( ( void * ) &object->mat,
						 object->totcol, *nmaterials );
			object->totcol = (char)*nmaterials;
		}
	}			/* else no synchronization needed, they are of equal length */

	return result;		/* 1 if changed, 0 otherwise */
}

void V24_EXPP_incr_mats_us( Material ** matlist, int len )
{
	int i;
	Material *mat;

	if( len <= 0 )
		return;

	for( i = 0; i < len; i++ ) {
		mat = matlist[i];
		if( mat )
			mat->id.us++;
	}

	return;
}

static PyObject *V24_Material_getColorComponent( V24_BPy_Material * self, 
							void * closure )
{
	switch ( (int)closure ) {
	case EXPP_MAT_COMP_R:
		return PyFloat_FromDouble( ( double ) self->material->r );
	case EXPP_MAT_COMP_G:
		return PyFloat_FromDouble( ( double ) self->material->g );
	case EXPP_MAT_COMP_B:
		return PyFloat_FromDouble( ( double ) self->material->b );
	case EXPP_MAT_COMP_SPECR:
		return PyFloat_FromDouble( ( double ) self->material->specr );
	case EXPP_MAT_COMP_SPECG:
		return PyFloat_FromDouble( ( double ) self->material->specg );
	case EXPP_MAT_COMP_SPECB:
		return PyFloat_FromDouble( ( double ) self->material->specb );
	case EXPP_MAT_COMP_MIRR:
		return PyFloat_FromDouble( ( double ) self->material->mirr );
	case EXPP_MAT_COMP_MIRG:
		return PyFloat_FromDouble( ( double ) self->material->mirg );
	case EXPP_MAT_COMP_MIRB:
		return PyFloat_FromDouble( ( double ) self->material->mirb );
	case EXPP_MAT_COMP_SSSR:
		return PyFloat_FromDouble( ( double ) self->material->sss_col[0] );
	case EXPP_MAT_COMP_SSSG:
		return PyFloat_FromDouble( ( double ) self->material->sss_col[1] );
	case EXPP_MAT_COMP_SSSB:
		return PyFloat_FromDouble( ( double ) self->material->sss_col[2] );
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"unknown color component specified" );
	}
}

static PyObject *V24_Material_getColorband( V24_BPy_Material * self, void * type)
{
	switch( (long)type ) {
    case 0:	/* these are backwards, but that how it works */
		return V24_EXPP_PyList_fromColorband( self->material->ramp_col );
    case 1:
		return V24_EXPP_PyList_fromColorband( self->material->ramp_spec );
	}
	Py_RETURN_NONE;
}

int V24_Material_setColorband( V24_BPy_Material * self, PyObject * value, void * type)
{
	switch( (long)type ) {
    case 0:	/* these are backwards, but that how it works */
		return V24_EXPP_Colorband_fromPyList( &self->material->ramp_col, value );
    case 1:
		return V24_EXPP_Colorband_fromPyList( &self->material->ramp_spec, value );
	}
	return 0;
}

/* #####DEPRECATED###### */

static PyObject *V24_Matr_oldsetAdd( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setAdd );
}

static PyObject *V24_Matr_oldsetAlpha( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setAlpha );
}

static PyObject *V24_Matr_oldsetAmb( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setAmb );
}

static PyObject *V24_Matr_oldsetDiffuseDarkness( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setDiffuseDarkness );
}

static PyObject *V24_Matr_oldsetDiffuseShader( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setDiffuseShader );
}

static PyObject *V24_Matr_oldsetDiffuseSize( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setDiffuseSize );
}

static PyObject *V24_Matr_oldsetDiffuseSmooth( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setDiffuseSmooth );
}

static PyObject *V24_Matr_oldsetEmit( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setEmit );
}

static PyObject *V24_Matr_oldsetFilter( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFilter );
}

static PyObject *V24_Matr_oldsetFlareBoost( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFlareBoost );
}

static PyObject *V24_Matr_oldsetFlareSeed( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFlareSeed );
}

static PyObject *V24_Matr_oldsetFlareSize( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFlareSize );
}

static PyObject *V24_Matr_oldsetFresnelMirr( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFresnelMirr );
}

static PyObject *V24_Matr_oldsetFresnelMirrFac( V24_BPy_Material * self,
					     PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFresnelMirrFac );
}

static PyObject *V24_Matr_oldsetFresnelTrans( V24_BPy_Material * self,
					   PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFresnelTrans );
}

static PyObject *V24_Matr_oldsetFresnelTransFac( V24_BPy_Material * self,
					      PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setFresnelTransFac );
}

static PyObject *V24_Matr_oldsetHaloSeed( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setHaloSeed );
}

static PyObject *V24_Matr_oldsetHaloSize( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setHaloSize );
}

static PyObject *V24_Matr_oldsetHardness( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setHardness );
}

static PyObject *V24_Matr_oldsetIOR( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setIOR );
}

static PyObject *V24_Matr_oldsetNFlares( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setNFlares );
}

static PyObject *V24_Matr_oldsetNLines( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setNLines );
}

static PyObject *V24_Matr_oldsetNRings( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setNRings );
}

static PyObject *V24_Matr_oldsetNStars( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setNStars );
}

static PyObject *V24_Matr_oldsetRayMirr( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setRayMirr );
}

static PyObject *V24_Matr_oldsetRoughness( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setRoughness );
}

static PyObject *V24_Matr_oldsetMirrDepth( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setMirrDepth );
}

static PyObject *V24_Matr_oldsetRef( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setRef );
}

static PyObject *V24_Matr_oldsetRefracIndex( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setRefracIndex );
}

static PyObject *V24_Matr_oldsetRms( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setRms );
}

static PyObject *V24_Matr_oldsetSpec( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setSpec );
}

static PyObject *V24_Matr_oldsetSpecShader( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setSpecShader );
}

static PyObject *V24_Matr_oldsetSpecSize( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setSpecSize );
}

static PyObject *V24_Matr_oldsetSpecSmooth( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setSpecSmooth );
}

static PyObject *V24_Matr_oldsetSpecTransp( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setSpecTransp );
}

static PyObject *V24_Matr_oldsetSubSize( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setSubSize );
}

static PyObject *V24_Matr_oldsetTranslucency( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setTranslucency );
}

static PyObject *V24_Matr_oldsetTransDepth( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setTransDepth );
}

static PyObject *V24_Matr_oldsetZOffset( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setZOffset );
}

static PyObject *V24_Matr_oldsetRGBCol( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Material_setRGBCol );
}

static PyObject *V24_Matr_oldsetSpecCol( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Material_setSpecCol );
}

static PyObject *V24_Matr_oldsetMirCol( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapperTuple( (void *)self, args,
			(setter)V24_Material_setMirCol );
}


/* Possible modes are traceable, shadow, shadeless, wire, vcolLight,
 * vcolPaint, halo, ztransp, zinvert, haloRings, env, haloLines,
 * onlyShadow, xalpha, star, faceTexture, haloTex, haloPuno, noMist,
 * haloShaded, haloFlare */

static PyObject *V24_Matr_oldsetMode( V24_BPy_Material * self, PyObject * args )
{
	unsigned int i, flag = 0, ok = 0;
	PyObject *value, *error;
	char *m[28] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL
	};

	/* 
	 * check for a single integer argument; do a quick check for now
	 * that the value is not larger than double the highest flag bit
	 */

	if ( (PySequence_Size( args ) == 1)
		    && PyInt_Check ( PyTuple_GET_ITEM ( args , 0 ) )
		    && PyArg_ParseTuple( args, "i", &flag ) 
			&& (flag & MA_MODE_MASK ) == flag ) {
			ok = 1;

	/*
	 * check for either an empty argument list, or up to 28 strings
	 */

	} else if( PyArg_ParseTuple( args, "|ssssssssssssssssssssssssssss",
			       &m[0], &m[1], &m[2], &m[3], &m[4], &m[5], &m[6],
			       &m[7], &m[8], &m[9], &m[10], &m[11], &m[12],
			       &m[13], &m[14], &m[15], &m[16], &m[17], &m[18],
			       &m[19], &m[20], &m[21], &m[22], &m[23], &m[24],
			       &m[25], &m[26], &m[27] ) ) {
		for( i = 0; i < 28; i++ ) {
			if( m[i] == NULL )
				break;
			if( strcmp( m[i], "Traceable" ) == 0 )
				flag |= MA_TRACEBLE;
			else if( strcmp( m[i], "Shadow" ) == 0 )
				flag |= MA_SHADOW;
			else if( strcmp( m[i], "Shadeless" ) == 0 )
				flag |= MA_SHLESS;
			else if( strcmp( m[i], "Wire" ) == 0 )
				flag |= MA_WIRE;
			else if( strcmp( m[i], "VColLight" ) == 0 )
				flag |= MA_VERTEXCOL;
			else if( strcmp( m[i], "VColPaint" ) == 0 )
				flag |= MA_VERTEXCOLP;
			else if( strcmp( m[i], "Halo" ) == 0 )
				flag |= MA_HALO;
			else if( strcmp( m[i], "ZTransp" ) == 0 )
				flag |= MA_ZTRA;
			else if( strcmp( m[i], "ZInvert" ) == 0 )
				flag |= MA_ZINV;
			else if( strcmp( m[i], "HaloRings" ) == 0 )
				flag |= MA_HALO_RINGS;
			else if( strcmp( m[i], "HaloLines" ) == 0 )
				flag |= MA_HALO_LINES;
			else if( strcmp( m[i], "OnlyShadow" ) == 0 )
				flag |= MA_ONLYSHADOW;
			else if( strcmp( m[i], "HaloXAlpha" ) == 0 )
				flag |= MA_HALO_XALPHA;
			else if( strcmp( m[i], "HaloStar" ) == 0 )
				flag |= MA_STAR;
			else if( strcmp( m[i], "TexFace" ) == 0 )
				flag |= MA_FACETEXTURE;
			else if( strcmp( m[i], "HaloTex" ) == 0 )
				flag |= MA_HALOTEX;
			else if( strcmp( m[i], "HaloPuno" ) == 0 )
				flag |= MA_HALOPUNO;
			else if( strcmp( m[i], "NoMist" ) == 0 )
				flag |= MA_NOMIST;
			else if( strcmp( m[i], "HaloShaded" ) == 0 )
				flag |= MA_HALO_SHADE;
			else if( strcmp( m[i], "HaloFlare" ) == 0 )
				flag |= MA_HALO_FLARE;
			else if( strcmp( m[i], "Radio" ) == 0 )
				flag |= MA_RADIO;
			/* ** Mirror ** */
			else if( strcmp( m[i], "RayMirr" ) == 0 )
				flag |= MA_RAYMIRROR;
			else if( strcmp( m[i], "ZTransp" ) == 0 )
				flag |= MA_ZTRA;
			else if( strcmp( m[i], "RayTransp" ) == 0 )
				flag |= MA_RAYTRANSP;
			else if( strcmp( m[i], "OnlyShadow" ) == 0 )
				flag |= MA_ONLYSHADOW;
			else if( strcmp( m[i], "NoMist" ) == 0 )
				flag |= MA_NOMIST;
			else if( strcmp( m[i], "Env" ) == 0 )
				flag |= MA_ENV;
			else
				return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
								"unknown Material mode argument" ) );
		}
		ok = 1;
	}

	/* if neither input method worked, then throw an exception */

	if ( ok == 0 )
		return ( V24_EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected nothing, an integer or up to 22 string argument(s)" ) );
	/* build tuple, call wrapper */

	value = Py_BuildValue("(i)", flag);
	error = V24_EXPP_setterWrapper( (void *)self, value, (setter)V24_Material_setMode );
	Py_DECREF ( value );
	return error;
}

static PyObject *V24_Matr_oldsetIpo( V24_BPy_Material * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args, (setter)V24_Material_setIpo );
}

/*
 * clearIpo() returns True/False depending on whether material has an Ipo
 */

static PyObject *V24_Material_clearIpo( V24_BPy_Material * self )
{
	/* if Ipo defined, delete it and return true */

	if( self->material->ipo ) {
		PyObject *value = Py_BuildValue( "(O)", Py_None );
		V24_EXPP_setterWrapper( (void *)self, value, (setter)V24_Material_setIpo );
		Py_DECREF ( value );
		return V24_EXPP_incr_ret_True();
	}
	return V24_EXPP_incr_ret_False(); /* no ipo found */
}

