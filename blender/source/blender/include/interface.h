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

#ifndef INTERFACE_H
#define INTERFACE_H

/* general defines */

#define UI_MAX_DRAW_STR	180
#define UI_MAX_NAME_STR	64
#define UI_ARRAY	29

/* block->font, for now: bold = medium+1 */
#define UI_HELV			0
#define UI_HELVB		1


/* MAART: moved from Button.h */
/* Button types */
#define CHA	32
#define SHO	64
#define INT	96
#define FLO	128
#define FUN	192
#define BIT	256

#define BUTPOIN	(128+64+32)

#define BUT	(1<<9)
#define ROW	(2<<9)
#define TOG	(3<<9)
#define SLI	(4<<9)
#define	NUM	(5<<9)
#define TEX	(6<<9)
#define TOG3	(7<<9)
#define TOGR	(8<<9)
#define TOGN	(9<<9)
#define LABEL	(10<<9)
#define MENU	(11<<9)
#define ICONROW	(12<<9)
#define ICONTOG	(13<<9)
#define NUMSLI	(14<<9)
#define COL		(15<<9)
#define IDPOIN	(16<<9)
#define HSVSLI 	(17<<9)
#define SCROLL	(18<<9)
#define BLOCK	(19<<9)
#define BUTM	(20<<9)
#define SEPR	(21<<9)
#define LINK	(22<<9)
#define INLINK	(23<<9)
#define KEYEVT	(24<<9)

#define BUTTYPE	(31<<9)

#define MAXBUTSTR	20


/* return from uiDoBlock */
#define UI_CONT				0
#define UI_NOTHING			1
#define UI_RETURN_CANCEL	2
#define UI_RETURN_OK		4
#define UI_RETURN_OUT		8
#define UI_RETURN			14

/* uiBut->flag */
#define UI_SELECT		1
#define UI_MOUSE_OVER	2
#define UI_ACTIVE		4
#define UI_HAS_ICON		8
#define UI_TEXT_LEFT	16

/* uiBlock->flag */
#define UI_BLOCK_LOOP		1
#define UI_BLOCK_REDRAW		2
#define UI_BLOCK_RET_1		4
#define UI_BLOCK_BUSY		8
#define UI_BLOCK_NUMSELECT	16
#define UI_BLOCK_ENTER_OK	32
#define UI_BLOCK_MENUMODE	128

/* uiBlock->dt */
#define UI_EMBOSSX		0	/* Rounded embossed button */
#define UI_EMBOSSW		1	/* Flat bordered button */
#define UI_EMBOSSN		2	/* No border */
#define UI_EMBOSSF		3	/* Square embossed button */
#define UI_EMBOSSM		4	/* Colored Border */
#define UI_EMBOSSP		5	/* Borderless coloured button */

/* uiBlock->direction */
#define UI_TOP		0
#define UI_DOWN		1
#define UI_LEFT		2
#define UI_RIGHT	3

/* uiBlock->autofill */
#define UI_BLOCK_COLLUMNS	1
#define UI_BLOCK_ROWS		2

#endif
