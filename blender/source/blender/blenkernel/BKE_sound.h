/**
 * sound.h (mar-2001 nzc)
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
#ifndef BKE_SOUND_H
#define BKE_SOUND_H

struct PackedFile;
struct bSound;
struct bSample;
struct ListBase;

extern struct ListBase *samples;

/*  void *sound_get_listener(void); implemented in src!also declared there now  */

void sound_set_packedfile(struct bSample* sample, struct PackedFile* pf);
struct PackedFile* sound_find_packedfile(struct bSound* sound);
void sound_free_sample(struct bSample* sample);
void sound_free_sound(struct bSound* sound);

#endif
