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
#ifndef __BLENDERWORLDINFO_H
#define __BLENDERWORLDINFO_H
#include "MT_CmMatrix4x4.h"
#include "KX_WorldInfo.h"
#include "KX_BlenderGL.h"


class BlenderWorldInfo : public KX_WorldInfo
{
	bool			m_hasworld;
	float			m_backgroundred;
	float			m_backgroundgreen;
	float			m_backgroundblue;

	bool			m_hasmist;
	float			m_miststart;
	float			m_mistdistance;
	float			m_mistred;
	float			m_mistgreen;
	float			m_mistblue;
	
public:
	BlenderWorldInfo(struct World* blenderworld);
	~BlenderWorldInfo();

	bool	hasWorld();
    bool	hasMist();
    float	getBackColorRed();
    float	getBackColorGreen();
    float	getBackColorBlue();

    float	getMistStart();
    float	getMistDistance();
    float	getMistColorRed();
    float	getMistColorGreen();
    float	getMistColorBlue();     

		void	
	setMistStart(
		float d
	);

		void	
	setMistDistance(
		float d
	);

		void	
	setMistColorRed(
		float d
	);

		void	
	setMistColorGreen(
		float d
	);

		void	
	setMistColorBlue(
		float d
	);   
};

#endif //__BLENDERWORLDINFO_H
