/**
 * Functions for writing avi-format files.
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

#include <string.h>

#ifdef WIN32
#include "BLI_winstuff.h"
#endif

#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"

#include "AVI_avi.h"

#include "BKE_bad_level_calls.h"
#include "BKE_global.h"

#include "IMB_imbuf_types.h"
#include "IMB_imbuf.h"

static AviMovie *avi=NULL;
static int sframe;

void makeavistring (char *string) 
{
	char txt[64];

	if (string==0) return;

	strcpy(string, G.scene->r.pic);
	BLI_convertstringcode(string, G.sce, G.scene->r.cfra);

	RE_make_existing_file(string);

	if (strcasecmp(string + strlen(string) - 4, ".avi")) {
		sprintf(txt, "%04d_%04d.avi", (G.scene->r.sfra) , (G.scene->r.efra) );
		strcat(string, txt);
	}
}

void start_avi(void)
{
	int x, y;
	char name[256];
	AviFormat format;
	int quality, framerate;
	
	makeavistring(name);

	sframe = (G.scene->r.sfra);
	x = R.rectx;
	y = R.recty;

	quality= R.r.quality;
	framerate= R.r.frs_sec;
	
	avi = MEM_mallocN (sizeof(AviMovie), "avimovie");

	if (R.r.imtype != AVI_FORMAT_MJPEG) format = AVI_FORMAT_AVI_RGB;
	else format = AVI_FORMAT_MJPEG;

	if (AVI_open_compress (name, avi, 1, format) != AVI_ERROR_NONE) {
		error("open movie");
		MEM_freeN (avi);
		avi = NULL;
		return;
	}
			
	AVI_set_compress_option (avi, AVI_OPTION_TYPE_MAIN, 0, AVI_OPTION_WIDTH, &x);
	AVI_set_compress_option (avi, AVI_OPTION_TYPE_MAIN, 0, AVI_OPTION_HEIGHT, &y);
	AVI_set_compress_option (avi, AVI_OPTION_TYPE_MAIN, 0, AVI_OPTION_QUALITY, &quality);		
	AVI_set_compress_option (avi, AVI_OPTION_TYPE_MAIN, 0, AVI_OPTION_FRAMERATE, &framerate);

	avi->interlace= 0;
	avi->odd_fields= 0;
/* 	avi->interlace= R.r.mode & R_FIELDS; */
/* 	avi->odd_fields= (R.r.mode & R_ODDFIELD)?1:0; */
	
	printf("Created avi: %s\n", name);
}

void append_avi(int frame)
{
	unsigned int *rt1, *rt2, *temp;
	int y;

	if (avi == NULL) {
		G.afbreek = 1;
		return;
	}

		/* note that libavi free's the buffer... stupid interface - zr */
	temp = MEM_mallocN(R.rectx*R.recty*4, "append_avi buf");

	rt1= temp;
	rt2= R.rectot + (R.recty-1)*R.rectx;
	for (y=0; y < R.recty; y++, rt1+= R.rectx, rt2-= R.rectx) {
		memcpy (rt1, rt2, R.rectx*4);
	}

	IMB_convert_rgba_to_abgr(R.rectx*R.recty, temp);
	
	AVI_write_frame (avi, (frame-sframe), AVI_FORMAT_RGB32, 
										temp, R.rectx*R.recty*4);
	printf ("added frame %3d (frame %3d in avi): ", frame, frame-sframe);
}

void end_avi(void)
{
	if (avi == NULL) return;

	AVI_close_compress (avi);
	MEM_freeN (avi);
	avi= NULL;
}
