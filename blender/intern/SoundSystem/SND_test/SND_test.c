/*  SND_test.c   nov 2000
*  
*  testfile for the SND module
* 
* janco verduin
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

#include "SND_C-api.h"
#include "BlenderWaveCacheCApi.h"
#include "OpenALC-Api.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>

static int buf[3];

float oPos[3]={3.0, 0.0,-1.0};
float oVel[3]={0.0, 0.0, 1.0};
float oOri[6]={0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

void* ReadFile(char *filename)
{
	int file, filelen;
	void *data = NULL;

#if defined(WIN32)	
	file = open(filename, O_BINARY|O_RDONLY);
#else
	file = open(filename, 0|O_RDONLY);
#endif

	if (file == -1) {
		printf("can't open file.\n");
		printf("press q for quit.\n");

	}
	else {
		filelen = lseek(file, 0, SEEK_END);
		lseek(file, 0, SEEK_SET);
		
		if (filelen != 0){
			data = malloc(filelen);
			if (read(file, data, filelen) != filelen) {
				free(data);
				data = NULL;
			}
		}
		close(file);
		
	}
	return (data);
}

int main(int argc, char* argv[])
{
	int ch;
	char* samplename = NULL;
	void* sampleinmemory = NULL;
	SND_CacheHandle	wavecache = NULL;
	SND_SceneHandle	scene = NULL;
	SND_ObjectHandle object = NULL;
	
	wavecache = SND_GetWaveCache();
	scene = SND_CreateOpenALScene(wavecache);

	samplename = "2.wav";
	sampleinmemory = ReadFile(samplename);
	
	if (sampleinmemory) {

		object = SND_CreateObject();
		SND_AddMemoryLocation(samplename, sampleinmemory);
		SND_SetSampleName(object, samplename);
		SND_AddObject(scene, object);
		printf("go your gang...\n");
		printf("1: play\n");
		printf("2: stop\n");
		printf("q: quit\n");
	}
	do
	{
		ch = getchar();
		ch = toupper(ch);
		switch (ch)
		{
		case '1':
			{
				SND_SetPitch(object, 1.0);
				SND_SetGain(object, 1.0);
				SND_StartSound(object);
				break;
			}
		case '2':
			{
				SND_StopSound(object);
				break;
			}
		default:
			break;
		}

		SND_Proceed(scene);

	} while (ch != 'Q');
	
	if (object) {
		
		SND_RemoveObject(scene, object);
		SND_DeleteObject(object);
	}
	
	SND_DeleteScene(scene);
	SND_DeleteCache();
	
	return 0;
	
}
