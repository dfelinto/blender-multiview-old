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

#ifndef BIF_EDITSOUND_H
#define BIF_EDITSOUND_H

struct bSound;
struct bSample;
struct ListBase;
struct PackedFile;

void sound_init_audio(void);
void sound_initialize_sounds(void);
void sound_exit_audio(void);
int sound_get_mixrate(void);

void* sound_get_audiodevice(void);
void* sound_get_listener(void);

int sound_set_sample(struct bSound* sound, struct bSample* sample);
int sound_sample_is_null(struct bSound* sound);
int sound_load_sample(struct bSound* sound);

struct bSample* sound_find_sample(struct bSound* sound);
struct bSample* sound_new_sample(struct bSound* sound);

struct bSound* sound_new_sound(char *name);
struct bSound* sound_make_copy(struct bSound* originalsound);
void sound_end_all_sounds(void);

void sound_initialize_sample(struct bSound * sound);
void sound_load_samples(void);

void sound_play_sound(struct bSound *sound);
void sound_stop_all_sounds(void);

void sound_set_position(void *object,
						struct bSound *sound,
						float obmatrix[4][4]);

#endif

