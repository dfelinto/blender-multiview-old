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
#ifndef __CCONTROL_REFRESHER_H__
#define __CCONTROL_REFRESHER_H__

#include "stdafx.h"


class CControlRefresher
{
public:
	CControlRefresher(COleControl* control, DWORD interval);
	~CControlRefresher(void);

	void Start(void);
	void Stop(void);
	bool IsRunning(void) const;
	void SetInterval(DWORD interval);
	DWORD GetInterval(void) const;

protected:
	static DWORD WINAPI ThreadFunction(LPVOID pParam);

	DWORD m_interval;
	DWORD m_lastTime;
	bool m_exit;
	bool m_ack;
	COleControl* m_control;
	HANDLE m_hThread;
	DWORD m_dwThreadId;
};

#endif
