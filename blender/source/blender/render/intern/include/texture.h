/*
 * texture_ext.h
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

#ifndef TEXTURE_EXT_H
#define TEXTURE_EXT_H "$Id$"
#define TEXTURE_EXT_H "Copyright (C) 2001 NaN Technologies B.V.

/* no types!!! */
/*  #include "texture_types.h" */

struct Tex;
struct MTex;
struct HaloRen;
struct LampRen;
/**
 * Takes uv coordinates (R.uv[], O.dxuv, O.dyuv), find texture colour
 * at that spot (using imagewrap()). 
 * Result is kept in R.vcol (float vector 3)
 */
void render_realtime_texture(void);

/**
 * Do texture mapping for materials. Communicates with R.... variables.
 */
void do_material_tex(void);

/* unsorted */
int blend(struct Tex *tex, float *texvec);
int clouds(struct Tex *tex, float *texvec);
int cubemap(struct MTex *mtex, float x, float y, float z, float *adr1, float *adr2);
int cubemap_glob(struct MTex *mtex, float x, float y, float z, float *adr1, float *adr2);
int cubemap_ob(struct MTex *mtex, float x, float y, float z, float *adr1, float *adr2);
void do_2d_mapping(struct MTex *mtex, float *t, float *dxt, float *dyt);
void do_halo_tex(struct HaloRen *har, float xn, float yn, float *colf);
void do_lamp_tex(struct LampRen *la, float *lavec);
void do_sky_tex(void);
int magic(struct Tex *tex, float *texvec);
int marble(struct Tex *tex, float *texvec);
int multitex(struct Tex *tex, float *texvec, float *dxt, float *dyt);
int plugintex(struct Tex *tex, float *texvec, float *dxt, float *dyt);
int stucci(struct Tex *tex, float *texvec);
int texnoise(struct Tex *tex);
int wood(struct Tex *tex, float *texvec);

#endif /* TEXTURE_EXT_H */










