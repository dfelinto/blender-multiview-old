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

#ifndef BIF_DRAWTEXT_H
#define BIF_DRAWTEXT_H

struct ScrArea;
struct SpaceText;
struct Text;

void unlink_text(struct Text *text);

void init_textspace(struct ScrArea *sa);
void free_textspace(struct SpaceText *st);

void txt_write_file(struct Text *text);
void add_text_fs(char *file);

void free_txt_data(void);
void pop_space_text(struct SpaceText *st);

#endif
