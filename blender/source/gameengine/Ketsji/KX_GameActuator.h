 
//
// actuator for global game stuff
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

#ifndef __KX_GAMEACTUATOR
#define __KX_GAMEACTUATOR

#include "SCA_IActuator.h"

class KX_GameActuator : public SCA_IActuator
{
	Py_Header;
	int							m_mode;
	bool						m_restart;
	STR_String					m_filename;
	STR_String					m_loadinganimationname;
	class KX_Scene*				m_scene;
	class KX_KetsjiEngine*		m_ketsjiengine;

 public:
	enum KX_GameActuatorMode
	{
		KX_GAME_NODEF = 0,
		KX_GAME_LOAD,
		KX_GAME_START,
		KX_GAME_RESTART,
		KX_GAME_QUIT,
		KX_GAME_MAX

	};

	KX_GameActuator(SCA_IObject* gameobj,
					 int mode,
					 const STR_String& filename,
					 const STR_String& loadinganimationname,
					 KX_Scene* scene,
					 KX_KetsjiEngine* ketsjiEngine,
					 PyTypeObject* T=&Type);
	virtual ~KX_GameActuator();

	virtual CValue* GetReplica();

	virtual bool Update(double curtime,double deltatime);
	
	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	
}; /* end of class KX_GameActuator */

#endif

