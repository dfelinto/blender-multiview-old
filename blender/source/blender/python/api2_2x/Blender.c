/* 
 * $Id$
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
 * Contributor(s): Michel Selten, Willian P. Germano, Joseph Gilbert,
 * Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/
struct ID; /*keep me up here */

#include "Blender.h" /*This must come first */

/* for open, close in V24_Blender_Load */
#include <fcntl.h>
#include "BDR_editobject.h"	/* exit_editmode() */
#include "BIF_usiblender.h"
#include "BLI_blenlib.h"
#include "BLO_writefile.h"
#include "BKE_blender.h"
#include "BKE_exotic.h"
#include "BKE_global.h"
#include "BKE_packedFile.h"
#include "BKE_utildefines.h"
#include "BKE_object.h"
#include "BKE_scene.h"
#include "BKE_text.h"
#include "BKE_ipo.h"
#include "BKE_library.h"
#include "BKE_main.h"
#include "BPI_script.h"
#include "BSE_headerbuttons.h"
#include "DNA_screen_types.h"	/* for SPACE_VIEW3D */
#include "DNA_userdef_types.h"
#include "DNA_packedFile_types.h"
#include "EXPP_interface.h" /* for bpy_gethome() */
#include "gen_utils.h"
#include "modules.h"
#include "constant.h"
#include "../BPY_extern.h" /* BPY_txt_do_python_Text */
#include "../BPY_menus.h"	/* to update menus */
#include "Armature.h"
#include "BezTriple.h"
#include "Camera.h"
#include "Constraint.h"
#include "Curve.h"
#include "CurNurb.h"
#include "Draw.h"
#include "Effect.h"
#include "Ipo.h"
#include "Ipocurve.h"
#include "IDProp.h"
#include "Key.h"
#include "Lamp.h"
#include "Lattice.h"
#include "Library.h"
#include "Mathutils.h"
#include "Geometry.h"
#include "Mesh.h"
#include "Metaball.h"
#include "Modifier.h"
#include "NMesh.h"
#include "Object.h"
#include "Group.h"
#include "Registry.h"
#include "Scene.h"
#include "Sound.h"
#include "SurfNurb.h"
#include "Sys.h"
#include "Text.h"
#include "Texture.h"
#include "Window.h"
#include "World.h"
#include "Types.h"

/**********************************************************/
/* Python API function prototypes for the Blender module.	*/
/**********************************************************/
static PyObject *V24_Blender_Set( PyObject * self, PyObject * args );
static PyObject *V24_Blender_Get( PyObject * self, PyObject * value );
static PyObject *V24_Blender_Redraw( PyObject * self, PyObject * args );
static PyObject *V24_Blender_Quit( PyObject * self );
static PyObject *V24_Blender_Load( PyObject * self, PyObject * args );
static PyObject *V24_Blender_Save( PyObject * self, PyObject * args );
static PyObject *V24_Blender_Run( PyObject * self, PyObject * value );
static PyObject *V24_Blender_ShowHelp( PyObject * self, PyObject * script );
static PyObject *V24_Blender_UpdateMenus( PyObject * self);
static PyObject *V24_Blender_PackAll( PyObject * self);
static PyObject *V24_Blender_UnpackAll( PyObject * self, PyObject * value);
static PyObject *V24_Blender_CountPackedFiles( PyObject * self );

extern PyObject *V24_Text3d_Init( void ); /* missing in some include */

/*****************************************************************************/
/* The following string definitions are used for documentation strings.	 */
/* In Python these will be written to the console when doing a		 */
/* Blender.__doc__	 */
/*****************************************************************************/
static char V24_Blender_Set_doc[] =
	"(request, data) - Update settings in Blender\n\
\n\
(request) A string identifying the setting to change\n\
	'curframe'	- Sets the current frame using the number in data";

static char V24_Blender_Get_doc[] = "(request) - Retrieve settings from Blender\n\
\n\
(request) A string indentifying the data to be returned\n\
	'curframe'	- Returns the current animation frame\n\
	'curtime'	- Returns the current animation time\n\
	'staframe'	- Returns the start frame of the animation\n\
	'endframe'	- Returns the end frame of the animation\n\
	'filename'	- Returns the name of the last file read or written\n\
	'homedir' - Returns Blender's home dir\n\
	'datadir' - Returns the dir where scripts can save their data, if available\n\
	'scriptsdir' - Returns the main dir where scripts are kept, if available\n\
	'uscriptsdir' - Returns the user defined dir for scripts, if available\n\
	'version'	- Returns the Blender version number";

static char V24_Blender_Redraw_doc[] = "() - Redraw all 3D windows";

static char V24_Blender_Quit_doc[] =
	"() - Quit Blender.  The current data is saved as 'quit.blend' before leaving.";

static char V24_Blender_Load_doc[] = "(filename) - Load the given file.\n\
Supported formats:\n\
Blender, DXF, Inventor 1.0 ASCII, VRML 1.0 asc, STL, Videoscape, radiogour.\n\
\n\
Notes:\n\
1 - () - an empty argument loads the default .B.blend file;\n\
2 - if the substring '.B.blend' occurs inside 'filename', the default\n\
.B.blend file is loaded;\n\
3 - If a Blender file is loaded the script ends immediately.\n\
4 - The current data is always preserved as an autosave file, for safety;\n\
5 - This function only works if the script where it's executed is the\n\
only one running at the moment.";

static char V24_Blender_Save_doc[] =
	"(filename) - Save data to a file based on the filename's extension.\n\
Supported are: Blender's .blend and the builtin exporters:\n\
VRML 1.0 (.wrl), Videoscape (.obj), DXF (.dxf) and STL (.stl)\n\
(filename) - A filename with one of the supported extensions.\n\
Note 1: 'filename' should not contain the substring \".B.blend\" in it.\n\
Note 2: only .blend raises an error if file wasn't saved.\n\
\tYou can use Blender.sys.exists(filename) to make sure the file was saved\n\
\twhen writing to one of the other formats.";

static char V24_Blender_Run_doc[] =
	"(script) - Run the given Python script.\n\
(script) - the path to a file or the name of an available Blender Text.";

static char V24_Blender_ShowHelp_doc[] =
"(script) - Show help for the given Python script.\n\
  This will try to open the 'Scripts Help Browser' script, so to have\n\
any help displayed the passed 'script' must be properly documented\n\
with the expected strings (check API ref docs or any bundled script\n\
for examples).\n\n\
(script) - the filename of a script in the default or user defined\n\
           scripts dir (no need to supply the full path name)."; 

static char V24_Blender_UpdateMenus_doc[] =
	"() - Update the menus where scripts are registered.  Only needed for\n\
scripts that save other new scripts in the default or user defined folders.";

static char V24_Blender_PackAll_doc[] =
"() - Pack all files.\n\
All files will packed into the blend file.";
static char V24_Blender_UnpackAll_doc[] =
"(mode) - Unpack files.\n\
All files will be unpacked using specified mode.\n\n\
(mode) - the unpack mode.";

static char V24_Blender_CountPackedFiles_doc[] =
"() - Returns the number of packed files.";

/*****************************************************************************/
/* Python method structure definition.		 */
/*****************************************************************************/
static struct PyMethodDef Blender_methods[] = {
	{"Set", V24_Blender_Set, METH_VARARGS, V24_Blender_Set_doc},
	{"Get", V24_Blender_Get, METH_O, V24_Blender_Get_doc},
	{"Redraw", V24_Blender_Redraw, METH_VARARGS, V24_Blender_Redraw_doc},
	{"Quit", ( PyCFunction ) V24_Blender_Quit, METH_NOARGS, V24_Blender_Quit_doc},
	{"Load", V24_Blender_Load, METH_VARARGS, V24_Blender_Load_doc},
	{"Save", V24_Blender_Save, METH_VARARGS, V24_Blender_Save_doc},
	{"Run", V24_Blender_Run, METH_O, V24_Blender_Run_doc},
	{"ShowHelp", V24_Blender_ShowHelp, METH_O, V24_Blender_ShowHelp_doc},
	{"CountPackedFiles", ( PyCFunction ) V24_Blender_CountPackedFiles, METH_NOARGS, V24_Blender_CountPackedFiles_doc},
	{"PackAll", ( PyCFunction ) V24_Blender_PackAll, METH_NOARGS, V24_Blender_PackAll_doc},
	{"UnpackAll", V24_Blender_UnpackAll, METH_O, V24_Blender_UnpackAll_doc},
	{"UpdateMenus", ( PyCFunction ) V24_Blender_UpdateMenus, METH_NOARGS,
	 V24_Blender_UpdateMenus_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Global variables	 */
/*****************************************************************************/
PyObject *g_blenderdict;

/*****************************************************************************/
/* Function:	V24_Blender_Set		 */
/* Python equivalent:	Blender.Set		 */
/*****************************************************************************/
static PyObject *V24_Blender_Set( PyObject * self, PyObject * args )
{
	char *name, *dir = NULL;
	PyObject *arg;

	if( !PyArg_ParseTuple( args, "sO", &name, &arg ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected 2 args, where the first is always a string" );

	if( V24_StringEqual( name, "curframe" ) ) {
		if( !PyInt_Check( arg ) )
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					"expected an integer" );

		G.scene->r.cfra = (short)PyInt_AsLong( arg ) ;

		/*	update all objects, so python scripts can export all objects
		 in a scene without worrying about the view layers */
		scene_update_for_newframe(G.scene, (1<<20) - 1);
		
	} else if (V24_StringEqual( name , "uscriptsdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.pythondir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "yfexportdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.yfexportdir, dir, FILE_MAXDIR);		
	} else if (V24_StringEqual( name , "fontsdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.fontdir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "texturesdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.textudir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "texpluginsdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.plugtexdir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "seqpluginsdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.plugseqdir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "renderdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.renderdir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "soundsdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.sounddir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "tempdir" ) ) {
		if ( !PyArg_Parse( arg , "s" , &dir ))
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected a string" );
		BLI_strncpy(U.tempdir, dir, FILE_MAXDIR);
	} else if (V24_StringEqual( name , "compressfile" ) ) {
		int value = PyObject_IsTrue( arg );
		
		if (value==-1)
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
					"expected an integer" );
		
		if (value)		
		 U.flag |= USER_FILECOMPRESS;
		else
		 U.flag &= ~USER_FILECOMPRESS;
		
	}else
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"value given is not a blender setting" ) );
	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Function:		V24_Blender_Get	 */
/* Python equivalent:	Blender.Get		 */
/*****************************************************************************/
static PyObject *V24_Blender_Get( PyObject * self, PyObject * value )
{
	PyObject *ret = NULL;
	char *str = PyString_AsString(value);

	if( !str )
		return V24_EXPP_ReturnPyObjError (PyExc_TypeError,
			"expected string argument");

	if( V24_StringEqual( str, "curframe" ) )
		ret = PyInt_FromLong( G.scene->r.cfra );
	else if( V24_StringEqual( str, "curtime" ) )
		ret = PyFloat_FromDouble( frame_to_float( G.scene->r.cfra ) );
	else if( V24_StringEqual( str, "rt" ) )
		ret = PyInt_FromLong( (long)frame_to_float( G.rt ) );
	else if( V24_StringEqual( str, "staframe" ) )
		ret = PyInt_FromLong( G.scene->r.sfra );
	else if( V24_StringEqual( str, "endframe" ) )
		ret = PyInt_FromLong( G.scene->r.efra );
	else if( V24_StringEqual( str, "filename" ) ) {
		if ( strstr(G.main->name, ".B.blend") != 0)
			ret = PyString_FromString("");
		else
			ret = PyString_FromString(G.main->name);
	}
	else if( V24_StringEqual( str, "homedir" ) ) {
		char *hdir = bpy_gethome(0);
		if( hdir && BLI_exists( hdir ))
			ret = PyString_FromString( hdir );
		else
			ret = V24_EXPP_incr_ret( Py_None );
	}
	else if( V24_StringEqual( str, "datadir" ) ) {
		char datadir[FILE_MAXDIR];
		char *sdir = bpy_gethome(1);

		if (sdir) {
			BLI_make_file_string( "/", datadir, sdir, "bpydata" );
			if( BLI_exists( datadir ) )
				ret = PyString_FromString( datadir );
		}
		if (!ret) ret = V24_EXPP_incr_ret( Py_None );
	}
	else if(V24_StringEqual(str, "udatadir")) {
		if (U.pythondir[0] != '\0') {
			char upydir[FILE_MAXDIR];

			BLI_strncpy(upydir, U.pythondir, FILE_MAXDIR);
			BLI_convertstringcode(upydir, G.sce, 0);

			if (BLI_exists(upydir)) {
				char udatadir[FILE_MAXDIR];

				BLI_make_file_string("/", udatadir, upydir, "bpydata");

				if (BLI_exists(udatadir))
					ret = PyString_FromString(udatadir);
			}
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}
	else if( V24_StringEqual( str, "scriptsdir" ) ) {
		char *sdir = bpy_gethome(1);

		if (sdir)
			ret = PyString_FromString(sdir);
		else
			ret = V24_EXPP_incr_ret( Py_None );
	}
	else if( V24_StringEqual( str, "uscriptsdir" ) ) {
		if (U.pythondir[0] != '\0') {
			char upydir[FILE_MAXDIR];

			BLI_strncpy(upydir, U.pythondir, FILE_MAXDIR);
			BLI_convertstringcode(upydir, G.sce, 0);

			if( BLI_exists( upydir ) )
				ret = PyString_FromString( upydir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}
	/* USER PREFS: */
	else if( V24_StringEqual( str, "yfexportdir" ) ) {
		if (U.yfexportdir[0] != '\0') {
			char yfexportdir[FILE_MAXDIR];

			BLI_strncpy(yfexportdir, U.yfexportdir, FILE_MAXDIR);
			BLI_convertstringcode(yfexportdir, G.sce, 0);

			if( BLI_exists( yfexportdir ) )
				ret = PyString_FromString( yfexportdir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}
	/* fontsdir */
	else if( V24_StringEqual( str, "fontsdir" ) ) {
		if (U.fontdir[0] != '\0') {
			char fontdir[FILE_MAXDIR];

			BLI_strncpy(fontdir, U.fontdir, FILE_MAXDIR);
			BLI_convertstringcode(fontdir, G.sce, 0);

			if( BLI_exists( fontdir ) )
				ret = PyString_FromString( fontdir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}	
	/* texturesdir */
	else if( V24_StringEqual( str, "texturesdir" ) ) {
		if (U.textudir[0] != '\0') {
			char textudir[FILE_MAXDIR];

			BLI_strncpy(textudir, U.textudir, FILE_MAXDIR);
			BLI_convertstringcode(textudir, G.sce, 0);

			if( BLI_exists( textudir ) )
				ret = PyString_FromString( textudir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}		
	/* texpluginsdir */
	else if( V24_StringEqual( str, "texpluginsdir" ) ) {
		if (U.plugtexdir[0] != '\0') {
			char plugtexdir[FILE_MAXDIR];

			BLI_strncpy(plugtexdir, U.plugtexdir, FILE_MAXDIR);
			BLI_convertstringcode(plugtexdir, G.sce, 0);

			if( BLI_exists( plugtexdir ) )
				ret = PyString_FromString( plugtexdir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}			
	/* seqpluginsdir */
	else if( V24_StringEqual( str, "seqpluginsdir" ) ) {
		if (U.plugseqdir[0] != '\0') {
			char plugseqdir[FILE_MAXDIR];

			BLI_strncpy(plugseqdir, U.plugseqdir, FILE_MAXDIR);
			BLI_convertstringcode(plugseqdir, G.sce, 0);

			if( BLI_exists( plugseqdir ) )
				ret = PyString_FromString( plugseqdir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}			
	/* renderdir */
	else if( V24_StringEqual( str, "renderdir" ) ) {
		if (U.renderdir[0] != '\0') {
			char renderdir[FILE_MAXDIR];

			BLI_strncpy(renderdir, U.renderdir, FILE_MAXDIR);
			BLI_convertstringcode(renderdir, G.sce, 0);

			if( BLI_exists( renderdir ) )
				ret = PyString_FromString( renderdir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}		
	/* soundsdir */
	else if( V24_StringEqual( str, "soundsdir" ) ) {
		if (U.sounddir[0] != '\0') {
			char sounddir[FILE_MAXDIR];

			BLI_strncpy(sounddir, U.sounddir, FILE_MAXDIR);
			BLI_convertstringcode(sounddir, G.sce, 0);

			if( BLI_exists( sounddir ) )
				ret = PyString_FromString( sounddir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}		
	/* tempdir */
	else if( V24_StringEqual( str, "tempdir" ) ) {
		if (U.tempdir[0] != '\0') {
			char tempdir[FILE_MAXDIR];

			BLI_strncpy(tempdir, U.tempdir, FILE_MAXDIR);
			BLI_convertstringcode(tempdir, G.sce, 0);

			if( BLI_exists( tempdir ) )
				ret = PyString_FromString( tempdir );
		}
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}
	/* icondir */
	else if( V24_StringEqual( str, "icondir" ) ) {
		
		char icondirstr[FILE_MAXDIR];

		#ifdef WIN32
			BLI_make_file_string("/", icondirstr, BLI_gethome(), "icons");
		#else
			BLI_make_file_string("/", icondirstr, BLI_gethome(), ".blender/icons");
		#endif

		if( BLI_exists( icondirstr ) )
			ret = PyString_FromString( icondirstr );
		
		if (!ret) ret = V24_EXPP_incr_ret(Py_None);
	}
	/* According to the old file (opy_blender.c), the following if
	   statement is a quick hack and needs some clean up. */
	else if( V24_StringEqual( str, "vrmloptions" ) ) {
		ret = PyDict_New(  );

		V24_EXPP_dict_set_item_str( ret, "twoside",
			PyInt_FromLong( U.vrmlflag & USER_VRML_TWOSIDED ) );

		V24_EXPP_dict_set_item_str( ret, "layers",
			PyInt_FromLong( U.vrmlflag & USER_VRML_LAYERS ) );

		V24_EXPP_dict_set_item_str( ret, "autoscale",
			PyInt_FromLong( U.vrmlflag & USER_VRML_AUTOSCALE ) );

	} /* End 'quick hack' part. */
	else if(V24_StringEqual( str, "version" ))
		ret = PyInt_FromLong( G.version );
		
	else if(V24_StringEqual( str, "compressfile" ))
		ret = PyInt_FromLong( (U.flag & USER_FILECOMPRESS) >> 15  );
		
	else
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError, "unknown attribute" );

	if (ret) return ret;
	else
		return V24_EXPP_ReturnPyObjError (PyExc_MemoryError,
			"could not create pystring!");
}

/*****************************************************************************/
/* Function:		V24_Blender_Redraw		 */
/* Python equivalent:	Blender.Redraw			 */
/*****************************************************************************/
static PyObject *V24_Blender_Redraw( PyObject * self, PyObject * args )
{
	return V24_M_Window_Redraw( self, args );
}

/*****************************************************************************/
/* Function:		V24_Blender_Quit		 */
/* Python equivalent:	Blender.Quit			 */
/*****************************************************************************/
static PyObject *V24_Blender_Quit( PyObject * self )
{
	BIF_write_autosave(  );	/* save the current data first */

	exit_usiblender(  );	/* renames last autosave to quit.blend */

	Py_RETURN_NONE;
}

/**
 * Blender.Load
 * loads Blender's .blend, DXF, radiogour(?), STL, Videoscape,
 * Inventor 1.0 ASCII, VRML 1.0 asc.
 */
static PyObject *V24_Blender_Load( PyObject * self, PyObject * args )
{
	char *fname = NULL;
	int keep_oldfname = 0;
	Script *script = NULL;
	char str[32], name[FILE_MAXDIR];
	int file, is_blend_file = 0;

	if( !PyArg_ParseTuple( args, "|si", &fname, &keep_oldfname ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
			"expected filename and optional int or nothing as arguments" );

	if( fname ) {
		if( strlen( fname ) > FILE_MAXDIR )	/* G.main->name's max length */
			return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "filename too long!" );
		else if( !BLI_exists( fname ) )
			return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "requested file doesn't exist!" );

		if( keep_oldfname )
			BLI_strncpy( name, G.sce, FILE_MAXDIR );
	}

	/* We won't let a new .blend file be loaded if there are still other
	 * scripts running, since loading a new file will close and remove them. */

	if( G.main->script.first != G.main->script.last )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "there are other scripts running at the Scripts win, close them first!" );

	if( fname ) {
		file = open( fname, O_BINARY | O_RDONLY );

		if( file <= 0 ) {
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "cannot open file!" );
		} else {
			read( file, str, 31 );
			close( file );

			if( strncmp( str, "BLEN", 4 ) == 0 )
				is_blend_file = 1;
		}
	} else
		is_blend_file = 1;	/* no arg given means default: .B.blend */

	if( is_blend_file ) {

		int during_slink = during_scriptlink(  );

		/* when loading a .blend file from a scriptlink, the scriptlink pointer
		 * in BPY_do_pyscript becomes invalid during a loop.  Inform it here.
		 * Also do not allow a nested scriptlink (called from inside another)
		 * to load .blend files, to avoid nasty problems. */
		if( during_slink >= 1 ) {
			if( during_slink == 1 )
				disable_where_scriptlink( -1 );
			else {
				return V24_EXPP_ReturnPyObjError
					( PyExc_EnvironmentError,
					  "Blender.Load: cannot load .blend files from a nested scriptlink." );
			}
		}

		/* trick: mark the script so that its script struct won't be freed after
		 * the script is executed (to avoid a double free warning on exit): */
		script = G.main->script.first;
		if( script )
			script->flags |= SCRIPT_GUI;

		BIF_write_autosave(  );	/* for safety let's preserve the current data */
	}

	if( G.obedit )
		exit_editmode(EM_FREEDATA);

	if (G.background) { /* background mode */
		if (is_blend_file)
			BKE_read_file(fname, NULL);
		else {
			return V24_EXPP_ReturnPyObjError(PyExc_AttributeError,
				"only .blend files can be loaded from command line,\n\
	other file types require interactive mode.");
		}
	}
	else { /* interactive mode */
		/* for safety, any filename with .B.blend is considered the default one.
		 * It doesn't seem necessary to compare file attributes (like st_ino and
		 * st_dev, according to the glibc info pages) to find out if the given
		 * filename, that may have been given with a twisted misgiving path, is the
		 * default one for sure.  Taking any .B.blend file as the default is good
		 * enough here.  Note: the default file requires extra clean-up done by
		 * BIF_read_homefile: freeing the user theme data. */
		if( !fname || ( strstr( fname, ".B.blend" ) && is_blend_file ) )
			BIF_read_homefile(0);
		else
			BIF_read_file( fname );

		if( fname && keep_oldfname ) {
			/*BLI_strncpy(G.main->name, name, FILE_MAXDIR); */
			BLI_strncpy( G.sce, name, FILE_MAXDIR );
		}
	}

	Py_RETURN_NONE;
}

static PyObject *V24_Blender_Save( PyObject * self, PyObject * args )
{
	char *fname = NULL;
	int overwrite = 0, len = 0;
	char *error = NULL;
	Library *li;

	if( !PyArg_ParseTuple( args, "s|i", &fname, &overwrite ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected filename and optional int (overwrite flag) as arguments" );

	for( li = G.main->library.first; li; li = li->id.next ) {
		if( BLI_streq( li->name, fname ) ) {
			return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "cannot overwrite used library" );
		}
	}

	/* for safety, any filename with .B.blend is considered the default one
	 * and not accepted here. */
	if( strstr( fname, ".B.blend" ) )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "filename can't contain the substring \".B.blend\" in it." );

	len = strlen( fname );

	if( len > FILE_MAXDIR + FILE_MAXFILE )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "filename is too long!" );
	else if( BLI_exists( fname ) && !overwrite )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "file already exists and overwrite flag was not given." );

	disable_where_script( 1 );	/* to avoid error popups in the write_* functions */

	if( BLI_testextensie( fname, ".blend" ) ) {
		int writeflags;
		if( G.fileflags & G_AUTOPACK )
			packAll(  );
		
		writeflags= G.fileflags & ~G_FILE_COMPRESS;
		if(U.flag & USER_FILECOMPRESS)
		writeflags |= G_FILE_COMPRESS;
		
		if( !BLO_write_file( fname, writeflags, &error ) ) {
			disable_where_script( 0 );
			return V24_EXPP_ReturnPyObjError( PyExc_SystemError,
						      error );
		}
	} else if( BLI_testextensie( fname, ".dxf" ) )
		write_dxf( fname );
	else if( BLI_testextensie( fname, ".stl" ) )
		write_stl( fname );
	else if( BLI_testextensie( fname, ".wrl" ) )
		write_vrml( fname );
	else if( BLI_testextensie( fname, ".obj" ) )
		write_videoscape( fname );
	else {
		disable_where_script( 0 );
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "unknown file extension." );
	}

	disable_where_script( 0 );

	Py_RETURN_NONE;
}

static PyObject *V24_Blender_ShowHelp(PyObject *self, PyObject *script)
{
	char hspath[FILE_MAXDIR + FILE_MAXFILE]; /* path to help_browser.py */
	char *sdir = bpy_gethome(1);
	PyObject *rkeyd = NULL, *arglist = NULL;

	if (!PyString_Check(script))
		return V24_EXPP_ReturnPyObjError(PyExc_TypeError,
			"expected a script filename as argument");

	/* first we try to find the help_browser script */

	if (sdir) BLI_make_file_string("/", hspath, sdir, "help_browser.py");

	if (!sdir || (!BLI_exists(hspath) && (U.pythondir[0] != '\0'))) {
			char upydir[FILE_MAXDIR];

			BLI_strncpy(upydir, U.pythondir, FILE_MAXDIR);
			BLI_convertstringcode(upydir, G.sce, 0);
			BLI_make_file_string("/", hspath, upydir, "help_browser.py");

			if (!BLI_exists(hspath))
				return V24_EXPP_ReturnPyObjError(PyExc_RuntimeError,
					"can't find script help_browser.py");
	}

	/* now we store the passed script in the registry dict and call the
	 * help_browser to show help info for it */
	rkeyd = PyDict_New();
	if (!rkeyd)
		return V24_EXPP_ReturnPyObjError(PyExc_MemoryError,
			"can't create py dictionary!");

	/* note: don't use V24_EXPP_dict_set_item_str for 'script', which is an
	 * argument to the function we're in and so shouldn't be decref'ed: */
	PyDict_SetItemString(rkeyd, "script", script);

	V24_EXPP_dict_set_item_str(bpy_registryDict, "__help_browser", rkeyd);

	arglist = Py_BuildValue("(s)", hspath);
	V24_Blender_Run(self, arglist);
	Py_DECREF(arglist);

	Py_RETURN_NONE;
}

static PyObject *V24_Blender_Run(PyObject *self, PyObject *value)
{
	char *fname = PyString_AsString(value);
	Text *text = NULL;
	int is_blender_text = 0;
	Script *script = NULL;

	if (!fname)
		return V24_EXPP_ReturnPyObjError(PyExc_TypeError,
			"expected a filename or a Blender Text name as argument");

	if (!BLI_exists(fname)) {	/* if there's no such filename ... */
		text = G.main->text.first;	/* try an already existing Blender Text */

		while (text) {
			if (!strcmp(fname, text->id.name + 2)) break;
			text = text->id.next;
		}

		if (!text) {
			return V24_EXPP_ReturnPyObjError(PyExc_AttributeError,
				"no such file or Blender text");
		}
		else is_blender_text = 1;	/* fn is already a Blender Text */
	}

	else {
		text = add_text(fname);

		if (!text) {
			return V24_EXPP_ReturnPyObjError(PyExc_RuntimeError,
				"couldn't create Blender Text from given file");
		}
	}

	/* (this is messy, check Draw.c's V24_Method_Register and Window.c's file
	 * selector for more info)
	 * - caller script is the one that called this V24_Blender_Run function;
	 * - called script is the argument to this function: fname;
	 * To mark scripts whose global dicts can't be freed right after
	 * the script execution (or better, 'first pass', since these scripts
	 * leave callbacks for gui or file/image selectors) we flag them.  But to
	 * get a pointer to them we need to check which one is currently
	 * running (if none we're already at a spacescript).  To make sure only
	 * the called script will have the SCRIPT_RUNNING flag on, we unset it
	 * for the caller script here: */
	script = G.main->script.first;
	while (script) {
		if (script->flags & SCRIPT_RUNNING) break;
		script = script->id.next;
	}

	if (script) script->flags &= ~SCRIPT_RUNNING; /* unset */

	BPY_txt_do_python_Text(text); /* call new script */

	if (script) script->flags |= SCRIPT_RUNNING; /* set */

	if (!is_blender_text) free_libblock(&G.main->text, text);

	Py_RETURN_NONE;
}

static PyObject * V24_Blender_UpdateMenus( PyObject * self )
{

	BPyMenu_RemoveAllEntries();

	if (BPyMenu_Init(1) == -1)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"invalid scripts dir");

	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Function:		V24_Blender_PackAll		 */
/* Python equivalent:	Blender.PackAll			 */
/*****************************************************************************/
static PyObject *V24_Blender_PackAll( PyObject * self)
{
	packAll();
	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Function:		V24_Blender_UnpackAll		 */
/* Python equivalent:	Blender.UnpackAll			 */
/*****************************************************************************/
static PyObject *V24_Blender_UnpackAll( PyObject * self, PyObject *value)
{
	int mode = PyInt_AsLong(value);
	
	if (mode==-1)
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError, "expected an int Blender.UnpackModes");
	unpackAll(mode);
	Py_RETURN_NONE;
}
 
/*****************************************************************************/
/* Function:		V24_Blender_CountPackedFiles		 */
/* Python equivalent:	Blender.CountPackedFiles			 */
/*****************************************************************************/
static PyObject *V24_Blender_CountPackedFiles( PyObject * self )
{
	int nfiles = countPackedFiles();
	return PyInt_FromLong( nfiles );
}
static PyObject *V24_Blender_UnpackModesDict( void )
{
	PyObject *UnpackModes = V24_PyConstant_New(  );
	if( UnpackModes ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) UnpackModes;
		V24_PyConstant_Insert( d, "EQUAL", PyInt_FromLong((long)PF_EQUAL) );
		V24_PyConstant_Insert( d, "DIFFERS",PyInt_FromLong((long)PF_DIFFERS) );
		V24_PyConstant_Insert( d, "NOFILE", PyInt_FromLong((long)PF_NOFILE) );
		V24_PyConstant_Insert( d, "WRITE_ORIGINAL", PyInt_FromLong((long)PF_WRITE_ORIGINAL) );
		V24_PyConstant_Insert( d, "WRITE_LOCAL", PyInt_FromLong((long)PF_WRITE_LOCAL) );
		V24_PyConstant_Insert( d, "USE_LOCAL", PyInt_FromLong((long)PF_USE_LOCAL) );
		V24_PyConstant_Insert( d, "USE_ORIGINAL", PyInt_FromLong((long)PF_USE_ORIGINAL) );
		V24_PyConstant_Insert( d, "KEEP", PyInt_FromLong((long)PF_KEEP) );
		V24_PyConstant_Insert( d, "NOOP", PyInt_FromLong((long)PF_NOOP) );
		V24_PyConstant_Insert( d, "ASK", PyInt_FromLong((long)PF_EQUAL) );
	}
	return UnpackModes;
}

/*****************************************************************************/
/* Function:		initBlender		 */
/*****************************************************************************/

void V24_M_Blender_Init(void)
{
	PyObject *module;
	PyObject *dict, *smode, *SpaceHandlers, *UnpackModes;
	
	/* G.scene should only aver be NULL if blender is executed in 
	background mode, not loading a blend file and executing a python script eg.
	blender -P somescript.py -b
	The if below solves the segfaults that happen when python runs and
	G.scene is NULL */
	if(G.background && G.main->scene.first==NULL) {
		Scene *sce= add_scene("1");
		/*set_scene(sce);*/ /* causes a crash */
		G.scene= sce;
	}
	
	module = Py_InitModule3("Blender", Blender_methods,
		"The main Blender module");

	types_InitAll();	/* set all our pytypes to &PyType_Type */

	/* constants for packed files*/
	UnpackModes = V24_Blender_UnpackModesDict(  );
	if( UnpackModes )
		PyModule_AddObject( module, "UnpackModes", UnpackModes );
 
	SpaceHandlers = V24_PyConstant_New();
	if (SpaceHandlers) {
		V24_BPy_constant *d = (V24_BPy_constant *)SpaceHandlers;

		V24_PyConstant_Insert(d,"VIEW3D_EVENT",PyInt_FromLong(SPACEHANDLER_VIEW3D_EVENT));
		V24_PyConstant_Insert(d,"VIEW3D_DRAW", PyInt_FromLong(SPACEHANDLER_VIEW3D_DRAW));

		PyModule_AddObject(module, "SpaceHandlers", SpaceHandlers);
	}

	if (G.background)
		smode = PyString_FromString("background");
	else
		smode = PyString_FromString("interactive");

	dict = PyModule_GetDict(module);
	g_blenderdict = dict;

	PyModule_AddIntConstant(module, "TRUE", 1);
	PyModule_AddIntConstant( module, "FALSE", 0 );

	V24_EXPP_dict_set_item_str(dict, "bylink", V24_EXPP_incr_ret_False());
	PyDict_SetItemString(dict, "link", Py_None);
	V24_EXPP_dict_set_item_str(dict, "event", PyString_FromString(""));
	V24_EXPP_dict_set_item_str(dict, "mode", smode);

	PyDict_SetItemString(dict, "Armature", V24_Armature_Init());
	PyDict_SetItemString(dict, "BezTriple", V24_BezTriple_Init());
	PyDict_SetItemString(dict, "BGL", V24_BGL_Init());
	PyDict_SetItemString(dict, "CurNurb", V24_CurNurb_Init());
	PyDict_SetItemString(dict, "Constraint", V24_Constraint_Init());
	PyDict_SetItemString(dict, "Curve", V24_Curve_Init());
	PyDict_SetItemString(dict, "Camera", V24_Camera_Init());
	PyDict_SetItemString(dict, "Draw", V24_Draw_Init());
	PyDict_SetItemString(dict, "Effect", V24_Effect_Init());
	PyDict_SetItemString(dict, "Ipo", V24_Ipo_Init());
	PyDict_SetItemString(dict, "IpoCurve", V24_IpoCurve_Init());
	PyDict_SetItemString(dict, "Image", V24_Image_Init());
	PyDict_SetItemString(dict, "Key", V24_Key_Init());
	PyDict_SetItemString(dict, "Lamp", V24_Lamp_Init());
	PyDict_SetItemString(dict, "Lattice", V24_Lattice_Init());
	PyDict_SetItemString(dict, "Library", oldLibrary_Init());
	PyDict_SetItemString(dict, "Material", V24_Material_Init());
	PyDict_SetItemString(dict, "Mesh", V24_Mesh_Init()); 
	PyDict_SetItemString(dict, "Metaball", V24_Metaball_Init());
	PyDict_SetItemString(dict, "Mathutils", V24_Mathutils_Init());
	PyDict_SetItemString(dict, "Geometry", V24_Geometry_Init());
	PyDict_SetItemString(dict, "Modifier", V24_Modifier_Init());
	PyDict_SetItemString(dict, "NMesh", NMesh_Init());
	PyDict_SetItemString(dict, "Noise", V24_Noise_Init());
	PyDict_SetItemString(dict, "Object", V24_Object_Init());
	PyDict_SetItemString(dict, "Group", V24_Group_Init());
	PyDict_SetItemString(dict, "Registry", V24_Registry_Init());
	PyDict_SetItemString(dict, "Scene", V24_Scene_Init());
	PyDict_SetItemString(dict, "Sound", V24_Sound_Init());
	PyDict_SetItemString(dict, "SurfNurb", V24_SurfNurb_Init());
	PyDict_SetItemString(dict, "sys", sys_Init());
	PyDict_SetItemString(dict, "Types", V24_Types_Init());
	PyDict_SetItemString(dict, "Text", V24_Text_Init());
	PyDict_SetItemString(dict, "Text3d", V24_Text3d_Init());
	PyDict_SetItemString(dict, "Texture", V24_Texture_Init());
	PyDict_SetItemString(dict, "Window", V24_Window_Init());
	PyDict_SetItemString(dict, "World", V24_World_Init());

}
