/**
 * blenlib/DNA_world_types.h (mar-2001 nzc)
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
#ifndef DNA_WORLD_TYPES_H
#define DNA_WORLD_TYPES_H

#include "DNA_ID.h"
#include "DNA_scriptlink_types.h"

struct Ipo;
struct MTex;


/**
 * World defines general modelling data such as a background fill,
 * gravity, colour model, stars, etc. It mixes game-data, rendering
 * data and modelling data. */
typedef struct World {
	ID id;
	
	short colormodel, totex;
	short texact, mistype;
	
	float horr, horg, horb, hork;
	float zenr, zeng, zenb, zenk;
	float ambr, ambg, ambb, ambk;

	unsigned int fastcol;	

	float exposure;

	/**
	 * Gravitation constant for the game world
	 */
	float gravity;

	/**
	 * Radius of the activity bubble, in Manhattan length. Objects
	 * outside the box are activity-culled. */
	float activityBoxRadius;
	
	short skytype;
	/**
	 * Some world modes
	 * bit 0: Do mist
	 * bit 1: Do stars
	 * bit 2: (reserved) depth of field
	 * bit 3: (gameengine): Activity culling is enabled.
	 */
	short mode;
	
	float misi, miststa, mistdist, misthi;
	
	float starr, starg, starb, stark;
	float starsize, starmindist;
	float stardist, starcolnoise;
	
	short dofsta, dofend, dofmin, dofmax;

	int pad1;
	struct Ipo *ipo;
	struct MTex *mtex[8];

	ScriptLink scriptlink;
} World;

/* **************** WORLD ********************* */

/* skytype */
#define WO_SKYBLEND		1
#define WO_SKYREAL		2
#define WO_SKYPAPER		4
/* tijdens render: */
#define WO_SKYTEX		8
#define WO_ZENUP		16

/* mode */
#define WO_MIST	               1
#define WO_STARS               2
#define WO_DOF                 4
#define WO_ACTIVITY_CULLING	   8

/* mapto */
#define WOMAP_BLEND		1
#define WOMAP_HORIZ		2
#define WOMAP_ZENUP		4
#define WOMAP_ZENDOWN	8

#endif
