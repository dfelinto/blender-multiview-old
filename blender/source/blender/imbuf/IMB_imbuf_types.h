/**
 * IMB_imbuf_types.h (mar-2001 nzc)
 *
 * Types needed for using the image buffer.
 *
 * Imbuf is external code, slightly adapted to live in the Blender
 * context. It requires an external jpeg module, and the avi-module
 * (also external code) in order to function correctly.
 *
 * This file contains types and some constants that go with them. Most
 * are self-explanatory (e.g. IS_amiga tests whether the buffer
 * contains an Amiga-format file).
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
#ifndef IMB_IMBUF_TYPES_H
#define IMB_IMBUF_TYPES_H

#include <stdio.h>        /* for size_t */
#include "DNA_listBase.h" /* for ListBase */
struct _AviMovie;
struct Mdec;

/* the basic imbuf type */

typedef struct ImBuf{
	short	x,y;		/* breedte in pixels, hoogte in scanlines */
	short	skipx;		/* breedte in ints om bij volgende scanline te komen */
	unsigned char	depth;		/* actieve aantal bits/bitplanes */
	unsigned char	cbits;		/* aantal active bits in cmap */
	unsigned short	mincol;
	unsigned short	maxcol;
	int	type;		/* 0=abgr, 1=bitplanes */
	int	ftype;
	unsigned int	*cmap;		/* int array van kleuren */
	unsigned int	*rect;		/* databuffer */
	unsigned int	**planes;	/* bitplanes */
	int	flags;
	int	mall;		/* wat is er intern gemalloced en mag weer vrijgegeven worden */
	short	xorig, yorig;
	char	name[1023];
	char	namenull;
	int	userflags;
	int	*zbuf;
	void *userdata;
	unsigned char *encodedbuffer;
	unsigned int   encodedsize;
	unsigned int   encodedbuffersize;
} ImBuf;

/* Moved from BKE_bmfont_types.h because it is a userflag bit mask. */
typedef enum {
	IB_BITMAPFONT = 1 << 0,
	IB_BITMAPDIRTY = 1 << 1
} ImBuf_userflagsMask;


/* From iff.h. This was once moved away by Frank, now Nzc moves it
 * back. Such is the way it is... It is a long list of defines, and
 * there are a few external defines in the back. Most of the stuff is
 * probably imbuf_intern only. This will need to be merged later
 * on. */

#define IB_rect			(1 << 0)
#define IB_planes		(1 << 1)
#define IB_cmap			(1 << 2)

#define IB_vert			(1 << 4)
#define IB_freem		(1 << 6)
#define IB_test			(1 << 7)

#define IB_ttob			(1 << 8)
#define IB_subdlta		(1 << 9)
#define IB_fields		(1 << 11)
#define IB_zbuf			(1 << 13)

#define IB_mem			(1 << 14)

#define AMI	    (1 << 31)
#define PNG	    (1 << 30)
#define Anim	(1 << 29)
#define TGA	    (1 << 28)
#define JPG		(1 << 27)
#define BMP		(1 << 26)

#define RAWTGA	(TGA | 1)

#define JPG_STD	(JPG | (0 << 8))
#define JPG_VID	(JPG | (1 << 8))
#define JPG_JST	(JPG | (2 << 8))
#define JPG_MAX	(JPG | (3 << 8))
#define JPG_MSK	(0xffffff00)

#define AM_ham	    (0x0800 | AMI)
#define AM_hbrite   (0x0080 | AMI)

#define C233	1
#define YUVX	2
#define HAMX	3
#define TANX	4

#define AN_c233			(Anim | C233)
#define AN_yuvx			(Anim | YUVX)
#define AN_hamx			(Anim | HAMX)
#define AN_tanx			(Anim | TANX)

#define IS_amiga(x)		(x->ftype & AMI)
#define IS_ham(x)		((x->ftype & AM_ham) == AM_ham)
#define IS_hbrite(x)	((x->ftype & AM_hbrite) == AM_hbrite)

#define IS_anim(x)		(x->ftype & Anim)
#define IS_hamx(x)		(x->ftype == AN_hamx)
#define IS_tga(x)		(x->ftype & TGA)
#define IS_png(x)		(x->ftype & PNG)
#define IS_bmp(x)		(x->ftype & BMP)

#define IMAGIC 	0732
#define IS_iris(x)		(x->ftype == IMAGIC)

#define IS_jpg(x)		(x->ftype & JPG)
#define IS_stdjpg(x)	((x->ftype & JPG_MSK) == JPG_STD)
#define IS_vidjpg(x)	((x->ftype & JPG_MSK) == JPG_VID)
#define IS_jstjpg(x)	((x->ftype & JPG_MSK) == JPG_JST)
#define IS_maxjpg(x)	((x->ftype & JPG_MSK) == JPG_MAX)

#endif

