/*
 * SoundDefines.h
 *
 * this is where all kinds of defines are stored
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

#ifndef __SOUNDDEFINES_H
#define __SOUNDDEFINES_H

/* the types of devices */
enum
{
	snd_e_dummydevice = 0,
	snd_e_fmoddevice,
	snd_e_openaldevice
};

/* general stuff */
#define NUM_BUFFERS						128
#define NUM_SOURCES						16

/* openal related stuff */
#define AL_LOOPING						0x1007

/* fmod related stuff */
#ifdef WIN32
#define MIXRATE							22050
#else
#define MIXRATE							44100
#endif
#define NUM_FMOD_MIN_HW_CHANNELS		16
#define NUM_FMOD_MAX_HW_CHANNELS		16

/* activelist defines */
enum
{
	SND_REMOVE_ACTIVE_OBJECT = 0,
	SND_ADD_ACTIVE_OBJECT,
	SND_DO_NOTHING
};

/* playstate flags */
enum
{
	SND_UNKNOWN = -1,
	SND_INITIAL,
	SND_MUST_PLAY,
	SND_PLAYING,
	SND_MUST_STOP,
	SND_STOPPED,
	SND_MUST_PAUSE,
	SND_PAUSED,
	SND_MUST_RESUME,
	SND_MUST_STOP_WHEN_FINISHED,
	SND_MUST_BE_DELETED
};

/* loopmodes */
enum
{
	SND_LOOP_OFF = 0,
	SND_LOOP_NORMAL,
	SND_LOOP_BIDIRECTIONAL
};


/* cd playstate flags */
enum
{
	SND_CD_ALL = 0,
	SND_CD_TRACK,
	SND_CD_TRACKLOOP
};

/* sample types */
enum
{
	SND_WAVE_FORMAT_UNKNOWN = 0,
	SND_WAVE_FORMAT_PCM,
	SND_WAVE_FORMAT_ADPCM,
	SND_WAVE_FORMAT_ALAW = 6,
	SND_WAVE_FORMAT_MULAW,
	SND_WAVE_FORMAT_DIALOGIC_OKI_ADPCM = 17,
	SND_WAVE_FORMAT_CONTROL_RES_VQLPC = 34,
	SND_WAVE_FORMAT_GSM_610 = 49,
	SND_WAVE_FORMAT_MPEG3 = 85
};

#endif //__SOUNDDEFINES_H
