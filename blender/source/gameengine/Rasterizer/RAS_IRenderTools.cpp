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

#include "RAS_IRenderTools.h"



void RAS_IRenderTools::SetViewMat(const MT_Transform& trans)
{
	trans.getValue(m_viewmat);
}
	


void RAS_IRenderTools::SetClientObject(void* obj)
{
	if (m_clientobject != obj)
	{
		m_clientobject = obj;
		m_modified = true;
	}
}



void RAS_IRenderTools::SetAuxilaryClientInfo(void* inf)
{
	m_auxilaryClientInfo = inf;
}



void RAS_IRenderTools::AddLight(struct RAS_LightObject* lightobject)
{
	m_lights.push_back(lightobject);
}



void RAS_IRenderTools::RemoveLight(struct RAS_LightObject* lightobject)
{
	std::vector<struct	RAS_LightObject*>::iterator lit = 
		std::find(m_lights.begin(),m_lights.end(),lightobject);

	if (!(lit==m_lights.end()))
		m_lights.erase(lit);

}
