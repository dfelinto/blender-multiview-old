/**
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
#ifndef PHYP_PHYSICSOBJECT_WRAPPER
#define PHYP_PHYSICSOBJECT_WRAPPER

#include "Value.h"
#include "PHY_DynamicTypes.h"

class	KX_PhysicsObjectWrapper : public PyObjectPlus
{
	Py_Header;

	PyObject*			_getattr(char* attr);
	virtual int 			_setattr(char *attr, PyObject *value);
public:
	KX_PhysicsObjectWrapper(class PHY_IPhysicsController* ctrl,class PHY_IPhysicsEnvironment* physenv,PyTypeObject *T = &Type);
	virtual ~KX_PhysicsObjectWrapper();
	
	KX_PYMETHOD(KX_PhysicsObjectWrapper , SetPosition);
	KX_PYMETHOD(KX_PhysicsObjectWrapper,SetLinearVelocity);
	KX_PYMETHOD(KX_PhysicsObjectWrapper,SetAngularVelocity);
	KX_PYMETHOD(KX_PhysicsObjectWrapper,SetActive);

private:
	class PHY_IPhysicsController*	m_ctrl;
	PHY_IPhysicsEnvironment* m_physenv;
};

#endif //PHYP_PHYSICSOBJECT_WRAPPER
