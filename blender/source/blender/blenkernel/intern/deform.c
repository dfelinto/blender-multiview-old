/*  deform.c   June 2001
 *  
 *  support for deformation groups
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

#include <string.h>

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"

#include "DNA_object_types.h"

#include "BKE_deform.h"


void color_temperature (float input, unsigned char *r, unsigned char *g, unsigned char *b)
{
	
	/* blue to red */
	
	float fr = (float)(*r);
	float fg = (float)(*g);
	float fb = (float)(*b);

	if (input < 0.0F)
		input = 0.0F;

	if (input > 1.0F)
		input = 1.0F;

	if (input<=0.25f){
		fr=0.0f;
		fg=255.0f * (input*4.0f);
		fb=255.0f;
	}
	else if (input<=0.50f){
		fr=0.0f;
		fg=255.0f;
		fb=255.0f * (1.0f-((input-0.25f)*4.0f)); 
	}
	else if (input<=0.75){
		fr=255.0f * ((input-0.50f)*4.0f);
		fg=255.0f;
		fb=0.0f;
	}
	else if (input<=1.0){
		fr=255.0f;
		fg=255.0f * (1.0f-((input-0.75f)*4.0f)); 
		fb=0.0f;
	}

	(*r) = (unsigned char)(fr * ((input/2.0f)+0.5f));
	(*g) = (unsigned char)(fg * ((input/2.0f)+0.5f));
	(*b) = (unsigned char)(fb * ((input/2.0f)+0.5f));


};

void copy_defgroups(ListBase *outbase, ListBase *inbase)
{
	bDeformGroup *defgroup, *defgroupn;

	outbase->first= outbase->last= 0;

	for (defgroup = inbase->first; defgroup; defgroup=defgroup->next){
		defgroupn= copy_defgroup(defgroup);
		BLI_addtail(outbase, defgroupn);
	}
}

bDeformGroup* copy_defgroup (bDeformGroup *ingroup)
{
	bDeformGroup *outgroup;

	if (!ingroup)
		return NULL;

	outgroup=MEM_callocN(sizeof(bDeformGroup), "deformGroup");
	
	/* For now, just copy everything over. */
	memcpy (outgroup, ingroup, sizeof(bDeformGroup));

	outgroup->next=outgroup->prev=NULL;

	return outgroup;
}

