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

#include "KX_LightIpoSGController.h"

#include "KX_ScalarInterpolator.h"
#include "KX_Light.h"

#include "RAS_LightObject.h"


bool KX_LightIpoSGController::Update(double currentTime)
{
	if (m_modified)
	{
		T_InterpolatorList::iterator i;
		for (i = m_interpolators.begin(); !(i == m_interpolators.end()); ++i) {
			(*i)->Execute(m_ipotime);//currentTime);
		}
		
		RAS_LightObject *lightobj;

		SG_Spatial* ob = (SG_Spatial*)m_pObject;
		KX_LightObject* kxlight = (KX_LightObject*) ob->GetSGClientObject();
		lightobj = kxlight->GetLightData();
		//lightobj = (KX_Light*) 

		if (m_modify_energy) {
			lightobj->m_energy = m_energy;
		}

		if (m_modify_color) {
			lightobj->m_red   = m_col_rgb[0];
			lightobj->m_green = m_col_rgb[1];
			lightobj->m_blue  = m_col_rgb[2];
		}

		if (m_modify_dist) {
			lightobj->m_distance = m_dist;
		}

		m_modified=false;
	}
	return false;
}


void KX_LightIpoSGController::AddInterpolator(KX_IInterpolator* interp)
{
	this->m_interpolators.push_back(interp);
}

SG_Controller*	KX_LightIpoSGController::GetReplica(class SG_Node* destnode)
{
	KX_LightIpoSGController* iporeplica = new KX_LightIpoSGController(*this);
	// clear object that ipo acts on
	iporeplica->ClearObject();

	// dirty hack, ask Gino for a better solution in the ipo implementation
	// hacken en zagen, in what we call datahiding, not written for replication :(

	T_InterpolatorList oldlist = m_interpolators;
	iporeplica->m_interpolators.clear();

	T_InterpolatorList::iterator i;
	for (i = oldlist.begin(); !(i == oldlist.end()); ++i) {
		KX_ScalarInterpolator* copyipo = new KX_ScalarInterpolator(*((KX_ScalarInterpolator*)*i));
		iporeplica->AddInterpolator(copyipo);

		MT_Scalar* scaal = ((KX_ScalarInterpolator*)*i)->GetTarget();
		int orgbase = (int)this;
		int orgloc = (int)scaal;
		int offset = orgloc-orgbase;
		int newaddrbase = (int)iporeplica + offset;
		MT_Scalar* blaptr = (MT_Scalar*) newaddrbase;
		copyipo->SetNewTarget((MT_Scalar*)blaptr);
	}
	
	return iporeplica;
}

KX_LightIpoSGController::~KX_LightIpoSGController()
{

	T_InterpolatorList::iterator i;
	for (i = m_interpolators.begin(); !(i == m_interpolators.end()); ++i) {
		delete (*i);
	}
	
}
