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
 * 
 */

#ifndef BSE_FILESEL_H
#define BSE_FILESEL_H

struct SpaceFile;
struct direntry;
struct ID;

void clear_global_filesel_vars(void);
void filesel_statistics(struct SpaceFile *sfile, int *totfile, int *selfile, float *totlen, float *sellen);
void checkdir(char *dir);
void test_flags_file(struct SpaceFile *sfile);
void sort_filelist(struct SpaceFile *sfile);
void read_dir(struct SpaceFile *sfile);
void freefilelist(struct SpaceFile *sfile);
void parent(struct SpaceFile *sfile);
void swapselect_file(struct SpaceFile *sfile);
void drawfilespace(void);
void activate_fileselect(int type, char *title, char *file, void (*func)(char *));
void activate_imageselect(int type, char *title, char *file, void (*func)(char *));
void activate_databrowse(struct ID *id, int idcode, int fromcode, int retval, short *menup, void (*func)(unsigned short));
void filesel_prevspace(void);
void free_filesel_spec(char *dir);
void winqreadfilespace(unsigned short event, short val, char ascii);
void main_to_filelist(struct SpaceFile *sfile);   

#endif
