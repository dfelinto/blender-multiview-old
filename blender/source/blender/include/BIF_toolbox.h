/* toolbox (SPACEKEY) related */

/* $Id$ 
*/
/*
/* ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
/*
/* The contents of this file may be used under the terms of either the GNU
/* General Public License Version 2 or later (the "GPL", see
/* http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
/* later (the "BL", see http://www.blender.org/BL/ ) which has to be
/* bought from the Blender Foundation to become active, in which case the
/* above mentioned GPL option does not apply.
/*
/* The Original Code is Copyright (C) 2002 by NaN Holding BV.
/* All rights reserved.
/*
/* The Original Code is: all of this file.
/*
/* Contributor(s): none yet.
/*
/* ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

	/* TBOXX: breedte in pixels */
#define TBOXXL  80
#define TBOXXR  170
#define TBOXX	(TBOXXL+TBOXXR)
	/* TBOXEL: aantal elementen onder elkaar */
#define TBOXEL	14
	/* TBOXH: hoogte van 1 element */
#define TBOXH	20
#define TBOXY	TBOXH*TBOXEL

#define TBOXBLACK	1
#define TBOXDGREY	2
#define TBOXWHITE	3
#define TBOXGREY	4
#define TBOXLGREY	5

/* toolbox menu code defines
   -> SPACE->(MAIN_ENTRY) */
#ifdef MAART
enum {
	TBOX_MAIN_FILE,
	TBOX_MAIN_ADD,
	TBOX_MAIN_EDIT,
	TBOX_MAIN_OBJECT1,
	TBOX_MAIN_OBJECT2,
	TBOX_MAIN_MESH,
	TBOX_MAIN_CURVE,
	TBOX_MAIN_KEY,
	TBOX_MAIN_RENDER,
	TBOX_MAIN_VIEW,
	TBOX_MAIN_PYTOOL = 13
};

#else
enum {
	TBOX_MAIN_ADD,
	TBOX_MAIN_FILE,
	TBOX_MAIN_EDIT,
	TBOX_MAIN_OBJECT1,
	TBOX_MAIN_OBJECT2,
	TBOX_MAIN_MESH,
	TBOX_MAIN_CURVE,
	TBOX_MAIN_KEY,
	TBOX_MAIN_RENDER,
	TBOX_MAIN_VIEW,
	TBOX_MAIN_PYTOOL = 13
};
#endif

/* protos */

/* toolbox.c */
void ColorFunc (int i);
void mygetcursor (short int *index);
void tbox_setinfo (int x, int y);
void dummy (void);
void bgnpupdraw (int startx, int starty, int endx, int endy);
void endpupdraw (void);
void asciitoraw (int ch, short unsigned int *event, short unsigned int *qual);
void tbox_execute (void);
void tbox_getmouse (short int *mval);
void tbox_setmain (int val);
void bgntoolbox (void);
void endtoolbox (void);
void tbox_embossbox (short int x1, short int y1, short int x2, short int y2, short int type);
void tbox_drawelem_body (int x, int y, int type);
void tbox_drawelem_text (int x, int y, int type);
void tbox_drawelem (int x, int y, int type);
void tbox_getactive (int *x, int *y);
void drawtoolbox (void);
void toolbox (void);

void notice (char *str, ...);
void error (char *fmt, ...);

int saveover (char *filename);
int okee (char *fmt, ...);

short button (short *var, short min, short max, char *str);
short fbutton (float *var, float min, float max, char *str);
short sbutton (char *var, float min, float max, char *str);	/* __NLA */
int movetolayer_buts (unsigned int *lay);
void draw_numbuts_tip (char *str, int x1, int y1, int x2, int y2);
int do_clever_numbuts (char *name, int tot, int winevent);
void add_numbut (int nr, int type, char *str, float min, float max, void *poin, char *tip);
void clever_numbuts (void);
void replace_names_but (void);

void BIF_screendump(void);
void write_screendump(char *name);
