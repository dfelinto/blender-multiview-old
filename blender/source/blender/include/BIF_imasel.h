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
#ifndef BIF_IMASEL_H
#define BIF_IMASEL_H

struct SpaceImaSel;
struct OneSelectableIma;
struct ScrArea;
struct ImaDir;

void imadir_parent(struct SpaceImaSel *simasel);
int  bitset(int l,  int bit);
void free_sel_ima(struct OneSelectableIma *firstima);

void write_new_pib(struct SpaceImaSel *simasel);
void free_ima_dir(struct ImaDir *firstdir);
void check_for_pib(struct SpaceImaSel *simasel);
void clear_ima_dir(struct SpaceImaSel *simasel);
void check_ima_dir_name(char *dir);
int get_ima_dir(char *dirname, int dtype, int *td, struct ImaDir **first);
void get_next_image(struct SpaceImaSel *simasel);
void get_file_info(struct SpaceImaSel *simasel);
void get_pib_file(struct SpaceImaSel *simasel);
void change_imadir(struct SpaceImaSel *simasel);
void init_imaselspace(struct ScrArea *sa);
void check_imasel_copy(struct SpaceImaSel *simasel);
void free_imasel(struct SpaceImaSel *simasel);

#endif
