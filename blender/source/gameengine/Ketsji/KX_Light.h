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
#ifndef __KX_LIGHT
#define __KX_LIGHT

#include "RAS_LightObject.h"
#include "KX_GameObject.h"

class KX_LightObject : public KX_GameObject
{
	RAS_LightObject		m_lightobj;
	class RAS_IRenderTools*	m_rendertools;	//needed for registering and replication of lightobj

public:
	KX_LightObject(void* sgReplicationInfo,SG_Callbacks callbacks,class RAS_IRenderTools* rendertools,const struct RAS_LightObject&	lightobj);
	virtual ~KX_LightObject();
	virtual CValue*		GetReplica();
	RAS_LightObject*	GetLightData() { return &m_lightobj;}
};

#endif //__KX_LIGHT
