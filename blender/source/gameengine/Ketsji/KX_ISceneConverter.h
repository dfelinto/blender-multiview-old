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
#ifndef __KX_ISCENECONVERTER_H
#define __KX_ISCENECONVERTER_H

#include "STR_String.h"

#include "KX_Python.h"

class KX_ISceneConverter 
{

public:
	KX_ISceneConverter() {}
	virtual ~KX_ISceneConverter () {};

	/*
	scenename: name of the scene to be converted,
		if the scenename is empty, convert the 'default' scene (whatever this means)
	destinationscene: pass an empty scene, everything goes into this
	dictobj: python dictionary (for pythoncontrollers)
	*/
	virtual void ConvertScene(const STR_String& scenename,
						class KX_Scene* destinationscene,
						PyObject* dictobj,
						class SCA_IInputDevice* keyinputdev,
						class RAS_IRenderTools* rendertools, 
						class RAS_ICanvas*  canvas)=0;
	
	virtual void	SetAlwaysUseExpandFraming(bool to_what) = 0;

	virtual void	SetNewFileName(const STR_String& filename) = 0;
	virtual bool	TryAndLoadNewFile() = 0;
};
#endif //__KX_ISCENECONVERTER_H
