/**
 * Draw a random number, and put it in a property
 *
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

#ifndef __KX_RANDOMACTUATOR
#define __KX_RANDOMACTUATOR

#include "SCA_IActuator.h"
#include "SCA_RandomNumberGenerator.h"

class SCA_RandomActuator : public SCA_IActuator
{
	Py_Header;
	/** Property to assign to */
	STR_String m_propname;
	
	/** First parameter. The meaning of the parameters depends on the        
	 *  distribution */
	float m_parameter1;
	/** Second parameter. The meaning of the parameters depends on the        
	 *  distribution */
	float m_parameter2;

	/** The base generator */
	SCA_RandomNumberGenerator *m_base;

	/** just a generic, persistent counter */
	int m_counter;

	/** cache for the previous draw */
	long m_previous;

	/** apply constraints for the chosen distribution to the parameters */
	void enforceConstraints(void);
	
 public:

	enum KX_RANDOMACT_MODE {
		KX_RANDOMACT_NODEF,
		KX_RANDOMACT_BOOL_CONST,
		KX_RANDOMACT_BOOL_UNIFORM,
		KX_RANDOMACT_BOOL_BERNOUILLI,		
		KX_RANDOMACT_INT_CONST,
		KX_RANDOMACT_INT_UNIFORM,
		KX_RANDOMACT_INT_POISSON,		
		KX_RANDOMACT_FLOAT_CONST,
		KX_RANDOMACT_FLOAT_UNIFORM,
		KX_RANDOMACT_FLOAT_NORMAL,
		KX_RANDOMACT_FLOAT_NEGATIVE_EXPONENTIAL,
		KX_RANDOMACT_MAX
	};
	/** distribution type */
	KX_RANDOMACT_MODE m_distribution;
	
	SCA_RandomActuator(class SCA_IObject* gameobj,
					  long seed,
					  KX_RANDOMACT_MODE mode,
					  float para1,
					  float para2,
					  const STR_String &propName,
					  PyTypeObject* T=&Type);
	virtual ~SCA_RandomActuator();
	virtual bool Update(double curtime,double deltatime);
	
	virtual CValue* GetReplica();
	
	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	/* 1. setSeed                                                            */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetSeed);
	/* 2. getSeed                                                            */
	KX_PYMETHOD_DOC(SCA_RandomActuator,GetSeed);
	/* 3. setPara1 -removed-                                                 */
	/* 4. getPara1                                                           */
	KX_PYMETHOD_DOC(SCA_RandomActuator,GetPara1);
	/* 5. setPara2 -removed-                                                 */
	/* 6. getPara2                                                           */
	KX_PYMETHOD_DOC(SCA_RandomActuator,GetPara2);
	/* 7. setDistribution -removed-                                          */
	/* 8. getDistribution                                                    */
	KX_PYMETHOD_DOC(SCA_RandomActuator,GetDistribution);
	/* 9. setProperty                                                        */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetProperty);
	/* 10. getProperty                                                       */
	KX_PYMETHOD_DOC(SCA_RandomActuator,GetProperty);
	/* 11. setBoolConst                                                      */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetBoolConst);
	/* 12. setBoolUniform, */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetBoolUniform);
	/* 13. setBoolBernouilli,  */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetBoolBernouilli);
	/* 14. setIntConst,*/
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetIntConst);
	/* 15. setIntUniform,*/
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetIntUniform);
	/* 16. setIntPoisson,		*/
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetIntPoisson);
	/* 17. setFloatConst,*/
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetFloatConst);
	/* 18. setFloatUniform, */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetFloatUniform);
	/* 19. setFloatNormal, */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetFloatNormal);
	/* 20. setFloatNegativeExponential, */
	KX_PYMETHOD_DOC(SCA_RandomActuator,SetFloatNegativeExponential);
	
}; /* end of class KX_EditObjectActuator : public SCA_PropertyActuator */

#endif
