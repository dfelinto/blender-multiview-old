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

#include "BSP_MeshDrawer.h"

#include "BSP_TMesh.h"

#if defined(WIN32) || defined(__APPLE__)
#	ifdef WIN32
#		include <windows.h>
#		include <GL/gl.h>
#		include <GL/glu.h>
#	else // WIN32
#		include <AGL/gl.h>
#	endif // WIN32
#else // defined(WIN32) || defined(__APPLE__)
#	include <GL/gl.h>
#	include <GL/glu.h>
#endif // defined(WIN32) || defined(__APPLE__)

#include <vector>

using namespace std;

	void
BSP_MeshDrawer::
DrawMesh(
	BSP_TMesh &mesh,
	int render_mode
){


	if (render_mode == e_none) return;

	// decompose polygons into triangles.

	glEnable(GL_LIGHTING);


	if (render_mode == e_wireframe || render_mode == e_wireframe_shaded) {

		glColor3f(0.0, 0.0, 0.0);

		if (render_mode == e_wireframe) {
			glDisable(GL_LIGHTING);
		} else {
			glEnable(GL_LIGHTING);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0,1.0);

		glBegin(GL_TRIANGLES);
			DrawPolies(mesh);
		glEnd();

		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBegin(GL_TRIANGLES);
			DrawPolies(mesh);
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {

		glEnable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);
			DrawPolies(mesh);
		glEnd();
	}
	

}


	void
BSP_MeshDrawer::
DrawPolies(
	BSP_TMesh &mesh
){

	const vector<BSP_TVertex> & verts = mesh.VertexSet();
	const vector<BSP_TFace> &faces = mesh.FaceSet();

	// just draw the edges for now.

	vector<BSP_TVertex>::const_iterator vertex_it = verts.begin();


	vector<BSP_TFace>::const_iterator faces_it = faces.begin();
	vector<BSP_TFace>::const_iterator faces_end = faces.end();

	for (;faces_it != faces_end; ++faces_it ){	

		glNormal3f(
			faces_it->m_normal.x(),
			faces_it->m_normal.y(),
			faces_it->m_normal.z()
		);

		glVertex3f(
			verts[faces_it->m_verts[0]].m_pos.x(),
			verts[faces_it->m_verts[0]].m_pos.y(),
			verts[faces_it->m_verts[0]].m_pos.z()
		);
		glVertex3f(
			verts[faces_it->m_verts[1]].m_pos.x(),
			verts[faces_it->m_verts[1]].m_pos.y(),
			verts[faces_it->m_verts[1]].m_pos.z()
		);
		glVertex3f(
			verts[faces_it->m_verts[2]].m_pos.x(),
			verts[faces_it->m_verts[2]].m_pos.y(),
			verts[faces_it->m_verts[2]].m_pos.z()
		);
	}
}
	









