/**
 * Cast a ray and feel for objects
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

#ifndef __KX_RAYSENSOR_H
#define __KX_RAYSENSOR_H

#include "SCA_ISensor.h"
#include "MT_Point3.h"

class KX_RaySensor : public SCA_ISensor
{
	Py_Header;
	STR_String		m_propertyname;
	bool			m_bFindMaterial;
	double			m_distance;
	class KX_Scene* m_ketsjiScene;
	bool			m_bTriggered;
	int				m_axis;
	bool			m_rayHit;
	MT_Point3		m_hitPosition;
	SCA_IObject*	m_hitObject;
	MT_Vector3		m_hitNormal;
	MT_Vector3		m_rayDirection;

public:
	KX_RaySensor(class SCA_EventManager* eventmgr,
					SCA_IObject* gameobj,
					const STR_String& propname,
					bool fFindMaterial,
					double distance,
					int axis,
					class KX_Scene* ketsjiScene,
					PyTypeObject* T = &Type);
	virtual ~KX_RaySensor();
	virtual CValue* GetReplica();

	virtual bool Evaluate(CValue* event);
	virtual bool IsPositiveTrigger();

	KX_PYMETHOD_DOC(KX_RaySensor,GetHitObject);
	KX_PYMETHOD_DOC(KX_RaySensor,GetHitPosition);
	KX_PYMETHOD_DOC(KX_RaySensor,GetHitNormal);
	KX_PYMETHOD_DOC(KX_RaySensor,GetRayDirection);

	virtual PyObject*  _getattr(char *attr);
	
};
#endif //__KX_RAYSENSOR_H
