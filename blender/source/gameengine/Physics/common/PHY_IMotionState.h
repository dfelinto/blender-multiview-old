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
#ifndef PHY__MOTIONSTATE_H
#define PHY__MOTIONSTATE_H


/**
	PHY_IMotionState is the Interface to explicitly synchronize the world transformation.
	Default implementations for mayor graphics libraries like OpenGL and DirectX can be provided.
*/
class	PHY_IMotionState

{
	public:
		
		virtual ~PHY_IMotionState();

		virtual void	getWorldPosition(float& posX,float& posY,float& posZ)=0;
		virtual void	getWorldScaling(float& scaleX,float& scaleY,float& scaleZ)=0;
		virtual void	getWorldOrientation(float& quatIma0,float& quatIma1,float& quatIma2,float& quatReal)=0;
		
		virtual void	setWorldPosition(float posX,float posY,float posZ)=0;
		virtual	void	setWorldOrientation(float quatIma0,float quatIma1,float quatIma2,float quatReal)=0;

		virtual	void	calculateWorldTransformations()=0;
};

#endif //PHY__MOTIONSTATE_H
