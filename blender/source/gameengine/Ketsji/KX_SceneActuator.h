 
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

#ifndef __KX_SCENEACTUATOR
#define __KX_SCENEACTUATOR

#include "SCA_IActuator.h"

class KX_SceneActuator : public SCA_IActuator
{
	Py_Header;
	
	int							m_mode;
	// (restart) has become a toggle internally... not in the interface though
	bool						m_restart;
	// (set Scene) Scene
	/** The current scene. */
	class	KX_Scene*			m_scene;
	class	KX_KetsjiEngine*	m_KetsjiEngine;
	/** The scene to switch to. */
	STR_String					m_nextSceneName;
	
	// (Set Camera) Object
	class KX_Camera*			m_camera;

	/** Is this a valid scene? */
	class KX_Scene* FindScene(char* sceneName);
	/** Is this a valid camera? */
	class KX_Camera* FindCamera(char* cameraName);
	
 public:
	enum SCA_SceneActuatorMode
	{
		KX_SCENE_NODEF = 0,
		KX_SCENE_RESTART,
		KX_SCENE_SET_SCENE,
		KX_SCENE_SET_CAMERA,
		KX_SCENE_ADD_FRONT_SCENE,
		KX_SCENE_ADD_BACK_SCENE,
		KX_SCENE_REMOVE_SCENE,
		KX_SCENE_SUSPEND,
		KX_SCENE_RESUME,
		KX_SCENE_MAX
	};
	
	KX_SceneActuator(SCA_IObject* gameobj,
					 int mode,
					 KX_Scene* scene,
					 KX_KetsjiEngine* ketsjiEngine,
					 const STR_String& nextSceneName,
					 KX_Camera* camera,
					 PyTypeObject* T=&Type);
	virtual ~KX_SceneActuator();

	virtual CValue* GetReplica();

	virtual bool Update(double curtime,double deltatime);
	
	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	/* 1. set                                                                */
	/* Removed */
	  
	/* 2. setUseRestart:                                                     */
	KX_PYMETHOD_DOC(KX_SceneActuator,SetUseRestart);
	/* 3. getUseRestart:                                                     */
	KX_PYMETHOD_DOC(KX_SceneActuator,GetUseRestart);
	/* 4. setScene:                                                          */
	KX_PYMETHOD_DOC(KX_SceneActuator,SetScene);
	/* 5. getScene:                                                          */
	KX_PYMETHOD_DOC(KX_SceneActuator,GetScene);
	/* 6. setCamera:                                                          */
	KX_PYMETHOD_DOC(KX_SceneActuator,SetCamera);
	/* 7. getCamera:                                                          */
	KX_PYMETHOD_DOC(KX_SceneActuator,GetCamera);
	
}; /* end of class KXSceneActuator */

#endif
