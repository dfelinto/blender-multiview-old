/**
 * blenlib/BKE_image.h (mar-2001 nzc)
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
#ifndef BKE_IMAGE_H
#define BKE_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

struct Image;
struct ImBuf;
struct Tex;
struct anim;

void free_image(struct Image *me);
void free_image_buffers(struct Image *ima);
struct Image *add_image(char *name);
void free_unused_animimages(void);
void makepicstring(char *string, int frame);
struct anim *openanim(char * name, int flags);
int calcimanr(int cfra, struct Tex *tex);
void do_laseroptics_patch(struct ImBuf *ibuf);
void de_interlace_ng(struct ImBuf *ibuf);
void de_interlace_st(struct ImBuf *ibuf);
void load_image(struct Image * ima, int flags, char *relabase, int framenum);
void ima_ibuf_is_nul(struct Tex *tex);
int imagewrap(struct Tex *tex, float *texvec);
int imagewraposa(struct Tex *tex, float *texvec, float *dxt, float *dyt);
void converttopremul(struct ImBuf *ibuf);
void makemipmap(struct Image *ima);

#ifdef __cplusplus
}
#endif

#endif
