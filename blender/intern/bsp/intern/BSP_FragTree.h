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

#ifndef BSP_FragTree_h

#define BSP_FragTree_h

class BSP_FragNode;

#include "MEM_SmartPtr.h"
#include "MEM_NonCopyable.h"
#include "BSP_MeshPrimitives.h"

class BSP_CSGMesh;
class BSP_MeshFragment;
class BSP_CSGISplitter;

class BSP_FragTree : public MEM_NonCopyable
{
public :

	/**
	 * Create a new BSP_FragTree allocated
	 * on the heap for mesh. Note mesh will 
	 * be divided up by this operation. If you
	 * want to retain the original mesh make a copy 
	 * of it first.
	 */
	
	static
		MEM_SmartPtr<BSP_FragTree>
	New(
		BSP_CSGMesh *mesh,
		BSP_CSGISplitter & splitter
	);
	

	/**
	 * Push a mesh fragment down the tree,
	 * splitting the mesh as it goes. 
	 * upon reaching leaves it puts polygons from fragments
	 * of type keep into the output fragment.
	 */

		void
	Push(
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *output,
		const BSP_Classification keep,
		const BSP_Classification current,
		const bool dominant,
		BSP_CSGISplitter & splitter
	); 

		void
	Classify(
		BSP_CSGMesh *mesh,
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *out_frag,
		BSP_MeshFragment *on_frag,
		BSP_CSGISplitter & splitter
	);


	~BSP_FragTree(
	);

private :

	friend class BSP_FragNode;

	BSP_FragTree(
	);

	BSP_FragTree(
		BSP_CSGMesh *mesh
	);

		void
	Build(
		BSP_MeshFragment * frag,
		BSP_CSGISplitter & splitter
	);


		void
	Classify(
		BSP_MeshFragment * frag,
		BSP_MeshFragment *in_frag,
		BSP_MeshFragment *out_frag,
		BSP_MeshFragment *on_frag,
		const BSP_Classification current,
		BSP_CSGISplitter & splitter
	);

private :

	/**
	 * pointer to the mesh for this tree.
	 * Tree is only valid whilst mesh is around.
	 */

	BSP_CSGMesh *m_mesh;
	
	/**
	 * The node owned by this tree.
	 */

	MEM_SmartPtr<BSP_FragNode> m_node;
	
};


#endif




