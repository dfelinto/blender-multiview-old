/**
 * blenlib/DNA_world_types.h (mar-2001 nzc)
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
	
	/**
	 * Exposure= mult factor. unused now, but maybe back later. Kept in to be upward compat.
	 * New is exp/range control. linfac & logfac are constants... don't belong in
	 * file, but allocating 8 bytes for temp mem isnt useful either.
	 */
	float exposure, exp, range;	
	float linfac, logfac;

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
	
	/* unused now: DOF */
	short dofsta, dofend, dofmin, dofmax;
	
	/* ambient occlusion */
	float aodist, aodistfac, aoenergy, pad;
	short aomode, aosamp, aomix, aocolor;
	
	int physicsEngine;

	struct Ipo *ipo;
	struct MTex *mtex[8];

	ScriptLink scriptlink;

} World;

/* **************** WORLD ********************* */

/* skytype */
#define WO_SKYBLEND		1
#define WO_SKYREAL		2
#define WO_SKYPAPER		4
/* while render: */
#define WO_SKYTEX		8
#define WO_ZENUP		16

/* mode */
#define WO_MIST	               1
#define WO_STARS               2
#define WO_DOF                 4
#define WO_ACTIVITY_CULLING	   8
#define WO_AMB_OCC	   		  16

/* aomix */
#define WO_AOADD	0
#define WO_AOSUB	1
#define WO_AOADDSUB	2

/* aomode (use distances & random sampling modes) */
#define WO_AODIST		1
#define WO_AORNDSMP		2

/* aocolor */
#define WO_AOPLAIN	0
#define WO_AOSKYCOL	1
#define WO_AOSKYTEX	2

/* texco (also in DNA_material_types.h) */
#define TEXCO_ANGMAP	64

/* mapto */
#define WOMAP_BLEND		1
#define WOMAP_HORIZ		2
#define WOMAP_ZENUP		4
#define WOMAP_ZENDOWN	8

/* physicsEngine */
#define WOPHY_NONE		0
#define WOPHY_ENJI		1
#define WOPHY_SUMO		2
#define WOPHY_DYNAMO		3
#define WOPHY_ODE		4

#endif

