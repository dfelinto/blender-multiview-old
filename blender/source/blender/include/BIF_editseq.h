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

#ifndef BIF_EDITSEQ_H
#define BIF_EDITSEQ_H

struct Sequence;

void				add_duplicate_seq(void);
void				add_sequence(int type);
void				borderselect_seq(void);
void				boundbox_seq(void);
void				change_sequence(void);
void				clear_last_seq(void);
void				clever_numbuts_seq(void);
void				del_seq(void);
void				enter_meta(void);
struct Sequence*	find_nearest_seq(int *hand);
int					insert_gap(int gap, int cfra);
void				make_meta(void);
void				mouse_select_seq(void);
void				no_gaps(void);
void				seq_snapmenu(void);
void				set_filter_seq(void);
void				swap_select_seq(void);
void				touch_seq_files(void);
void				transform_seq(int mode);
void				un_meta(void);

#endif
