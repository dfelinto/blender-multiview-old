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

#ifndef NAN_INCLUDED_IK_JacobianSolver_h

#define NAN_INCLUDED_IK_JacobianSolver_h

/**
 * @author Laurence Bourn
 * @date 28/6/2001
 */

#include "TNT/cmat.h"
#include <vector>
#include "MT_Vector3.h"
#include "IK_Chain.h"

class IK_JacobianSolver {

public :

	/**
	 * Create a new IK_JacobianSolver on the heap
	 * @return A newly created IK_JacobianSolver you take ownership of the object
     * and responsibility for deleting it
	 */


	static 
		IK_JacobianSolver *
	New(
	);

	/**
	 * Compute a solution for a chain.
     * @param chain Reference to the chain to modify
	 * @param g_position Reference to the goal position.
	 * @param g_pose -not used- Reference to the goal pose. 
	 * @param tolerance The maximum allowed distance between solution
     * and goal for termination.
	 * @param max_iterations should be in the range (50 - 500) 
     * @param max_angle_change The maximum change in the angle vector 
     * of the chain (0.1 is a good value)
	 *
     * @return True iff goal position reached.
     */

		bool
	Solve(
		IK_Chain &chain,
		const MT_Vector3 &g_position,
		const MT_Vector3 &g_pose,
		const MT_Scalar tolerance,
		const int max_iterations,
		const MT_Scalar max_angle_change
	);

	~IK_JacobianSolver(
	);


private :

	/**
	 * Private constructor to force use of New()
	 */

	IK_JacobianSolver(
	);


	/** 
	 * Compute the inverse jacobian matrix of the chain.
	 * Uses a damped least squares solution when the matrix is 
	 * is approaching singularity
     */

		int
	ComputeInverseJacobian(
		IK_Chain &chain,
		const MT_Scalar x_length,
		const MT_Scalar max_angle_change
	);

		void
	ComputeBetas(
		IK_Chain &chain,
		const MT_Vector3 d_pos
	);

	/** 
	 * Updates the angles of the chain with the newly
	 * computed values
     **/

		void
	UpdateChain(
		IK_Chain &chain
	);
	
	/**
	 * Make sure all the matrices are of the correct size
     **/

		void
	ArmMatrices(
		int dof
	);


private :

	/// the vector of intermediate betas
	TNT::Vector<MT_Scalar> m_beta;

	/// the vector of computed angle changes
	TNT::Vector<MT_Scalar> m_d_theta;

	/// the contraint gradients for each angle.
	TNT::Vector<MT_Scalar> m_dh;

	/// Space required for SVD computation

	TNT::Vector<MT_Scalar> m_svd_w;
	TNT::Matrix<MT_Scalar> m_svd_v;
	TNT::Matrix<MT_Scalar> m_svd_u;

	TNT::Matrix<MT_Scalar> m_svd_w_diag;
	TNT::Matrix<MT_Scalar> m_svd_v_t;
	TNT::Matrix<MT_Scalar> m_svd_u_t;
	TNT::Matrix<MT_Scalar> m_svd_inverse;
	TNT::Matrix<MT_Scalar> m_svd_temp1;


};




#endif



