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
#ifndef __KX_IPOCONVERT_H
#define __KX_IPOCONVERT_H

struct Object;

void BL_ConvertIpos(struct Object* blenderobject,
					class KX_GameObject* gameobj, 
					class KX_BlenderSceneConverter *converter
					);

void BL_ConvertLampIpos(struct Lamp* blenderlight,
						class KX_GameObject* lightobj, 
						class KX_BlenderSceneConverter *converter);

void BL_ConvertWorldIpos(struct World* blenderworld, 
						class KX_BlenderSceneConverter *converter);

void BL_ConvertCameraIpos(struct Camera* blendercamera,
						class KX_GameObject* cameraobj, 
						class KX_BlenderSceneConverter *converter);


#endif //__KX_IPOCONVERT_H
