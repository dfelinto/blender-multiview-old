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

#ifndef BSP_FragNode_h

#define BSP_FragNode_h

#include "BSP_FragTree.h"
#include "BSP_MeshFragment.h"
#include "MT_Plane3.h"

class BSP_CSGISplitter;

class BSP_FragNode : public MEM_NonCopyable
{
private:

	/**
	 * The plane defining this node.
	 */

	MT_Plane3 m_plane;

	/**
	 * Children of this node.
	 */

	BSP_FragTree m_in_tree;
	BSP_FragTree m_out_tree;

private :

	BSP_FragNode(
		const MT_Plane3 & plane,
		BSP_CSGMesh *mesh
	);
	
public :

	/**
	 * Public methods
	 * Should only be called by BSP_FragTree
	 */

	~BSP_FragNode(
	);

	static
		MEM_SmartPtr<BSP_FragNode>
	New(
		const MT_Plane3 & plane,
		BSP_CSGMesh *mesh
	);

		void
	Build(
		BSP_MeshFragment *frag,
		BSP_CSGISplitter & splitter
	);

		void
	Push(
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *output,
		const BSP_Classification keep,
		const bool dominant,
		BSP_CSGISplitter & splitter
	);		

		void
	Classify(
		BSP_MeshFragment * frag,
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *out_frag,
		BSP_MeshFragment *on_frag,
		BSP_CSGISplitter & splitter
	);

	/**
	 * Accessor methods
	 */

		BSP_FragTree &
	InTree(
	);

		BSP_FragTree &
	OutTree(
	);
		
		MT_Plane3&
	Plane(
	);

};



#endif