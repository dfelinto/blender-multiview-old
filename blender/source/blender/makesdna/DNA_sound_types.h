/**
 * blenlib/DNA_sound_types.h (mar-2001 nzc)
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
#ifndef DNA_SOUND_TYPES_H
#define DNA_SOUND_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"

/* stupid... could easily be solved */
#include "DNA_view2d_types.h"

/* extern int noaudio; * defined in sound.c . also not very nice */
/*  extern ListBase *samples; don't do this in DNA, but in BKE_... instead */

struct bSample;
struct Ipo;
struct PackedFile;
struct SpaceLink;

/* should not be here! */
#
#
typedef struct bSample {
	ID id;
	void *data;
	void *snd_sample;
	short type, bits;
	short channels;
	int len, rate;
//	int buffer;
	int alindex;
	char fakedata[16];
	int flags;
	char name[160];
	struct PackedFile * packedfile;
	short us;
} bSample;



typedef struct bSound {
	ID id;
	char name[160];
	struct bSample *sample;
	struct PackedFile *packedfile;
	struct PackedFile *newpackedfile;
	void *snd_sound;
	struct Ipo *ipo;
	float volume, panning;
	/**
	 * Sets the rollofffactor. The	rollofffactor is a per-Source parameter
	 * the application can use to increase or decrease	the range of a source
	 * by decreasing or increasing the attenuation, respectively. The default
	 * value is 1. The implementation is free to optimize for a rollofffactor
	 * value of 0, which indicates that the application does not wish any
	 * distance attenuation on the respective Source.
	 */
	float attenuation;
	float pitch;
	/**
	 * min_gain indicates the minimal gain which is always guaranteed for this sound
	 */
	float min_gain;
	/**
	 * max_gain indicates the maximal gain which is always guaranteed for this sound
	 */
	float max_gain;
	/**
	 * Sets the referencedistance at which the listener will experience gain.
	 */
	float distance;
	int flags;
//	unsigned int loopstart;
//	unsigned int loopend;
	char channels;
	char highprio;
	char pad[6];
	
} bSound;

typedef struct bSoundListener {
	ID id;
	/**
	 * Overall gain
	 */
	float gain;
	/**
	 * Sets a scaling to exaggerate or deemphasize the Doppler (pitch) shift
	 * resulting from the calculation.
	 */
	float dopplerfactor;
	/**
	 * Sets the value of the propagation speed relative to which the source
	 * velocities are interpreted.
	 */
	float dopplervelocity;
	short numsoundsblender;
	short numsoundsgameengine;
	
} bSoundListener;

typedef struct SpaceSound {
	struct SpaceLink *next, *prev;
	int spacetype, pad;
	struct ScrArea *area;
	
	View2D v2d;
	
	bSound *sound;
	short mode, sndnr;
	short xof, yof;
	short flag, lock;
	int pad2;
} SpaceSound;


enum SAMPLE_FileTypes {
	SAMPLE_INVALID = -1,		// must be negative
	SAMPLE_UNKNOWN = 0,
	SAMPLE_RAW,
	SAMPLE_WAV,
	SAMPLE_MP2,
	SAMPLE_MP3,
	SAMPLE_OGG_VORBIS,
	SAMPLE_WMA,
	SAMPLE_ASF,
	SAMPLE_AIFF
};


#define SOUND_CHANNELS_STEREO	0
#define SOUND_CHANNELS_LEFT		1
#define SOUND_CHANNELS_RIGHT	2

enum SOUND_FLAGS_BITS {
	SOUND_FLAGS_LOOP_BIT = 0,
	SOUND_FLAGS_FIXED_VOLUME_BIT,
	SOUND_FLAGS_FIXED_PANNING_BIT,
	SOUND_FLAGS_3D_BIT,
	SOUND_FLAGS_BIDIRECTIONAL_LOOP_BIT,
	SOUND_FLAGS_PRIORITY_BIT
};

#define SOUND_FLAGS_LOOP 			(1 << SOUND_FLAGS_LOOP_BIT)
#define SOUND_FLAGS_FIXED_VOLUME 	(1 << SOUND_FLAGS_FIXED_VOLUME_BIT)
#define SOUND_FLAGS_FIXED_PANNING 	(1 << SOUND_FLAGS_FIXED_PANNING_BIT)
#define SOUND_FLAGS_3D				(1 << SOUND_FLAGS_3D_BIT)
#define SOUND_FLAGS_BIDIRECTIONAL_LOOP (1 << SOUND_FLAGS_BIDIRECTIONAL_LOOP_BIT)
#define SOUND_FLAGS_PRIORITY		(1 << SOUND_FLAGS_PRIORITY_BIT)

enum SAMPLE_FLAGS_BITS {
	SAMPLE_NEEDS_SAVE_BIT = 0
};

#define SAMPLE_NEEDS_SAVE		(1 << SAMPLE_NEEDS_SAVE_BIT)

/* to DNA_sound_types.h*/

#endif
