/**
 * blenlib/BKE_main.h (mar-2001 nzc)
 *
 * Main is the root of the 'database' of a Blender context. All data
 * is stuffed into lists, and all these lists are knotted to here. A
 * Blender file is not much more but a binary dump of these
 * lists. This list of lists is not serialized itself.
 *
 * Oops... this should be a _types.h file.
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
#ifndef BKE_MAIN_H
#define BKE_MAIN_H

#include "DNA_listBase.h"

struct Library;

typedef struct Main {
	struct Main *next, *prev;
	char name[160];
	short versionfile, rt;
	struct Library *curlib;
	ListBase scene;
	ListBase library;
	ListBase object;
	ListBase mesh;
	ListBase curve;
	ListBase mball;
	ListBase mat;
	ListBase tex;
	ListBase image;
	ListBase ika;
	ListBase wave;
	ListBase latt;
	ListBase lamp;
	ListBase camera;
	ListBase ipo;
	ListBase key;
	ListBase world;
	ListBase screen;
	ListBase vfont;
	ListBase text;
	ListBase sound;
	ListBase group;
	ListBase armature;	/* NLA */
	ListBase action;	/* NLA */
} Main;


#endif
