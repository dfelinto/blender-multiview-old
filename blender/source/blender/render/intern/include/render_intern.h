/*
 * render_int.h
 * misc internal defines for renderer
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

	/* XXX, should die, no good reason to write 
	 * regular (non-file related) endian dependant
	 * code.
	 */
#if defined(__sgi) || defined(__sparc) || defined (__PPC__) || defined (__ppc__) || defined (__BIG_ENDIAN__)
#define RCOMP	3
#define GCOMP	2
#define BCOMP	1
#define ACOMP	0
#else
#define RCOMP	0
#define GCOMP	1
#define BCOMP	2
#define ACOMP	3
#endif

#define VECCOPY(v1,v2) 		{*(v1)= *(v2); *(v1+1)= *(v2+1); *(v1+2)= *(v2+2);}
#define QUATCOPY(v1,v2) 	{*(v1)= *(v2); *(v1+1)= *(v2+1); *(v1+2)= *(v2+2); *(v1+3)= *(v2+3);}

#define CLAMPIS(a, b, c)	((a)<(b) ? (b) : (a)>(c) ? (c) : (a))

typedef struct { 
	int a, b, c, d;
} byte16;
#define COPY_16(a,b)		(*((byte16 *)(a))= *((byte16 *)(b)))
