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

#ifndef NAN_INCLUDED_ExternVColorEditor_h

#define NAN_INCLUDED_ExternVColorEditor_h

#include "common/NonCopyable.h"
#include "LOD_ManMesh2.h"
#include "MT_Vector3.h"
#include "../extern/LOD_decimation.h"

class LOD_ExternVColorEditor : public NonCopyable
{

public : 

	// Creation
	///////////

	static
		LOD_ExternVColorEditor *
	New(
		LOD_Decimation_InfoPtr
	); 	

	// vertex colors
	/////////////////

		void
	RemoveVertexColors(
		const std::vector<LOD_VertexInd> &sorted_verts
	);

	// Return the color for vertex v

		MT_Vector3
	IndexColor(
		const LOD_VertexInd &v
	) const ;

	// Set the color for vertex v

		void
	SetColor(
		MT_Vector3 c,
		const LOD_VertexInd &v
	);
		

private :

	LOD_Decimation_InfoPtr m_extern_info;

private :
	
		
	LOD_ExternVColorEditor(
		LOD_Decimation_InfoPtr extern_info
	);

};

#endif
