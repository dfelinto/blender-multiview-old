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
#ifndef __KX_SCALARINTERPOLATOR_H
#define __KX_SCALARINTERPOLATOR_H

#include <vector>

#include "KX_IScalarInterpolator.h"

typedef unsigned short BL_IpoChannel;

class BL_ScalarInterpolator : public KX_IScalarInterpolator {
public:
	BL_ScalarInterpolator() {} // required for use in STL list
	BL_ScalarInterpolator(struct Ipo *ipo, BL_IpoChannel channel) :
		m_blender_ipo(ipo),
		m_channel(channel)
		{}

	virtual ~BL_ScalarInterpolator() {}
	
	virtual float GetValue(float currentTime) const;

	BL_IpoChannel GetChannel() const { return m_channel; }

private:
	struct Ipo    *m_blender_ipo;
	BL_IpoChannel  m_channel;
};


class BL_InterpolatorList : public std::vector<KX_IScalarInterpolator *> {
public:
	BL_InterpolatorList(struct Ipo *ipo);
	~BL_InterpolatorList();

	KX_IScalarInterpolator *GetScalarInterpolator(BL_IpoChannel channel);	
};


#endif //__KX_SCALARINTERPOLATOR_H
