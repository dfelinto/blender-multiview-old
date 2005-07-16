/*
 * Copyright (c) 2005 Erwin Coumans <www.erwincoumans.com>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erwin Coumans makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 */

#ifndef MY_MOTIONSTATE_H
#define MY_MOTIONSTATE_H

#include "PHY_IMotionState.h"
#include <SimdTransform.h>


class	MyMotionState : public PHY_IMotionState

{
	public:
		MyMotionState();

		virtual ~MyMotionState();

		virtual void	getWorldPosition(float& posX,float& posY,float& posZ);
		virtual void	getWorldScaling(float& scaleX,float& scaleY,float& scaleZ);
		virtual void	getWorldOrientation(float& quatIma0,float& quatIma1,float& quatIma2,float& quatReal);
		
		virtual void	setWorldPosition(float posX,float posY,float posZ);
		virtual	void	setWorldOrientation(float quatIma0,float quatIma1,float quatIma2,float quatReal);
		
		virtual	void	calculateWorldTransformations();
		
		SimdTransform	m_worldTransform;

};

#endif //MY_MOTIONSTATE_H