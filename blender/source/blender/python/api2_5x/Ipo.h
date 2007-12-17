/* 
 * $Id: Ipo.h 11398 2007-07-28 06:28:33Z campbellbarton $
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
 * Contributor(s): Jacques Guignot
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_IPO_H
#define EXPP_IPO_H

#include <Python.h>
#include "DNA_ipo_types.h"

/*****************************************************************************/
/* Python BPyIpo structure definition:                                     */
/*****************************************************************************/
typedef struct {
	PyObject_HEAD		/* required macro */
	Ipo * ipo;		/* libdata must be second */
	short iter;
	short mtex; 
} BPyIpo;

extern PyTypeObject BPyIpo_Type;

#define BPyIpo_Check(v) PyObject_TypeCheck(v, &BPyIpo_Type) /* for type checking */

/* Checks this is an IPO and it matches the ID */
#define BPyIpo_BlockType_Check(v, idcode) (BPyIpo_Check(v) && ( idcode == ((BPyIpo *)v)->ipo->blocktype ))

/* 
 *   prototypes
 */

PyObject * IpoType_Init( void );
PyObject *Ipo_CreatePyObject( struct Ipo *ipo );
#define Ipo_FromPyObject(py_obj) (((BPyIpo *)py_obj)->ipo)


#endif				/* EXPP_IPO_H */
