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
// this will be put in a class later on

#ifndef __RAS_BUCKETMANAGER
#define __RAS_BUCKETMANAGER

#include "MT_Transform.h"
#include "RAS_MaterialBucket.h"
#include "GEN_Map.h"

class RAS_BucketManager
{

	GEN_Map<class RAS_IPolyMaterial,class RAS_MaterialBucket*> m_MaterialBuckets;

public:
	RAS_BucketManager();
	virtual ~RAS_BucketManager();

	void Renderbuckets(const MT_Transform & cameratrans,
							RAS_IRasterizer* rasty,
							class RAS_IRenderTools* rendertools);

	RAS_MaterialBucket* RAS_BucketManagerFindBucket(RAS_IPolyMaterial * material);
	

private:
	void RAS_BucketManagerClearAll();

};

#endif //__RAS_BUCKETMANAGER

