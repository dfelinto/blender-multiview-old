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

#include "KX_SG_NodeRelationships.h"


/**
 * Implementation of classes defined in KX_SG_NodeRelationships.h
 */

/** 
 * first of all KX_NormalParentRelation
 */

	KX_NormalParentRelation *
KX_NormalParentRelation::
New(
) {
	return new KX_NormalParentRelation();
}		

	void
KX_NormalParentRelation::
UpdateChildCoordinates(
	SG_Spatial * child,
	const SG_Spatial * parent
){
	assert(child != NULL);

	// This way of accessing child coordinates is a bit cumbersome
	// be nice to have non constant reference access to these values.

	const MT_Vector3 & child_scale = child->GetLocalScale();
	const MT_Point3 & child_pos = child->GetLocalPosition();
	const MT_Matrix3x3 & child_rotation = child->GetLocalOrientation();

	// the childs world locations which we will update.	
	
	MT_Vector3 child_w_scale;
	MT_Point3 child_w_pos;
	MT_Matrix3x3 child_w_rotation;
		
	if (parent) {

		const MT_Vector3 & p_world_scale = parent->GetWorldScaling();
		const MT_Point3 & p_world_pos = parent->GetWorldPosition();
		const MT_Matrix3x3 & p_world_rotation = parent->GetWorldOrientation();

		child_w_scale = p_world_scale * child_scale;
		child_w_rotation = p_world_rotation * child_rotation;

		child_w_pos = p_world_pos + p_world_scale * 
			(p_world_rotation * child_pos);
	} else {

		child_w_scale = child_scale;
		child_w_pos = child_pos;
		child_w_rotation = child_rotation;
	}

	child->SetWorldScale(child_w_scale);
	child->SetWorldPosition(child_w_pos);
	child->SetWorldOrientation(child_w_rotation);
}

	SG_ParentRelation *
KX_NormalParentRelation::
NewCopy(
){
	return new KX_NormalParentRelation();
}

KX_NormalParentRelation::
~KX_NormalParentRelation(
){
	//nothing to do
}


KX_NormalParentRelation::
KX_NormalParentRelation(
){
	// nothing to do
}

/** 
 * Next KX_VertexParentRelation
 */


	KX_VertexParentRelation *
KX_VertexParentRelation::
New(
){
	return new KX_VertexParentRelation();
}
		
/** 
 * Method inherited from KX_ParentRelation
 */

	void
KX_VertexParentRelation::
UpdateChildCoordinates(
	SG_Spatial * child,
	const SG_Spatial * parent
){

	assert(child != NULL);

	const MT_Vector3 & child_scale = child->GetLocalScale();
	const MT_Point3 & child_pos = child->GetLocalPosition();
	const MT_Matrix3x3 & child_rotation = child->GetLocalOrientation();

	// the childs world locations which we will update.	
	
	MT_Vector3 child_w_scale;
	MT_Point3 child_w_pos;
	MT_Matrix3x3 child_w_rotation;
		
	if (parent) {

		// This is a vertex parent so we do not inherit orientation 
		// information.

		const MT_Vector3 & p_world_scale = parent->GetWorldScaling();
		const MT_Point3 & p_world_pos = parent->GetWorldPosition();
		const MT_Matrix3x3 & p_world_rotation = parent->GetWorldOrientation();

		child_w_scale = child_scale;
		child_w_rotation = child_rotation;
		child_w_pos = p_world_pos + child_pos;

	} else {

		child_w_scale = child_scale;
		child_w_pos = child_pos;
		child_w_rotation = child_rotation;
	}

	child->SetWorldScale(child_w_scale);
	child->SetWorldPosition(child_w_pos);
	child->SetWorldOrientation(child_w_rotation);
}

/** 
 * Method inherited from KX_ParentRelation
 */

	SG_ParentRelation *
KX_VertexParentRelation::
NewCopy(
){
	return new KX_VertexParentRelation();
};

KX_VertexParentRelation::
~KX_VertexParentRelation(
){
	//nothing to do
}


KX_VertexParentRelation::
KX_VertexParentRelation(
){
	//nothing to do
}


/**
 * Slow parent relationship
 */

	KX_SlowParentRelation *
KX_SlowParentRelation::
New(
	MT_Scalar relaxation
){
	return new 	KX_SlowParentRelation(relaxation);
}	

/** 
 * Method inherited from KX_ParentRelation
 */

	void
KX_SlowParentRelation::
UpdateChildCoordinates(
	SG_Spatial * child,
	const SG_Spatial * parent
){
	assert(child != NULL);

	const MT_Vector3 & child_scale = child->GetLocalScale();
	const MT_Point3 & child_pos = child->GetLocalPosition();
	const MT_Matrix3x3 & child_rotation = child->GetLocalOrientation();

	// the childs world locations which we will update.	
	
	MT_Vector3 child_w_scale;
	MT_Point3 child_w_pos;
	MT_Matrix3x3 child_w_rotation;
		
	if (parent) {

		// This is a slow parent relation
		// first compute the normal child world coordinates.

		MT_Vector3 child_n_scale;
		MT_Point3 child_n_pos;
		MT_Matrix3x3 child_n_rotation;

		const MT_Vector3 & p_world_scale = parent->GetWorldScaling();
		const MT_Point3 & p_world_pos = parent->GetWorldPosition();
		const MT_Matrix3x3 & p_world_rotation = parent->GetWorldOrientation();

		child_n_scale = p_world_scale * child_scale;
		child_n_rotation = p_world_rotation * child_rotation;

		child_n_pos = p_world_pos + p_world_scale * 
			(p_world_rotation * child_pos);


		if (m_initialized) {

			// get the current world positions

			child_w_scale = child->GetWorldScaling();
			child_w_pos = child->GetWorldPosition();
			child_w_rotation = child->GetWorldOrientation();	

			// now 'interpolate' the normal coordinates with the last 
			// world coordinates to get the new world coordinates.

			// problem 1:
			// The child world scale needs to be initialized in some way for this 
			// to make sense
			// problem 2:
			// This is way of doing interpolation is nonsense

			int i;

			MT_Scalar weight = MT_Scalar(1)/(m_relax + 1);
			for (i=0;i <3 ;i++) {
				child_w_scale[i] = (m_relax * child_w_scale[i] + child_n_scale[i]) * weight;
				child_w_pos[i] = (m_relax * child_w_pos[i] + child_n_pos[i]) * weight;
				child_w_rotation[0][i] = (m_relax * child_w_rotation[0][i] + child_n_rotation[0][i]) * weight;
				child_w_rotation[1][i] = (m_relax * child_w_rotation[1][i] + child_n_rotation[1][i]) * weight;
				child_w_rotation[2][i] = (m_relax * child_w_rotation[2][i] + child_n_rotation[2][i]) * weight;
			}
		} else {
			child_w_scale = child_n_scale;
			child_w_pos = child_n_pos;
			child_w_rotation = child_n_rotation;
			m_initialized = true;
		}
			
	} else {

		child_w_scale = child_scale;
		child_w_pos = child_pos;
		child_w_rotation = child_rotation;
	}

	child->SetWorldScale(child_w_scale);
	child->SetWorldPosition(child_w_pos);
	child->SetWorldOrientation(child_w_rotation);
}

/** 
 * Method inherited from KX_ParentRelation
 */

	SG_ParentRelation *
KX_SlowParentRelation::
NewCopy(
){
	return new 	KX_SlowParentRelation(m_relax);
}

KX_SlowParentRelation::
KX_SlowParentRelation(
	MT_Scalar relaxation
):
	m_relax(relaxation),
	m_initialized(false)
{
	//nothing to do
}

KX_SlowParentRelation::
~KX_SlowParentRelation(
){
	//nothing to do
}




