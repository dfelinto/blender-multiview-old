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

#include "KX_WorldIpoController.h"

#include "KX_ScalarInterpolator.h"
#include "KX_WorldInfo.h"

bool KX_WorldIpoController::Update(double currentTime)
{
	if (m_modified)
	{
		T_InterpolatorList::iterator i;
		for (i = m_interpolators.begin(); !(i == m_interpolators.end()); ++i) {
			(*i)->Execute(m_ipotime);//currentTime);
		}
		
		KX_WorldInfo *world;

		if (m_modify_mist_start) {
			world->setMistStart(m_mist_start);
		}

		if (m_modify_mist_color) {
			world->setMistColorRed(m_mist_rgb[0]);
			world->setMistColorGreen(m_mist_rgb[1]);
			world->setMistColorBlue(m_mist_rgb[2]);
		}

		if (m_modify_mist_dist) {
			world->setMistDistance(m_mist_dist);
		}

		m_modified=false;
	}
	return false;
}


void KX_WorldIpoController::AddInterpolator(KX_IInterpolator* interp)
{
	this->m_interpolators.push_back(interp);
}


SG_Controller*	KX_WorldIpoController::GetReplica(class SG_Node* destnode)
{
	KX_WorldIpoController* iporeplica = new KX_WorldIpoController(*this);
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

KX_WorldIpoController::~KX_WorldIpoController()
{

	T_InterpolatorList::iterator i;
	for (i = m_interpolators.begin(); !(i == m_interpolators.end()); ++i) {
		delete (*i);
	}
	
}
