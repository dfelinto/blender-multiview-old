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
 * 
 * Just the functions to maintain a central event
 * queue.
 */

#include <stdlib.h>
#include <string.h>

#include "BIF_mainqueue.h"

typedef struct {
	unsigned short event;
	short val;
	char ascii;
} QEvent;

static QEvent mainqueue[MAXQUEUE];
static unsigned int nevents= 0;

unsigned short mainqread(short *val, char *ascii)
{
	if (nevents) {
		nevents--;
		
		*val= mainqueue[nevents].val;
		*ascii= mainqueue[nevents].ascii;
		return mainqueue[nevents].event;
	} else
		return 0;
}

void mainqenter(unsigned short event, short val)
{
	mainqenter_ext(event, val, 0);
}

void mainqenter_ext(unsigned short event, short val, char ascii)
{
	if (!event)
		return;

	if (nevents<MAXQUEUE) {
		memmove(mainqueue+1, mainqueue, sizeof(*mainqueue)*nevents);
		
		mainqueue[0].event= event;
		mainqueue[0].val= val;
		mainqueue[0].ascii= ascii;
		
		nevents++;
	}
}

void mainqpushback(unsigned short event, short val, char ascii)
{
	if (nevents<MAXQUEUE) {
		mainqueue[nevents].event= event;
		mainqueue[nevents].val= val;
		mainqueue[nevents].ascii= ascii;
		nevents++;
	}
}

unsigned short mainqtest()
{
	if (nevents)
		return mainqueue[nevents-1].event;
	else
		return 0;
}
