/**
 * blenlib/DNA_image_types.h (mar-2001 nzc)
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
#ifndef DNA_IMAGE_TYPES_H
#define DNA_IMAGE_TYPES_H

#include "DNA_ID.h"

struct PackedFile;
struct anim;
struct ImBuf;

typedef struct Image {
	ID id;
	
	char name[160];
	
	struct anim *anim;
	struct ImBuf *ibuf;
	struct ImBuf *mipmap[10];
	
	short ok, flag;
	short lastframe, lastquality;

	/* texture pagina */
	short tpageflag, totbind;
	short xrep, yrep;
	short twsta, twend;
	unsigned int bindcode;
	unsigned int *repbind;	/* om subregio's te kunnen repeaten */
	
	struct PackedFile * packedfile;

	float lastupdate;
	short animspeed;
	short reserved1;
} Image;

/*  in Image struct */
#define MAXMIPMAP	10

/* **************** IMAGE ********************* */

/* flag */
#define IMA_HALVE		1
#define IMA_BW			2
#define IMA_FROMANIM	4
#define IMA_USED		8
#define	IMA_REFLECT		16

/* tpageflag */
#define IMA_TILES		1
#define IMA_TWINANIM	2
#define IMA_COLCYCLE	4	/* Depreciated */

#endif
