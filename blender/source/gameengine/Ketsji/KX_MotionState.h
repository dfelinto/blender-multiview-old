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
#ifndef __KX_MOTIONSTATE
#define __KX_MOTIONSTATE

#include "PHY_IMotionState.h"

class KX_MotionState : public PHY_IMotionState
{
	class	SG_Spatial*		m_node;

public:
	KX_MotionState(class SG_Spatial* spatial);
	virtual ~KX_MotionState();

	virtual void	getWorldPosition(float& posX,float& posY,float& posZ);
	virtual void	getWorldScaling(float& scaleX,float& scaleY,float& scaleZ);
	virtual void	getWorldOrientation(float& quatIma0,float& quatIma1,float& quatIma2,float& quatReal);
	virtual void	setWorldPosition(float posX,float posY,float posZ);
	virtual	void	setWorldOrientation(float quatIma0,float quatIma1,float quatIma2,float quatReal);

	virtual	void	calculateWorldTransformations();
};

#endif //__KX_MOTIONSTATE
