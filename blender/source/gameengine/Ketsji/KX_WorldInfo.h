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
#ifndef __KX_WORLDINFO_H
#define __KX_WORLDINFO_H

#include "MT_Scalar.h"


class MT_CmMatrix4x4;


class KX_WorldInfo
{
public:
	KX_WorldInfo(){}
	virtual ~KX_WorldInfo();

	virtual bool	hasWorld()=0;
	virtual bool	hasMist()=0;
	virtual float	getBackColorRed()=0;
	virtual float	getBackColorGreen()=0;
	virtual float	getBackColorBlue()=0;
	virtual float	getMistStart()=0;
	virtual float	getMistDistance()=0;
	virtual float	getMistColorRed()=0;
	virtual float	getMistColorGreen()=0;
	virtual float	getMistColorBlue()=0;

	virtual void	setMistStart(float)=0;
	virtual void	setMistDistance(float)=0;
	virtual void	setMistColorRed(float)=0;
	virtual void	setMistColorGreen(float)=0;
	virtual void	setMistColorBlue(float)=0;
};

#endif //__KX_WORLDINFO_H
