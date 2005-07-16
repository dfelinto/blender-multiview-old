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
#ifndef GL_SIMPLEX_1TO4_H
#define GL_SIMPLEX_1TO4_H

#include "CollisionShapes/Simplex1to4Shape.h"

#include "NarrowPhaseCollision/SimplexSolverInterface.h"

///GL_Simplex1to4 is a class to debug a Simplex Solver with 1 to 4 points. 
///Can be used by GJK.
class GL_Simplex1to4 : public BU_Simplex1to4
{
	SimplexSolverInterface*	m_simplexSolver;

	public:

	GL_Simplex1to4();

	void	CalcClosest(float* m);

	void	SetSimplexSolver(SimplexSolverInterface* simplexSolver) {
		m_simplexSolver = simplexSolver;
	}

};

#endif //GL_SIMPLEX_1TO4_H