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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 *
 * @author Laurence
 */

#ifndef NAN_INCLUDED_IK_Solver_Class
#define NAN_INCLUDED_IK_Solver_Class 

#include "IK_QChain.h"
#include "IK_QJacobianSolver.h"
#include "IK_QSegment.h"
#include "MEM_SmartPtr.h"

/**
 * This class just contains all instances of internal data 
 * associated with the external chain structure needed for 
 * an ik solve. A pointer to this class gets hidden in the 
 * external structure as a void pointer.
 */

class IK_QSolver_Class {

public :

	static 
		IK_QSolver_Class *
	New(
	){
		MEM_SmartPtr<IK_QSolver_Class> output (new IK_QSolver_Class);
	
		MEM_SmartPtr<IK_QJacobianSolver> solver (IK_QJacobianSolver::New());

		if (output == NULL ||
			solver == NULL
		) {
			return NULL;
		}

		output->m_solver = solver.Release();
	
		return output.Release();
	};
	
		IK_QChain &	
	Chain(
	) {
		return m_chain;
	};
		
		IK_QJacobianSolver &
	Solver(
	) {
		return m_solver.Ref();
	}

	~IK_QSolver_Class(
	) {
		// nothing to do
	}


private :

	IK_QSolver_Class(
	) {
	};

	IK_QChain m_chain;
	MEM_SmartPtr<IK_QJacobianSolver> m_solver;

};	

#endif
