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

#ifndef __SG_SPATIAL_H
#define __SG_SPATIAL_H

#include <MT_Vector3.h>
#include <MT_Point3.h>
#include <MT_Matrix3x3.h> // or Quaternion later ?

#include "SG_IObject.h"

class SG_Node;
class SG_ParentRelation;

class SG_Spatial : public SG_IObject
{

protected:
	MT_Point3		m_localPosition;
	MT_Matrix3x3	m_localRotation;
	MT_Vector3		m_localScaling;

	MT_Point3		m_worldPosition;
	MT_Matrix3x3	m_worldRotation;
	MT_Vector3		m_worldScaling;
	
	SG_ParentRelation * m_parent_relation;

public:

	/** 
	 * Define the realtionship this node has with it's parent
	 * node. You should pass an unshared instance of an SG_ParentRelation
	 * allocated on the heap to this method. Ownership of this
	 * instance is assumed by this class. 
	 * You may call this function several times in the lifetime 
	 * of a node to change the relationship dynamically. 
	 * You must call this method before the first call to UpdateSpatialData().
	 * An assertion willl be fired at run-time in debug if this is not 
	 * the case.
	 * The relation is activated only if no controllers of this object
	 * updated the coordinates of the child.
	 */

		void
	SetParentRelation(
		SG_ParentRelation *relation
	);


	/**
	 * Apply a translation relative to the current position.
	 * if local then the translation is assumed to be in the 
	 * local coordinates of this object. If not then the translation
	 * is assumed to be in global coordinates. In this case 
	 * you must provide a pointer to the parent of this object if it 
	 * exists otherwise if there is no parent set it to NULL
	 */ 

		void
	RelativeTranslate(
		const MT_Vector3& trans,
		const SG_Spatial *parent,
		bool local
	);

		void				
	SetLocalPosition(
		const MT_Point3& trans
	);

		void				
	SetWorldPosition(
		const MT_Point3& trans
	);
	
		void				
	RelativeRotate(
		const MT_Matrix3x3& rot,
		bool local
	);

		void				
	SetLocalOrientation(
		const MT_Matrix3x3& rot
	);

		void				
	SetWorldOrientation(
		const MT_Matrix3x3& rot
	);

		void				
	RelativeScale(
		const MT_Vector3& scale
	);

		void				
	SetLocalScale(
		const MT_Vector3& scale
	);

		void				
	SetWorldScale(
		const MT_Vector3& scale
	);

	const 
		MT_Point3&
	GetLocalPosition(
	) const	;

	const 
		MT_Matrix3x3&
	GetLocalOrientation(
	) const	;

	const 
		MT_Vector3&	
	GetLocalScale(
	) const;

	const 
		MT_Point3&
	GetWorldPosition(
	) const	;

	const 
		MT_Matrix3x3&	
	GetWorldOrientation(
	) const	;

	const 
		MT_Vector3&	
	GetWorldScaling(
	) const	;


	void	ComputeWorldTransforms(		const SG_Spatial *parent);

	
protected:
	friend SG_Controller;
	
	/** 
	 * Protected constructor this class is not
	 * designed for direct instantiation
	 */

	SG_Spatial(
		void* clientobj,
		void* clientinfo,
		SG_Callbacks callbacks
	);

	SG_Spatial(
		const SG_Spatial& other
	);


	virtual ~SG_Spatial();

	/** 
	 * Update the world coordinates of this spatial node. This also informs
	 * any controllers to update this object. 
	 */ 

		void 
	UpdateSpatialData(
		const SG_Spatial *parent,
		double time
	);

};
#endif //__SG_SPATIAL_H
