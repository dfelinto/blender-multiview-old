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

#ifndef BL_DEFORMABLEGAMEOBJECT
#define BL_DEFORMABLEGAMEOBJECT

#ifdef WIN32
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32

#include "KX_GameObject.h"
#include "RAS_Deformer.h"

class BL_DeformableGameObject : public KX_GameObject  
{
public:

	RAS_Deformer		*m_pDeformer;	
	CValue*		GetReplica();
	virtual void Relink(GEN_Map<GEN_HashedPtr, void*>*map)
	{
		if (m_pDeformer)
			m_pDeformer->Relink (map);
	};
	void ProcessReplica(KX_GameObject* replica);

	BL_DeformableGameObject(void* sgReplicationInfo, SG_Callbacks callbacks) :
		KX_GameObject(sgReplicationInfo,callbacks),
		m_pDeformer(NULL)
{
	  };
	virtual ~BL_DeformableGameObject();

};

#endif

