/**
 * Sense if other objects are near
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

#ifndef KX_NEARSENSOR_H
#define KX_NEARSENSOR_H

#include "KX_TouchSensor.h"
class KX_Scene;

class KX_NearSensor : public KX_TouchSensor
{
	Py_Header;
	double	m_Margin;
	double  m_ResetMargin;
	KX_Scene*	m_scene;

public:
	KX_NearSensor(class SCA_EventManager* eventmgr,class KX_GameObject* gameobj,double margin,double resetmargin,bool bFindMaterial,const STR_String& touchedpropname,class KM_Scene* scene,PyTypeObject* T=&Type);
	virtual ~KX_NearSensor(); 
	virtual CValue* GetReplica();
	virtual bool Evaluate(CValue* event);

	virtual void ReParent(SCA_IObject* parent);
	//virtual void HandleCollision(void* obj1,void* obj2,
	//					 const DT_CollData * coll_data); 

	virtual PyObject*  _getattr(char *attr);

};
#endif //KX_NEARSENSOR_H
