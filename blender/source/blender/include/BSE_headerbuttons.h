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

#ifndef BSE_HEADERBUTTONS_H
#define BSE_HEADERBUTTONS_H

struct uiBlock;
struct ID;

void free_matcopybuf(void);
void clear_matcopybuf(void);
void write_videoscape_fs(void);
void write_vrml_fs(void);
void write_dxf_fs(void);
void do_global_buttons(unsigned short event);
void do_global_buttons2(short event);
int buttons_do_unpack(void);
struct Scene *copy_scene(struct Scene *sce, int level);
void do_info_buttons(unsigned short event);

int start_progress_bar();
void end_progress_bar();
int progress_bar(float done, char *busy_info);

void update_for_newframe(void);

void info_buttons(void);
void do_seq_buttons(short event);
void seq_buttons(void);
void do_view3d_buttons(short event); 
void sound_buttons(void);
void do_action_buttons(unsigned short event);
void do_ipo_buttons(short event);
void do_buts_buttons(short event);
void do_oops_buttons(short event);
void do_sound_buttons(unsigned short event);
void do_layer_buttons(short event);
void do_nla_buttons(unsigned short event);

void nla_buttons(void);
void action_buttons(void);
void buts_buttons(void);
void file_buttons(void);
void image_buttons(void);
void imasel_buttons(void);
void ipo_buttons(void);
void oops_buttons(void);
void text_buttons(void);
void view3d_buttons(void);

void buttons_active_id(struct ID **id, struct ID **idfrom);

void do_headerbuttons(short event);

#endif /*  BSE_HEADERBUTTONS_H */
