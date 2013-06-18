/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2006 Blender Foundation
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Ton Roosendaal.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/imbuf/intern/openexr/openexr_multi.h
 *  \ingroup openexr
 */


#ifndef __OPENEXR_MULTI_H__
#define __OPENEXR_MULTI_H__

/* experiment with more advanced exr api */

/* XXX layer+pass name max 64? */
/* This api also supports max 8 channels per pass now. easy to fix! */
#define EXR_LAY_MAXNAME     64
#define EXR_PASS_MAXNAME    64
#define EXR_VIEW_MAXNAME    64
#define EXR_TOT_MAXNAME     64
#define EXR_PASS_MAXCHAN    24


#ifdef __cplusplus
extern "C" {
#endif

void *IMB_exr_get_handle(void);
void *IMB_exr_get_handle_name(const char* name);
void    IMB_exr_add_channel(void *handle, const char *layname, const char *passname, const char *view, int xstride, int ystride, float *rect);

int     IMB_exr_begin_read(void *handle, const char *filename, int *width, int *height);
int     IMB_exr_begin_write(void *handle, const char *filename, int width, int height, int compress);
void    IMB_exrtile_begin_write(void *handle, const char *filename, int mipmap, int width, int height, int tilex, int tiley);
int     IMB_exrmultiview_begin_write(void *handle, const char *filename, int width, int height, int compress, int splitviews);

void    IMB_exr_set_channel(void *handle, const char *layname, const char *passname, int xstride, int ystride, float *rect);

void    IMB_exr_read_channels(void *handle);
void    IMB_exr_write_channels(void *handle);
void    IMB_exrtile_write_channels(void *handle, int partx, int party, int level, const char *viewname);
void    IMB_exrmultiview_write_channels(void *handle, int view_id);
void    IMB_exr_clear_channels(void *handle);

void    IMB_exr_multilayer_convert(void *handle, void *base,
                                   void * (*addview)(void *base, const char *str),
                                   void * (*addlayer)(void *base, const char *str),
                                   void (*addpass)(void *base, void *lay, const char *str, float *rect, int totchan, const char *chan_id, const char *view, int view_id));

void    IMB_exr_close(void *handle);

void    IMB_exr_add_view(void *handle, const char *name);

void    IMB_exr_get_multiView_name(void *handle, int view_id, char *view);
int     IMB_exr_get_multiView_count(void *handle);

int IMB_exr_split_token(const char *str, const char *end, const char **token);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* __OPENEXR_MULTI_H */
