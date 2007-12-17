/*
 * $Id: Particle.c 12894 2007-12-15 19:23:55Z campbellbarton $
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
 * Contributor(s): Jacques Guignot, Jean-Michel Soler 
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Particle.h" /*This must come first */

#include "DNA_object_types.h"
#include "BKE_effect.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_object.h"
#include "BLI_blenlib.h"
#include "gen_utils.h"

/*****************************************************************************/
/* Python API function prototypes for the Particle module.                   */
/*****************************************************************************/
PyObject *V24_M_Particle_New( PyObject * self, PyObject * args );
PyObject *V24_M_Particle_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* Python V24_BPy_Particle methods declarations:                                 */
/*****************************************************************************/
PyObject *V24_Effect_getType( V24_BPy_Effect * self );
PyObject *V24_Effect_setType( V24_BPy_Effect * self, PyObject * args );
PyObject *V24_Effect_getFlag( V24_BPy_Effect * self );
PyObject *V24_Effect_setFlag( V24_BPy_Effect * self, PyObject * args );
PyObject *V24_Particle_getSta( V24_BPy_Particle * self );
PyObject *V24_Particle_setSta( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getEnd( V24_BPy_Particle * self );
PyObject *V24_Particle_setEnd( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getLifetime( V24_BPy_Particle * self );
PyObject *V24_Particle_setLifetime( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getNormfac( V24_BPy_Particle * self );
PyObject *V24_Particle_setNormfac( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getObfac( V24_BPy_Particle * self );
PyObject *V24_Particle_setObfac( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getRandfac( V24_BPy_Particle * self );
PyObject *V24_Particle_setRandfac( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getTexfac( V24_BPy_Particle * self );
PyObject *V24_Particle_setTexfac( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getRandlife( V24_BPy_Particle * self );
PyObject *V24_Particle_setRandlife( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getNabla( V24_BPy_Particle * self );
PyObject *V24_Particle_setNabla( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getVectsize( V24_BPy_Particle * self );
PyObject *V24_Particle_setVectsize( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getTotpart( V24_BPy_Particle * self );
PyObject *V24_Particle_setTotpart( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getTotkey( V24_BPy_Particle * self );
PyObject *V24_Particle_setTotkey( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getSeed( V24_BPy_Particle * self );
PyObject *V24_Particle_setSeed( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getForce( V24_BPy_Particle * self );
PyObject *V24_Particle_setForce( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getMult( V24_BPy_Particle * self );
PyObject *V24_Particle_setMult( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getLife( V24_BPy_Particle * self );
PyObject *V24_Particle_setLife( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getMat( V24_BPy_Particle * self );
PyObject *V24_Particle_setMat( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getChild( V24_BPy_Particle * self );
PyObject *V24_Particle_setChild( V24_BPy_Particle * self, PyObject * a );
PyObject *V24_Particle_getDefvec( V24_BPy_Particle * self );
PyObject *V24_Particle_setDefvec( V24_BPy_Particle * self, PyObject * a );

/*****************************************************************************/
/* Python V24_Particle_Type callback function prototypes:                        */
/*****************************************************************************/
void V24_ParticleDeAlloc( V24_BPy_Particle * msh );
//int ParticlePrint (V24_BPy_Particle *msh, FILE *fp, int flags);
int V24_ParticleSetAttr( V24_BPy_Particle * msh, char *name, PyObject * v );
PyObject *V24_ParticleGetAttr( V24_BPy_Particle * msh, char *name );
PyObject *V24_ParticleRepr( void );
PyObject *V24_ParticleCreatePyObject( struct Effect *particle );
int V24_ParticleCheckPyObject( PyObject * py_obj );
struct Particle *V24_ParticleFromPyObject( PyObject * py_obj );

 
/*****************************************************************************/
/* Python V24_BPy_Particle methods table:                                        */
/*****************************************************************************/
static PyMethodDef V24_BPy_Particle_methods[] = {
	{"getType", ( PyCFunction ) V24_Effect_getType,
	 METH_NOARGS, "() - Return Effect type"},
	{"setType", ( PyCFunction ) V24_Effect_setType,
	 METH_VARARGS, "() - Set Effect type"},
	{"getFlag", ( PyCFunction ) V24_Effect_getFlag,
	 METH_NOARGS, "() - Return Effect flag"},
	{"setFlag", ( PyCFunction ) V24_Effect_setFlag,
	 METH_VARARGS, "() - Set Effect flag"},
	{"getStartTime", ( PyCFunction ) V24_Particle_getSta,
	 METH_NOARGS, "()-Return particle start time"},
	{"setStartTime", ( PyCFunction ) V24_Particle_setSta, METH_VARARGS,
	 "()- Sets particle start time"},
	{"getEndTime", ( PyCFunction ) V24_Particle_getEnd,
	 METH_NOARGS, "()-Return particle end time"},
	{"setEndTime", ( PyCFunction ) V24_Particle_setEnd, METH_VARARGS,
	 "()- Sets particle end time"},
	{"getLifetime", ( PyCFunction ) V24_Particle_getLifetime,
	 METH_NOARGS, "()-Return particle life time"},
	{"setLifetime", ( PyCFunction ) V24_Particle_setLifetime, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getNormfac", ( PyCFunction ) V24_Particle_getNormfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setNormfac", ( PyCFunction ) V24_Particle_setNormfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getObfac", ( PyCFunction ) V24_Particle_getObfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setObfac", ( PyCFunction ) V24_Particle_setObfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getRandfac", ( PyCFunction ) V24_Particle_getRandfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setRandfac", ( PyCFunction ) V24_Particle_setRandfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getTexfac", ( PyCFunction ) V24_Particle_getTexfac,
	 METH_NOARGS, "()-Return particle life time"},
	{"setTexfac", ( PyCFunction ) V24_Particle_setTexfac, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getRandlife", ( PyCFunction ) V24_Particle_getRandlife,
	 METH_NOARGS, "()-Return particle life time"},
	{"setRandlife", ( PyCFunction ) V24_Particle_setRandlife, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getNabla", ( PyCFunction ) V24_Particle_getNabla,
	 METH_NOARGS, "()-Return particle life time"},
	{"setNabla", ( PyCFunction ) V24_Particle_setNabla, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getVectsize", ( PyCFunction ) V24_Particle_getVectsize,
	 METH_NOARGS, "()-Return particle life time"},
	{"setVectsize", ( PyCFunction ) V24_Particle_setVectsize, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getTotpart", ( PyCFunction ) V24_Particle_getTotpart,
	 METH_NOARGS, "()-Return particle life time"},
	{"setTotpart", ( PyCFunction ) V24_Particle_setTotpart, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getTotkey", ( PyCFunction ) V24_Particle_getTotkey,
	 METH_NOARGS, "()-Return particle life time"},
	{"setTotkey", ( PyCFunction ) V24_Particle_setTotkey, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getSeed", ( PyCFunction ) V24_Particle_getSeed,
	 METH_NOARGS, "()-Return particle life time"},
	{"setSeed", ( PyCFunction ) V24_Particle_setSeed, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getForce", ( PyCFunction ) V24_Particle_getForce,
	 METH_NOARGS, "()-Return particle life time"},
	{"setForce", ( PyCFunction ) V24_Particle_setForce, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getMult", ( PyCFunction ) V24_Particle_getMult,
	 METH_NOARGS, "()-Return particle life time"},
	{"setMult", ( PyCFunction ) V24_Particle_setMult, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getLife", ( PyCFunction ) V24_Particle_getLife,
	 METH_NOARGS, "()-Return particle life time"},
	{"setLife", ( PyCFunction ) V24_Particle_setLife, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getMat", ( PyCFunction ) V24_Particle_getMat,
	 METH_NOARGS, "()-Return particle life time"},
	{"setMat", ( PyCFunction ) V24_Particle_setMat, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getChild", ( PyCFunction ) V24_Particle_getChild,
	 METH_NOARGS, "()-Return particle life time"},
	{"setChild", ( PyCFunction ) V24_Particle_setChild, METH_VARARGS,
	 "()- Sets particle life time "},
	{"getDefvec", ( PyCFunction ) V24_Particle_getDefvec,
	 METH_NOARGS, "()-Return particle life time"},
	{"setDefvec", ( PyCFunction ) V24_Particle_setDefvec, METH_VARARGS,
	 "()- Sets particle life time "},
	
	
	{NULL, NULL, 0, NULL}
};

/**************** prototypes ********************/
PyObject *V24_Particle_Init( void );


/*****************************************************************************/
/* Python V24_Particle_Type structure definition:                                */
/*****************************************************************************/

PyTypeObject V24_Particle_Type = {
	PyObject_HEAD_INIT( NULL )
		0,
	"Particle",
	sizeof( V24_BPy_Particle ),
	0,

	( destructor ) V24_ParticleDeAlloc,
	0,
	( getattrfunc ) V24_ParticleGetAttr,
	( setattrfunc ) V24_ParticleSetAttr,
	0,
	( reprfunc ) V24_ParticleRepr,
	0,
	0,
	0,
	0,
	0, 0, 0, 0, 0, 0,
	0,
	0, 0, 0, 0, 0, 0,
	V24_BPy_Particle_methods,
	0,
};
/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* Blender.Particle.__doc__                                                  */
/*****************************************************************************/
char V24_M_Particle_doc[] = "The Blender Particle module\n\n\
This module provides access to **Object Data** in Blender.\n\
Functions :\n\
	New(object mesh's name) : creates a new part object and adds it to the given mesh object \n\
	Get(name) : retreives a particle  with the given name (mandatory)\n\
	get(name) : same as Get.  Kept for compatibility reasons.\n";
char V24_M_Particle_New_doc[] = "New(name) : creates a new part object and adds it to the given mesh object\n";
char V24_M_Particle_Get_doc[] = "xxx";


/*****************************************************************************/
/* Python method structure definition for Blender.Particle module:           */
/*****************************************************************************/
struct PyMethodDef V24_M_Particle_methods[] = {
	{"New", ( PyCFunction ) V24_M_Particle_New, METH_VARARGS, V24_M_Particle_New_doc},
	{"Get", V24_M_Particle_Get, METH_VARARGS, V24_M_Particle_Get_doc},
	{"get", V24_M_Particle_Get, METH_VARARGS, V24_M_Particle_Get_doc},
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/* Function:              V24_M_Particle_New                                     */
/* Python equivalent:     Blender.Effect.Particle.New                        */
/* Description :          Create a particle effect and add a link            */
/*                        to the given mesh-type Object                      */
/* Data  :                String  mesh object name                           */
/* Return :               pyobject particle                                  */
/*****************************************************************************/
PyObject *V24_M_Particle_New( PyObject * self, PyObject * args )
{
	V24_BPy_Effect *pyeffect;
	Effect *bleffect = 0;
	Object *ob;
	char *name = NULL;

	if( !PyArg_ParseTuple( args, "s", &name ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected string argument" );

	for( ob = G.main->object.first; ob; ob = ob->id.next )
		if( !strcmp( name, ob->id.name + 2 ) )
			break;

	if( !ob )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError, 
				"object does not exist" );

	if( ob->type != OB_MESH )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError, 
				"object is not a mesh" );

	pyeffect = ( V24_BPy_Effect * ) PyObject_NEW( V24_BPy_Effect, &V24_Effect_Type );
	if( !pyeffect )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
				"couldn't create Effect Data object" );

	bleffect = add_effect( EFF_PARTICLE );
	if( !bleffect ) {
		Py_DECREF( pyeffect );
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"couldn't create Effect Data in Blender" );
	}

	pyeffect->effect = (PartEff *)bleffect;
	BLI_addtail( &ob->effect, bleffect );

	return ( PyObject * ) pyeffect;
}

/*****************************************************************************/
/* Function:              V24_M_Particle_Get                                     */
/* Python equivalent:     Blender.Effect.Particle.Get                        */
/*****************************************************************************/
PyObject *V24_M_Particle_Get( PyObject * self, PyObject * args )
{
	/*arguments : string object name
	   int : position of effect in the obj's effect list  */
	char *name = 0;
	Object *object_iter;
	Effect *eff;
	V24_BPy_Particle *wanted_eff;
	int num, i;

	if( !PyArg_ParseTuple( args, "si", &name, &num ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected string int argument" ) );

	object_iter = G.main->object.first;
	if( !object_iter )
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"Scene contains no object" );

	while( object_iter ) {
		if( strcmp( name, object_iter->id.name + 2 ) ) {
			object_iter = object_iter->id.next;
			continue;
		}

		if( object_iter->effect.first != NULL ) {
			eff = object_iter->effect.first;
			for( i = 0; i < num; i++ ) {
				if( eff->type != EFF_PARTICLE )
					continue;
				eff = eff->next;
				if( !eff )
					return ( V24_EXPP_ReturnPyObjError
						 ( PyExc_AttributeError,
						   "Object" ) );
			}
			wanted_eff =
				( V24_BPy_Particle * ) PyObject_NEW( V24_BPy_Particle,
								 &V24_Particle_Type );
			wanted_eff->particle = eff;
			return ( PyObject * ) wanted_eff;
		}
		object_iter = object_iter->id.next;
	}
	Py_INCREF( Py_None );
	return Py_None;
}

/*****************************************************************************/
/* Function:              V24_Particle_Init                                      */
/*****************************************************************************/
PyObject *V24_Particle_Init( void )
{
	PyObject *V24_submodule;
	
	if( PyType_Ready( &V24_Particle_Type) < 0)
		return NULL;
	
	V24_submodule =
		Py_InitModule3( "Blender.Particle", V24_M_Particle_methods,	V24_M_Particle_doc );
	return ( V24_submodule );
}

/*****************************************************************************/
/* Python V24_BPy_Particle methods:                                                */
/*****************************************************************************/

PyObject *V24_Particle_getSta( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->sta );
}



PyObject *V24_Particle_setSta( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->sta = val;
	Py_INCREF( Py_None );
	return Py_None;
}

PyObject *V24_Particle_getEnd( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->end );
}



PyObject *V24_Particle_setEnd( V24_BPy_Particle * self, PyObject * args )
{
	float val = 0;
	PartEff *ptr = ( PartEff * ) self->particle;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->end = val;
	Py_INCREF( Py_None );
	return Py_None;
}

PyObject *V24_Particle_getLifetime( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->lifetime );
}



PyObject *V24_Particle_setLifetime( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->lifetime = val;
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject *V24_Particle_getNormfac( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->normfac );
}



PyObject *V24_Particle_setNormfac( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->normfac = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getObfac( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->obfac );
}



PyObject *V24_Particle_setObfac( V24_BPy_Particle * self, PyObject * args )
{
	float val = 0;
	PartEff *ptr = ( PartEff * ) self->particle;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->obfac = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getRandfac( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->randfac );
}



PyObject *V24_Particle_setRandfac( V24_BPy_Particle * self, PyObject * args )
{
	float val = 0;
	PartEff *ptr = ( PartEff * ) self->particle;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->randfac = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getTexfac( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->texfac );
}



PyObject *V24_Particle_setTexfac( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->texfac = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getRandlife( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->randlife );
}



PyObject *V24_Particle_setRandlife( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->randlife = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getNabla( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->nabla );
}



PyObject *V24_Particle_setNabla( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->nabla = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getVectsize( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyFloat_FromDouble( ptr->vectsize );
}



PyObject *V24_Particle_setVectsize( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val = 0;
	if( !PyArg_ParseTuple( args, "f", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected float argument" ) );
	ptr->vectsize = val;
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject *V24_Particle_getTotpart( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyInt_FromLong( ptr->totpart );
}



PyObject *V24_Particle_setTotpart( V24_BPy_Particle * self, PyObject * args )
{
	int val = 0;
	PartEff *ptr = ( PartEff * ) self->particle;
	if( !PyArg_ParseTuple( args, "i", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int argument" ) );
	ptr->totpart = val;
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject *V24_Particle_getTotkey( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyInt_FromLong( ptr->totkey );
}



PyObject *V24_Particle_setTotkey( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	int val = 0;
	if( !PyArg_ParseTuple( args, "i", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int argument" ) );
	ptr->totkey = val;
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getSeed( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return PyInt_FromLong( ptr->seed );
}



PyObject *V24_Particle_setSeed( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	int val = 0;
	if( !PyArg_ParseTuple( args, "i", &val ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_AttributeError,
						"expected int argument" ) );
	ptr->seed = val;
	Py_INCREF( Py_None );
	return Py_None;
}

PyObject *V24_Particle_getForce( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return Py_BuildValue( "(f,f,f)", ptr->force[0], ptr->force[1],
			      ptr->force[2] );
}


PyObject *V24_Particle_setForce( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val[3];
	if( PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );
	val[0] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 0 ) );
	val[1] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 1 ) );
	val[2] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 2 ) );
	/*
	   if (!PyArg_ParseTuple(args, "fff", val,val+1,val+2 ))
	   return(V24_EXPP_ReturnPyObjError(PyExc_AttributeError,\
	   "expected three float arguments"));
	 */
	ptr->force[0] = val[0];
	ptr->force[1] = val[1];
	ptr->force[2] = val[2];
	Py_INCREF( Py_None );
	return Py_None;
}

PyObject *V24_Particle_getMult( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return Py_BuildValue( "(f,f,f,f)",
			      ptr->mult[0], ptr->mult[1], ptr->mult[2],
			      ptr->mult[3] );
}


PyObject *V24_Particle_setMult( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val[4];
	if( PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );
	val[0] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 0 ) );
	val[1] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 1 ) );
	val[2] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 2 ) );
	val[3] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 3 ) );
	ptr->mult[0] = val[0];
	ptr->mult[1] = val[1];
	ptr->mult[2] = val[2];
	ptr->mult[3] = val[3];
	Py_INCREF( Py_None );
	return Py_None;
}




PyObject *V24_Particle_getLife( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return Py_BuildValue( "(f,f,f,f)",
			      ptr->life[0], ptr->life[1], ptr->life[2],
			      ptr->life[3] );
}


PyObject *V24_Particle_setLife( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val[4];
	if( PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );
	val[0] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 0 ) );
	val[1] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 1 ) );
	val[2] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 2 ) );
	val[3] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 3 ) );
	ptr->life[0] = val[0];
	ptr->life[1] = val[1];
	ptr->life[2] = val[2];
	ptr->life[3] = val[3];
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getChild( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return Py_BuildValue( "(f,f,f,f)",
			      ptr->child[0], ptr->child[1], ptr->child[2],
			      ptr->child[3] );
}


PyObject *V24_Particle_setChild( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val[4];
	if( PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );
	val[0] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 0 ) );
	val[1] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 1 ) );
	val[2] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 2 ) );
	val[3] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 3 ) );
	ptr->child[0] = (short)val[0];
	ptr->child[1] = (short)val[1];
	ptr->child[2] = (short)val[2];
	ptr->child[3] = (short)val[3];
	Py_INCREF( Py_None );
	return Py_None;
}



PyObject *V24_Particle_getMat( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return Py_BuildValue( "(f,f,f,f)",
			      ptr->mat[0], ptr->mat[1], ptr->mat[2],
			      ptr->mat[3] );
}


PyObject *V24_Particle_setMat( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val[4];
	if( PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );
	val[0] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 0 ) );
	val[1] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 1 ) );
	val[2] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 2 ) );
	val[3] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 3 ) );
	ptr->mat[0] = (short)val[0];
	ptr->mat[1] = (short)val[1];
	ptr->mat[2] = (short)val[2];
	ptr->mat[3] = (short)val[3];
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject *V24_Particle_getDefvec( V24_BPy_Particle * self )
{

	PartEff *ptr = ( PartEff * ) self->particle;
	return Py_BuildValue( "(f,f,f)",
			      ptr->defvec[0], ptr->defvec[1], ptr->defvec[2] );
}


PyObject *V24_Particle_setDefvec( V24_BPy_Particle * self, PyObject * args )
{
	PartEff *ptr = ( PartEff * ) self->particle;
	float val[3];
	if( PyTuple_Size( args ) == 1 )
		args = PyTuple_GetItem( args, 0 );
	val[0] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 0 ) );
	val[1] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 1 ) );
	val[2] = (float)PyFloat_AsDouble( PyTuple_GetItem( args, 2 ) );
	ptr->defvec[0] = val[0];
	ptr->defvec[1] = val[1];
	ptr->defvec[2] = val[2];
	Py_INCREF( Py_None );
	return Py_None;
}


/*****************************************************************************/
/* Function:    V24_ParticleDeAlloc                                              */
/* Description: This is a callback function for the V24_BPy_Particle type. It is   */
/*              the destructor function.                                     */
/*****************************************************************************/
void V24_ParticleDeAlloc( V24_BPy_Particle * self )
{
	PartEff *ptr = ( PartEff * ) self;
	PyObject_DEL( ptr );
}

/*****************************************************************************/
/* Function:    V24_ParticleGetAttr                                              */
/* Description: This is a callback function for the V24_BPy_Particle type. It is */
/*              the function that accesses V24_BPy_Particle "member variables"   */
/*              and  methods.                                                */
/*****************************************************************************/


PyObject *V24_ParticleGetAttr( V24_BPy_Particle * self, char *name )
{

	if( strcmp( name, "seed" ) == 0 )
		return V24_Particle_getSeed( self );
	else if( strcmp( name, "nabla" ) == 0 )
		return V24_Particle_getNabla( self );
	else if( strcmp( name, "sta" ) == 0 )
		return V24_Particle_getSta( self );
	else if( strcmp( name, "end" ) == 0 )
		return V24_Particle_getEnd( self );
	else if( strcmp( name, "lifetime" ) == 0 )
		return V24_Particle_getLifetime( self );
	else if( strcmp( name, "normfac" ) == 0 )
		return V24_Particle_getNormfac( self );
	else if( strcmp( name, "obfac" ) == 0 )
		return V24_Particle_getObfac( self );
	else if( strcmp( name, "randfac" ) == 0 )
		return V24_Particle_getRandfac( self );
	else if( strcmp( name, "texfac" ) == 0 )
		return V24_Particle_getTexfac( self );
	else if( strcmp( name, "randlife" ) == 0 )
		return V24_Particle_getRandlife( self );
	else if( strcmp( name, "vectsize" ) == 0 )
		return V24_Particle_getVectsize( self );
	else if( strcmp( name, "totpart" ) == 0 )
		return V24_Particle_getTotpart( self );
	else if( strcmp( name, "force" ) == 0 )
		return V24_Particle_getForce( self );
	else if( strcmp( name, "mult" ) == 0 )
		return V24_Particle_getMult( self );
	else if( strcmp( name, "life" ) == 0 )
		return V24_Particle_getLife( self );
	else if( strcmp( name, "child" ) == 0 )
		return V24_Particle_getChild( self );
	else if( strcmp( name, "mat" ) == 0 )
		return V24_Particle_getMat( self );
	else if( strcmp( name, "defvec" ) == 0 )
		return V24_Particle_getDefvec( self );


	return Py_FindMethod( V24_BPy_Particle_methods, ( PyObject * ) self,
			      name );
}

/*****************************************************************************/
/* Function:    V24_ParticleSetAttr                                              */
/* Description: This is a callback function for the V24_BPy_Particle type.     */
/*              It is the  function that sets Particle Data attributes     */
/*              (member vars)    */
/*****************************************************************************/
int V24_ParticleSetAttr( V24_BPy_Particle * self, char *name, PyObject * value )
{

	PyObject *valtuple;
	PyObject *error = NULL;

	valtuple = Py_BuildValue( "(N)", value );

	if( !valtuple )
		return V24_EXPP_ReturnIntError( PyExc_MemoryError,
					    "V24_ParticleSetAttr: couldn't create PyTuple" );

	if( strcmp( name, "seed" ) == 0 )
		error = V24_Particle_setSeed( self, valtuple );
	else if( strcmp( name, "nabla" ) == 0 )
		error = V24_Particle_setNabla( self, valtuple );
	else if( strcmp( name, "sta" ) == 0 )
		error = V24_Particle_setSta( self, valtuple );
	else if( strcmp( name, "end" ) == 0 )
		error = V24_Particle_setEnd( self, valtuple );
	else if( strcmp( name, "lifetime" ) == 0 )
		error = V24_Particle_setLifetime( self, valtuple );
	else if( strcmp( name, "normfac" ) == 0 )
		error = V24_Particle_setNormfac( self, valtuple );
	else if( strcmp( name, "obfac" ) == 0 )
		error = V24_Particle_setObfac( self, valtuple );
	else if( strcmp( name, "randfac" ) == 0 )
		error = V24_Particle_setRandfac( self, valtuple );
	else if( strcmp( name, "texfac" ) == 0 )
		error = V24_Particle_setTexfac( self, valtuple );
	else if( strcmp( name, "randlife" ) == 0 )
		error = V24_Particle_setRandlife( self, valtuple );
	else if( strcmp( name, "nabla" ) == 0 )
		error = V24_Particle_setNabla( self, valtuple );
	else if( strcmp( name, "vectsize" ) == 0 )
		error = V24_Particle_setVectsize( self, valtuple );
	else if( strcmp( name, "totpart" ) == 0 )
		error = V24_Particle_setTotpart( self, valtuple );
	else if( strcmp( name, "seed" ) == 0 )
		error = V24_Particle_setSeed( self, valtuple );
	else if( strcmp( name, "force" ) == 0 )
		error = V24_Particle_setForce( self, valtuple );
	else if( strcmp( name, "mult" ) == 0 )
		error = V24_Particle_setMult( self, valtuple );
	else if( strcmp( name, "life" ) == 0 )
		error = V24_Particle_setLife( self, valtuple );
	else if( strcmp( name, "child" ) == 0 )
		error = V24_Particle_setChild( self, valtuple );
	else if( strcmp( name, "mat" ) == 0 )
		error = V24_Particle_setMat( self, valtuple );
	else if( strcmp( name, "defvec" ) == 0 )
		error = V24_Particle_setDefvec( self, valtuple );

	else {
		Py_DECREF( valtuple );

		if( ( strcmp( name, "Types" ) == 0 ) ||
		    ( strcmp( name, "Modes" ) == 0 ) )
			return ( V24_EXPP_ReturnIntError( PyExc_AttributeError,
						      "constant dictionary -- cannot be changed" ) );

		else
			return ( V24_EXPP_ReturnIntError( PyExc_KeyError,
						      "attribute not found" ) );
	}

	Py_DECREF(valtuple);
	if( error != Py_None )
		return -1;

	Py_DECREF( Py_None );
	return 0;
}

/*****************************************************************************/
/* Function:    ParticlePrint                                                */
/* Description: This is a callback function for the V24_BPy_Particle type. It    */
/*              particles a meaninful string to 'print' particle objects.    */
/*****************************************************************************/
/*
int ParticlePrint(V24_BPy_Particle *self, FILE *fp, int flags) 
{ 
  printf("Hi, I'm a particle!");	
  return 0;
}
*/
/*****************************************************************************/
/* Function:    V24_ParticleRepr                                                 */
/* Description: This is a callback function for the V24_BPy_Particle type. It    */
/*              particles a meaninful string to represent particle objects.  */
/*****************************************************************************/
PyObject *V24_ParticleRepr( void )
{
	return PyString_FromString( "Particle" );
}

PyObject *V24_ParticleCreatePyObject( struct Effect * particle )
{
	V24_BPy_Particle *blen_object;


	blen_object =
		( V24_BPy_Particle * ) PyObject_NEW( V24_BPy_Particle,
						 &V24_Particle_Type );

	if( blen_object == NULL ) {
		return ( NULL );
	}
	blen_object->particle = particle;
	return ( ( PyObject * ) blen_object );

}

int V24_ParticleCheckPyObject( PyObject * py_obj )
{
	return ( py_obj->ob_type == &V24_Particle_Type );
}


struct Particle *V24_ParticleFromPyObject( PyObject * py_obj )
{
	V24_BPy_Particle *blen_obj;

	blen_obj = ( V24_BPy_Particle * ) py_obj;
	return ( ( struct Particle * ) blen_obj->particle );

}
