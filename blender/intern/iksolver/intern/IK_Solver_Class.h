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

#ifndef NAN_INCLUDED_IK_Solver_Class

#define NAN_INCLUDED_IK_Solver_Class 

#include "IK_Chain.h"
#include "IK_JacobianSolver.h"
#include "IK_Segment.h"
#include "MEM_SmartPtr.h"

class IK_Solver_Class {

public :

	static 
		IK_Solver_Class *
	New(
	){
		MEM_SmartPtr<IK_Solver_Class> output (new IK_Solver_Class);
	
		MEM_SmartPtr<IK_JacobianSolver> solver (IK_JacobianSolver::New());

		if (output == NULL ||
			solver == NULL
		) {
			return NULL;
		}

		output->m_solver = solver.Release();
	
		return output.Release();
	};
	
		IK_Chain &	
	Chain(
	) {
		return m_chain;
	};
		
		IK_JacobianSolver &
	Solver(
	) {
		return m_solver.Ref();
	}

	~IK_Solver_Class(
	) {
		// nothing to do
	}


private :

	IK_Solver_Class(
	) {
	};

	IK_Chain m_chain;
	MEM_SmartPtr<IK_JacobianSolver> m_solver;

};	

#endif
