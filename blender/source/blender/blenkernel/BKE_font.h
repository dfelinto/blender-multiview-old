/**
 * blenlib/BKE_vfont.h (mar-2001 nzc)
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
 */
#ifndef BKE_VFONT_H
#define BKE_VFONT_H

struct VFont;
struct Object;
struct Curve;
struct objfnt;

void BKE_font_register_builtin(void *mem, int size);

void free_vfont(struct VFont *sc); 
struct VFont *load_vfont(char *name);

struct chartrans *text_to_curve(struct Object *ob, int mode);
void font_duplilist(struct Object *par);

#endif
