/**
 * $Id$
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


#include "KX_BlenderScalarInterpolator.h"

extern "C" int IPO_GetChannels(struct Ipo *ipo, short *channels);
extern "C" float IPO_GetFloatValue(struct Ipo *ipo, /*IPO_Channel*/ short channel, float ctime);


static const int BL_MAX_CHANNELS = 32;

float BL_ScalarInterpolator::GetValue(float currentTime) const {
	return IPO_GetFloatValue(m_blender_ipo, m_channel, currentTime);
}

typedef short IPO_Channel;  

BL_InterpolatorList::BL_InterpolatorList(struct Ipo *ipo) {
	IPO_Channel channels[BL_MAX_CHANNELS];

	int num_channels = IPO_GetChannels(ipo, channels);

	int i;

	for (i = 0; i != num_channels; ++i) {
		BL_ScalarInterpolator *new_ipo =
			new BL_ScalarInterpolator(ipo, channels[i]); 

		//assert(new_ipo);
		push_back(new_ipo);
	}
}

BL_InterpolatorList::~BL_InterpolatorList() {
	BL_InterpolatorList::iterator i;
	for (i = begin(); !(i == end()); ++i) {
		delete *i;
	}
}


KX_IScalarInterpolator *BL_InterpolatorList::GetScalarInterpolator(BL_IpoChannel channel) {
	BL_InterpolatorList::iterator i = begin();
	while (!(i == end()) && 
		   (static_cast<BL_ScalarInterpolator *>(*i))->GetChannel() != 
		   channel) {
		++i;
	}
	
	return (i == end()) ? 0 : *i;
}	

