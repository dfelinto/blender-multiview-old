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

#include <stdlib.h>

#ifdef WIN32
#include "BLI_winstuff.h"
#endif

#include "DNA_space_types.h"
#include "DNA_screen_types.h"

#include "BKE_global.h"

#include "BIF_gl.h"
#include "BIF_interface.h"
#include "BIF_mywindow.h"
#include "BIF_screen.h"
#include "BIF_space.h"

#include "winlay.h"

#if 0
static void copy_back_to_front(void)
{
	int actually_swap= 0;
	int winx, winy;
	char *data;

	winlay_get_winsize(&winx, &winy);
	
	if (actually_swap) {
		data= malloc(4*winx*winy);
		glReadPixels(0, 0, winx, winy, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	
	mywinset(1);
	glRasterPos2f(-0.5,-0.5);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_FRONT);
	glCopyPixels(0, 0, winx, winy, GL_COLOR);
	glDrawBuffer(GL_BACK);
	glFinish();

	if (actually_swap) {
		glRasterPos2f(-0.5,-0.5);
		glDrawPixels(winx, winy, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glFinish();
		free(data);
	}
}
#endif

static void screen_swapbuffers_REDRAW(bScreen *sc)
{
	ScrArea *sa;
	int doswap= 0, swap;
	
	/* dit is een nieuwe implementatie: uitsluitend met redraws en normale swapbuffer */

	/* allemaal front ok? */
	sa= sc->areabase.first;
	while(sa) {
		if(sa->win && (sa->win_swap & WIN_FRONT_OK)==0) break;
		if(!sa->headertype) sa->head_swap= WIN_EQUAL;
		if((sa->head_swap & WIN_FRONT_OK)==0) break;
		sa= sa->next;
	}
	if(sa==0) return;

	sa= sc->areabase.first;
	while(sa) {
	
		swap= sa->win_swap;
		if( (swap & WIN_BACK_OK) == 0) {
			scrarea_do_windraw(sa);
			doswap= 1;
			sa->win_swap= swap | WIN_BACK_OK;
		}
		else if( sa->win_swap==WIN_BACK_OK) doswap= 1;
		
		swap= sa->head_swap;
		if( (swap & WIN_BACK_OK) == 0) {
			if (sa->headertype) scrarea_do_headdraw(sa);
			doswap= 1;
			sa->head_swap = swap | WIN_BACK_OK;
		}
		else if( sa->head_swap==WIN_BACK_OK) doswap= 1;
	
		sa= sa->next;
	}

	/* de hele backbuffer moet nu OK zijn */
	if(doswap) {
		myswapbuffers();
	}
}

#include "BMF_Api.h"
#include <stdio.h>

static void draw_debug_win(int win)
{
	static int drawcounter= 0;
	char buf[64];
	int x, y;
	int w, h;
	
	bwin_getsuborigin(win, &x, &y);
	bwin_getsize(win, &w, &h);

	mywinset(win);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glColor3f(0.8, 0.8, 0.8);
	glRecti(0, 0, w, h);

	glColor3f(0.6, 0.6, 0.6);
	glRecti(2, 2, w-4, h-4);
	
	glColor3ub(0, 0, 0);
	glRasterPos2i(5, 5);
	
	sprintf(buf, "win: %d - (%d, %d, %d, %d) %d\n", win, x, y, w, h, drawcounter++);
	BMF_DrawString(G.font, buf);
}

static void screen_swapbuffers_DEBUG(bScreen *sc)
{
	ScrArea *sa;

	for (sa= sc->areabase.first; sa; sa= sa->next) {
		draw_debug_win(sa->win);
		if (sa->headwin) draw_debug_win(sa->headwin);
	}
	
	myswapbuffers();
}

static void screen_swapbuffers_DEBUG_SWAP(bScreen *sc)
{
	ScrArea *sa;
	int doswap= 0, swap;
	
	/* dit is een nieuwe implementatie: uitsluitend met redraws en normale swapbuffer */

	/* allemaal front ok? */
	sa= sc->areabase.first;
	while(sa) {
		if(sa->win && (sa->win_swap & WIN_FRONT_OK)==0) break;
		if(!sa->headertype) sa->head_swap= WIN_EQUAL;
		if((sa->head_swap & WIN_FRONT_OK)==0) break;
		sa= sa->next;
	}
	if(sa==0) return;

	sa= sc->areabase.first;
	while(sa) {
	
		swap= sa->win_swap;
		if( (swap & WIN_BACK_OK) == 0) {
			scrarea_do_windraw(sa);
			draw_debug_win(sa->win);
			
			doswap= 1;
			sa->win_swap= swap | WIN_BACK_OK;
		}
		else if( sa->win_swap==WIN_BACK_OK) doswap= 1;
		
		swap= sa->head_swap;
		if( (swap & WIN_BACK_OK) == 0) {
			if (sa->headertype) {
				scrarea_do_headdraw(sa);
				draw_debug_win(sa->headwin);
			}
			doswap= 1;
			sa->head_swap = swap | WIN_BACK_OK;
		}
		else if( sa->head_swap==WIN_BACK_OK) doswap= 1;
	
		sa= sa->next;
	}

	/* de hele backbuffer moet nu OK zijn */
	if(doswap) {
		myswapbuffers();
	}
}

static void screen_swapbuffers_SIMPLE(bScreen *sc)
{
	ScrArea *sa;
	
	mywinset(1);
	glClearColor(0.8, 0.6, 0.7, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	for (sa= sc->areabase.first; sa; sa= sa->next) {
		scrarea_do_windraw(sa);
		if (sa->headertype) scrarea_do_headdraw(sa);
	}
	
	myswapbuffers();
}

static int drawmode_default= 'r';
int debug_swapbuffers_override= 0;

void set_debug_swapbuffers_ovveride(bScreen *sc, int mode)
{
	ScrArea *sa;
	for (sa= sc->areabase.first; sa; sa= sa->next) {
		sa->win_swap= 0;
		sa->head_swap= 0;
	}
	debug_swapbuffers_override= mode;
}

void screen_swapbuffers(void)
{
	ScrArea *tempsa;

	bScreen *sc= G.curscreen;
	int drawmode;
	
	if (debug_swapbuffers_override) {
		drawmode= debug_swapbuffers_override;
	} else {
		drawmode= drawmode_default;
	}
	
	tempsa= curarea;
	areawinset(1);
	
	if (drawmode=='s') {
		screen_swapbuffers_SIMPLE(sc);
	} else if (drawmode=='d') {
		screen_swapbuffers_DEBUG(sc);
	} else if (drawmode=='f') {
		screen_swapbuffers_DEBUG_SWAP(sc);
	} else {
		screen_swapbuffers_REDRAW(sc);
	}

	areawinset(tempsa->win);
}