/**
 * blenlib/DNA_text_types.h (mar-2001 nzc)
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
#ifndef DNA_TEXT_TYPES_H
#define DNA_TEXT_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"

typedef struct TextLine {
	struct TextLine *next, *prev;

	char *line;
	int len, blen;
} TextLine;

typedef struct Text {
	ID id;
	
	char *name;
	
	int flags, nlines;
	
	ListBase lines;
	TextLine *curl, *sell;
	int curc, selc;
	
	char *undo_buf;
	int undo_pos, undo_len;
	
	void *compiled;
} Text;


#define TXT_OFFSET 35
#define TXT_TABSIZE	4
#define TXT_INIT_UNDO 1024
#define TXT_MAX_UNDO	(TXT_INIT_UNDO*TXT_INIT_UNDO)

/* text flags */
#define TXT_ISDIRTY             0x0001
#define TXT_ISTMP               0x0002
#define TXT_ISMEM               0x0004
#define TXT_ISEXT               0x0008
#define TXT_ISSCRIPT            0x0010
#define TXT_READONLY            0x0100
#define TXT_FOLLOW              0x0200 	// always follow cursor (console)


#endif
