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


#ifdef WIN32
// don't show these anoying STL warnings
#pragma warning (disable:4786)
#endif

#include "GEN_Map.h"
#include "RAS_MaterialBucket.h"
#include "STR_HashedString.h"
#include "RAS_MeshObject.h"
#define KX_NUM_MATERIALBUCKETS 100
#include "RAS_IRasterizer.h"
#include "RAS_IRenderTools.h"

#include "RAS_BucketManager.h"


RAS_BucketManager::RAS_BucketManager()
{

}

RAS_BucketManager::~RAS_BucketManager()
{
		RAS_BucketManagerClearAll();
}


void RAS_BucketManager::Renderbuckets(
	const MT_Transform& cameratrans, RAS_IRasterizer* rasty, RAS_IRenderTools* rendertools)
{
	int numbuckets = m_MaterialBuckets.size();
		
	//default_gl_light();

	int i;
	
	rasty->EnableTextures(false);
	rasty->SetDepthMask(RAS_IRasterizer::KX_DEPTHMASK_ENABLED);
	
	// beginning each frame, clear (texture/material) caching information
	rasty->ClearCachingInfo();

	RAS_MaterialBucket::StartFrame();

	for (i=0;i<numbuckets;i++)
	{
		RAS_MaterialBucket** bucketptr = m_MaterialBuckets.at(i);
		if (bucketptr)
		{
			(*bucketptr)->ClearScheduledPolygons();
		}
	}

	vector<RAS_MaterialBucket*> alphabuckets;

	// if no visibility method is define, everything is drawn
	
	for (i=0;i<numbuckets;i++)
	{
		RAS_MaterialBucket** bucketptr = m_MaterialBuckets.at(i);
		if (bucketptr)
		{
			if (!(*bucketptr)->IsTransparant())
			{
				(*bucketptr)->Render(cameratrans,rasty,rendertools);
			} else
			{
				alphabuckets.push_back(*bucketptr);
			}
		}
	}
	
	rasty->SetDepthMask(RAS_IRasterizer::KX_DEPTHMASK_DISABLED);
	
	int numalphabuckets = alphabuckets.size();
	for (vector<RAS_MaterialBucket*>::const_iterator it=alphabuckets.begin();
	!(it==alphabuckets.end());it++)
	{
		(*it)->Render(cameratrans,rasty,rendertools);
	}

	alphabuckets.clear();	


	RAS_MaterialBucket::EndFrame();

	rasty->SetDepthMask(RAS_IRasterizer::KX_DEPTHMASK_ENABLED);
}

RAS_MaterialBucket* RAS_BucketManager::RAS_BucketManagerFindBucket(RAS_IPolyMaterial * material)
{

	RAS_MaterialBucket** bucketptr = 	m_MaterialBuckets[*material];
	RAS_MaterialBucket* bucket=NULL;
	if (!bucketptr)
	{
		bucket = new RAS_MaterialBucket(material);
		m_MaterialBuckets.insert(*material,bucket);

	} else
	{
		bucket = *bucketptr;
	}

	return bucket;
}

void RAS_BucketManager::RAS_BucketManagerClearAll()
{

	int numbuckets = m_MaterialBuckets.size();
	for (int i=0;i<numbuckets;i++)
	{
		RAS_MaterialBucket** bucketptr = m_MaterialBuckets.at(i);
		if (bucketptr)
		{
			delete (*bucketptr);
			*bucketptr=NULL;

		}
	}
	m_MaterialBuckets.clear();
	
}
