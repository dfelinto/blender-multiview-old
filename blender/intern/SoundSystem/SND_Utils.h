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
#ifndef	SND_UTILS_H
#define SND_UTILS_H

#include "SND_WaveSlot.h"

#ifdef __cplusplus
extern "C"
{ 
#endif

typedef struct
{
	unsigned char	riff[4];
	signed int		size;
	unsigned char	type[4];
} WavFileHeader;

typedef struct
{
	unsigned short	format;
	unsigned short	numberofchannels;
	unsigned int	samplerate;
	unsigned int	bytespersec;
	unsigned short	blockalignment;
	unsigned short	bitrate;
} WavFmtHeader;

typedef struct
{
	unsigned short	size;
	unsigned short	samplesperblock;
} WavFmtExHeader;

typedef struct
{
	unsigned int		Manufacturer;
	unsigned int		Product;
	unsigned int		SamplePeriod;
	unsigned int		Note;
	unsigned int		FineTune;
	unsigned int		SMPTEFormat;
	unsigned int		SMPTEOffest;
	unsigned int		loops;
	unsigned int		SamplerData;
	struct
	{
		unsigned int	Identifier;
		unsigned int	Type;
		unsigned int	Start;
		unsigned int	End;
		unsigned int	Fraction;
		unsigned int	Count;
	} Loop[1];
} WavSampleHeader;

typedef struct
{
	unsigned char	id[4];
	unsigned int	size;
} WavChunkHeader;

/**  
 *	loads a sample and returns a pointer
 */
extern void* SND_LoadSample(char *filename);

extern bool SND_IsSampleValid(const STR_String& name, void* memlocation);
extern unsigned int SND_GetSampleFormat(void* sample);
extern unsigned int SND_GetNumberOfChannels(void* sample);
extern unsigned int SND_GetSampleRate(void* sample);
extern unsigned int SND_GetBitRate(void* sample);
extern unsigned int SND_GetNumberOfSamples(void* sample);
extern unsigned int SND_GetHeaderSize(void* sample);
extern unsigned int SND_GetExtraChunk(void* sample);

extern void SND_GetSampleInfo(signed char* sample, SND_WaveSlot* waveslot);

#ifdef __cplusplus
}
#endif

#endif
