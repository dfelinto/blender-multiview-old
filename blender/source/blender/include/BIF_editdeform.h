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

#ifndef BIF_DEFORM_H
#define BIF_DEFORM_H

struct Object;
struct MDeformVert;
struct MDeformWeight;
struct bDeformGroup;

void unique_vertexgroup_name (struct bDeformGroup *dg, struct Object *ob);
void add_defgroup (struct Object *ob);
void del_defgroup (struct Object *ob);
void assign_verts_defgroup (void);
void remove_verts_defgroup (int allverts);
void sel_verts_defgroup (int select);
struct MDeformWeight *verify_defweight (struct MDeformVert *dv, int defgroup);
void verify_defgroups (struct Object *ob);

#endif

