/**
 * Implementationclass to derive controllers from
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

#ifndef __SG_CONTROLLER_H
#define __SG_CONTROLLER_H


#include "SG_IObject.h"
class SG_Controller 
{
public:
	SG_Controller(
	) :
		m_pObject(NULL) {
	}

	virtual 
	~SG_Controller(
	) {};

	virtual 
		bool	
	Update(
		double time
	)=0;

	virtual 
		void 
	SetObject (
		SG_IObject* object
	);

		void
	ClearObject(
	);

	virtual 
		void	
	SetSimulatedTime(
		double time
	)=0;

	virtual	
		SG_Controller*	
	GetReplica(
		class SG_Node* destnode
	)=0;

	/**
	 * Hacky way of passing options to specific controllers
	 * @param option An integer identifying the option.
	 * @param value  The value of this option.
	 * @attention This has been placed here to give sca-elements 
	 * @attention some control over the controllers. This is 
	 * @attention necessary because the identity of the controller
	 * @attention is lost on the way here.
	 */
	virtual
		void
	SetOption(
		int option,
		int value
	)=0;

	/**
	 * Option-identifiers: SG_CONTR_<controller-type>_<option>. 
	 * Options only apply to a specific controller type. The 
	 * semantics are defined by whoever uses the setting.
	 */
	enum SG_Controller_option {
		SG_CONTR_NODEF = 0,
		SG_CONTR_IPO_IPO_AS_FORCE,
		SG_CONTR_IPO_FORCES_ACT_LOCAL,
		SG_CONTR_CAMIPO_LENS,
		SG_CONTR_CAMIPO_CLIPEND,
		SG_CONTR_CAMIPO_CLIPSTART,
		SG_CONTR_MAX
	};

protected:
	SG_IObject*		m_pObject;

};

#endif //__SG_CONTROLLER_H
