/**
 * header_image.c oct-2003
 *
 * Functions to draw the "UV/Image Editor" window header
 * and handle user events sent to it.
 * 
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "DNA_ID.h"
#include "DNA_image_types.h"
#include "DNA_scene_types.h"
#include "DNA_screen_types.h"
#include "DNA_space_types.h"
#include "DNA_userdef_types.h"

#include "BDR_drawmesh.h"
#include "BDR_unwrapper.h"

#include "BKE_brush.h"
#include "BKE_global.h"
#include "BKE_image.h"
#include "BKE_main.h"

#include "BIF_drawimage.h"
#include "BIF_editsima.h"
#include "BIF_interface.h"
#include "BIF_resources.h"
#include "BIF_screen.h"
#include "BIF_space.h"
#include "BIF_transform.h"

#include "BSE_filesel.h"
#include "BSE_headerbuttons.h"

#include "BPY_extern.h"
#include "BPY_menus.h"

#include "IMB_imbuf_types.h"
#include "BSE_trans_types.h"

#include "blendef.h"
#include "mydevice.h"

void do_image_buttons(unsigned short event)
{
	ID *id, *idtest;
	int nr;

	if(curarea->win==0) return;

	switch(event) {
	case B_SIMAGEHOME:
		image_home();
		break;

	case B_SIMABROWSE:	
		if(G.sima->imanr== -2) {
			activate_databrowse((ID *)G.sima->image, ID_IM, 0, B_SIMABROWSE,
											&G.sima->imanr, do_image_buttons);
			return;
		}
		if(G.sima->imanr < 0) break;
	
		nr= 1;
		id= (ID *)G.sima->image;

		idtest= G.main->image.first;
		while(idtest) {
			if(nr==G.sima->imanr) {
				break;
			}
			nr++;
			idtest= idtest->next;
		}
		if(idtest==0) { /* no new */
			return;
		}
	
		if(idtest!=id) {
			G.sima->image= (Image *)idtest;
			if(idtest->us==0) idtest->us= 1;
			allqueue(REDRAWIMAGE, 0);
		}
		/* also when image is the same: assign! 0==no tileflag: */
		image_changed(G.sima, 0);
		BIF_undo_push("Assign image UV");

		break;
		
	case B_SIMAGEDRAW:
		if(G.f & G_FACESELECT) {
			make_repbind(G.sima->image);
			image_changed(G.sima, 1);
		}
		allqueue(REDRAWVIEW3D, 0);
		allqueue(REDRAWIMAGE, 0);
		break;

	case B_SIMAGEDRAW1:
		image_changed(G.sima, 2);		/* 2: only tileflag */
		allqueue(REDRAWVIEW3D, 0);
		allqueue(REDRAWIMAGE, 0);
		break;
		
	case B_SIMAGEPAINTTOOL:
		if(G.sima->flag & SI_DRAWTOOL)
			/* add new brush if none exists */
			brush_check_exists(&G.scene->toolsettings->imapaint.brush);
		allqueue(REDRAWIMAGE, 0);
		allqueue(REDRAWVIEW3D, 0);
		break;

	case B_SIMAPACKIMA:
		pack_image_sima();
		break;
		
	case B_SIMA_USE_ALPHA:
		G.sima->flag &= ~(SI_SHOW_ALPHA|SI_SHOW_ZBUF);
		scrarea_queue_winredraw(curarea);
		scrarea_queue_headredraw(curarea);
		break;
	case B_SIMA_SHOW_ALPHA:
		G.sima->flag &= ~(SI_USE_ALPHA|SI_SHOW_ZBUF);
		scrarea_queue_winredraw(curarea);
		scrarea_queue_headredraw(curarea);
		break;
	case B_SIMA_SHOW_ZBUF:
		G.sima->flag &= ~(SI_SHOW_ALPHA|SI_USE_ALPHA);
		scrarea_queue_winredraw(curarea);
		scrarea_queue_headredraw(curarea);
		break;
	}
}

static void do_image_view_viewnavmenu(void *arg, int event)
{
	switch(event) {
	case 1: /* Zoom In */
		image_viewzoom(PADPLUSKEY, 0);
		break;
	case 2: /* Zoom Out */
		image_viewzoom(PADMINUS, 0);
		break;
	case 3: /* Zoom 8:1 */
		image_viewzoom(PAD8, 0);
		break;
	case 4: /* Zoom 4:1 */
		image_viewzoom(PAD4, 0);
		break;
	case 5: /* Zoom 2:1 */
		image_viewzoom(PAD2, 0);
		break;
	case 6: /* Zoom 1:1 */
		image_viewzoom(PAD1, 0);
		break;
	case 7: /* Zoom 1:2 */
		image_viewzoom(PAD2, 1);
		break;
	case 8: /* Zoom 1:4 */
		image_viewzoom(PAD4, 1);
		break;
	case 9: /* Zoom 1:8 */
		image_viewzoom(PAD8, 1);
		break;
	}
	allqueue(REDRAWIMAGE, 0);
}

static uiBlock *image_view_viewnavmenu(void *arg_unused)
{
/*		static short tog=0; */
	uiBlock *block;
	short yco= 0, menuwidth=120;
	
	block= uiNewBlock(&curarea->uiblocks, "image_view_viewnavmenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_view_viewnavmenu, NULL);
	
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom In|NumPad +", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom Out|NumPad -",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");

	uiDefBut(block, SEPR, 0, "",        0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 1:8|Shift+NumPad 8", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 3, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 1:4|Shift+NumPad 4", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 4, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 1:2|Shift+NumPad 2", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 1:1|NumPad 1", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 6, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 2:1|NumPad 2", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 4:1|NumPad 4", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 8, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Zoom 8:1|NumPad 8", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 9, "");

	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 50);
	return block;
}

static void do_image_viewmenu(void *arg, int event)
{

	switch(event) {
	case 0: /* Update Automatically */
		if(G.sima->lock) G.sima->lock = 0;
		else G.sima->lock = 1;
		break;
	case 1: /* View All */
		do_image_buttons(B_SIMAGEHOME);
		break;
	case 2: /* Maximize Window */
		/* using event B_FULL */
		break;
	case 5: /* Draw Shadow Mesh */
		if(G.sima->flag & SI_DRAWSHADOW)
			G.sima->flag &= ~SI_DRAWSHADOW;
		else
			G.sima->flag |= SI_DRAWSHADOW;
		allqueue(REDRAWIMAGE, 0);
		break;
	case 6: /* Draw Faces */
		if(G.f & G_DRAWFACES)
			G.f &= ~G_DRAWFACES;
		else
			G.f |= G_DRAWFACES;
		allqueue(REDRAWIMAGE, 0);
		break;
	case 7: /* Properties  Panel */
		add_blockhandler(curarea, IMAGE_HANDLER_PROPERTIES, UI_PNL_UNSTOW);
		break;
	case 8: /* Paint Panel... */
		add_blockhandler(curarea, IMAGE_HANDLER_PAINT, UI_PNL_UNSTOW);
		break;
	case 9:
		image_viewcentre();
	case 10: /* Display Normalized Coordinates */
		G.sima->flag ^= SI_COORDFLOATS;
		allqueue(REDRAWIMAGE, 0);
		break;
	case 11: /* Curves Panel... */
		add_blockhandler(curarea, IMAGE_HANDLER_CURVES, UI_PNL_UNSTOW);
		break;
	case 12: /* composite preview */
		toggle_blockhandler(curarea, IMAGE_HANDLER_PREVIEW, 0);
		scrarea_queue_winredraw(curarea);

	}
	allqueue(REDRAWVIEW3D, 0);
}

static uiBlock *image_viewmenu(void *arg_unused)
{
/*	static short tog=0; */
	uiBlock *block;
	short yco= 0, menuwidth=120;
	
	block= uiNewBlock(&curarea->uiblocks, "image_viewmenu", UI_EMBOSSP, UI_HELV, curarea->headwin);
	uiBlockSetButmFunc(block, do_image_viewmenu, NULL);

	uiDefIconTextBut(block, BUTM, 1, ICON_MENU_PANEL, "Properties...",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_MENU_PANEL, "Paint Tool...|C",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 8, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_MENU_PANEL, "Curves Tool...",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 11, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_MENU_PANEL, "Composite Preview...|Shift P",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 12, "");

	if(G.sima->flag & SI_COORDFLOATS) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Display Normalized Coordinates|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 10, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Display Normalized Coordinates|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 10, "");
	if(G.f & G_DRAWFACES) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Draw Faces", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 6, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Draw Faces|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 6, "");
	if(G.sima->flag & SI_DRAWSHADOW) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Draw Shadow Mesh", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Draw Shadow Mesh|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");

	uiDefBut(block, SEPR, 0, "",        0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");
	uiDefIconTextBlockBut(block, image_view_viewnavmenu, NULL, ICON_RIGHTARROW_THIN, "View Navigation", 0, yco-=20, 120, 19, "");

	if(G.sima->lock) {
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Update Automatically|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 0, "");
	} else {
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Update Automatically|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 0, "");
	} 

	uiDefBut(block, SEPR, 0, "",					0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");
	
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "View Selected|NumPad .",			0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 9, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "View All|Home", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 1, "");
		
	if(!curarea->full) uiDefIconTextBut(block, BUTM, B_FULL, ICON_BLANK1, "Maximize Window|Ctrl UpArrow", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");
	else uiDefIconTextBut(block, BUTM, B_FULL, ICON_BLANK1, "Tile Window|Ctrl DownArrow", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");
	
	if(curarea->headertype==HEADERTOP) {
		uiBlockSetDirection(block, UI_DOWN);
	}
	else {
		uiBlockSetDirection(block, UI_TOP);
		uiBlockFlipOrder(block);
	}

	uiTextBoundsBlock(block, 50);
	
	return block;
}

static void do_image_selectmenu(void *arg, int event)
{
	switch(event)
	{
	case 0: /* Border Select */
		borderselect_sima(UV_SELECT_ALL);
		break;
	case 8: /* Border Select Pinned */
		borderselect_sima(UV_SELECT_PINNED);
		break;
	case 1: /* Select/Deselect All */
		select_swap_tface_uv();
		break;
	case 2: /* Unlink Selection */
		unlink_selection();
		break;
	case 3: /* Linked UVs */
		select_linked_tface_uv(2);
		break;
	case 4: /* Toggle Local UVs Stick to Vertex in Mesh */
		if(G.sima->flag & SI_LOCALSTICKY)
			G.sima->flag &= ~SI_LOCALSTICKY;
		else {
			G.sima->flag |= SI_LOCALSTICKY;
			G.sima->flag &= ~SI_STICKYUVS;
		}
		allqueue(REDRAWIMAGE, 0);
		break;  
	case 5: /* Toggle UVs Stick to Vertex in Mesh */
		if(G.sima->flag & SI_STICKYUVS) {
			G.sima->flag &= ~SI_STICKYUVS;
			G.sima->flag |= SI_LOCALSTICKY;
		}
		else {
			G.sima->flag |= SI_STICKYUVS;
			G.sima->flag &= ~SI_LOCALSTICKY;
		}
		allqueue(REDRAWIMAGE, 0);
		break;  
	case 6: /* Toggle Active Face Select */
		if(G.sima->flag & SI_SELACTFACE)
			G.sima->flag &= ~SI_SELACTFACE;
		else
			G.sima->flag |= SI_SELACTFACE;
		allqueue(REDRAWIMAGE, 0);
		break;
	case 7: /* Pinned UVs */
		select_pinned_tface_uv();
		break;
	}
}

static uiBlock *image_selectmenu(void *arg_unused)
{
	uiBlock *block;
	short yco= 0, menuwidth=120;

	block= uiNewBlock(&curarea->uiblocks, "image_selectmenu", UI_EMBOSSP, UI_HELV, curarea->headwin);
	uiBlockSetButmFunc(block, do_image_selectmenu, NULL);

	if(G.sima->flag & SI_SELACTFACE) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Active Face Select|C", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 6, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Active Face Select|C", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 6, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

	if(G.sima->flag & SI_LOCALSTICKY) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Stick Local UVs to Mesh Vertex|Shift C", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 4, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Stick Local UVs to Mesh Vertex|Shift C", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 4, "");

	if(G.sima->flag & SI_STICKYUVS) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Stick UVs to Mesh Vertex|Ctrl C", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Stick UVs to Mesh Vertex|Ctrl C", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Border Select|B", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 0, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Border Select Pinned|Shift B", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 8, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Select/Deselect All|A", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Unlink Selection|Alt L", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");
	
	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Pinned UVs|Shift P", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Linked UVs|Ctrl L", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 3, "");

	if(curarea->headertype==HEADERTOP) {
		uiBlockSetDirection(block, UI_DOWN);
	}
	else {
		uiBlockSetDirection(block, UI_TOP);
		uiBlockFlipOrder(block);
	}

	uiTextBoundsBlock(block, 50);

	return block;
}

static void do_image_image_rtmappingmenu(void *arg, int event)
{
	switch(event) {
	case 0: /* UV Co-ordinates */
		G.sima->image->flag &= ~IMA_REFLECT;
		break;
	case 1: /* Reflection */
		G.sima->image->flag |= IMA_REFLECT;
		break;
	}

 	allqueue(REDRAWVIEW3D, 0);
}

static uiBlock *image_image_rtmappingmenu(void *arg_unused)
{
	uiBlock *block;
	short yco = 20, menuwidth = 120;

	block= uiNewBlock(&curarea->uiblocks, "image_image_rtmappingmenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_image_rtmappingmenu, NULL);
	
	if (G.sima->image->flag & IMA_REFLECT) {
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "UV Co-ordinates",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 0, "");
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Reflection",		0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 1, "");
	} else {
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "UV Co-ordinates",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 0, "");
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Reflection",		0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 1, "");
	}

	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	return block;
}

static void do_image_imagemenu(void *arg, int event)
{
	/* events >=20 are registered bpython scripts */
	if (event >= 20) BPY_menu_do_python(PYMENU_IMAGE, event - 20);
	
	switch(event)
	{
	case 0:
		open_image_sima((G.qual==LR_CTRLKEY));
		break;
	case 1:
		replace_image_sima((G.qual==LR_CTRLKEY));
		break;
	case 2:
		pack_image_sima();
		break;
	case 4: /* Texture Painting */
		if(G.sima->flag & SI_DRAWTOOL) G.sima->flag &= ~SI_DRAWTOOL;
		else G.sima->flag |= SI_DRAWTOOL;
		break;
	case 5:
		save_as_image_sima();
		break;
	case 6:
		reload_image_sima();
		break;
	case 7:
		new_image_sima();
		break;
	case 8:
		save_image_sima();
		break;
	}
}

static uiBlock *image_imagemenu(void *arg_unused)
{
	uiBlock *block;
	short yco= 0, menuwidth=150;
	BPyMenu *pym;
	int i = 0;

	block= uiNewBlock(&curarea->uiblocks, "image_imagemenu", UI_EMBOSSP, UI_HELV, curarea->headwin);
	uiBlockSetButmFunc(block, do_image_imagemenu, NULL);

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "New...|Alt N", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Open...|Alt O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 0, "");
	
	if (G.sima->image) {
		uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Replace...", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");
		uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Reload|Alt R", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 6, "");
		uiDefBut(block, SEPR, 0, "",        0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

		uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Save|Alt S", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");
		uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Save As...", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 5, "");
		uiDefBut(block, SEPR, 0, "",        0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	
		
		if (G.sima->image->packedfile) {
			uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Unpack Image...", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");
		} else {
			uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Pack Image", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");
		}
			
		uiDefBut(block, SEPR, 0, "",        0, yco-=7, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");

		if(G.sima->flag & SI_DRAWTOOL) {
			uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Texture Painting", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 4, "");
		} else {
			uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Texture Painting", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 4, "");
		}
		
		uiDefBut(block, SEPR, 0, "",        0, yco-=7, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");
		
		uiDefIconTextBlockBut(block, image_image_rtmappingmenu, NULL, ICON_RIGHTARROW_THIN, "Realtime Texture Mapping", 0, yco-=20, 120, 19, "");
		// uiDefIconTextBut(block, BUTM, 1, ICON_MENU_PANEL, "Realtime Texture Animation|",		0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");
	}
	
	/* note that we acount for the N previous entries with i+20: */
	for (pym = BPyMenuTable[PYMENU_IMAGE]; pym; pym = pym->next, i++) {

		uiDefIconTextBut(block, BUTM, 1, ICON_PYTHON, pym->name, 0, yco-=20, menuwidth, 19, 
				 NULL, 0.0, 0.0, 1, i+20, 
				 pym->tooltip?pym->tooltip:pym->filename);
	}
	
	if(curarea->headertype==HEADERTOP) {
		uiBlockSetDirection(block, UI_DOWN);
	}
	else {
		uiBlockSetDirection(block, UI_TOP);
		uiBlockFlipOrder(block);
	}

	uiTextBoundsBlock(block, 80);

	return block;
}

static void do_image_uvs_showhidemenu(void *arg, int event)
{
	switch(event) {
	case 4: /* show hidden faces */
		reveal_tface_uv();
		break;
	case 5: /* hide selected faces */
		hide_tface_uv(0);
		break;
	case 6: /* hide deselected faces */
		hide_tface_uv(1);
		break;
	}
	allqueue(REDRAWVIEW3D, 0);
}

static uiBlock *image_uvs_showhidemenu(void *arg_unused)
{
	uiBlock *block;
	short yco = 20, menuwidth = 120;

	block= uiNewBlock(&curarea->uiblocks, "image_uvs_showhidemenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_uvs_showhidemenu, NULL);
	
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Show Hidden Faces|Alt H",		0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 4, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Hide Selected Faces|H",		0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 5, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Hide Deselected Faces|Shift H",		0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 6, "");

	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	return block;
}

static void do_image_uvs_propfalloffmenu(void *arg, int event)
{
	G.scene->prop_mode= event;
	allqueue(REDRAWVIEW3D, 1);
}

static uiBlock *image_uvs_propfalloffmenu(void *arg_unused)
{
	uiBlock *block;
	short yco = 20, menuwidth = 120;

	block= uiNewBlock(&curarea->uiblocks, "image_uvs_propfalloffmenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_uvs_propfalloffmenu, NULL);
	
	if (G.scene->prop_mode==PROP_SMOOTH) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Smooth|Shift O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_SMOOTH, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Smooth|Shift O",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_SMOOTH, "");
	if (G.scene->prop_mode==PROP_SPHERE) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Sphere|Shift O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_SPHERE, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Sphere|Shift O",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_SPHERE, "");
	if (G.scene->prop_mode==PROP_ROOT) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Root|Shift O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_ROOT, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Root|Shift O",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_ROOT, "");
	if (G.scene->prop_mode==PROP_SHARP) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Sharp|Shift O",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_SHARP, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Sharp|Shift O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_SHARP, "");
	if (G.scene->prop_mode==PROP_LIN) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Linear|Shift O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_LIN, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Linear|Shift O",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_LIN, "");
	if (G.scene->prop_mode==PROP_CONST) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Constant|Shift O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_CONST, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Constant|Shift O",	0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, PROP_CONST, "");
		
	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	return block;
}

static void do_image_uvs_transformmenu(void *arg, int event)
{
	switch(event) {
	case 0: /* Grab */
		initTransform(TFM_TRANSLATION, CTX_NONE);
		Transform();
		break;
	case 1: /* Rotate */
		initTransform(TFM_ROTATION, CTX_NONE);
		Transform();
		break;
	case 2: /* Scale */
		initTransform(TFM_RESIZE, CTX_NONE);
		Transform();
		break;
	}
}

static uiBlock *image_uvs_transformmenu(void *arg_unused)
{
	uiBlock *block;
	short yco = 20, menuwidth = 120;

	block= uiNewBlock(&curarea->uiblocks, "image_uvs_transformmenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_uvs_transformmenu, NULL);
	
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Grab/Move|G", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 0, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Rotate|R", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Scale|S", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");

	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	return block;
}

static void do_image_uvs_mirrormenu(void *arg, int event)
{
	switch(event) {
	case 0: /* X axis */
		mirror_tface_uv('x');
		break;
	case 1: /* Y axis */
		mirror_tface_uv('y');
		break;
	}
	
	BIF_undo_push("Mirror UV");
}

static uiBlock *image_uvs_mirrormenu(void *arg_unused)
{
	uiBlock *block;
	short yco = 20, menuwidth = 120;

	block= uiNewBlock(&curarea->uiblocks, "image_uvs_mirrormenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_uvs_mirrormenu, NULL);
	
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "X Axis|M, 1", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 0, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Y Axis|M, 2", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");

	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	return block;
}

static void do_image_uvs_weldalignmenu(void *arg, int event)
{
	switch(event) {
	case 0: /* Weld */
		weld_align_tface_uv('w');
		break;
	case 1: /* Align X */
		weld_align_tface_uv('x');
		break;
	case 2: /* Align Y */
		weld_align_tface_uv('y');
		break;
	}
	
	if(event==0) BIF_undo_push("Weld UV");
	else if(event==1 || event==2) BIF_undo_push("Align UV");
}

static uiBlock *image_uvs_weldalignmenu(void *arg_unused)
{
	uiBlock *block;
	short yco = 20, menuwidth = 120;

	block= uiNewBlock(&curarea->uiblocks, "image_uvs_weldalignmenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_uvs_weldalignmenu, NULL);
	
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Weld|W, 1", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 0, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Align X|W, 2", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Align Y|W, 3", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");

	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	return block;
}

static void do_image_uvs_scriptsmenu(void *arg, int event)
{
	BPY_menu_do_python(PYMENU_UV, event);

	allqueue(REDRAWIMAGE, 0);
}

static uiBlock *image_uvs_scriptsmenu (void *args_unused)
{
	uiBlock *block;
	BPyMenu *pym;
	int i= 0;
	short yco = 20, menuwidth = 120;
	
	block= uiNewBlock(&curarea->uiblocks, "image_uvs_scriptsmenu", UI_EMBOSSP, UI_HELV, G.curscreen->mainwin);
	uiBlockSetButmFunc(block, do_image_uvs_scriptsmenu, NULL);
	
	/* note that we acount for the N previous entries with i+20: */
	for (pym = BPyMenuTable[PYMENU_UV]; pym; pym = pym->next, i++) {
		
		uiDefIconTextBut(block, BUTM, 1, ICON_PYTHON, pym->name, 0, yco-=20, menuwidth, 19, 
						 NULL, 0.0, 0.0, 1, i, 
						 pym->tooltip?pym->tooltip:pym->filename);
	}
	
	uiBlockSetDirection(block, UI_RIGHT);
	uiTextBoundsBlock(block, 60);
	
	return block;
}

static void do_image_uvsmenu(void *arg, int event)
{

	switch(event) {
	case 1: /* UVs Constrained Rectangular */
		if(G.sima->flag & SI_BE_SQUARE) G.sima->flag &= ~SI_BE_SQUARE;
		else G.sima->flag |= SI_BE_SQUARE;
		break;
	case 2: /* UVs Clipped to Image Size */
		if(G.sima->flag & SI_CLIP_UV) G.sima->flag &= ~SI_CLIP_UV;
		else G.sima->flag |= SI_CLIP_UV;
		break;
	case 3: /* Limit Stitch UVs */
		stitch_uv_tface(1);
		break;
	case 4: /* Stitch UVs */
		stitch_uv_tface(0);
		break;
	case 5: /* Proportional Edit (toggle) */
		if(G.scene->proportional)
			G.scene->proportional= 0;
		else
			G.scene->proportional= 1;
		break;
	case 7: /* UVs Snap to Pixel */
		G.sima->flag ^= SI_PIXELSNAP;
		break;
    case 8:
		pin_tface_uv(1);
		break;
    case 9:
		pin_tface_uv(0);
		break;
    case 10:
		unwrap_lscm(0);
		break;
	case 11:
		if(G.sima->flag & SI_LIVE_UNWRAP) G.sima->flag &= ~SI_LIVE_UNWRAP;
		else G.sima->flag |= SI_LIVE_UNWRAP;
		break;
	case 12:
		minimize_stretch_tface_uv();
		break;
	}
}

static uiBlock *image_uvsmenu(void *arg_unused)
{
	uiBlock *block;
	short yco= 0, menuwidth=120;

	block= uiNewBlock(&curarea->uiblocks, "image_uvsmenu", UI_EMBOSSP, UI_HELV, curarea->headwin);
	uiBlockSetButmFunc(block, do_image_uvsmenu, NULL);

	// uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Transform Properties...|N", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 0, "");
	// uiDefBut(block, SEPR, 0, "",        0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");
	if(G.sima->flag & SI_PIXELSNAP) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Snap to Pixels|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Snap to Pixels|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 7, "");

	if(G.sima->flag & SI_BE_SQUARE) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Quads Constrained Rectangular|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Quads Constrained Rectangular|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 1, "");
	
	if(G.sima->flag & SI_CLIP_UV) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Layout Clipped to Image Size|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Layout Clipped to Image Size|", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 2, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

	if(G.sima->flag & SI_LIVE_UNWRAP) uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Live Unwrap Transform", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 11, "");
	else uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Live Unwrap Transform", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 11, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Unwrap|E", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 10, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Unpin|Alt P", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 9, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Pin|P", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 8, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Minimize Stretch|Ctrl V", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 12, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Limit Stitch...|Shift V", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 3, "");
	uiDefIconTextBut(block, BUTM, 1, ICON_BLANK1, "Stitch|V", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 0, 4, "");
	uiDefIconTextBlockBut(block, image_uvs_transformmenu, NULL, ICON_RIGHTARROW_THIN, "Transform", 0, yco-=20, 120, 19, "");

	uiDefBut(block, SEPR, 0, "",				0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");
	
	uiDefIconTextBlockBut(block, image_uvs_mirrormenu, NULL, ICON_RIGHTARROW_THIN, "Mirror", 0, yco-=20, 120, 19, "");
	uiDefIconTextBlockBut(block, image_uvs_weldalignmenu, NULL, ICON_RIGHTARROW_THIN, "Weld/Align", 0, yco-=20, 120, 19, "");
	
	uiDefBut(block, SEPR, 0, "",				0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");

	if(G.scene->proportional)
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_HLT, "Proportional Editing|O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 5, "");
	else
		uiDefIconTextBut(block, BUTM, 1, ICON_CHECKBOX_DEHLT, "Proportional Editing|O", 0, yco-=20, menuwidth, 19, NULL, 0.0, 0.0, 1, 5, "");

	uiDefIconTextBlockBut(block, image_uvs_propfalloffmenu, NULL, ICON_RIGHTARROW_THIN, "Proportional Falloff", 0, yco-=20, 120, 19, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	

	uiDefIconTextBlockBut(block, image_uvs_showhidemenu, NULL, ICON_RIGHTARROW_THIN, "Show/Hide Faces", 0, yco-=20, menuwidth, 19, "");

	uiDefBut(block, SEPR, 0, "", 0, yco-=6, menuwidth, 6, NULL, 0.0, 0.0, 0, 0, "");	
	
	uiDefIconTextBlockBut(block, image_uvs_scriptsmenu, NULL, ICON_RIGHTARROW_THIN, "Scripts", 0, yco-=20, 120, 19, "");
	
	if(curarea->headertype==HEADERTOP) {
		uiBlockSetDirection(block, UI_DOWN);
	}
	else {
		uiBlockSetDirection(block, UI_TOP);
		uiBlockFlipOrder(block);
	}

	uiTextBoundsBlock(block, 50);

	return block;
}

void image_buttons(void)
{
	uiBlock *block;
	short xco, xmax;
	char naam[256], *menuname;
	/* This should not be a static var */
	static int headerbuttons_packdummy;
	Image *ima= G.sima->image;

	headerbuttons_packdummy = 0;
		
	sprintf(naam, "header %d", curarea->headwin);
	block= uiNewBlock(&curarea->uiblocks, naam, UI_EMBOSS, UI_HELV, curarea->headwin);

	if(area_is_active_area(curarea)) uiBlockSetCol(block, TH_HEADER);
	else uiBlockSetCol(block, TH_HEADERDESEL);

	what_image(G.sima);

	curarea->butspacetype= SPACE_IMAGE;

	xco = 8;
	uiDefIconTextButC(block, ICONTEXTROW,B_NEWSPACE, ICON_VIEW3D, windowtype_pup(), xco,0,XIC+10,YIC, &(curarea->butspacetype), 1.0, SPACEICONMAX, 0, 0, "Current Window Type. Click for menu of available types.");
	xco+= XIC+14;

	uiBlockSetEmboss(block, UI_EMBOSSN);
	if(curarea->flag & HEADER_NO_PULLDOWN) {
		uiDefIconButBitS(block, TOG, HEADER_NO_PULLDOWN, B_FLIPINFOMENU, ICON_DISCLOSURE_TRI_RIGHT,
				xco,2,XIC,YIC-2,
				&(curarea->flag), 0, 0, 0, 0, "Show pulldown menus");
	} else {
		uiDefIconButBitS(block, TOG, HEADER_NO_PULLDOWN, B_FLIPINFOMENU, ICON_DISCLOSURE_TRI_DOWN,
				xco,2,XIC,YIC-2,
				&(curarea->flag), 0, 0, 0, 0, "Hide pulldown menus");
	}
	uiBlockSetEmboss(block, UI_EMBOSS);
	xco+=XIC;

	if((curarea->flag & HEADER_NO_PULLDOWN)==0) {
		/* pull down menus */
		uiBlockSetEmboss(block, UI_EMBOSSP);
	
		xmax= GetButStringLength("View");
		uiDefPulldownBut(block, image_viewmenu, NULL, "View", xco, -2, xmax-3, 24, "");
		xco+= xmax;
		
		if((G.f & G_FACESELECT) && !(ima && (G.sima->flag & SI_DRAWTOOL))) {
			xmax= GetButStringLength("Select");
			uiDefPulldownBut(block, image_selectmenu, NULL, "Select", xco, -2, xmax-3, 24, "");
			xco+= xmax;
		}
		
		if (ima && ima->ibuf && (ima->ibuf->userflags & IB_BITMAPDIRTY))
			menuname= "Image*";
		else
			menuname= "Image";
		xmax= GetButStringLength(menuname);
		uiDefPulldownBut(block, image_imagemenu, NULL, menuname, xco, -2, xmax-3, 24, "");
		xco+= xmax;
		
		if((G.f & G_FACESELECT) && !(ima && (G.sima->flag & SI_DRAWTOOL))) {
			xmax= GetButStringLength("UVs");
			uiDefPulldownBut(block, image_uvsmenu, NULL, "UVs", xco, -2, xmax-3, 24, "");
			xco+= xmax;
		}
	}
	
	/* other buttons: */
	uiBlockSetEmboss(block, UI_EMBOSS);

	xco= std_libbuttons(block, xco, 0, 0, NULL, B_SIMABROWSE, ID_IM, 0, (ID *)ima, 0, &(G.sima->imanr), 0, 0, B_IMAGEDELETE, 0, 0);

	if (ima) {
		xco+= 8;
	
		if (ima->packedfile) {
			headerbuttons_packdummy = 1;
		}
		uiDefIconButBitI(block, TOG, 1, B_SIMAPACKIMA, ICON_PACKAGE,	xco,0,XIC,YIC, &headerbuttons_packdummy, 0, 0, 0, 0, "Pack/Unpack this image");
		xco+= XIC+8;

		uiDefIconButBitI(block, TOG, SI_DRAWTOOL, B_SIMAGEPAINTTOOL, ICON_TPAINT_HLT, xco,0,XIC,YIC, &G.sima->flag, 0, 0, 0, 0, "Enables painting textures on the image with left mouse button");
		xco+= XIC+8;

		uiBlockBeginAlign(block);
		uiDefIconButBitI(block, TOG, SI_USE_ALPHA, B_SIMA_USE_ALPHA, ICON_TRANSP_HLT, xco,0,XIC,YIC, &G.sima->flag, 0, 0, 0, 0, "Draws image with alpha");
		xco+= XIC;
		uiDefIconButBitI(block, TOG, SI_SHOW_ALPHA, B_SIMA_SHOW_ALPHA, ICON_DOT, xco,0,XIC,YIC, &G.sima->flag, 0, 0, 0, 0, "Draws only alpha");
		xco+= XIC;
		if(ima->ibuf) {
			if(ima->ibuf->zbuf || ima->ibuf->zbuf_float) {
				uiDefIconButBitI(block, TOG, SI_SHOW_ZBUF, B_SIMA_SHOW_ZBUF, ICON_SOLID, xco,0,XIC,YIC, &G.sima->flag, 0, 0, 0, 0, "Draws zbuffer values");
				xco+= XIC;
			}
			else G.sima->flag &= ~SI_SHOW_ZBUF;	/* no confusing display for non-zbuf images */
		}		
		uiBlockEndAlign(block);
		xco+= 8;
	}

	/* draw LOCK */
	uiDefIconButS(block, ICONTOG, 0, ICON_UNLOCKED,	xco,0,XIC,YIC, &(G.sima->lock), 0, 0, 0, 0, "Updates other affected window spaces automatically to reflect changes in real time");

	/* Always do this last */
	curarea->headbutlen= xco+2*XIC;
	
	uiDrawBlock(block);
}

