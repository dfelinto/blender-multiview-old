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

#include "BL_DeformableGameObject.h"

BL_DeformableGameObject::~BL_DeformableGameObject()
{
	if (m_pDeformer)
		delete m_pDeformer;		//	__NLA : Temporary until we decide where to put this
}

void	BL_DeformableGameObject::ProcessReplica(KX_GameObject* replica)
{
	KX_GameObject::ProcessReplica(replica);

	if (m_pDeformer){
		((BL_DeformableGameObject*)replica)->m_pDeformer = m_pDeformer->GetReplica();
	}

}

CValue*		BL_DeformableGameObject::GetReplica()
{

	BL_DeformableGameObject* replica = new BL_DeformableGameObject(*this);//m_float,GetName());
	
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);

	ProcessReplica(replica);
	return replica;

}
