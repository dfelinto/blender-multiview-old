/**
 * $Id$
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

#include "SCA_IScene.h"



SCA_IScene::SCA_IScene()
{
}



SCA_IScene::~SCA_IScene()
{
	// release debugprop list
	for (std::vector<SCA_DebugProp*>::iterator it = m_debugList.begin();
		!(it==m_debugList.end());it++)
	{
		delete (*it);
	}
}


std::vector<SCA_DebugProp*>& SCA_IScene::GetDebugProperties() 
{
	return m_debugList;
}



void SCA_IScene::AddDebugProperty(class CValue* debugprop,
								  const STR_String &name)
{
	SCA_DebugProp* dprop = new SCA_DebugProp();
	dprop->m_obj = debugprop;
	dprop->m_name = name;
	m_debugList.push_back(dprop);
}
