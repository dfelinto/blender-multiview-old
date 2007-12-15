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
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Willian P. Germano, Nathan Letwory, Stephen Swaney,
 * Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Lamp.h" /*This must come first*/

#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_object.h"
#include "BKE_library.h"
#include "BLI_blenlib.h"
#include "BIF_space.h"
#include "BSE_editipo.h"
#include "mydevice.h"
#include "Ipo.h"
#include "constant.h"
#include "gen_utils.h"
#include "gen_library.h"

/*****************************************************************************/
/* Python V24_BPy_Lamp defaults:                                                 */
/*****************************************************************************/

/* Lamp types */

/* NOTE:
 these are the same values as LA_* from DNA_lamp_types.h
 is there some reason we are not simply using those #defines?
 s. swaney 8-oct-2004
*/

#define EXPP_LAMP_TYPE_LAMP 0
#define EXPP_LAMP_TYPE_SUN  1
#define EXPP_LAMP_TYPE_SPOT 2
#define EXPP_LAMP_TYPE_HEMI 3
#define EXPP_LAMP_TYPE_AREA 4
#define EXPP_LAMP_TYPE_YF_PHOTON 5
/*
  define a constant to keep magic numbers out of the code
  this value should be equal to the last EXPP_LAMP_TYPE_*
*/
#define EXPP_LAMP_TYPE_MAX  5

/* Lamp mode flags */

#define EXPP_LAMP_MODE_SHADOWS       1
#define EXPP_LAMP_MODE_HALO          2
#define EXPP_LAMP_MODE_LAYER         4
#define EXPP_LAMP_MODE_QUAD          8
#define EXPP_LAMP_MODE_NEGATIVE     16
#define EXPP_LAMP_MODE_ONLYSHADOW   32
#define EXPP_LAMP_MODE_SPHERE       64
#define EXPP_LAMP_MODE_SQUARE      128
#define EXPP_LAMP_MODE_TEXTURE     256
#define EXPP_LAMP_MODE_OSATEX      512
#define EXPP_LAMP_MODE_DEEPSHADOW 1024
#define EXPP_LAMP_MODE_NODIFFUSE  2048
#define EXPP_LAMP_MODE_NOSPECULAR 4096
#define EXPP_LAMP_MODE_SHAD_RAY	  8192
/* Lamp MIN, MAX values */

#define EXPP_LAMP_SAMPLES_MIN 1
#define EXPP_LAMP_SAMPLES_MAX 16
#define EXPP_LAMP_BUFFERSIZE_MIN 512
#define EXPP_LAMP_BUFFERSIZE_MAX 5120
#define EXPP_LAMP_ENERGY_MIN  0.0
#define EXPP_LAMP_ENERGY_MAX 10.0
#define EXPP_LAMP_DIST_MIN    0.1f
#define EXPP_LAMP_DIST_MAX 5000.0
#define EXPP_LAMP_SPOTSIZE_MIN   1.0
#define EXPP_LAMP_SPOTSIZE_MAX 180.0
#define EXPP_LAMP_SPOTBLEND_MIN 0.00
#define EXPP_LAMP_SPOTBLEND_MAX 1.00
#define EXPP_LAMP_CLIPSTART_MIN    0.1f
#define EXPP_LAMP_CLIPSTART_MAX 1000.0
#define EXPP_LAMP_CLIPEND_MIN    1.0
#define EXPP_LAMP_CLIPEND_MAX 5000.0
#define EXPP_LAMP_BIAS_MIN 0.01f
#define EXPP_LAMP_BIAS_MAX 5.00
#define EXPP_LAMP_SOFTNESS_MIN   1.0
#define EXPP_LAMP_SOFTNESS_MAX 100.0
#define EXPP_LAMP_HALOINT_MIN 0.0
#define EXPP_LAMP_HALOINT_MAX 5.0
#define EXPP_LAMP_HALOSTEP_MIN  0
#define EXPP_LAMP_HALOSTEP_MAX 12
#define EXPP_LAMP_QUAD1_MIN 0.0
#define EXPP_LAMP_QUAD1_MAX 1.0
#define EXPP_LAMP_QUAD2_MIN 0.0
#define EXPP_LAMP_QUAD2_MAX 1.0
#define EXPP_LAMP_COL_MIN 0.0
#define EXPP_LAMP_COL_MAX 1.0

/* Raytracing settings */
#define EXPP_LAMP_RAYSAMPLES_MIN 1
#define EXPP_LAMP_RAYSAMPLES_MAX 16
#define EXPP_LAMP_AREASIZE_MIN 0.01f
#define EXPP_LAMP_AREASIZE_MAX 100.0f

/* V24_Lamp_setComponent() keys for which color to get/set */
#define	EXPP_LAMP_COMP_R			0x00
#define	EXPP_LAMP_COMP_G			0x01
#define	EXPP_LAMP_COMP_B			0x02

#define IPOKEY_RGB       0
#define IPOKEY_ENERGY    1
#define IPOKEY_SPOTSIZE  2
#define IPOKEY_OFFSET    3
#define IPOKEY_SIZE      4

/*****************************************************************************/
/* Python API function prototypes for the Lamp module.                       */
/*****************************************************************************/
static PyObject *V24_M_Lamp_New( PyObject * self, PyObject * args,
			     PyObject * keywords );
static PyObject *V24_M_Lamp_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* Blender.Lamp.__doc__                                                      */
/*****************************************************************************/
static char V24_M_Lamp_doc[] = "The Blender Lamp module\n\n\
This module provides control over **Lamp Data** objects in Blender.\n\n\
Example::\n\n\
  from Blender import Lamp\n\
  l = Lamp.New('Spot')            # create new 'Spot' lamp data\n\
  l.setMode('square', 'shadow')   # set these two lamp mode flags\n\
  ob = Object.New('Lamp')         # create new lamp object\n\
  ob.link(l)                      # link lamp obj with lamp data\n";

static char V24_M_Lamp_New_doc[] = "Lamp.New (type = 'Lamp', name = 'LampData'):\n\
        Return a new Lamp Data object with the given type and name.";

static char V24_M_Lamp_Get_doc[] = "Lamp.Get (name = None):\n\
        Return the Lamp Data with the given name, None if not found, or\n\
        Return a list with all Lamp Data objects in the current scene,\n\
        if no argument was given.";

/*****************************************************************************/
/* Python method structure definition for Blender.Lamp module:               */
/*****************************************************************************/
struct PyMethodDef M_Lamp_methods[] = {
	{"New", ( PyCFunction ) V24_M_Lamp_New, METH_VARARGS | METH_KEYWORDS,
	 V24_M_Lamp_New_doc},
	{"Get", V24_M_Lamp_Get, METH_VARARGS, V24_M_Lamp_Get_doc},
	{"get", V24_M_Lamp_Get, METH_VARARGS, V24_M_Lamp_Get_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_BPy_Lamp methods declarations:                                     */
/*****************************************************************************/
static PyObject *V24_Lamp_getType( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getTypesConst( void );
static PyObject *V24_Lamp_getMode( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getModesConst( void );
static PyObject *V24_Lamp_getSamples( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getRaySamplesX( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getRaySamplesY( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getAreaSizeX( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getAreaSizeY( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getBufferSize( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getHaloStep( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getEnergy( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getDist( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getSpotSize( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getSpotBlend( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getClipStart( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getClipEnd( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getBias( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getSoftness( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getHaloInt( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getQuad1( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getQuad2( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getCol( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getIpo( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_getComponent( V24_BPy_Lamp * self, void * closure );
static PyObject *V24_Lamp_clearIpo( V24_BPy_Lamp * self );
static PyObject *V24_Lamp_insertIpoKey( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetIpo( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetType( V24_BPy_Lamp * self, PyObject * value );
static PyObject *V24_Lamp_oldsetMode( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetSamples( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetRaySamplesX( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetRaySamplesY( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetAreaSizeX( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetAreaSizeY( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetBufferSize( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetHaloStep( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetEnergy( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetDist( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetSpotSize( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetSpotBlend( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetClipStart( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetClipEnd( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetBias( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetSoftness( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetHaloInt( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetQuad1( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetQuad2( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_oldsetCol( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_copy( V24_BPy_Lamp * self );
static int V24_Lamp_setIpo( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setType( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setMode( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setSamples( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setRaySamplesX( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setRaySamplesY( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setAreaSizeX( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setAreaSizeY( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setBufferSize( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setHaloStep( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setEnergy( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setDist( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setSpotSize( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setSpotBlend( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setClipStart( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setClipEnd( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setBias( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setSoftness( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setHaloInt( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setQuad1( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setQuad2( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setCol( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_getScriptLinks( V24_BPy_Lamp * self, PyObject * value );
static PyObject *V24_Lamp_addScriptLink( V24_BPy_Lamp * self, PyObject * args );
static PyObject *V24_Lamp_clearScriptLinks( V24_BPy_Lamp * self, PyObject * args );
static int V24_Lamp_setComponent( V24_BPy_Lamp * self, PyObject * value, void * closure );

/*****************************************************************************/
/* Python V24_BPy_Lamp methods table:                                            */
/*****************************************************************************/
static PyMethodDef V24_BPy_Lamp_methods[] = {
	/* name, method, flags, doc */
	
	{"getType", ( PyCFunction ) V24_Lamp_getType, METH_NOARGS,
	 "() - return Lamp type - 'Lamp':0, 'Sun':1, 'Spot':2, 'Hemi':3, 'Area':4, 'Photon':5"},
	{"getMode", ( PyCFunction ) V24_Lamp_getMode, METH_NOARGS,
	 "() - return Lamp mode flags (or'ed value)"},
	{"getSamples", ( PyCFunction ) V24_Lamp_getSamples, METH_NOARGS,
	 "() - return Lamp samples value"},
	{"getRaySamplesX", ( PyCFunction ) V24_Lamp_getRaySamplesX, METH_NOARGS,
	 "() - return Lamp raytracing samples on the X axis"},
	{"getRaySamplesY", ( PyCFunction ) V24_Lamp_getRaySamplesY, METH_NOARGS,
	 "() - return Lamp raytracing samples on the Y axis"},
	{"getAreaSizeX", ( PyCFunction ) V24_Lamp_getAreaSizeX, METH_NOARGS,
	 "() - return Lamp area size on the X axis"},
	{"getAreaSizeY", ( PyCFunction ) V24_Lamp_getAreaSizeY, METH_NOARGS,
	 "() - return Lamp area size on the Y axis"},
	{"getBufferSize", ( PyCFunction ) V24_Lamp_getBufferSize, METH_NOARGS,
	 "() - return Lamp buffer size value"},
	{"getHaloStep", ( PyCFunction ) V24_Lamp_getHaloStep, METH_NOARGS,
	 "() - return Lamp halo step value"},
	{"getEnergy", ( PyCFunction ) V24_Lamp_getEnergy, METH_NOARGS,
	 "() - return Lamp energy value"},
	{"getDist", ( PyCFunction ) V24_Lamp_getDist, METH_NOARGS,
	 "() - return Lamp clipping distance value"},
	{"getSpotSize", ( PyCFunction ) V24_Lamp_getSpotSize, METH_NOARGS,
	 "() - return Lamp spot size value"},
	{"getSpotBlend", ( PyCFunction ) V24_Lamp_getSpotBlend, METH_NOARGS,
	 "() - return Lamp spot blend value"},
	{"getClipStart", ( PyCFunction ) V24_Lamp_getClipStart, METH_NOARGS,
	 "() - return Lamp clip start value"},
	{"getClipEnd", ( PyCFunction ) V24_Lamp_getClipEnd, METH_NOARGS,
	 "() - return Lamp clip end value"},
	{"getBias", ( PyCFunction ) V24_Lamp_getBias, METH_NOARGS,
	 "() - return Lamp bias value"},
	{"getSoftness", ( PyCFunction ) V24_Lamp_getSoftness, METH_NOARGS,
	 "() - return Lamp softness value"},
	{"getHaloInt", ( PyCFunction ) V24_Lamp_getHaloInt, METH_NOARGS,
	 "() - return Lamp halo intensity value"},
	{"getQuad1", ( PyCFunction ) V24_Lamp_getQuad1, METH_NOARGS,
	 "() - return light intensity value #1 for a Quad Lamp"},
	{"getQuad2", ( PyCFunction ) V24_Lamp_getQuad2, METH_NOARGS,
	 "() - return light intensity value #2 for a Quad Lamp"},
	{"getCol", ( PyCFunction ) V24_Lamp_getCol, METH_NOARGS,
	 "() - return light rgb color triplet"},
	{"setName", ( PyCFunction ) V24_GenericLib_setName_with_method, METH_VARARGS,
	 "(str) - rename Lamp"},
	{"setType", ( PyCFunction ) V24_Lamp_oldsetType, METH_O,
	 "(str) - change Lamp type, which can be 'Lamp', 'Sun', 'Spot', 'Hemi', 'Area', 'Photon'"},
	{"setMode", ( PyCFunction ) V24_Lamp_oldsetMode, METH_VARARGS,
	 "([up to eight str's]) - Set Lamp mode flag(s)"},
	{"setSamples", ( PyCFunction ) V24_Lamp_oldsetSamples, METH_VARARGS,
	 "(int) - change Lamp samples value"},
	{"setRaySamplesX", ( PyCFunction ) V24_Lamp_oldsetRaySamplesX, METH_VARARGS,
	 "(int) - change Lamp ray X samples value in [1,16]"},
	{"setRaySamplesY", ( PyCFunction ) V24_Lamp_oldsetRaySamplesY, METH_VARARGS,
	 "(int) - change Lamp ray Y samples value in [1,16]"},
	{"setAreaSizeX", ( PyCFunction ) V24_Lamp_oldsetAreaSizeX, METH_VARARGS,
	 "(float) - change Lamp ray X size for area lamps, value in [0.01, 100.0]"},
	{"setAreaSizeY", ( PyCFunction ) V24_Lamp_oldsetAreaSizeY, METH_VARARGS,
	 "(float) - change Lamp ray Y size for area lamps, value in [0.01, 100.0]"},
	{"setBufferSize", ( PyCFunction ) V24_Lamp_oldsetBufferSize, METH_VARARGS,
	 "(int) - change Lamp buffer size value"},
	{"setHaloStep", ( PyCFunction ) V24_Lamp_oldsetHaloStep, METH_VARARGS,
	 "(int) - change Lamp halo step value"},
	{"setEnergy", ( PyCFunction ) V24_Lamp_oldsetEnergy, METH_VARARGS,
	 "(float) - change Lamp energy value"},
	{"setDist", ( PyCFunction ) V24_Lamp_oldsetDist, METH_VARARGS,
	 "(float) - change Lamp clipping distance value"},
	{"setSpotSize", ( PyCFunction ) V24_Lamp_oldsetSpotSize, METH_VARARGS,
	 "(float) - change Lamp spot size value"},
	{"setSpotBlend", ( PyCFunction ) V24_Lamp_oldsetSpotBlend, METH_VARARGS,
	 "(float) - change Lamp spot blend value"},
	{"setClipStart", ( PyCFunction ) V24_Lamp_oldsetClipStart, METH_VARARGS,
	 "(float) - change Lamp clip start value"},
	{"setClipEnd", ( PyCFunction ) V24_Lamp_oldsetClipEnd, METH_VARARGS,
	 "(float) - change Lamp clip end value"},
	{"setBias", ( PyCFunction ) V24_Lamp_oldsetBias, METH_VARARGS,
	 "(float) - change Lamp draw size value"},
	{"setSoftness", ( PyCFunction ) V24_Lamp_oldsetSoftness, METH_VARARGS,
	 "(float) - change Lamp softness value"},
	{"setHaloInt", ( PyCFunction ) V24_Lamp_oldsetHaloInt, METH_VARARGS,
	 "(float) - change Lamp halo intensity value"},
	{"setQuad1", ( PyCFunction ) V24_Lamp_oldsetQuad1, METH_VARARGS,
	 "(float) - change light intensity value #1 for a Quad Lamp"},
	{"setQuad2", ( PyCFunction ) V24_Lamp_oldsetQuad2, METH_VARARGS,
	 "(float) - change light intensity value #2 for a Quad Lamp"},
	{"setCol", ( PyCFunction ) V24_Lamp_oldsetCol, METH_VARARGS,
	 "(f,f,f) or ([f,f,f]) - change light's rgb color triplet"},
	{"getScriptLinks", ( PyCFunction ) V24_Lamp_getScriptLinks, METH_O,
	 "(eventname) - Get a list of this lamp's scriptlinks (Text names) "
	 "of the given type\n"
	 "(eventname) - string: FrameChanged, Redraw or Render."},
	{"addScriptLink", ( PyCFunction ) V24_Lamp_addScriptLink, METH_VARARGS,
	 "(text, evt) - Add a new lamp scriptlink.\n"
	 "(text) - string: an existing Blender Text name;\n"
	 "(evt) string: FrameChanged, Redraw or Render."},
	{"clearScriptLinks", ( PyCFunction ) V24_Lamp_clearScriptLinks,
	 METH_VARARGS,
	 "() - Delete all scriptlinks from this lamp.\n"
	 "([s1<,s2,s3...>]) - Delete specified scriptlinks from this lamp."},
	{"getIpo", ( PyCFunction ) V24_Lamp_getIpo, METH_NOARGS,
	 "() - get IPO for this lamp"},
	{"clearIpo", ( PyCFunction ) V24_Lamp_clearIpo, METH_NOARGS,
	 "() - unlink the IPO for this lamp"},
	{"setIpo", ( PyCFunction ) V24_Lamp_oldsetIpo, METH_VARARGS,
	 "( lamp-ipo ) - link an IPO to this lamp"},
	 {"insertIpoKey", ( PyCFunction ) V24_Lamp_insertIpoKey, METH_VARARGS,
	 "( Lamp IPO type ) - Inserts a key into IPO"},
	{"__copy__", ( PyCFunction ) V24_Lamp_copy, METH_NOARGS,
	 "() - Makes a copy of this lamp."},
	{"copy", ( PyCFunction ) V24_Lamp_copy, METH_NOARGS,
	 "() - Makes a copy of this lamp."},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Lamp_getseters[] = {
	GENERIC_LIB_GETSETATTR,
	{"bias",
	 (getter)V24_Lamp_getBias, (setter)V24_Lamp_setBias,
	 "Lamp shadow map sampling bias",
	 NULL},
	{"bufferSize",
	 (getter)V24_Lamp_getBufferSize, (setter)V24_Lamp_setBufferSize,
	 "Lamp shadow buffer size",
	 NULL},
	{"clipEnd",
	 (getter)V24_Lamp_getClipEnd, (setter)V24_Lamp_setClipEnd,
	 "Lamp shadow map clip end",
	 NULL},
	{"clipStart",
	 (getter)V24_Lamp_getClipStart, (setter)V24_Lamp_setClipStart,
	 "Lamp shadow map clip start",
	 NULL},
	{"col",
	 (getter)V24_Lamp_getCol, (setter)V24_Lamp_setCol,
	 "Lamp RGB color triplet",
	 NULL},
	{"dist",
	 (getter)V24_Lamp_getDist, (setter)V24_Lamp_setDist,
	 "Lamp clipping distance",
	 NULL},
	{"energy",
	 (getter)V24_Lamp_getEnergy, (setter)V24_Lamp_setEnergy,
	 "Lamp light intensity",
	 NULL},
	{"haloInt",
	 (getter)V24_Lamp_getHaloInt, (setter)V24_Lamp_setHaloInt,
	 "Lamp spotlight halo intensity",
	 NULL},
	{"haloStep",
	 (getter)V24_Lamp_getHaloStep, (setter)V24_Lamp_setHaloStep,
	 "Lamp volumetric halo sampling frequency",
	 NULL},
	{"ipo",
	 (getter)V24_Lamp_getIpo, (setter)V24_Lamp_setIpo,
	 "Lamp Ipo",
	 NULL},
	{"mode",
	 (getter)V24_Lamp_getMode, (setter)V24_Lamp_setMode,
	 "Lamp mode bitmask",
	 NULL},
	{"quad1",
	 (getter)V24_Lamp_getQuad1, (setter)V24_Lamp_setQuad1,
	 "Quad lamp linear distance attenuation",
	 NULL},
	{"quad2",
	 (getter)V24_Lamp_getQuad2, (setter)V24_Lamp_setQuad2,
	 "Quad lamp quadratic distance attenuation",
	 NULL},
	{"samples",
	 (getter)V24_Lamp_getSamples, (setter)V24_Lamp_setSamples,
	 "Lamp shadow map samples",
	 NULL},
	{"raySamplesX",
	 (getter)V24_Lamp_getRaySamplesX, (setter)V24_Lamp_setRaySamplesX,
	 "Lamp raytracing samples on the X axis",
	 NULL},
	{"raySamplesY",
	 (getter)V24_Lamp_getRaySamplesY, (setter)V24_Lamp_setRaySamplesY,
	 "Lamp raytracing samples on the Y axis",
	 NULL},
	{"areaSizeX",
	 (getter)V24_Lamp_getAreaSizeX, (setter)V24_Lamp_setAreaSizeX,
	 "Lamp X size for an arealamp",
	 NULL},
	{"areaSizeY",
	 (getter)V24_Lamp_getAreaSizeY, (setter)V24_Lamp_setAreaSizeY,
	 "Lamp Y size for an arealamp",
	 NULL},
	{"softness",
	 (getter)V24_Lamp_getSoftness, (setter)V24_Lamp_setSoftness,
	 "Lamp shadow sample area size",
	 NULL},
	{"spotBlend",
	 (getter)V24_Lamp_getSpotBlend, (setter)V24_Lamp_setSpotBlend,
	 "Lamp spotlight edge softness",
	 NULL},
	{"spotSize",
	 (getter)V24_Lamp_getSpotSize, (setter)V24_Lamp_setSpotSize,
	 "Lamp spotlight beam angle (in degrees)",
	 NULL},
	{"type",
	 (getter)V24_Lamp_getType, (setter)V24_Lamp_setType,
	 "Lamp type",
	 NULL},
	{"R",
	 (getter)V24_Lamp_getComponent, (setter)V24_Lamp_setComponent,
	 "Lamp color red component",
	 (void *)EXPP_LAMP_COMP_R},
	{"r",
	 (getter)V24_Lamp_getComponent, (setter)V24_Lamp_setComponent,
	 "Lamp color red component",
	 (void *)EXPP_LAMP_COMP_R},
	{"G",
	 (getter)V24_Lamp_getComponent, (setter)V24_Lamp_setComponent,
	 "Lamp color green component",
	 (void *)EXPP_LAMP_COMP_G},
	{"g",
	 (getter)V24_Lamp_getComponent, (setter)V24_Lamp_setComponent,
	 "Lamp color green component",
	 (void *)EXPP_LAMP_COMP_G},
	{"B",
	 (getter)V24_Lamp_getComponent, (setter)V24_Lamp_setComponent,
	 "Lamp color blue component",
	 (void *)EXPP_LAMP_COMP_B},
	{"b",
	 (getter)V24_Lamp_getComponent, (setter)V24_Lamp_setComponent,
	 "Lamp color blue component",
	 (void *)EXPP_LAMP_COMP_B},
	{"Modes",
	 (getter)V24_Lamp_getModesConst, (setter)NULL,
	 "Dictionary of values for 'mode' attribute",
	 NULL},
	{"Types",
	 (getter)V24_Lamp_getTypesConst, (setter)NULL,
	 "Dictionary of values for 'type' attribute",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python TypeLamp callback function prototypes:                             */
/*****************************************************************************/
static void V24_Lamp_dealloc( V24_BPy_Lamp * lamp );
static int V24_Lamp_compare( V24_BPy_Lamp * a, V24_BPy_Lamp * b );
static PyObject *V24_Lamp_repr( V24_BPy_Lamp * lamp );

/*****************************************************************************/
/* Python TypeLamp structure definition:                                     */
/*****************************************************************************/
PyTypeObject V24_Lamp_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Lamp",             /* char *tp_name; */
	sizeof( V24_BPy_Lamp ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) V24_Lamp_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Lamp_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) V24_Lamp_repr,     /* reprfunc tp_repr; */

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
	V24_BPy_Lamp_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Lamp_getseters,         /* struct PyGetSetDef *tp_getset; */
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
/* Function:              V24_M_Lamp_New                                         */
/* Python equivalent:     Blender.Lamp.New                                   */
/*****************************************************************************/
static PyObject *V24_M_Lamp_New( PyObject * self, PyObject * args,
			     PyObject * keywords )
{
	char *type_str = "Lamp";
	char *name_str = "Lamp";
	static char *kwlist[] = { "type_str", "name_str", NULL };
	V24_BPy_Lamp *py_lamp;	/* for Lamp Data object wrapper in Python */
	Lamp *bl_lamp;		/* for actual Lamp Data we create in Blender */

	if( !PyArg_ParseTupleAndKeywords( args, keywords, "|ss", kwlist,
					  &type_str, &name_str ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected string(s) or empty argument" ) );
	
	bl_lamp = add_lamp( name_str );	/* first create in Blender */
	
	if( bl_lamp )		/* now create the wrapper obj in Python */
		py_lamp = ( V24_BPy_Lamp * ) V24_Lamp_CreatePyObject( bl_lamp );
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"couldn't create Lamp Data in Blender" ) );

	/* let's return user count to zero, because ... */
	bl_lamp->id.us = 0;	/* ... add_lamp() incref'ed it */

	if( py_lamp == NULL )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create Lamp Data object" ) );

	if( strcmp( type_str, "Lamp" ) == 0 )
		bl_lamp->type = ( short ) EXPP_LAMP_TYPE_LAMP;
	else if( strcmp( type_str, "Sun" ) == 0 )
		bl_lamp->type = ( short ) EXPP_LAMP_TYPE_SUN;
	else if( strcmp( type_str, "Spot" ) == 0 )
		bl_lamp->type = ( short ) EXPP_LAMP_TYPE_SPOT;
	else if( strcmp( type_str, "Hemi" ) == 0 )
		bl_lamp->type = ( short ) EXPP_LAMP_TYPE_HEMI;
	else if( strcmp( type_str, "Area" ) == 0 )
		bl_lamp->type = ( short ) EXPP_LAMP_TYPE_AREA;
	else if( strcmp( type_str, "Photon" ) == 0 )
		bl_lamp->type = ( short ) EXPP_LAMP_TYPE_YF_PHOTON;
	else
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"unknown lamp type" ) );

	return ( PyObject * ) py_lamp;
}

/*****************************************************************************/
/* Function:              V24_M_Lamp_Get                                         */
/* Python equivalent:     Blender.Lamp.Get                                   */
/* Description:           Receives a string and returns the lamp data obj    */
/*                        whose name matches the string.  If no argument is  */
/*                        passed in, a list of all lamp data names in the    */
/*                        current scene is returned.                         */
/*****************************************************************************/
static PyObject *V24_M_Lamp_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Lamp *lamp_iter;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected string argument (or nothing)" ) );

	lamp_iter = G.main->lamp.first;

	if( name ) {		/* (name) - Search lamp by name */

		V24_BPy_Lamp *wanted_lamp = NULL;

		while( ( lamp_iter ) && ( wanted_lamp == NULL ) ) {

			if( strcmp( name, lamp_iter->id.name + 2 ) == 0 )
				wanted_lamp =
					( V24_BPy_Lamp * )
					V24_Lamp_CreatePyObject( lamp_iter );

			lamp_iter = lamp_iter->id.next;
		}

		if( wanted_lamp == NULL ) { /* Requested lamp doesn't exist */
			char error_msg[64];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Lamp \"%s\" not found", name );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_NameError, error_msg ) );
		}

		return ( PyObject * ) wanted_lamp;
	}

	else {		/* () - return a list of all lamps in the scene */
		int index = 0;
		PyObject *lamplist, *pyobj;

		lamplist = PyList_New( BLI_countlist( &( G.main->lamp ) ) );

		if( lamplist == NULL )
			return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
							"couldn't create PyList" ) );

		while( lamp_iter ) {
			pyobj = V24_Lamp_CreatePyObject( lamp_iter );

			if( !pyobj ) {
				Py_DECREF(lamplist);
				return ( V24_EXPP_ReturnPyObjError
					 ( PyExc_MemoryError,
					   "couldn't create PyLamp" ) );
			}

			PyList_SET_ITEM( lamplist, index, pyobj );

			lamp_iter = lamp_iter->id.next;
			index++;
		}

		return lamplist;
	}
}

static PyObject *V24_Lamp_TypesDict( void )
{	/* create the Blender.Lamp.Types constant dict */
	PyObject *Types = V24_PyConstant_New(  );

	if( Types ) {
		V24_BPy_constant *c = ( V24_BPy_constant * ) Types;

		V24_PyConstant_Insert( c, "Lamp",
				 PyInt_FromLong( EXPP_LAMP_TYPE_LAMP ) );
		V24_PyConstant_Insert( c, "Sun",
				 PyInt_FromLong( EXPP_LAMP_TYPE_SUN ) );
		V24_PyConstant_Insert( c, "Spot",
				 PyInt_FromLong( EXPP_LAMP_TYPE_SPOT ) );
		V24_PyConstant_Insert( c, "Hemi",
				 PyInt_FromLong( EXPP_LAMP_TYPE_HEMI ) );
		V24_PyConstant_Insert( c, "Area",
				 PyInt_FromLong( EXPP_LAMP_TYPE_AREA ) );
		V24_PyConstant_Insert( c, "Photon",
				 PyInt_FromLong( EXPP_LAMP_TYPE_YF_PHOTON ) );
	}

	return Types;
}

static PyObject *V24_Lamp_ModesDict( void )
{			/* create the Blender.Lamp.Modes constant dict */
	PyObject *Modes = V24_PyConstant_New(  );

	if( Modes ) {
		V24_BPy_constant *c = ( V24_BPy_constant * ) Modes;

		V24_PyConstant_Insert( c, "Shadows",
				 PyInt_FromLong( EXPP_LAMP_MODE_SHADOWS ) );
		V24_PyConstant_Insert( c, "Halo",
				 PyInt_FromLong( EXPP_LAMP_MODE_HALO ) );
		V24_PyConstant_Insert( c, "Layer",
				 PyInt_FromLong( EXPP_LAMP_MODE_LAYER ) );
		V24_PyConstant_Insert( c, "Quad",
				 PyInt_FromLong( EXPP_LAMP_MODE_QUAD ) );
		V24_PyConstant_Insert( c, "Negative",
				 PyInt_FromLong( EXPP_LAMP_MODE_NEGATIVE ) );
		V24_PyConstant_Insert( c, "Sphere",
				 PyInt_FromLong( EXPP_LAMP_MODE_SPHERE ) );
		V24_PyConstant_Insert( c, "Square",
				 PyInt_FromLong( EXPP_LAMP_MODE_SQUARE ) );
		V24_PyConstant_Insert( c, "OnlyShadow",
				 PyInt_FromLong( EXPP_LAMP_MODE_ONLYSHADOW ) );
		V24_PyConstant_Insert( c, "NoDiffuse",
				 PyInt_FromLong( EXPP_LAMP_MODE_NODIFFUSE ) );
		V24_PyConstant_Insert( c, "NoSpecular",
				 PyInt_FromLong( EXPP_LAMP_MODE_NOSPECULAR ) );
		V24_PyConstant_Insert( c, "RayShadow",
				 PyInt_FromLong( EXPP_LAMP_MODE_SHAD_RAY ) );
	}

	return Modes;
}

/*****************************************************************************/
/* Function:              V24_Lamp_Init                                          */
/*****************************************************************************/
/* Needed by the Blender module, to register the Blender.Lamp submodule */
PyObject *V24_Lamp_Init( void )
{
	PyObject *submodule, *Types, *Modes;

	if( PyType_Ready( &V24_Lamp_Type ) < 0)
		return NULL;

	Types = V24_Lamp_TypesDict(  );
	Modes = V24_Lamp_ModesDict(  );

	submodule =
		Py_InitModule3( "Blender.Lamp", M_Lamp_methods, V24_M_Lamp_doc );

	if( Types )
		PyModule_AddObject( submodule, "Types", Types );
	if( Modes )
		PyModule_AddObject( submodule, "Modes", Modes );

	PyModule_AddIntConstant( submodule, "RGB",      IPOKEY_RGB );
	PyModule_AddIntConstant( submodule, "ENERGY",   IPOKEY_ENERGY );
	PyModule_AddIntConstant( submodule, "SPOTSIZE", IPOKEY_SPOTSIZE );
	PyModule_AddIntConstant( submodule, "OFFSET",   IPOKEY_OFFSET );
	PyModule_AddIntConstant( submodule, "SIZE",     IPOKEY_SIZE );
	
	return submodule;
}

/* Three Python V24_Lamp_Type helper functions needed by the Object module: */

/*****************************************************************************/
/* Function:    V24_Lamp_CreatePyObject                                          */
/* Description: This function will create a new V24_BPy_Lamp from an existing    */
/*              Blender lamp structure.                                      */
/*****************************************************************************/
PyObject *V24_Lamp_CreatePyObject( Lamp * lamp )
{
	V24_BPy_Lamp *pylamp;
	float *rgb[3];

	pylamp = ( V24_BPy_Lamp * ) PyObject_NEW( V24_BPy_Lamp, &V24_Lamp_Type );

	if( !pylamp )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create V24_BPy_Lamp object" );

	pylamp->lamp = lamp;

	rgb[0] = &lamp->r;
	rgb[1] = &lamp->g;
	rgb[2] = &lamp->b;

	pylamp->color = ( V24_BPy_rgbTuple * ) rgbTuple_New( rgb );
	Py_INCREF(pylamp->color);
	
	return ( PyObject * ) pylamp;
}

/*****************************************************************************/
/* Function:    V24_Lamp_FromPyObject                                            */
/* Description: This function returns the Blender lamp from the given        */
/*              PyObject.                                                    */
/*****************************************************************************/
Lamp *V24_Lamp_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Lamp * ) pyobj )->lamp;
}

/*****************************************************************************/
/* Python V24_BPy_Lamp methods:                                                  */
/*****************************************************************************/

/* Lamp.__copy__ */
static PyObject *V24_Lamp_copy( V24_BPy_Lamp * self )
{
	Lamp *lamp = copy_lamp(self->lamp );
	lamp->id.us = 0;
	return V24_Lamp_CreatePyObject(lamp);
}

static PyObject *V24_Lamp_getType( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->type );
}

static PyObject *V24_Lamp_getMode( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->mode );
}

static PyObject *V24_Lamp_getSamples( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->samp );
}

static PyObject *V24_Lamp_getRaySamplesX( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->ray_samp );
}

static PyObject *V24_Lamp_getRaySamplesY( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->ray_sampy );
}

static PyObject *V24_Lamp_getAreaSizeX( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->area_size );
}

static PyObject *V24_Lamp_getAreaSizeY( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->area_sizey );
}

static PyObject *V24_Lamp_getBufferSize( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->bufsize );
}

static PyObject *V24_Lamp_getHaloStep( V24_BPy_Lamp * self )
{
	return PyInt_FromLong( self->lamp->shadhalostep );
}

static PyObject *V24_Lamp_getEnergy( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->energy );
}

static PyObject *V24_Lamp_getDist( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->dist );
}

static PyObject *V24_Lamp_getSpotSize( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->spotsize );
}

static PyObject *V24_Lamp_getSpotBlend( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->spotblend );
}

static PyObject *V24_Lamp_getClipStart( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->clipsta );
}

static PyObject *V24_Lamp_getClipEnd( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->clipend );
}

static PyObject *V24_Lamp_getBias( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->bias );
}

static PyObject *V24_Lamp_getSoftness( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->soft );
}

static PyObject *V24_Lamp_getHaloInt( V24_BPy_Lamp * self )
{
	return PyFloat_FromDouble( self->lamp->haint );
}

static PyObject *V24_Lamp_getQuad1( V24_BPy_Lamp * self )
{				/* should we complain if Lamp is not of type Quad? */
	return PyFloat_FromDouble( self->lamp->att1 );
}

static PyObject *V24_Lamp_getQuad2( V24_BPy_Lamp * self )
{			/* should we complain if Lamp is not of type Quad? */
	return PyFloat_FromDouble( self->lamp->att2 );
}

static PyObject *V24_Lamp_getCol( V24_BPy_Lamp * self )
{
	return rgbTuple_getCol( self->color );
}

static int V24_Lamp_setType( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setIValueRange ( value, &self->lamp->type,
				  				0, EXPP_LAMP_TYPE_MAX, 'h' );
}

static int V24_Lamp_setMode( V24_BPy_Lamp * self, PyObject * value )
{
	short param;
	static short bitmask = EXPP_LAMP_MODE_SHADOWS
				| EXPP_LAMP_MODE_HALO
				| EXPP_LAMP_MODE_LAYER
				| EXPP_LAMP_MODE_QUAD
				| EXPP_LAMP_MODE_NEGATIVE
				| EXPP_LAMP_MODE_ONLYSHADOW
				| EXPP_LAMP_MODE_SPHERE
				| EXPP_LAMP_MODE_SQUARE
				| EXPP_LAMP_MODE_NODIFFUSE
				| EXPP_LAMP_MODE_NOSPECULAR
				| EXPP_LAMP_MODE_SHAD_RAY;

	if( !PyInt_Check ( value ) ) {
		char errstr[128];
		sprintf ( errstr , "expected int bitmask of 0x%04x", bitmask );
		return V24_EXPP_ReturnIntError( PyExc_TypeError, errstr );
	}
	param = (short)PyInt_AS_LONG ( value );

	if ( ( param & bitmask ) != param )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
						"invalid bit(s) set in mask" );

	self->lamp->mode = param; 

	return 0;
}

static int V24_Lamp_setSamples( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->lamp->samp,
								EXPP_LAMP_SAMPLES_MIN,
								EXPP_LAMP_SAMPLES_MAX, 'h' );
}


static int V24_Lamp_setRaySamplesX( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->lamp->ray_samp, 
								EXPP_LAMP_RAYSAMPLES_MIN,
								EXPP_LAMP_RAYSAMPLES_MAX, 'h' );
}

static int V24_Lamp_setRaySamplesY( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->lamp->ray_sampy,
								EXPP_LAMP_RAYSAMPLES_MIN,
								EXPP_LAMP_RAYSAMPLES_MAX, 'h' );
}

static int V24_Lamp_setAreaSizeX( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->area_size, 
								EXPP_LAMP_AREASIZE_MIN,
								EXPP_LAMP_AREASIZE_MAX );
}

static int V24_Lamp_setAreaSizeY( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->area_sizey, 
								EXPP_LAMP_AREASIZE_MIN,
								EXPP_LAMP_AREASIZE_MAX );
}

static int V24_Lamp_setBufferSize( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->lamp->bufsize,
								EXPP_LAMP_BUFFERSIZE_MIN,
								EXPP_LAMP_BUFFERSIZE_MAX, 'h' );
}

static int V24_Lamp_setHaloStep( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setIValueClamped ( value, &self->lamp->shadhalostep,
								EXPP_LAMP_HALOSTEP_MIN,
								EXPP_LAMP_HALOSTEP_MAX, 'h' );
}

static int V24_Lamp_setEnergy( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->energy, 
								EXPP_LAMP_ENERGY_MIN,
								EXPP_LAMP_ENERGY_MAX );
}

static int V24_Lamp_setDist( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->dist, 
								EXPP_LAMP_DIST_MIN,
								EXPP_LAMP_DIST_MAX );
}

static int V24_Lamp_setSpotSize( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->spotsize, 
								EXPP_LAMP_SPOTSIZE_MIN,
								EXPP_LAMP_SPOTSIZE_MAX );
}

static int V24_Lamp_setSpotBlend( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->spotblend, 
								EXPP_LAMP_SPOTBLEND_MIN,
								EXPP_LAMP_SPOTBLEND_MAX );
}

static int V24_Lamp_setClipStart( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->clipsta, 
								EXPP_LAMP_CLIPSTART_MIN,
								EXPP_LAMP_CLIPSTART_MAX );
}

static int V24_Lamp_setClipEnd( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->clipend, 
								EXPP_LAMP_CLIPEND_MIN,
								EXPP_LAMP_CLIPEND_MAX );
}

static int V24_Lamp_setBias( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->bias,
								EXPP_LAMP_BIAS_MIN,
								EXPP_LAMP_BIAS_MAX );
}

static int V24_Lamp_setSoftness( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->soft,
								EXPP_LAMP_SOFTNESS_MIN,
								EXPP_LAMP_SOFTNESS_MAX );
}

static int V24_Lamp_setHaloInt( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->haint,
								EXPP_LAMP_HALOINT_MIN,
								EXPP_LAMP_HALOINT_MAX );
}

static int V24_Lamp_setQuad1( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->att1,
								EXPP_LAMP_QUAD1_MIN,
								EXPP_LAMP_QUAD1_MAX );
}

static int V24_Lamp_setQuad2( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_EXPP_setFloatClamped ( value, &self->lamp->att2,
								EXPP_LAMP_QUAD2_MIN,
								EXPP_LAMP_QUAD2_MAX );
}

static PyObject *V24_Lamp_getComponent( V24_BPy_Lamp * self, void * closure )
{
	switch ( (int)closure ) {
	case EXPP_LAMP_COMP_R:
		return PyFloat_FromDouble( self->lamp->r );
	case EXPP_LAMP_COMP_G:
		return PyFloat_FromDouble( self->lamp->g );
	case EXPP_LAMP_COMP_B:
		return PyFloat_FromDouble( self->lamp->b );
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"unknown color component specified" );
	}
}

static int V24_Lamp_setComponent( V24_BPy_Lamp * self, PyObject * value,
							void * closure )
{
	float color;

	if( !PyNumber_Check ( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
						"expected float argument in [0.0,1.0]" );

	color = (float)PyFloat_AsDouble( value );
	color = V24_EXPP_ClampFloat( color, EXPP_LAMP_COL_MIN, EXPP_LAMP_COL_MAX );

	switch ( (int)closure ) {
	case EXPP_LAMP_COMP_R:
		self->lamp->r = color;
		return 0;
	case EXPP_LAMP_COMP_G:
		self->lamp->g = color;
		return 0;
	case EXPP_LAMP_COMP_B:
		self->lamp->b = color;
		return 0;
	}
	return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"unknown color component specified" );
}

static int V24_Lamp_setCol( V24_BPy_Lamp * self, PyObject * args )
{
	return rgbTuple_setCol( self->color, args );
}

/* lamp.addScriptLink */
static PyObject *V24_Lamp_addScriptLink( V24_BPy_Lamp * self, PyObject * args )
{
	Lamp *lamp = self->lamp;
	ScriptLink *slink = NULL;

	slink = &( lamp )->scriptlink;

	return V24_EXPP_addScriptLink( slink, args, 0 );
}

/* lamp.clearScriptLinks */
static PyObject *V24_Lamp_clearScriptLinks( V24_BPy_Lamp * self, PyObject * args )
{
	Lamp *lamp = self->lamp;
	ScriptLink *slink = NULL;

	slink = &( lamp )->scriptlink;

	return V24_EXPP_clearScriptLinks( slink, args );
}

/* mat.getScriptLinks */
static PyObject *V24_Lamp_getScriptLinks( V24_BPy_Lamp * self, PyObject * value )
{
	Lamp *lamp = self->lamp;
	ScriptLink *slink = NULL;
	PyObject *ret = NULL;

	slink = &( lamp )->scriptlink;

	ret = V24_EXPP_getScriptLinks( slink, value, 0 );

	if( ret )
		return ret;
	else
		return NULL;
}

/*****************************************************************************/
/* Function:    V24_Lamp_dealloc                                                 */
/* Description: This is a callback function for the V24_BPy_Lamp type. It is     */
/*              the destructor function.                                     */
/*****************************************************************************/
static void V24_Lamp_dealloc( V24_BPy_Lamp * self )
{
	Py_DECREF( self->color );
	PyObject_DEL( self );
}

/*****************************************************************************/
/* Function:    V24_Lamp_compare                                                 */
/* Description: This is a callback function for the V24_BPy_Lamp type. It        */
/*              compares two V24_Lamp_Type objects. Only the "==" and "!="       */
/*              comparisons are meaningful. Returns 0 for equality and -1    */
/*              if they don't point to the same Blender Lamp struct.         */
/*              In Python it becomes 1 if they are equal, 0 otherwise.       */
/*****************************************************************************/
static int V24_Lamp_compare( V24_BPy_Lamp * a, V24_BPy_Lamp * b )
{
	return ( a->lamp == b->lamp ) ? 0 : -1;
}

/*****************************************************************************/
/* Function:    V24_Lamp_repr                                                    */
/* Description: This is a callback function for the V24_BPy_Lamp type. It        */
/*              builds a meaninful string to represent lamp objects.         */
/*****************************************************************************/
static PyObject *V24_Lamp_repr( V24_BPy_Lamp * self )
{
	return PyString_FromFormat( "[Lamp \"%s\"]", self->lamp->id.name + 2 );
}

static PyObject *V24_Lamp_getIpo( V24_BPy_Lamp * self )
{
	struct Ipo *ipo = self->lamp->ipo;

	if( !ipo )
		Py_RETURN_NONE;

	return V24_Ipo_CreatePyObject( ipo );
}

/*
 * this should accept a Py_None argument and just delete the Ipo link
 * (as V24_Lamp_clearIpo() does)
 */

static int V24_Lamp_setIpo( V24_BPy_Lamp * self, PyObject * value )
{
	return V24_GenericLib_assignData(value, (void **) &self->lamp->ipo, 0, 1, ID_IP, ID_LA);
}

/*
 * V24_Lamp_insertIpoKey()
 *  inserts Lamp IPO key for RGB,ENERGY,SPOTSIZE,OFFSET,SIZE
 */

static PyObject *V24_Lamp_insertIpoKey( V24_BPy_Lamp * self, PyObject * args )
{
	int key = 0, map;

	if( !PyArg_ParseTuple( args, "i", &( key ) ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
										"expected int argument" ) );

	map = texchannel_to_adrcode(self->lamp->texact);

	if (key == IPOKEY_RGB ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, LA_COL_R, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_COL_G, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_COL_B, 0);
	}
	if (key == IPOKEY_ENERGY ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_ENERGY, 0);
	}	
	if (key == IPOKEY_SPOTSIZE ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL,LA_SPOTSI, 0);
	}
	if (key == IPOKEY_OFFSET ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_OFS_X, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_OFS_Y, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_OFS_Z, 0);
	}
	if (key == IPOKEY_SIZE ) {
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_SIZE_X, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_SIZE_Y, 0);
		insertkey((ID *)self->lamp, ID_LA, NULL, NULL, map+MAP_SIZE_Z, 0);
	}

	allspace(REMAKEIPO, 0);
	V24_EXPP_allqueue(REDRAWIPO, 0);
	V24_EXPP_allqueue(REDRAWVIEW3D, 0);
	V24_EXPP_allqueue(REDRAWACTION, 0);
	V24_EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}

static PyObject *V24_Lamp_getModesConst( void )
{
	return Py_BuildValue
			( "{s:h,s:h,s:h,s:h,s:h,s:h,s:h,s:h,s:h,s:h,s:h}",
			  "Shadows", EXPP_LAMP_MODE_SHADOWS, "Halo",
			  EXPP_LAMP_MODE_HALO, "Layer", EXPP_LAMP_MODE_LAYER,
			  "Quad", EXPP_LAMP_MODE_QUAD, "Negative",
			  EXPP_LAMP_MODE_NEGATIVE, "OnlyShadow",
			  EXPP_LAMP_MODE_ONLYSHADOW, "Sphere",
			  EXPP_LAMP_MODE_SPHERE, "Square",
			  EXPP_LAMP_MODE_SQUARE, "NoDiffuse",
			  EXPP_LAMP_MODE_NODIFFUSE, "NoSpecular",
			  EXPP_LAMP_MODE_NOSPECULAR, "RayShadow",
			  EXPP_LAMP_MODE_SHAD_RAY);
}

static PyObject *V24_Lamp_getTypesConst( void )
{
	return Py_BuildValue( "{s:h,s:h,s:h,s:h,s:h,s:h}",
				      "Lamp", EXPP_LAMP_TYPE_LAMP,
				      "Sun", EXPP_LAMP_TYPE_SUN,
				      "Spot", EXPP_LAMP_TYPE_SPOT,
				      "Hemi", EXPP_LAMP_TYPE_HEMI, 
				      "Area", EXPP_LAMP_TYPE_AREA, 
				      "Photon", EXPP_LAMP_TYPE_YF_PHOTON );
}

/* #####DEPRECATED###### */

static PyObject *V24_Lamp_oldsetSamples( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setSamples );
}

static PyObject *V24_Lamp_oldsetRaySamplesX( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setRaySamplesX );
}

static PyObject *V24_Lamp_oldsetRaySamplesY( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setRaySamplesY );
}

static PyObject *V24_Lamp_oldsetAreaSizeX( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setAreaSizeX );
}

static PyObject *V24_Lamp_oldsetAreaSizeY( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setAreaSizeY );
}

static PyObject *V24_Lamp_oldsetBufferSize( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setBufferSize );
}

static PyObject *V24_Lamp_oldsetHaloStep( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setHaloStep );
}

static PyObject *V24_Lamp_oldsetEnergy( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setEnergy );
}

static PyObject *V24_Lamp_oldsetDist( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setDist );
}

static PyObject *V24_Lamp_oldsetSpotSize( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setSpotSize );
}

static PyObject *V24_Lamp_oldsetSpotBlend( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setSpotBlend );
}

static PyObject *V24_Lamp_oldsetClipStart( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setClipStart );
}

static PyObject *V24_Lamp_oldsetClipEnd( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setClipEnd );
}

static PyObject *V24_Lamp_oldsetBias( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setBias );
}

static PyObject *V24_Lamp_oldsetSoftness( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setSoftness );
}

static PyObject *V24_Lamp_oldsetHaloInt( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setHaloInt );
}

static PyObject *V24_Lamp_oldsetQuad1( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setQuad1 );
}

static PyObject *V24_Lamp_oldsetQuad2( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setQuad2 );
}

static PyObject *V24_Lamp_oldsetIpo( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setIpo );
}

static PyObject *V24_Lamp_oldsetCol( V24_BPy_Lamp * self, PyObject * args )
{
	return V24_EXPP_setterWrapper ( (void *)self, args, (setter)V24_Lamp_setCol );
}

/* 
 * the "not-well-behaved" methods which require more processing than 
 * just the simple wrapper
 */

/*
 * clearIpo() returns True/False depending on whether lamp has an Ipo
 */

static PyObject *V24_Lamp_clearIpo( V24_BPy_Lamp * self )
{
	/* if Ipo defined, delete it and return true */

	if( self->lamp->ipo ) {
		PyObject *value = Py_BuildValue( "(O)", Py_None );
		V24_EXPP_setterWrapper ( (void *)self, value, (setter)V24_Lamp_setIpo );
		Py_DECREF ( value );
		return V24_EXPP_incr_ret_True();
	}
	return V24_EXPP_incr_ret_False(); /* no ipo found */
}

/*
 * setType() accepts a string while mode setter takes an integer
 */

static PyObject *V24_Lamp_oldsetType( V24_BPy_Lamp * self, PyObject * value )
{
	char *type = PyString_AsString(value);
	PyObject *arg, *error;

	/* parse string argument */

	if( !value )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected string argument" ) );

	/* check for valid arguments, set type accordingly */

	if( !strcmp( type, "Lamp" ) )
		self->lamp->type = ( short ) EXPP_LAMP_TYPE_LAMP;
	else if( !strcmp( type, "Sun" ) )
		self->lamp->type = ( short ) EXPP_LAMP_TYPE_SUN;
	else if( !strcmp( type, "Spot" ) )
		self->lamp->type = ( short ) EXPP_LAMP_TYPE_SPOT;
	else if( !strcmp( type, "Hemi" ) )
		self->lamp->type = ( short ) EXPP_LAMP_TYPE_HEMI;
	else if( !strcmp( type, "Area" ) )
		self->lamp->type = ( short ) EXPP_LAMP_TYPE_AREA;
	else if( !strcmp( type, "Photon" ) )
		self->lamp->type = ( short ) EXPP_LAMP_TYPE_YF_PHOTON;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"unknown lamp type" );

	/* build tuple, call wrapper */

	arg = Py_BuildValue( "(i)", type );
	error = V24_EXPP_setterWrapper ( (void *)self, arg, (setter)V24_Lamp_setType );
	Py_DECREF ( arg );
	return error;
}

/*
 * setMode() accepts up to ten strings while mode setter takes an integer
 */

static PyObject *V24_Lamp_oldsetMode( V24_BPy_Lamp * self, PyObject * args )
{
	short i, flag = 0;
	PyObject *error, *value;
	char *name;

	/* check that we're passed a tuple of no more than 10 args*/

	if ( !PyTuple_Check( args ) || PyTuple_Size( args ) > 10 )
		return V24_EXPP_ReturnPyObjError ( PyExc_AttributeError,
					"expected up to 10 string arguments" );

	/* check each argument for type, find its value */

	for ( i = (short)PyTuple_Size( args ); i-- ; ) {
 		name = PyString_AsString ( PyTuple_GET_ITEM( args, i ) );
		if( !name )
			return V24_EXPP_ReturnPyObjError ( PyExc_AttributeError,
					"expected string argument" );

		if( !strcmp( name, "Shadows" ) )
			flag |= ( short ) EXPP_LAMP_MODE_SHADOWS;
		else if( !strcmp( name, "Halo" ) )
			flag |= ( short ) EXPP_LAMP_MODE_HALO;
		else if( !strcmp( name, "Layer" ) )
			flag |= ( short ) EXPP_LAMP_MODE_LAYER;
		else if( !strcmp( name, "Quad" ) )
			flag |= ( short ) EXPP_LAMP_MODE_QUAD;
		else if( !strcmp( name, "Negative" ) )
			flag |= ( short ) EXPP_LAMP_MODE_NEGATIVE;
		else if( !strcmp( name, "OnlyShadow" ) )
			flag |= ( short ) EXPP_LAMP_MODE_ONLYSHADOW;
		else if( !strcmp( name, "Sphere" ) )
			flag |= ( short ) EXPP_LAMP_MODE_SPHERE;
		else if( !strcmp( name, "Square" ) )
			flag |= ( short ) EXPP_LAMP_MODE_SQUARE;
		else if( !strcmp( name, "NoDiffuse" ) )
			flag |= ( short ) EXPP_LAMP_MODE_NODIFFUSE;
		else if( !strcmp( name, "NoSpecular" ) )
			flag |= ( short ) EXPP_LAMP_MODE_NOSPECULAR;
		else if( !strcmp( name, "RayShadow" ) )
			flag |= ( short ) EXPP_LAMP_MODE_SHAD_RAY;
		else
			return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
							"unknown lamp flag argument" );
	}

	/* build tuple, call wrapper */

	value = Py_BuildValue( "(i)", flag );
	error = V24_EXPP_setterWrapper ( (void *)self, value, (setter)V24_Lamp_setMode );
	Py_DECREF ( value );
	return error;
}

