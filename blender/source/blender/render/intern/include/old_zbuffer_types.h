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
 * Datatypes for old zbuffering algorithms.
 */

#ifndef OLD_ZBUFFER_TYPES_H
#define OLD_ZBUFFER_TYPES_H "$Id$"

/* ------------------------------------------------------------------------- */

typedef struct PixStr
{
	struct PixStr *next;
	int vlak0, vlak;
	unsigned int z;
	unsigned int mask;
	short aantal, ronde;
} PixStr;

/* ------------------------------------------------------------------------- */

typedef struct PixStrMain
{
	struct PixStr *ps;
	struct PixStrMain *next;
} PixStrMain;

/* ------------------------------------------------------------------------- */

#endif
