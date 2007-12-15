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
 * Contributor(s): Jacques Guignot
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#ifndef EXPP_PARTICLE_H
#define EXPP_PARTICLE_H

#include <Python.h>
#include "DNA_effect_types.h"

extern PyTypeObject V24_Particle_Type;

#define BPy_Particle_Check(v) ((v)->ob_type==&V24_Particle_Type)

/* Python V24_BPy_Particle structure definition */
typedef struct {
	PyObject_HEAD		/* required py macro */
	Effect * particle;
} V24_BPy_Particle;

#include "Effect.h"

/*****************************************************************************/
/* Python V24_Particle_Type callback function prototypes:                        */
/*****************************************************************************/
#if 0
void V24_ParticleDeAlloc( V24_BPy_Particle * msh );
//int ParticlePrint (V24_BPy_Particle *msh, FILE *fp, int flags);
int V24_ParticleSetAttr( V24_BPy_Particle * msh, char *name, PyObject * v );
PyObject *V24_ParticleGetAttr( V24_BPy_Particle * msh, char *name );
PyObject *V24_ParticleRepr( void );
PyObject *V24_ParticleCreatePyObject( struct Effect *particle );
int V24_ParticleCheckPyObject( PyObject * py_obj );
struct Particle *V24_ParticleFromPyObject( PyObject * py_obj );
#endif



#endif				/* EXPP_PARTICLE_H */
