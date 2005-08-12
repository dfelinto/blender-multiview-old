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
#ifndef GL_SHAPE_DRAWER_H
#define GL_SHAPE_DRAWER_H

class CollisionShape;
#include "SimdVector3.h"

/// OpenGL shape drawing
class GL_ShapeDrawer
{
	public:

		static 	void	DrawOpenGL(float* m, const CollisionShape* shape, const SimdVector3& color,int	debugMode);
		static void		DrawCoordSystem();
		
};

#endif //GL_SHAPE_DRAWER_H
