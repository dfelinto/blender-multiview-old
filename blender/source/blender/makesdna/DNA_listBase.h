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
 * These structs are the foundation for all linked lists in the
 * library system.
 *
 */

#ifndef DNA_LISTBASE_H
#define DNA_LISTBASE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Link
{
	struct Link *next,*prev;
} Link;

/* never change the size of this! genfile.c detects pointerlen with it */
typedef struct ListBase 
{
	void *first, *last;
} ListBase;

/* 8 byte alignment! */

#ifdef __cplusplus
}
#endif

#endif
