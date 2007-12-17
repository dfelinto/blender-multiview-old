/* 
 * $Id: MTex.h 11446 2007-07-31 16:11:32Z campbellbarton $
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
 * Contributor(s): Alex Mole
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_MTEX_H
#define EXPP_MTEX_H

#include <Python.h>
#include "DNA_ID.h"
#include "DNA_texture_types.h"

/*****************************************************************************/
/* Python BPyMTexObject structure definition                                      */
/*****************************************************************************/

typedef struct {
	PyObject_HEAD
	/* this is ok because materials, lamps and worlds cant be removed
	 * otherwise would need to point to their PyObject and become a user */
	ID * id;
	short index;
} BPyMTexObject;

extern PyTypeObject BPyMTex_Type;

#define BPyMTex_Check(v) PyObject_TypeCheck(v, &BPyMTex_Type)


/*****************************************************************************/
/* Module Blender.Texture.MTex - public functions                            */
/*****************************************************************************/

PyObject *MTexType_Init( void );
PyObject *MTex_CreatePyObject( ID * id, short index );
#define MTex_FromPyObject( py_obj ) ((( BPyMTexObject * ) py_obj )->mtex)
MTex *MTex_get_pointer( BPyMTexObject *self ); /* color uses this to get the color pointer */

#endif				/* EXPP_MTEX_H */
