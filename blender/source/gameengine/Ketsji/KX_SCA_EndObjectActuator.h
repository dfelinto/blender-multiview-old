//
// Add object to the game world on action of this actuator
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
// \source\gameengine\GameLogic\SCA_EndObjectActuator.h
// Please look here for revision history.

#ifndef __KX_SCA_ENDOBJECTACTUATOR
#define __KX_SCA_ENDOBJECTACTUATOR

#include "SCA_IActuator.h"

class SCA_IScene;

class KX_SCA_EndObjectActuator : public SCA_IActuator
{
	Py_Header;
	SCA_IScene*		m_scene;

 public:
	KX_SCA_EndObjectActuator(
		SCA_IObject* gameobj,
		SCA_IScene* scene,
		PyTypeObject* T=&Type
	);

	~KX_SCA_EndObjectActuator();

		CValue* 
	GetReplica(
	);

		bool 
	Update(
		double curtime,
		double deltatime
	);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

		PyObject*  
	_getattr(
		char *attr
	);
	
}; /* end of class KX_EditObjectActuator : public SCA_PropertyActuator */

#endif
