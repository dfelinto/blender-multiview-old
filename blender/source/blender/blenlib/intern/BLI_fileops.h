/**
 * blenlib/BLI_listBase.h    mar 2001 Nzc
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
 *
 * More low-level fileops from Daniel Dunbar. Two functions were also
 * defined in storage.c. These are the old fop_ prefixes. There is
 * definitely some redundancy here!
 * */

#ifndef BLI_FILEOPS_H
#define BLI_FILEOPS_H

char *first_slash(char *string);

/* only for the sane unix world: direct calls to system functions :( */
#ifndef WIN32
void BLI_setCmdCallBack(int (*f)(char*));
#endif

#endif
