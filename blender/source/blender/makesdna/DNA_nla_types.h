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


#ifndef DNA_NLA_TYPES_H
#define DNA_NLA_TYPES_H

struct bAction;
struct Ipo;

typedef struct bActionStrip {
	struct bActionStrip *next, *prev;
	short	flag;
	short	mode;
	int		reserved1;

	struct	Ipo *ipo;			/* Blending ipo */
	struct	bAction *act;		/* The action referenced by this strip */

	float	start, end;			/* The range of frames covered by this strip */
	float	actstart, actend;	/* The range of frames taken from the action */
	float	stridelen;			/* The stridelength (considered when flag & ACT_USESTRIDE) */
	float	repeat;				/* The number of times to repeat the action range */

	float	blendin, blendout;
} bActionStrip;

#define ACTSTRIPMODE_BLEND		0
#define ACTSTRIPMODE_ADD		1

#define ACTSTRIP_SELECT			0x00000001

#define ACTSTRIP_USESTRIDE		0x00000002
#define ACTSTRIP_BLENDTONEXT	0x00000004
#define ACTSTRIP_HOLDLASTFRAME	0x00000008

#define ACTSTRIP_SELECTBIT			0
#define ACTSTRIP_USESTRIDEBIT		1
#define ACTSTRIP_BLENDTONEXTBIT		2
#define ACTSTRIP_HOLDLASTFRAMEBIT	3
#endif

