/*
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
 * blenloader genfile private function prototypes
 */

#ifndef GENFILE_H
#define GENFILE_H

struct SDNA;

int dna_findstruct_nr(struct SDNA *sdna, char *str);
char *dna_get_structDNA_compareflags(struct SDNA *sdna, struct SDNA *newsdna);
void dna_switch_endian_struct(struct SDNA *oldsdna, int oldSDNAnr, char *data);
void *dna_reconstruct(struct SDNA *newsdna, struct SDNA *oldsdna, char *compflags, int oldSDNAnr, int blocks, void *data);

struct SDNA *dna_sdna_from_data(void *data, int datalen, int do_endian_swap);
void dna_freestructDNA(struct SDNA *sdna);

	
#endif
