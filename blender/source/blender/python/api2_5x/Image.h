/* 
 * $Id: Image.h 11398 2007-07-28 06:28:33Z campbellbarton $
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
 * Contributor(s): Willian P. Germano, Alex Mole
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_IMAGE_H
#define EXPP_IMAGE_H

#include <Python.h>
#include "DNA_image_types.h"


/*****************************************************************************/
/* Python BPyImageObject structure definition                                     */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD 
	Image * image; /* libdata must be second */
} BPyImageObject;

extern PyTypeObject BPyImage_Type;	/* The Image PyType Object */

#define BPyImage_Check(v) PyObject_TypeCheck(v, &BPyImage_Type) /*for type checking */

/*****************************************************************************/
/* Module Blender.Image - public functions                                   */
/*****************************************************************************/
void Image_Init( void );
PyObject *ImageType_Init( void );
PyObject *Image_CreatePyObject( Image * image );
#define Image_FromPyObject(py_obj) (((BPyImageObject *)py_obj)->image)

#endif				/* EXPP_IMAGE_H */
