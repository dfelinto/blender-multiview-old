/**
 * $Id$
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

#include "IK_ConjugateGradientSolver.h"


#define EPS 1.0e-10

	IK_ConjugateGradientSolver *
IK_ConjugateGradientSolver::
New(
){
	return new IK_ConjugateGradientSolver();
};

IK_ConjugateGradientSolver::
~IK_ConjugateGradientSolver(
){
	//nothing to do
};

// Compute the minimum of the potenial function
// starting at point p. On return p contains the 
// computed minima, iter the number of iterations performed,
// fret the potenial value at the minima

	void
IK_ConjugateGradientSolver::
Solve(
	TNT::Vector<MT_Scalar> &p,
	MT_Scalar ftol,
	int &iter,
	MT_Scalar &fret,
	DifferentiablePotenialFunctionNd &potenial,
	int max_its
){

	int j;
	MT_Scalar gg,gam,fp,dgg;

	int n = potenial.Dimension();
	ArmVectors(n);

	// initialize --- FIXME we probably have these
	// values to hand already.

	fp = potenial.Evaluate(p);
	potenial.Derivative(p,m_xi);

	for (j = 1; j<=n; j++) {
		m_g(j) = -m_xi(j);
		m_xi(j) = m_h(j) = m_g(j);
	}
	for (iter =1;iter <= max_its; iter++) {
		LineMinimize(p,m_xi,fret,potenial);

		if (2 *TNT::abs(fret-fp) <= ftol*(TNT::abs(fret) + TNT::abs(fp) + EPS)) {
			return;
		}
		fp = fret;
		potenial.Derivative(p,m_xi);
		dgg = gg = 0.0;
		
		for (j = 1; j<=n;j++) {
			gg += m_g(j)*m_g(j);
			//dgg+= xi(j)*xi(j);
			dgg += (m_xi(j) + m_g(j))*m_xi(j);
		}
		if (gg == 0.0) {
			return;
		}
		gam = dgg/gg;

		for (j = 1; j<=n; j++) {
			m_g(j) = -m_xi(j);
			m_xi(j) = m_h(j) = m_g(j) + gam*m_h(j);
		}
	}
	// FIXME throw exception
	//assert(false);
};


IK_ConjugateGradientSolver::
IK_ConjugateGradientSolver(
){
	//nothing to do
}

	void
IK_ConjugateGradientSolver::
ArmVectors(
	int dimension
){
	m_g.newsize(dimension);
	m_h.newsize(dimension);
	m_xi.newsize(dimension);
	m_xi_temp.newsize(dimension);
};

	void
IK_ConjugateGradientSolver::
LineMinimize(
	TNT::Vector<MT_Scalar> & p,
	const TNT::Vector<MT_Scalar> & xi,
	MT_Scalar &fret,
	DifferentiablePotenialFunctionNd &potenial
){
	MT_Scalar ax(0),bx(1); // initial bracket guess
	MT_Scalar cx,fa,fb,fc;

	MT_Scalar xmin(0); // the 1d function minima 

	potenial.SetLineVector(p,xi);
	IK_LineMinimizer::InitialBracket1d(ax,bx,cx,fa,fb,fc,potenial);
	fret = IK_LineMinimizer::DerivativeBrent1d(ax,bx,cx,potenial,xmin,0.001);

	// x_min in minimum along line and corresponds with 
	// p[] + x_min *xi[]
	
	TNT::vectorscale(m_xi_temp,xi,xmin);
	TNT::vectoradd(p,m_xi_temp);	
};	


#undef EPS		








