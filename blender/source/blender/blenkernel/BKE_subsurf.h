/* $Id$ 
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
#ifndef BKE_SUBSURF_H
#define BKE_SUBSURF_H

struct Mesh;
struct Object;
struct Displist;

void subsurf_to_mesh(struct Object *oldob, struct Mesh *newme);
void subsurf_make_mesh(struct Object *ob, short subdiv);
void subsurf_make_editmesh(struct Object *ob);
struct DispList* subsurf_mesh_to_displist(struct Mesh *me, struct DispList *dl, short subdiv);

#endif
