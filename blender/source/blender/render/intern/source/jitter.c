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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "MEM_guardedalloc.h"

#include "BLI_arithb.h"
#include "BLI_rand.h"
#include "render.h"
#include "render_intern.h"
#include "jitter.h"

float jit[64][2];

void init_render_jit(int nr);


void RE_jitterate1(float *jit1, float *jit2, int num, float rad1)
{
	int i , j , k;
	float vecx, vecy, dvecx, dvecy, x, y, len;

	for (i = 2*num-2; i>=0 ; i-=2) {
		dvecx = dvecy = 0.0;
		x = jit1[i];
		y = jit1[i+1];
		for (j = 2*num-2; j>=0 ; j-=2) {
			if (i != j){
				vecx = jit1[j] - x - 1.0;
				vecy = jit1[j+1] - y - 1.0;
				for (k = 3; k>0 ; k--){
					if( fabs(vecx)<rad1 && fabs(vecy)<rad1) {
						len=  sqrt(vecx*vecx + vecy*vecy);
						if(len>0 && len<rad1) {
							len= len/rad1;
							dvecx += vecx/len;
							dvecy += vecy/len;
						}
					}
					vecx += 1.0;

					if( fabs(vecx)<rad1 && fabs(vecy)<rad1) {
						len=  sqrt(vecx*vecx + vecy*vecy);
						if(len>0 && len<rad1) {
							len= len/rad1;
							dvecx += vecx/len;
							dvecy += vecy/len;
						}
					}
					vecx += 1.0;

					if( fabs(vecx)<rad1 && fabs(vecy)<rad1) {
						len=  sqrt(vecx*vecx + vecy*vecy);
						if(len>0 && len<rad1) {
							len= len/rad1;
							dvecx += vecx/len;
							dvecy += vecy/len;
						}
					}
					vecx -= 2.0;
					vecy += 1.0;
				}
			}
		}

		x -= dvecx/18.0 ;
		y -= dvecy/18.0;
		x -= floor(x) ;
		y -= floor(y);
		jit2[i] = x;
		jit2[i+1] = y;
	}
	memcpy(jit1,jit2,2 * num * sizeof(float));
}

void RE_jitterate2(float *jit1, float *jit2, int num, float rad2)
{
	int i, j;
	float vecx, vecy, dvecx, dvecy, x, y;

	for (i=2*num -2; i>= 0 ; i-=2){
		dvecx = dvecy = 0.0;
		x = jit1[i];
		y = jit1[i+1];
		for (j =2*num -2; j>= 0 ; j-=2){
			if (i != j){
				vecx = jit1[j] - x - 1.0;
				vecy = jit1[j+1] - y - 1.0;

				if( fabs(vecx)<rad2) dvecx+= vecx*rad2;
				vecx += 1.0;
				if( fabs(vecx)<rad2) dvecx+= vecx*rad2;
				vecx += 1.0;
				if( fabs(vecx)<rad2) dvecx+= vecx*rad2;

				if( fabs(vecy)<rad2) dvecy+= vecy*rad2;
				vecy += 1.0;
				if( fabs(vecy)<rad2) dvecy+= vecy*rad2;
				vecy += 1.0;
				if( fabs(vecy)<rad2) dvecy+= vecy*rad2;

			}
		}

		x -= dvecx/2 ;
		y -= dvecy/2;
		x -= floor(x) ;
		y -= floor(y);
		jit2[i] = x;
		jit2[i+1] = y;
	}
	memcpy(jit1,jit2,2 * num * sizeof(float));
}


void initjit(float *jit, int num)
{
	float *jit2, x, rad1, rad2, rad3;
	int i;

	if(num==0) return;

	jit2= MEM_mallocN(12 + 2*sizeof(float)*num, "initjit");
	rad1=  1.0/sqrt((float)num);
	rad2= 1.0/((float)num);
	rad3= sqrt((float)num)/((float)num);

	BLI_srand(31415926 + num);
	x= 0;
	for(i=0; i<2*num; i+=2) {
		jit[i]= x+ rad1*(0.5-BLI_drand());
		jit[i+1]= ((float)i/2)/num +rad1*(0.5-BLI_drand());
		x+= rad3;
		x -= floor(x);
	}

	for (i=0 ; i<24 ; i++) {
		RE_jitterate1(jit, jit2, num, rad1);
		RE_jitterate1(jit, jit2, num, rad1);
		RE_jitterate2(jit, jit2, num, rad2);
	}

	MEM_freeN(jit2);
}

void init_render_jit(int nr)
{
	static int lastjit= 0;

	if(lastjit==nr) return;

	memset(jit, 0, 64*2*4);
	initjit(jit[0], nr);

	lastjit= nr;

}

/* eof */
