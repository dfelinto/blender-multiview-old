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

#include "MyMotionState.h"
#include "SimdPoint3.h"

MyMotionState::MyMotionState()
{
	m_worldTransform.setIdentity();
}


MyMotionState::~MyMotionState()
{

}

void	MyMotionState::getWorldPosition(float& posX,float& posY,float& posZ)
{
	posX = m_worldTransform.getOrigin().x();
	posY = m_worldTransform.getOrigin().y();
	posZ = m_worldTransform.getOrigin().z();
}

void	MyMotionState::getWorldScaling(float& scaleX,float& scaleY,float& scaleZ)
{
	scaleX = 1.;
	scaleY = 1.;
	scaleZ = 1.;
}

void	MyMotionState::getWorldOrientation(float& quatIma0,float& quatIma1,float& quatIma2,float& quatReal)
{
	quatIma0 = m_worldTransform.getRotation().x();
	quatIma1 = m_worldTransform.getRotation().y();
	quatIma2 = m_worldTransform.getRotation().z();
	quatReal = m_worldTransform.getRotation()[3];
}
		
void	MyMotionState::setWorldPosition(float posX,float posY,float posZ)
{
	SimdPoint3 pos(posX,posY,posZ);
	m_worldTransform.setOrigin( pos );
}

void	MyMotionState::setWorldOrientation(float quatIma0,float quatIma1,float quatIma2,float quatReal)
{
	SimdQuaternion orn(quatIma0,quatIma1,quatIma2,quatReal);
	m_worldTransform.setRotation( orn );
}
		
void	MyMotionState::calculateWorldTransformations()
{

}
