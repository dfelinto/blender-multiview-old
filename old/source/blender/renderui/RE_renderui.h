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
 * Interface to transform the Blender scene into renderable data.
 *
 * @mainpage RE - Blender render ui external interface
 *
 * @section about About the RE-ui module
 *
 * This module provides a wrapper for calling rendering from within
 * Blender. 
 *
 * @section issues Known issues with RE-ui
 *
 *
 * @section dependencies Dependencies
 *
 * - Uses gl
 *
 * */

#ifndef RE_RENDERUI_H
#define RE_RENDERUI_H

#ifdef __cplusplus
extern "C" {
#endif

	/* preview: */
	
	void    RE_preview_changed(short win);

	/* only used in space.c */
	void    RE_previewrender(void);

	void RE_previewdraw(void);

#ifdef __cplusplus
}
#endif
#endif

  
