/*
 * gammacorrectiontables.h
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

#ifndef GAMMACORRECTIONTABLES_H
#define GAMMACORRECTIONTABLES_H



/* Default gamma. For most CRTs, gamma ranges from 2.2 to 2.5 (Foley), so    */
/* 2.35 seems appropriate enough. Experience teaches a different number      */
/* though. Old blender: 2.0. It  might be nice to make this a slider         */
#define RE_DEFAULT_GAMMA 2.0
/* This 400 is sort of based on the number of intensity levels needed for    */
/* the typical dynamic range of a medium, in this case CRTs. (Foley)         */
/* (Actually, it says the number should be between 400 and 535.)             */
#define RE_GAMMA_TABLE_SIZE 400

/**
 * Initialise the gamma lookup tables
 */
void makeGammaTables(float gamma);

/**
 * Returns true if the table is initialised, false otherwise
 */
int gammaTableIsInitialised(void);

/**
 * Apply gamma correction on col
 */
float gammaCorrect(float col);

/**
 * Apply inverse gamma correction on col
 */
float invGammaCorrect(float col);

/**
 * Tell whether or not to do gamma.
 */
int doGamma(void);

/**
 * Set/unset performing gamma corrections.
 */
void setDoGamma(int);

#endif
