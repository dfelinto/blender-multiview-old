/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
#ifndef __BLENDER_CONVERT
#define __BLENDER_CONVERT

#include "STR_String.h"
#include "KX_Python.h"
#include "KX_PhysicsEngineEnums.h"

class RAS_MeshObject* BL_ConvertMesh(struct Mesh* mesh,struct Object* lightobj,class RAS_IRenderTools* rendertools,class KX_Scene* scene, class KX_BlenderSceneConverter *converter);

void BL_ConvertBlenderObjects(struct Main* maggie,
							  const STR_String& scenename,
							  class KX_Scene* kxscene,
							  class KX_KetsjiEngine* ketsjiEngine,
							  e_PhysicsEngine	physics_engine,
							  PyObject* pythondictionary,
 							  class SCA_IInputDevice* keydev,
							  class RAS_IRenderTools* rendertools,
							  class RAS_ICanvas* canvas, 
							  class KX_BlenderSceneConverter* sceneconverter, 
							  bool alwaysUseExpandFraming
							  );

#endif // __BLENDER_CONVERT
