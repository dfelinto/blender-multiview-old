/**
 * blenlib/BKE_texture.h (mar-2001 nzc)
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
#ifndef BKE_TEXTURE_H
#define BKE_TEXTURE_H

struct Tex;
struct MTex;
struct PluginTex;
struct LampRen;
struct ColorBand;
struct HaloRen;

/*  in ColorBand struct */
#define MAXCOLORBAND 16


void free_texture(struct Tex *t); 
int test_dlerr(const char *name,  const char *symbol);
void open_plugin_tex(struct PluginTex *pit);
struct PluginTex *add_plugin_tex(char *str);
void free_plugin_tex(struct PluginTex *pit);
struct ColorBand *add_colorband(void);
int do_colorband(struct ColorBand *coba);
void default_tex(struct Tex *tex);
struct Tex *add_texture(char *name);
void default_mtex(struct MTex *mtex);
struct MTex *add_mtex(void);
struct Tex *copy_texture(struct Tex *tex);
void make_local_texture(struct Tex *tex);
void autotexname(struct Tex *tex);
void init_render_texture(struct Tex *tex);
void init_render_textures(void);
void end_render_texture(struct Tex *tex);
void end_render_textures(void);
int clouds(struct Tex *tex, float *texvec);
int blend(struct Tex *tex, float *texvec);
int wood(struct Tex *tex, float *texvec);
int marble(struct Tex *tex, float *texvec);
int magic(struct Tex *tex, float *texvec);
int stucci(struct Tex *tex, float *texvec);
int texnoise(struct Tex *tex);
int plugintex(struct Tex *tex, float *texvec, float *dxt, float *dyt);
void tubemap(float x, float y, float z, float *adr1, float *adr2);
void spheremap(float x, float y, float z, float *adr1, float *adr2);
void do_2d_mapping(struct MTex *mtex, float *t, float *dxt, float *dyt);
int multitex(struct Tex *tex, float *texvec, float *dxt, float *dyt);
void do_material_tex(void);
void do_halo_tex(struct HaloRen *har, float xn, float yn, float *colf);
void do_sky_tex(void);
void do_lamp_tex(struct LampRen *la, float *lavec);
void externtex(struct MTex *mtex, float *vec);
void externtexcol(struct MTex *mtex, float *orco, char *col);
void render_realtime_texture(void);           
#endif
