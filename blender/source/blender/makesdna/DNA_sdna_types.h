/**
 * blenlib/DNA_sdna.h (mar-2001 nzc)
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
#ifndef DNA_SDNA_H
#define DNA_SDNA_H

#
#
struct SDNA {
	char *data;
	int datalen, nr_names;
	char **names;
	int nr_types, pointerlen;
	char **types;
	short *typelens;
	int nr_structs;
	short **structs;
	
		/* wrong place for this really, its a simple
		 * cache for findstruct_nr.
		 */
	int lastfind;
};

#
#
typedef struct BHead {
	int code, len;
	void *old;
	int SDNAnr, nr;
} BHead;
#
#
typedef struct BHead4 {
	int code, len;
	int old;
	int SDNAnr, nr;
} BHead4;
#
#
typedef struct BHead8 {
	int code, len;
#ifdef WIN32
	/* This is a compiler type! */
	__int64 old;
#else
	long long old;
#endif	
	int SDNAnr, nr;
} BHead8;

#endif
