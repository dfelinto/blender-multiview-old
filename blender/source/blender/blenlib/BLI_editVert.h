/**
 * blenlib/BLI_editVert.h    mar 2001 Nzc
 *
 * Some editing types needed in the lib (unfortunately) for
 * scanfill.c
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
 */

#ifndef BLI_EDITVERT_H
#define BLI_EDITVERT_H

typedef struct EditVert
{
	struct EditVert *next, *prev, *vn;
	float no[3];
	float co[3];
	short xs, ys;
	char f, h, f1, hash;
	int	totweight;				/* __NLA */
	struct MDeformWeight *dw;	/* __NLA */
} EditVert;

typedef struct EditEdge
{
	struct EditEdge *next, *prev;
	struct EditVert *v1, *v2, *vn;
	short f,h;
	short f1, dir;
} EditEdge;

typedef struct EditVlak
{
	struct EditVlak *next, *prev;
	struct EditVert *v1, *v2, *v3, *v4;
	struct EditEdge *e1, *e2, *e3, *e4;
	float n[3];
	float uv[4][2];
	unsigned int col[4];
	struct TFace *tface;	/* a pointer to original tface. */
	char mat_nr, flag;
	char f, f1;
} EditVlak;

#endif
