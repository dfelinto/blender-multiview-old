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

#include "BKE_nla.h"
#include "BKE_blender.h"

#include "BLI_blenlib.h"

#include "DNA_space_types.h"
#include "DNA_nla_types.h"
#include "DNA_action_types.h"
#include "DNA_ID.h"
#include "DNA_ipo_types.h"

#include "MEM_guardedalloc.h"

void copy_actionstrip (bActionStrip **dst, bActionStrip **src){
	bActionStrip *dstrip;
	bActionStrip *sstrip = *src;

	if (!*src){
		*dst=NULL;
		return;
	}

	*dst = MEM_dupallocN(sstrip);

	dstrip = *dst;
	if (dstrip->act)
		dstrip->act->id.us++;

	if (dstrip->ipo)
		dstrip->ipo->id.us++;
}

void copy_nlastrips (ListBase *dst, ListBase *src)
{
	bActionStrip *strip;

	dst->first=dst->last=NULL;

	duplicatelist (dst, src);

	/* Update specific data */
	if (!dst->first)
		return;

	for (strip = dst->first; strip; strip=strip->next){
		if (strip->act)
			strip->act->id.us++;
		if (strip->ipo)
			strip->ipo->id.us++;
	}
}


void free_actionstrip(bActionStrip* strip)
{
	if (!strip)
		return;

	if (strip->act){
		strip->act->id.us--;
		strip->act = NULL;
	}
	if (strip->ipo){
		strip->ipo->id.us--;
		strip->ipo = NULL;
	}
}

void free_nlastrips (ListBase *nlalist)
{
	bActionStrip *strip;

	if (!nlalist->first)
		return;

	/* Do any specific freeing */
	for (strip=nlalist->first; strip; strip=strip->next)
	{
		free_actionstrip (strip);
	};

	/* Free the whole list */
	BLI_freelistN(nlalist);
}
