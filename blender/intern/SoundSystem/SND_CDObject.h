/*
 * SND_CDObject.h
 *
 * Implementation for CD playback
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

#ifndef __SND_CDOBJECT_H
#define __SND_CDOBJECT_H

#include "SND_Object.h"

class SND_CDObject : public SND_Object
{
private:

	/**
	 * Private to enforce singleton
	 */
	SND_CDObject();
	SND_CDObject(const SND_CDObject&);

	static SND_CDObject*	m_instance;
	MT_Scalar				m_gain;			/* the gain of the object */
	int						m_playmode;		/* the way CD is played back (all, random, track, trackloop) */
	int						m_track;		/* the track for 'track' and 'trackloop' */
	int						m_playstate;	/* flag for current state of object */
	bool					m_modified;
	bool					m_used;			/* flag for checking if we used the cd, if not don't 
												call the stop cd at the end */

public:
	static bool	CreateSystem();
	static bool DisposeSystem();
	static SND_CDObject* Instance();

	~SND_CDObject();
	
	void SetGain(MT_Scalar gain);
	void SetPlaymode(int playmode);
	void SetTrack(int track);
	void SetPlaystate(int playstate);
	void SetModified(bool modified);
	void SetUsed();
	bool GetUsed();

	bool IsModified() const;

	int			GetTrack() const;
	MT_Scalar	GetGain() const;
	int			GetPlaymode() const;
	int			GetPlaystate() const;
	
};
#endif //__SND_CDOBJECT_H
