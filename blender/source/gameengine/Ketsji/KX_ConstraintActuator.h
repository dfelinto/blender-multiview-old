/**
 * KX_ConstraintActuator.h
 *
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

#ifndef __KX_CONSTRAINTACTUATOR
#define __KX_CONSTRAINTACTUATOR

#include "SCA_IActuator.h"
#include "MT_Scalar.h"


class KX_ConstraintActuator : public SCA_IActuator
{
	Py_Header;
	
	// Damp time (int),
	int m_dampTime;
	// min (float),
	float m_minimumBound;
	// max (float),
	float m_maximumBound;
	// locrotxyz choice (pick one): only one choice allowed at a time!
	int m_locrot;

	/**
	 * Clamp <var> to <min>, <max>. Borders are included (in as far as
	 * float comparisons are good for equality...).
	 */
	void Clamp(MT_Scalar &var, float min, float max);

	
 public:
	enum KX_CONSTRAINTTYPE {
		KX_ACT_CONSTRAINT_NODEF = 0,
		KX_ACT_CONSTRAINT_LOCX,
		KX_ACT_CONSTRAINT_LOCY,
		KX_ACT_CONSTRAINT_LOCZ,
		KX_ACT_CONSTRAINT_ROTX,
		KX_ACT_CONSTRAINT_ROTY,
		KX_ACT_CONSTRAINT_ROTZ,
		KX_ACT_CONSTRAINT_MAX
	};

	bool IsValidMode(KX_CONSTRAINTTYPE m); 

	KX_ConstraintActuator(SCA_IObject* gameobj,
						  int damptime,
						  float min,
						  float max,
						  int locrot,
						  PyTypeObject* T=&Type);
	virtual ~KX_ConstraintActuator();
	virtual CValue* GetReplica() {
		KX_ConstraintActuator* replica = new KX_ConstraintActuator(*this);
		replica->ProcessReplica();
		// this will copy properties and so on...
		CValue::AddDataToReplica(replica);
		return replica;
	};

	virtual bool Update(double curtime,double deltatime);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	KX_PYMETHOD_DOC(KX_ConstraintActuator,SetDamp);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,GetDamp);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,SetMin);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,GetMin);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,SetMax);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,GetMax);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,SetLimit);
	KX_PYMETHOD_DOC(KX_ConstraintActuator,GetLimit);

};
#endif //__KX_CONSTRAINTACTUATOR
