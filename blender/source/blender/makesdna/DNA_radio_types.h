/**
 * radio_types.h    dec 2000 Nzc
 *
 * All type defs for the Blender core.
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
 * 
 */

#ifndef DNA_RADIO_TYPES_H
#define DNA_RADIO_TYPES_H

typedef struct Radio {
	short hemires, maxiter;
	short drawtype, flag;			/* bit 0 en 1: limits laten zien */
	short subshootp, subshoote, nodelim, maxsublamp;
	short pama, pami, elma, elmi;	/* patch en elem limits */
	int maxnode;
	float convergence;
	float radfac, gamma;		/* voor afbeelden */
	
} Radio;

#endif
