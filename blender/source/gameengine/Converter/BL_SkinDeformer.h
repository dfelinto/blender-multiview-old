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

#ifndef BL_SKINDEFORMER
#define BL_SKINDEFORMER

#ifdef WIN32
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32

#include "BL_MeshDeformer.h"
#include "BL_ArmatureObject.h"

#include "DNA_mesh_types.h"
#include "DNA_object_types.h"
#include "BKE_armature.h"

#include "RAS_Deformer.h"


class BL_SkinDeformer : public BL_MeshDeformer  
{
public:
//	void SetArmatureController (BL_ArmatureController *cont);
	virtual void Relink(GEN_Map<class GEN_HashedPtr, void*>*map)
	{
		void **h_obj = (*map)[m_armobj];
		if (h_obj){
			SetArmature( (BL_ArmatureObject*)(*h_obj) );
		}
		else
			m_armobj=NULL;
	}
	void SetArmature (class BL_ArmatureObject *armobj);
	BL_SkinDeformer(	struct Object *bmeshobj,
						class BL_SkinMeshObject *mesh)
						:BL_MeshDeformer(bmeshobj, mesh),
						m_armobj(NULL),
						m_defbase(&bmeshobj->defbase),
						m_lastUpdate(-1)
	{
		/* Build all precalculatable matrices for bones */

		GB_build_mats(bmeshobj->parent->obmat, bmeshobj->obmat, m_premat, m_postmat);
		GB_validate_defgroups((Mesh*)bmeshobj->data, m_defbase);
		// Validate bone data in bDeformGroups
/*
		for (bDeformGroup *dg=(bDeformGroup*)m_defbase->first; dg; dg=(bDeformGroup*)dg->next)
			dg->data = (void*)get_named_bone(barm, dg->name);
*/
	};	

	virtual void ProcessReplica();
	virtual RAS_Deformer *GetReplica();
	virtual ~BL_SkinDeformer();
	void Update (void);
	bool Apply (class RAS_IPolyMaterial *polymat);

protected:
	BL_ArmatureObject		*m_armobj;			//	Our parent object
	float					m_premat[4][4];
	float					m_postmat[4][4];
	float					m_time;
	double					m_lastUpdate;
	ListBase				*m_defbase;
};

#endif

