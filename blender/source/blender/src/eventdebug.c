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
#include "mydevice.h"
#include "blendef.h"

char *event_to_string(short evt) {
#define smap(evt)	case evt: return #evt
	switch (evt) {
	default: return "<unknown>";
	smap(CHANGED);
	smap(DRAWEDGES);
	smap(BACKBUFDRAW);
	smap(EXECUTE);
	smap(LOAD_FILE);
	smap(RESHAPE);
	smap(UI_BUT_EVENT);
	smap(REDRAWVIEW3D);
	smap(REDRAWBUTSHEAD);
	smap(REDRAWBUTSALL);
	smap(REDRAWBUTSVIEW);
	smap(REDRAWBUTSLAMP);
	smap(REDRAWBUTSMAT);
	smap(REDRAWBUTSTEX);
	smap(REDRAWBUTSANIM);
	smap(REDRAWBUTSWORLD);
	smap(REDRAWBUTSRENDER);
	smap(REDRAWBUTSEDIT);
	smap(REDRAWVIEWCAM);
	smap(REDRAWHEADERS);
	smap(REDRAWBUTSGAME);
	smap(REDRAWBUTSRADIO);
	smap(REDRAWVIEW3D_Z);
	smap(REDRAWALL);
	smap(REDRAWINFO);
	smap(RENDERPREVIEW);
	smap(REDRAWIPO);
	smap(REDRAWDATASELECT);
	smap(REDRAWSEQ);
	smap(REDRAWIMAGE);
	smap(REDRAWOOPS);
	smap(REDRAWIMASEL);
	smap(AFTERIMASELIMA);
	smap(AFTERIMASELGET);
	smap(AFTERIMAWRITE);
	smap(IMALEFTMOUSE);
	smap(AFTERPIBREAD);
	smap(REDRAWTEXT);
	smap(REDRAWBUTSSCRIPT);
	smap(REDRAWSOUND);
	smap(REDRAWBUTSSOUND);
	smap(REDRAWACTION);
	smap(LEFTMOUSE);
	smap(MIDDLEMOUSE);
	smap(RIGHTMOUSE);
	smap(MOUSEX);
	smap(MOUSEY);
	smap(TIMER0);
	smap(TIMER1);
	smap(TIMER2);
	smap(TIMER3);
	smap(KEYBD);
	smap(RAWKEYBD);
	smap(REDRAW);
	smap(INPUTCHANGE);
	smap(QFULL);
	smap(WINFREEZE);
	smap(WINTHAW);
	smap(WINCLOSE);
	smap(WINQUIT);
	smap(Q_FIRSTTIME);
	smap(AKEY);
	smap(BKEY);
	smap(CKEY);
	smap(DKEY);
	smap(EKEY);
	smap(FKEY);
	smap(GKEY);
	smap(HKEY);
	smap(IKEY);
	smap(JKEY);
	smap(KKEY);
	smap(LKEY);
	smap(MKEY);
	smap(NKEY);
	smap(OKEY);
	smap(PKEY);
	smap(QKEY);
	smap(RKEY);
	smap(SKEY);
	smap(TKEY);
	smap(UKEY);
	smap(VKEY);
	smap(WKEY);
	smap(XKEY);
	smap(YKEY);
	smap(ZKEY);
	smap(ZEROKEY);
	smap(ONEKEY);
	smap(TWOKEY);
	smap(THREEKEY);
	smap(FOURKEY);
	smap(FIVEKEY);
	smap(SIXKEY);
	smap(SEVENKEY);
	smap(EIGHTKEY);
	smap(NINEKEY);
	smap(CAPSLOCKKEY);
	smap(LEFTCTRLKEY);
	smap(LEFTALTKEY);
	smap(RIGHTALTKEY);
	smap(RIGHTCTRLKEY);
	smap(RIGHTSHIFTKEY);
	smap(LEFTSHIFTKEY);
	smap(ESCKEY);
	smap(TABKEY);
	smap(RETKEY);
	smap(SPACEKEY);
	smap(LINEFEEDKEY);
	smap(BACKSPACEKEY);
	smap(DELKEY);
	smap(SEMICOLONKEY);
	smap(PERIODKEY);
	smap(COMMAKEY);
	smap(QUOTEKEY);
	smap(ACCENTGRAVEKEY);
	smap(MINUSKEY);
	smap(SLASHKEY);
	smap(BACKSLASHKEY);
	smap(EQUALKEY);
	smap(LEFTBRACKETKEY);
	smap(RIGHTBRACKETKEY);
	smap(LEFTARROWKEY);
	smap(DOWNARROWKEY);
	smap(RIGHTARROWKEY);
	smap(UPARROWKEY);
	smap(PAD0);
	smap(PAD1);
	smap(PAD2);
	smap(PAD3);
	smap(PAD4);
	smap(PAD5);
	smap(PAD6);
	smap(PAD7);
	smap(PAD8);
	smap(PAD9);
	smap(PADPERIOD);
	smap(PADSLASHKEY);
	smap(PADASTERKEY);
	smap(PADMINUS);
	smap(PADENTER);
	smap(PADPLUSKEY);
	smap(F1KEY);
	smap(F2KEY);
	smap(F3KEY);
	smap(F4KEY);
	smap(F5KEY);
	smap(F6KEY);
	smap(F7KEY);
	smap(F8KEY);
	smap(F9KEY);
	smap(F10KEY);
	smap(F11KEY);
	smap(F12KEY);
	smap(PAUSEKEY);
	smap(INSERTKEY);
	smap(HOMEKEY);
	smap(PAGEUPKEY);
	smap(PAGEDOWNKEY);
	smap(ENDKEY);
	smap(REDRAWBUTSCONSTRAINT);
	smap(REDRAWNLA);
	}
	#undef smap
}
