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

#ifndef BIF_OOPS_H
#define BIF_OOPS_H

struct Curve;
struct Oops;
struct OopsLink;
struct SpaceOops;
struct Material;
struct Mesh;
struct MetaBall;
struct Object;
void add_curve_oopslinks(struct Curve *cu, struct Oops *oops, short flag);
void add_from_link(struct Oops *from, struct Oops *oops);
void add_material_oopslinks(struct Material *ma, struct Oops *oops, short flag);
void add_mball_oopslinks(struct MetaBall *mb, struct Oops *oops, short flag);
void add_mesh_oopslinks(struct Mesh *me, struct Oops *oops, short flag);
void add_object_oopslinks(struct Object *ob, struct Oops *oops, short flag);
struct Oops *add_oops(void *id);
struct OopsLink *add_oopslink(char *name, struct Oops *oops, short type, void *from, float xof, float yof);
struct Oops *add_test_oops(void *id);	/* incl links */
void add_texture_oops(struct Material *ma);
void build_oops(void);
struct Oops *find_oops(ID *id);
void free_oops(struct Oops *oops);	/* ook oops zelf */
void free_oopspace(struct SpaceOops *so);
void new_oops_location(struct Oops *);
int oops_test_overlap(struct Oops *test);
int oops_test_overlaphide(struct Oops *test);
float oopslink_totlen(struct Oops *oops);
void shrink_oops(void);
void shuffle_oops(void);
int test_oops(struct Oops *oops);
void test_oopslink(struct OopsLink *ol);
void test_oopslinko(struct OopsLink *ol);

#endif
