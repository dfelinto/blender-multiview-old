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

#ifndef BSP_CSGISplitter_h

#define BSP_CSGISplitter_h

class BSP_MeshFragment;
class BSP_CSGMesh;

class MT_Plane3;

/**
 * This class defines a mesh splitter interface.
 * It embodies the action of splitting a mesh by a plane.
 * Depending on the context of the operation subclasses
 * may wish to define different implementations. For example
 * with building a BSP tree from a mesh, the mesh does not
 * need to be kept consistent, it doesn't matter if the edges
 * are maintained etc. However when pushing polygons through
 * a BSP tree (say for CSG operations)it is important to 
 * try and maintain mesh connectivity and thus a different 
 * splitter implementation may be needed.
 * 
 * This is an abstract interface class.
 */

class BSP_CSGISplitter
{

public:

	/**
	 * Split the incoming mesh fragment (frag)
	 * by the plane, put the output into (in,out and on)
	 * Subclasses should clear the contents of the 
	 * in_coming fragment.
	 */

	virtual 
		void
	Split(
		const MT_Plane3& plane,
		BSP_MeshFragment *frag,
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *out_frag,
		BSP_MeshFragment *on_frag,
		BSP_MeshFragment *spanning_frag
	)= 0;

	/**
	 * Split the entire mesh with respect to the plane.
	 * and place ouput into (in,out and on).
	 * Subclasses should clear the contents of the 
	 * in_coming fragment.
	 */
	virtual 
		void
	Split(
		BSP_CSGMesh & mesh,
		const MT_Plane3& plane,
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *out_frag,
		BSP_MeshFragment *on_frag,
		BSP_MeshFragment *spanning_frag
	)=0;
		
		virtual
	~BSP_CSGISplitter(
	){
	};

protected :

	BSP_CSGISplitter(
	) {
		//nothing to do
	}
	
	BSP_CSGISplitter(
		const BSP_CSGISplitter &
	) {
		//nothing to do
	}
};



#endif

