/**
 * @mainpage KX_SG_NodeRelationships   

 * @section 
 *
 * This file provides common concrete implementations of 
 * SG_ParentRelation used by the game engine. These are
 * KX_SlowParentRelation a slow parent relationship.
 * KX_NormalParentRelation a normal parent relationship where 
 * orientation and position are inherited from the parent by
 * the child.
 * KX_VertexParentRelation only location information is 
 * inherited by the child. 
 *
 * @see SG_ParentRelation for more information about this 
 * interface	  
 *
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
 * 
 */

#include "SG_Spatial.h"
#include "SG_ParentRelation.h"


class KX_NormalParentRelation : public SG_ParentRelation
{

public :

	/**
	 * Allocate and construct a new KX_NormalParentRelation
	 * on the heap.
	 */

	static 
		KX_NormalParentRelation *
	New(
	);		

	/** 
	 * Method inherited from KX_ParentRelation
	 */

		void
	UpdateChildCoordinates(
		SG_Spatial * child,
		const SG_Spatial * parent
	);

	/** 
	 * Method inherited from KX_ParentRelation
	 */
	
		SG_ParentRelation *
	NewCopy(
	);

	~KX_NormalParentRelation(
	);

private :

	KX_NormalParentRelation(
	);

};


class KX_VertexParentRelation : public SG_ParentRelation
{

public :

	/**
	 * Allocate and construct a new KX_VertexParentRelation
	 * on the heap.
	 */

	static 
		KX_VertexParentRelation *
	New(
	);		

	/** 
	 * Method inherited from KX_ParentRelation
	 */

		void
	UpdateChildCoordinates(
		SG_Spatial * child,
		const SG_Spatial * parent
	);

	/** 
	 * Method inherited from KX_ParentRelation
	 */
	
		SG_ParentRelation *
	NewCopy(
	);

	~KX_VertexParentRelation(
	);

private :

	KX_VertexParentRelation(
	);

};


class KX_SlowParentRelation : public SG_ParentRelation
{

public :

	/**
	 * Allocate and construct a new KX_VertexParentRelation
	 * on the heap.
	 */

	static 
		KX_SlowParentRelation *
	New(
		MT_Scalar relaxation
	);		

	/** 
	 * Method inherited from KX_ParentRelation
	 */

		void
	UpdateChildCoordinates(
		SG_Spatial * child,
		const SG_Spatial * parent
	);

	/** 
	 * Method inherited from KX_ParentRelation
	 */
	
		SG_ParentRelation *
	NewCopy(
	);

	~KX_SlowParentRelation(
	);

private :

	KX_SlowParentRelation(
		MT_Scalar relaxation
	);

	// the relaxation coefficient.

	MT_Scalar m_relax;

	/**
	 * Looks like a hack flag to me.
	 * We need to compute valid world coordinates the first
	 * time we update spatial data of the child. This is done
	 * by just doing a normal parent relation the first time
	 * UpdateChildCoordinates is called and then doing the
	 * slow parent relation 
	 */

	bool m_initialized;

};














