/*  DNA_action_types.h   May 2001
 *  
 *  support for the "action" datatype
 *
 *	Reevan McKay
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


#ifndef DNA_ACTION_TYPES_H
#define DNA_ACTION_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"
#include "DNA_view2d_types.h"

struct SpaceLink;

typedef struct bPoseChannel{
	struct bPoseChannel	*next, *prev;
	ListBase			constraints;
	int					flag;
	float loc[3];
	float size[3];
	float quat[4];
	float obmat[4][4];
	char				name[32];	/* Channels need longer names than normal blender objects */
	int					reserved1;
} bPoseChannel;


typedef struct bPose{
	ListBase			chanbase;
} bPose;

typedef struct bActionChannel {
	struct bActionChannel	*next, *prev;
	struct Ipo				*ipo;
	ListBase				constraintChannels;
	int		flag;
	char	name[32];		/* Channel name */
	int		reserved1;

} bActionChannel;

typedef struct bAction {
	ID				id;
	ListBase		chanbase;	/* Channels in this action */
	bActionChannel	*achan;		/* Current action channel */
	bPoseChannel	*pchan;		/* Current pose channel */
} bAction;

typedef struct SpaceAction {
	struct SpaceLink *next, *prev;
	int spacetype, pad;
	struct ScrArea *area;

	View2D v2d;	
	bAction		*action;
	int	flag;
	short pin, reserved1;
	short	actnr;
	short	lock;
	int pad2;
} SpaceAction;

/* Action Channel flags */
#define	ACHAN_SELECTED	0x00000001
#define ACHAN_HILIGHTED	0x00000002


#endif

