/**
 * $Id$
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

#include "Basic.h"


int min_i(int a, int b) { 
	return (a<b)?a:b; 
}
int max_i(int a, int b) {
	return (b<a)?a:b; 
}
int clamp_i(int val, int min, int max) { 
	return min_i(max_i(val, min), max);
}

float min_f(float a, float b) { 
	return (a<b)?a:b; 
}
float max_f(float a, float b) { 
	return (b<a)?a:b; 
}
float clamp_f(float val, float min, float max) { 
	return min_f(max_f(val, min), max);
}

void rect_copy(int dst[2][2], int src[2][2]) {
	dst[0][0]= src[0][0], dst[0][1]= src[0][1];
	dst[1][0]= src[1][0], dst[1][1]= src[1][1];
}
int rect_contains_pt(int rect[2][2], int pt[2]){
	return ((rect[0][0] <= pt[0] && pt[0] <= rect[1][0]) &&
			(rect[0][1] <= pt[1] && pt[1] <= rect[1][1]));
}
int rect_width(int rect[2][2]) {
	return (rect[1][0]-rect[0][0]);
}
int rect_height(int rect[2][2]) {
	return (rect[1][1]-rect[0][1]);
}
