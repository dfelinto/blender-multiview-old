//
// Add object to the game world on action of this actuator. A copy is made
// of a referenced object. The copy inherits some properties from the owner
// of this actuator.
//
// $Id$
//
// ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
//
// The contents of this file may be used under the terms of either the GNU
// General Public License Version 2 or later (the "GPL", see
// http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
// later (the "BL", see http://www.blender.org/BL/ ) which has to be
// bought from the Blender Foundation to become active, in which case the
// above mentioned GPL option does not apply.
//
// The Original Code is Copyright (C) 2002 by NaN Holding BV.
// All rights reserved.
//
// The Original Code is: all of this file.
//
// Contributor(s): none yet.
//
// ***** END GPL/BL DUAL LICENSE BLOCK *****
//
// Previously existed as:
// \source\gameengine\GameLogic\SCA_AddObjectActuator.h
// Please look here for revision history.

#ifndef __KX_SCA_AddObjectActuator
#define __KX_SCA_AddObjectActuator

/* Actuator tree */
#include "SCA_IActuator.h"
#include "SCA_LogicManager.h"

#include "MT_Vector3.h"

class SCA_IScene;

class KX_SCA_AddObjectActuator : public SCA_IActuator
{
	Py_Header;

	/// Time field: lifetime of the new object
	int	m_timeProp;

	/// Original object reference (object to replicate)  	
	CValue*	m_OriginalObject;

	/// Object will be added to the following scene
	SCA_IScene*	m_scene;

	/// Linear velocity upon creation of the object. 
	MT_Vector3  m_linear_velocity;

	/// Apply the velocity locally 
	bool m_localFlag;
	
	SCA_IObject*	m_lastCreatedObject;
	
public:

	/** 
	 * This class also has the default constructors
	 * available. Use with care!
	 */

	KX_SCA_AddObjectActuator(
		SCA_IObject *gameobj,
		CValue* original,
		int time,
		SCA_IScene* scene,
		const MT_Vector3& linvel,
		bool local,
		PyTypeObject* T=&Type
	);

	~KX_SCA_AddObjectActuator(void);

		CValue* 
	GetReplica(
	) ;

		bool 
	Update(
		double curtime,
		double deltatime
	);

		PyObject*  
	_getattr(
		char *attr
	);

		SCA_IObject*	
	GetLastCreatedObject(
	) const ;

	/* 1. setObject */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,SetObject);
	/* 2. setTime */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,SetTime);
	/* 3. getTime */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,GetTime);
	/* 4. getObject */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,GetObject);
	/* 5. getLinearVelocity */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,GetLinearVelocity);
	/* 6. setLinearVelocity */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,SetLinearVelocity);
	/* 7. getLastCreatedObject */
	KX_PYMETHOD_DOC(KX_SCA_AddObjectActuator,GetLastCreatedObject);

	
}; /* end of class KX_SCA_AddObjectActuator : public KX_EditObjectActuator */

#endif





