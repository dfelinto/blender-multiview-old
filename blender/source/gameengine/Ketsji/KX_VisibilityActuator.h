/*
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
 * Actuator to toggle visibility/invisibility of objects
 */

#ifndef __KX_VISIBILITYACTUATOR
#define __KX_VISIBILITYACTUATOR

#include "SCA_IActuator.h"

class KX_VisibilityActuator : public SCA_IActuator
{
	Py_Header;

	/** Make visible? */
	bool m_visible;

 public:
	
	KX_VisibilityActuator(
		SCA_IObject* gameobj,
		bool visible,
		PyTypeObject* T=&Type
		);

	virtual
		~KX_VisibilityActuator(
			void
			);

	virtual CValue*
		GetReplica(
			void
			);

	virtual bool
		Update(
			double curtime,
			double deltatime
			);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);
	//KX_PYMETHOD_DOC
	KX_PYMETHOD_DOC(KX_VisibilityActuator,SetVisible);

};

#endif
