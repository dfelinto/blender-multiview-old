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

#ifndef BL_MESHDEFORMER
#define BL_MESHDEFORMER

#include "RAS_Deformer.h"
#include "DNA_object_types.h"
#include "MT_Point3.h"
#include <stdlib.h>

#ifdef WIN32
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32

class BL_MeshDeformer : public RAS_Deformer
{
public:
	void VerifyStorage();
	void RecalcNormals();
	virtual void Relink(GEN_Map<class GEN_HashedPtr, void*>*map){};
	BL_MeshDeformer(struct Object* obj, class BL_SkinMeshObject *meshobj):
		m_transverts(NULL),
		m_tvtot(0),
		m_transnors(NULL),
		m_pMeshObject(meshobj),
		m_bmesh((struct Mesh*)(obj->data)){};
	virtual ~BL_MeshDeformer();
	virtual void SetSimulatedTime(double time){};
	virtual bool Apply(class RAS_IPolyMaterial *mat);
	virtual void Update(void){};
	virtual	RAS_Deformer*	GetReplica(){return NULL;};
	//	virtual void InitDeform(double time){};
protected:
	class BL_SkinMeshObject	*m_pMeshObject;
	struct Mesh *m_bmesh;
	MT_Point3 *m_transnors;
	MT_Point3				*m_transverts;
	int						m_tvtot;

};

#endif
