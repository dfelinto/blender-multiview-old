/**
 * Jitter offset table
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

#include "gammaCorrectionTables.h"
#include <stdlib.h>
#include <math.h>

#include "render_intern.h"

/* There are two parts here: one for the old renderer, one for the unified   */
/* renderer. we start with the latter.                                       */

/* Default gamma. For most CRTs, gamma ranges from 2.2 to 2.5 (Foley), so    */
/* 2.35 seems appropriate enough. Experience teaches a different number      */
/* though. Old blender: 2.0. It  might be nice to make this a slider         */
#define RE_DEFAULT_GAMMA 2.0
/* This 400 is sort of based on the number of intensity levels needed for    */
/* the typical dynamic range of a medium, in this case CRTs. (Foley)         */
/* (Actually, it says the number should be between 400 and 535.)             */
#define RE_GAMMA_TABLE_SIZE 400

/* These indicate the status of the gamma lookup table --------------------- */

static float gamma_range_table[RE_GAMMA_TABLE_SIZE + 1];
static float gamfactor_table[RE_GAMMA_TABLE_SIZE];
static float inv_gamma_range_table[RE_GAMMA_TABLE_SIZE + 1];
static float inv_gamfactor_table[RE_GAMMA_TABLE_SIZE];
static float colour_domain_table[RE_GAMMA_TABLE_SIZE + 1];
static float colour_step;
static float inv_colour_step;
static float valid_gamma;
static float valid_inv_gamma;
static int   gamma_table_initialised = 0;
static int   do_gamma;
/* ------------------------------------------------------------------------- */

float gammaCorrect(float c)
{
	int i;
	float res = 0.0;
	
	i = floor(c * inv_colour_step);
	/* Clip to range [0,1]: outside, just do the complete calculation.       */
	/* We may have some performance problems here. Stretching up the LUT     */
	/* may help solve that, by exchanging LUT size for the interpolation.    */
	/* Negative colours are explicitly handled.                              */
	if (i < 0) res = -pow(abs(c), valid_gamma);
	else if (i >= RE_GAMMA_TABLE_SIZE ) res = pow(c, valid_gamma);
	else res = gamma_range_table[i] + 
  			 ( (c - colour_domain_table[i]) * gamfactor_table[i]); 
	
	return res;
} /* end of float gammaCorrect(float col) */

/* ------------------------------------------------------------------------- */

float invGammaCorrect(float col)
{
	int i;
	float res = 0.0;

	i = floor(col*inv_colour_step);
	/* Negative colours are explicitly handled.                              */
	if (i < 0) res = -pow(abs(col), valid_inv_gamma);
	else if (i >= RE_GAMMA_TABLE_SIZE) res = pow(col, valid_inv_gamma);
	else res = inv_gamma_range_table[i] + 
  			 ( (col - colour_domain_table[i]) * inv_gamfactor_table[i]);
			   
	return res;
} /* end of float invGammaCorrect(float col) */


/* ------------------------------------------------------------------------- */

void makeGammaTables(float gamma)
{
	/* we need two tables: one forward, one backward */
	int i;

	valid_gamma        = gamma;
	valid_inv_gamma    = 1.0 / gamma;
	colour_step        = 1.0 / RE_GAMMA_TABLE_SIZE;
	inv_colour_step    = (float) RE_GAMMA_TABLE_SIZE; 

	/* We could squeeze out the two range tables to gain some memory.        */	
	for (i = 0; i < RE_GAMMA_TABLE_SIZE; i++) {
		colour_domain_table[i]   = i * colour_step;
		gamma_range_table[i]     = pow(colour_domain_table[i],
										valid_gamma);
		inv_gamma_range_table[i] = pow(colour_domain_table[i],
										valid_inv_gamma);
	}

	/* The end of the table should match 1.0 carefully. In order to avoid    */
	/* rounding errors, we just set this explicitly. The last segment may    */
	/* have a different lenght than the other segments, but our              */
	/* interpolation is insensitive to that.                                 */
	colour_domain_table[RE_GAMMA_TABLE_SIZE]   = 1.0;
	gamma_range_table[RE_GAMMA_TABLE_SIZE]     = 1.0;
	inv_gamma_range_table[RE_GAMMA_TABLE_SIZE] = 1.0;

	/* To speed up calculations, we make these calc factor tables. They are  */
	/* multiplication factors used in scaling the interpolation.             */
	for (i = 0; i < RE_GAMMA_TABLE_SIZE; i++ ) {
		gamfactor_table[i] = inv_colour_step
			* (gamma_range_table[i + 1] - gamma_range_table[i]) ;
		inv_gamfactor_table[i] = inv_colour_step
			* (inv_gamma_range_table[i + 1] - inv_gamma_range_table[i]) ;
	}

	gamma_table_initialised = 1;
} /* end of void makeGammaTables(float gamma) */

/* ------------------------------------------------------------------------- */

int gammaTableIsInitialised(void)
{
	return gamma_table_initialised;
}

/* ------------------------------------------------------------------------- */
int doGamma()
{
	return do_gamma;
}

/* ------------------------------------------------------------------------- */

/**
 * Set/unset performing gamma corrections.
 */
void setDoGamma(int i)
{
	do_gamma = i;
}

/* eof */
