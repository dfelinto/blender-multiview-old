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

#ifndef BIF_SPACE_H
#define BIF_SPACE_H

struct ListBase;
struct ScrArea;
struct SpaceButs;
struct View2D;

#define REMAKEIPO		1
#define OOPS_TEST		2
#define REMAKEALLIPO	3	/* Reevan's ipo fixing test */

void	scrarea_do_windraw		(struct ScrArea *sa);
void	scrarea_do_winchange	(struct ScrArea *sa);
void	scrarea_do_winhandle	(struct ScrArea *sa, unsigned short event, short val, char ascii);
void	scrarea_do_headdraw		(struct ScrArea *sa);
void	scrarea_do_headchange	(struct ScrArea *sa);

/* space.c */
extern		 void space_set_commmandline_options(void);
extern       void allqueue(unsigned short event, short val);
extern       void allspace(unsigned short event, short val);
extern       void changebutspace(void);
extern       void changeview2d(void);
extern       void changeview3d(void);
extern       void copy_view3d_lock(short val);
extern       void drawemptyspace(void);
extern       void drawinfospace(void);
extern       void duplicatespacelist(struct ScrArea *area, struct ListBase *lb1, struct ListBase *lb2);
extern       void extern_set_butspace(int fkey);
extern       void force_draw(void);
extern		 void force_draw_all(void);
extern		 void force_draw_plus(int type);
extern       void freespacelist(struct ListBase *lb);
extern       void handle_view3d_lock(void);
extern       void init_butspace(struct ScrArea *sa);
extern       void init_filespace(struct ScrArea *sa);
extern       void init_imagespace(struct ScrArea *sa);
extern       void init_oopsspace(struct ScrArea *sa);
extern       void init_nlaspace(struct ScrArea *sa);
extern       void init_seqspace(struct ScrArea *sa);
extern       void init_v2d_oops(struct View2D *v2d);
extern       void initipo(struct ScrArea *sa);
extern       void initview3d(struct ScrArea *sa);
extern       void newspace(struct ScrArea *sa, int type);
extern       void set_rects_butspace(struct SpaceButs *buts);
extern       void winqread3d(unsigned short event, short val, char ascii);
extern       void winqreadbutspace(unsigned short event, short val, char ascii);
extern       void winqreadimagespace(unsigned short event, short val, char ascii);
extern       void winqreadinfospace(unsigned short event, short val, char ascii);
extern       void winqreadipo(unsigned short event, short val, char ascii);
extern       void winqreadoopsspace(unsigned short event, short val, char ascii);
extern       void winqreadnlaspace(unsigned short event, short val, char ascii);
extern       void winqreadsequence(unsigned short event, short val, char ascii);
extern       void test_butspace(void);
extern       void start_game(void);

extern		 void mainwindow_raise(void);
extern		 void mainwindow_make_active(void);
extern		 void mainwindow_close(void);

#endif
