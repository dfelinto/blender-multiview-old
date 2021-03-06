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
 * The Original Code is Copyright (C) 2007 Blender Foundation.
 * All rights reserved.
 *
 * 
 * Contributor(s): Blender Foundation
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/windowmanager/wm_stereo.h
 *  \ingroup wm
 */

#ifndef __WM_STEREO_H__
#define __WM_STEREO_H__

struct bContext;
struct wmEvent;
struct wmOperator;
struct wmWindow;
struct ReportList;

/* wm_stereo.c */
void	wm_method_draw_stereo(bContext *C, wmWindow *win);
int		wm_stereo_toggle_exec(bContext *C, wmOperator *op);
int		wm_stereo_toggle_invoke(bContext *C, wmOperator *op, const wmEvent *event);

#endif /* __WM_STEREO_H__ */

