/*
 * vectorops.h
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

#ifndef VECTOROPS_H
#define VECTOROPS_H 

/* ------------------------------------------------------------------------- */

void  MTC_diff3Int(int v1[3], int v2[3], int v3[3]);
void  MTC_cross3Int(int v1[3], int v2[3], int v3[3]);
int   MTC_dot3Int(int v1[3], int v2[3]); 

void  MTC_diff3Float(float v1[3], float v2[3], float v3[3]);
void  MTC_cross3Float(float v1[3], float v2[3], float v3[3]);
float MTC_dot3Float(float v1[3], float v2[3]); 
void  MTC_cp3Float(float v1[3], float v2[3]);
/**
 * Copy vector with a minus sign (so a = -b)
 */
void  MTC_cp3FloatInv(float v1[3], float v2[3]);

void  MTC_swapInt(int *i1, int *i2);

void  MTC_diff3DFF(double v1[3], float v2[3], float v3[3]);
void  MTC_cross3Double(double v1[3], double v2[3], double v3[3]);
float MTC_normalise3DF(float n[3]);

/* ------------------------------------------------------------------------- */
#endif /* VECTOROPS_H */
