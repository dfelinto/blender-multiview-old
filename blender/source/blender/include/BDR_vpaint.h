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

#ifndef BDR_VPAINT_H
#define BDR_VPAINT_H

struct Mesh;
struct MDeformVert;	/* __NLA */
unsigned int vpaint_get_current_col(void);
unsigned int rgba_to_mcol(float r, float g, float b, float a);
void do_shared_vertexcol(struct Mesh *me);
void make_vertexcol(void);
void copy_vpaint_undo(unsigned int *mcol, int tot);
void vpaint_undo(void);
void clear_vpaint(void);
void clear_vpaint_selectedfaces(void);
void vpaint_dogamma(void);
void sample_vpaint(void);
void init_vertexpaint(void);
void free_vertexpaint(void);
void vertex_paint(void);
void set_vpaint(void);  
void set_wpaint(void);

void weight_paint(void);
void wpaint_undo (void);
void copy_wpaint_undo (struct MDeformVert *dverts, int dcount);

#endif /*  BDR_VPAINT_H */
