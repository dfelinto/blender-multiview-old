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

#include "KX_NearSensor.h"
#include "SCA_LogicManager.h"
#include "KX_GameObject.h"
#include "KX_TouchEventManager.h"
#include "KX_Scene.h" // needed to create a replica


#ifdef PHYSICS_NOT_YET

KX_NearSensor::KX_NearSensor(SCA_EventManager* eventmgr,
							 KX_GameObject* gameobj,
							 double margin,
							 double resetmargin,
							 bool bFindMaterial,
							 const STR_String& touchedpropname,
							 class KX_Scene* scene,
							 PyTypeObject* T)
			 :KX_TouchSensor(eventmgr,
							 gameobj,
							 bFindMaterial,
							 touchedpropname,
							 scene,
							 T),
			 m_Margin(margin),
			 m_ResetMargin(resetmargin),
			 m_sumoScene(sumoscene)

{
	m_client_info.m_type = 4;
	m_client_info.m_clientobject = gameobj;
	m_client_info.m_auxilary_info = NULL;
	sumoObj->setClientObject(&m_client_info);
}



CValue* KX_NearSensor::GetReplica()
{
	KX_NearSensor* replica = new KX_NearSensor(*this);
	replica->m_colliders = new CListValue();
	replica->m_bCollision = false;
	replica->m_bTriggered= false;
	replica->m_hitObject = NULL;
	replica->m_bLastTriggered = false;
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);
	
	return replica;
}



void KX_NearSensor::ReParent(SCA_IObject* parent)
{
	DT_ShapeHandle shape = DT_Sphere(0.0);
				
	// this sumoObject is not deleted by a gameobj, so delete it ourself
	// later (memleaks)!

	SM_Object* sumoObj = new SM_Object(shape,NULL,NULL,NULL);
	sumoObj->setMargin(m_Margin);

	//sumoObj->setPosition(gameobj->NodeGetWorldPosition());
	//sumoobj->setPosition(m_sumoObj->getPosition());
	//sumoobj->setOrientation(m_sumoObj->getOrientation());
	//newobj->setRigidBody(this->m_sumoObj->isRigidBody());

	m_sumoObj = sumoObj;
	m_solidHandle = m_sumoObj->getObjectHandle();

	double radius = m_sumoObj->getMargin();
	sumoObj->setMargin(m_sumoObj->getMargin());
	
	m_client_info.m_type = 4;
	m_client_info.m_clientobject = parent;
	m_client_info.m_auxilary_info = NULL;
	sumoObj->setClientObject(&m_client_info);

	//m_sumoScene->add(*newobj);
	
	if (m_sumoObj)
	{
		DT_SetObjectResponse(m_resptable,
			m_sumoObj->getObjectHandle(),
			collisionResponse,
			DT_SIMPLE_RESPONSE,
			this);
	}

	SCA_ISensor::ReParent(parent);
}



KX_NearSensor::~KX_NearSensor()
{
	// for nearsensor, the sensor is the 'owner' of sumoobj
	// for touchsensor, it's the parent

	m_sumoScene->remove(*m_sumoObj);

	if (m_sumoObj)
		delete m_sumoObj;
}



bool KX_NearSensor::Evaluate(CValue* event)
{
	bool result = false;
	KX_GameObject* parent = (KX_GameObject*)GetParent();

	if (m_bTriggered != m_bLastTriggered)
	{
		m_bLastTriggered = m_bTriggered;
		if (m_bTriggered)
		{
			if (m_sumoObj)
			{
				m_sumoObj->setMargin(m_ResetMargin);
			}
		} else
		{
			if (m_sumoObj)
			{
				m_sumoObj->setMargin(m_Margin);
			}

		}
		result = true;
	}

	return result;
}



void KX_NearSensor::HandleCollision(void* obj1,void* obj2,const DT_CollData * coll_data)
{
	KX_TouchEventManager* toucheventmgr = (KX_TouchEventManager*)m_eventmgr;
	KX_GameObject* parent = (KX_GameObject*)GetParent();

	// need the mapping from SM_Objects to gameobjects now
	
	SM_ClientObjectInfo* client_info =(SM_ClientObjectInfo*) (obj1 == m_sumoObj? 
					((SM_Object*)obj2)->getClientObject() : 
					((SM_Object*)obj1)->getClientObject());

	KX_GameObject* gameobj = ( client_info ? 
			(KX_GameObject*)client_info->m_clientobject : 
			NULL);

	if (gameobj && (gameobj != parent))
	{
		if (!m_colliders->SearchValue(gameobj))
			m_colliders->Add(gameobj->AddRef());
		
		// only take valid colliders
		if (client_info->m_type == 1)
		{
			if ((m_touchedpropname.Length() == 0) || 
				(gameobj->GetProperty(m_touchedpropname)))
			{
				m_bTriggered = true;
				m_hitObject = gameobj;
			}
		}
	} else
	{
		
	}
}



// python embedding
PyTypeObject KX_NearSensor::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"KX_NearSensor",
	sizeof(KX_NearSensor),
	0,
	PyDestructor,
	0,
	__getattr,
	__setattr,
	0, //&MyPyCompare,
	__repr,
	0, //&cvalue_as_number,
	0,
	0,
	0,
	0
};



PyParentObject KX_NearSensor::Parents[] = {
	&KX_NearSensor::Type,
	&KX_TouchSensor::Type,
	&SCA_ISensor::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};



PyMethodDef KX_NearSensor::Methods[] = {
	{"setProperty", 
	 (PyCFunction) KX_NearSensor::sPySetProperty,      METH_VARARGS, SetProperty_doc},
	{"getProperty", 
	 (PyCFunction) KX_NearSensor::sPyGetProperty,      METH_VARARGS, GetProperty_doc},
	{"getHitObject", 
	 (PyCFunction) KX_NearSensor::sPyGetHitObject,     METH_VARARGS, GetHitObject_doc},
	{"getHitObjectList", 
	 (PyCFunction) KX_NearSensor::sPyGetHitObjectList, METH_VARARGS, GetHitObjectList_doc},
	{NULL,NULL} //Sentinel
};


PyObject*
KX_NearSensor::_getattr(char* attr)
{
  _getattr_up(KX_TouchSensor);
}

#endif //PHYSICS_NOT_YET
