/**  
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
 * Prepare the scene data for rendering.
 * 
 * $Id$
 */

#include "MEM_guardedalloc.h"

#include "render.h"
#include "render_intern.h"
#include "renderPreAndPost.h"
#include "RE_callbacks.h"

#include "shadbuf.h"
#include "envmap.h"
#include "renderHelp.h"
#include "shadowBuffer.h"

/**
 * Rotate all objects, make shadowbuffers and environment maps.
 */
void prepareScene()
{
	int a;

	
	if(R.rectot) MEM_freeN(R.rectot);
	R.rectot= 0;
	if(R.rectz) MEM_freeN(R.rectz);
	R.rectz= 0;

	RE_local_get_renderdata();

	/* SCHADUWBUFFER */
	for(a=0; a<R.totlamp; a++) {
		if(RE_local_test_break()) break;
		/* Again, switch between old and new shadowing system. The
         * buffer objects were initially created in
         * blenderWorldManipulation.c */
		if (R.r.mode & R_UNIFIED) {
			if (R.la[a]->shadowBufOb) {
				RE_buildShadowBuffer(R.la[a]->shadowBufOb,
									 R.la[a]);
			}
		} else {
			if(R.la[a]->shb) makeshadowbuf(R.la[a]);
		}
	}

	/* ENVIRONMENT MAPS */
	make_envmaps();
}

void finalizeScene(void)
{
	/* Among other things, it releases the shadow buffers. */
	RE_local_free_renderdata();
}


void doClipping( void (*projectfunc)(float *, float *) )
{
	setzbufvlaggen(projectfunc);	
}

/* eof */
