/**
 * @mainpage SG_ParentRelation   

 * @section 
 * 
 * This is an abstract interface class to the Scene Graph library. 
 * It allows you to specify how child nodes react to parent nodes.
 * Normally a child will use it's parent's transforms to compute
 * it's own global transforms. How this is performed depends on
 * the type of relation. For example if the parent is a vertex 
 * parent to this child then the child should not inherit any 
 * rotation information from the parent. Or if the parent is a
 * 'slow parent' to this child then the child should react 
 * slowly to changes in the parent's position. The exact relation
 * is left for you to implement by filling out this interface 
 * with concrete examples. 
 * 
 * There is exactly one SG_ParentRelation per SG_Node. Subclasses
 * should not be value types and should be allocated on the heap.
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

#ifndef __SG_ParentRelation_h
#define __SG_ParentRelation_h

class SG_Spatial;

class SG_ParentRelation {

public :

	/**
	 * Update the childs local and global coordinates
	 * based upon the parents global coordinates. 
	 * You must also handle the case when this node has no
	 * parent (parent == NULL). Usually you should just 
	 * copy the local coordinates of the child to the 
	 * world coordinates.
	 */ 
	
	virtual
		void
	UpdateChildCoordinates(
		SG_Spatial * child,
		const SG_Spatial * parent
	) = 0;

	virtual 
	~SG_ParentRelation(
	){
	};

	/** 
	 * You must provide a way of duplicating an
	 * instance of an SG_ParentRelation. This should
	 * return a pointer to a new duplicate allocated
	 * on the heap. Responsibilty for deleting the 
	 * duplicate resides with the caller of this method.
	 */

	virtual 
		SG_ParentRelation *
	NewCopy(
	) = 0;

protected :

	/** 
	 * Protected constructors 
	 * this class is not meant to be instantiated.
	 */

	SG_ParentRelation(
	) {	
	};

	/**
	 * Copy construction should not be implemented
	 */

	SG_ParentRelation(
		const SG_ParentRelation &
	); 
};	

#endif
