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
#ifndef __RAS_LIGHTOBJECT_H
#define __RAS_LIGHTOBJECT_H

#include "MT_CmMatrix4x4.h"

struct RAS_LightObject
{
	enum LightType{
		LIGHT_SPOT,
		LIGHT_SUN,
		LIGHT_NORMAL
	};
	bool	m_modified;
	int		m_layer;
	
	float	m_energy;
	float	m_distance;
	
	float	m_red;
	float	m_green;
	float	m_blue;

	float	m_att1;
	float	m_spotsize;
	float	m_spotblend;

	LightType	m_type;
	MT_CmMatrix4x4*	m_worldmatrix;
	
};

#endif //__RAS_LIGHTOBJECT_H
