/**
 * @file BLI_rand.h
 * 
 * Random number functions.
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
 
#ifndef BLI_RAND_H
#define BLI_RAND_H

	/** Seed the random number generator */
void	BLI_srand		(unsigned int seed);

	/** Return a pseudo-random number N where 0<=N<(2^31) */
int		BLI_rand		(void);

	/** Return a pseudo-random number N where 0.0<=N<1.0 */
double	BLI_drand		(void);

	/** Return a pseudo-random number N where 0.0f<=N<1.0f */
float	BLI_frand		(void);

	/** Fills a block of memory starting at @a addr
	 * and extending @a len bytes with pseudo-random
	 * contents. This routine does not use nor modify
	 * the state of the BLI random number generator.
	 */
void	BLI_fillrand	(void *addr, int len);

	/** Stores the BLI randum number generator state
	 * into the buffer in @a loc_r.
	 */
void	BLI_storerand	(unsigned int loc_r[2]);

	/** Retores the BLI randum number generator state
	 * from the buffer in @a loc.
	 */
void	BLI_restorerand	(unsigned int loc[2]);

#endif
