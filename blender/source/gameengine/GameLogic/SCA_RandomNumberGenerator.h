/**
 * Generate random numbers that can be used by other components. Each
 * generator needs its own generator, so that the seed can be set
 * on a per-generator basis.
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

#ifndef __KX_RANDOMNUMBERGENERATOR
#define __KX_RANDOMNUMBERGENERATOR


class SCA_RandomNumberGenerator {

	/** base seed */
	long m_seed;

	/* A bit silly.. The N parameter is a define in the .cpp file */
	/** the array for the state vector  */
	/* unsigned long mt[N]; */
	unsigned long mt[624];

	/** mti==N+1 means mt[KX_MT_VectorLenght] is not initialized */
	int mti; /* initialised in the cpp file */

	/** Calculate a start vector */
	void SetStartVector(void);
 public:
	SCA_RandomNumberGenerator(long seed);
	~SCA_RandomNumberGenerator();
	unsigned long Draw();
	float DrawFloat();
	long GetSeed();
	void SetSeed(long newseed);
};

#endif /* __KX_RANDOMNUMBERGENERATOR */
