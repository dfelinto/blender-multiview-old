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

#ifndef RAS_DEFORMER
#define RAS_DEFORMER

#ifdef WIN32
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32

#include "GEN_Map.h"

class RAS_Deformer
{
public:
	RAS_Deformer(){};
	virtual ~RAS_Deformer(){};
	virtual void Relink(GEN_Map<class GEN_HashedPtr, void*>*map)=0;
	virtual bool Apply(class RAS_IPolyMaterial *polymat)=0;
	virtual void Update(void)=0;
	virtual RAS_Deformer *GetReplica()=0;
protected:
	class RAS_MeshObject	*m_pMesh;
};

#endif

