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
#include "PIL_time.h"
#include "BLI_rand.h"

#ifdef WIN32
typedef unsigned __int64	r_uint64;
#else
typedef unsigned long long	r_uint64;
#endif

#define MULTIPLIER	0x5DEECE66D
#define ADDEND		0xB

#define LOWSEED		0x330E

static r_uint64 X= 0;

void BLI_srand(unsigned int seed) {
	X= (((r_uint64) seed)<<16) | LOWSEED;
}

int BLI_rand(void) {
	X= (MULTIPLIER*X + ADDEND)&0x0000FFFFFFFFFFFF;
	return (int) (X>>17);
}

double BLI_drand(void) {
	return (double) BLI_rand()/0x80000000;
}

float BLI_frand(void) {
	return (float) BLI_rand()/0x80000000;
}

void BLI_storerand(unsigned int loc_r[2]) {
	loc_r[0]= (unsigned int) (X>>32);
	loc_r[1]= (unsigned int) (X&0xFFFFFFFF);
}

void BLI_restorerand(unsigned int loc[2]) {
	X= ((r_uint64) loc[0])<<32;
	X|= loc[1];
}

void BLI_fillrand(void *addr, int len) {
	unsigned char *p= addr;
	unsigned int save[2];

	BLI_storerand(save);
	
	BLI_srand((unsigned int) (PIL_check_seconds_timer()*0x7FFFFFFF));
	while (len--) *p++= BLI_rand()&0xFF;
	BLI_restorerand(save);
}
