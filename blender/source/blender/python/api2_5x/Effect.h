/*
 * $Id: Effect.h 11398 2007-07-28 06:28:33Z campbellbarton $
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

#ifndef EXPP_EFFECT_H
#define EXPP_EFFECT_H

#include <Python.h>
#include "DNA_effect_types.h"
#include "DNA_object_types.h"

extern PyTypeObject BPyEffect_Type;

#define BPyEffect_Check(v) PyObject_TypeCheck(v, &BPyEffect_Type)

/* Python BPyEffectObject structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	PartEff * effect;
	Object * object;
} BPyEffectObject;

/*****************************************************************************/
/* Python BPyEffect_Type helpder function prototypes:                           */
/*****************************************************************************/
void Effect_Init( void );
PyObject *EffectType_Init( void );
int EffectCheckPyObject( PyObject * py_obj );
PyObject *Effect_CreatePyObject( Effect * eff, Object * ob );

#endif				/* EXPP_EFFECT_H */
