/**
 * Image Datablocks
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Python.h"
#include "opy_datablock.h"

#include "DNA_image_types.h"
#include "BKE_image.h"

#include "BPY_macros.h"
#include "b_interface.h"

PyObject *INITMODULE(Image)(void);

/* Image_Get */
DATABLOCK_GET(Imagemodule, image, getImageList())

char Imagemodule_load_doc[] = "(filename) - return image from file 'filename' as Image object";

PyObject *Imagemodule_load(PyObject *self, PyObject *args)
{
	char *name;
	Image *im;

	if (!PyArg_ParseTuple(args, "s", &name)) {
		PyErr_SetString(PyExc_TypeError, "filename expected");
		return 0;
	}
		
	im = add_image(name);
	if (im) {
		return DataBlock_fromData(im);
	} else {
		PyErr_SetString(PyExc_IOError, "couldn't load image");
		return 0;
	}	
}

DataBlockProperty Image_Properties[]= {
	{"xrep", "xrep", DBP_TYPE_SHO, 0, 1.0, 16.0},
	{"yrep", "yrep", DBP_TYPE_SHO, 0, 1.0, 16.0},
//	{"PackedFile", "*packedfile", DBP_TYPE_FUN, 0, 0.0, 0.0, {0}, {0}, 0, 0, get_DataBlock_func},
	{NULL}
};

#undef MethodDef
#define MethodDef(func) _MethodDef(func, Imagemodule)

struct PyMethodDef Imagemodule_methods[] = {
	MethodDef(get),
	MethodDef(load),
	// for compatibility:
	{"Load", Imagemodule_load, METH_VARARGS, Imagemodule_load_doc},
	{NULL, NULL}
};


/*
void Image_getattr(void *vdata, char *name)
{
}
*/


PyObject *INITMODULE(Image)(void) 
{
	PyObject *mod= Py_InitModule(SUBMODULE(Image), Imagemodule_methods);
	return mod;
}




