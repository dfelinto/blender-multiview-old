/*
 * initrender_ext.h
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

#ifndef INITRENDER_EXT_H
#define INITRENDER_EXT_H 

/* type includes */

#include "DNA_effect_types.h"        /* for PartEff type */
#include "render_types.h"

/* Functions */

void init_def_material(void);
void init_render_jit(int nr);
float  calc_weight(float *weight, int i, int j);
void defaultlamp(void);
void schrijfplaatje(char *name);
void initparts(void);
short setpart(short nr); /* return 0 als geen goede part */
void addparttorect(short nr, Part *part);
void add_to_blurbuf(int blur);
void oldRenderLoop(void); /* Calls the old renderer. Contains the PART and FIELD loops. */
void render(void);  /* Switch between the old and the unified renderer. */
/*  void write_screendump(char *name); not here !*/

#endif /* INITRENDER_EXT_H */

