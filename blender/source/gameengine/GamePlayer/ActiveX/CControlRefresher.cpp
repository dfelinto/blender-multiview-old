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

#include "CControlRefresher.h"



CControlRefresher::CControlRefresher(COleControl* control, DWORD interval)
{
	ASSERT(control);
	m_control = control;
	m_hThread = 0;
	m_dwThreadId = 0;
	m_interval = interval;
	m_exit = false;
	m_ack = false;
}


CControlRefresher::~CControlRefresher(void)
{
	Stop();
}


void CControlRefresher::Start(void)
{
	if (!m_hThread) 
	{
		m_exit = false;
		// Create the thread suspended in case we want to set variables
		m_hThread = ::CreateThread(0, 0, ThreadFunction, this, CREATE_SUSPENDED, &m_dwThreadId);
		// Anything higher will interfere with IE responsiveness.
		::SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);
		m_lastTime = ::GetTickCount();
		// Resume the thread, allowing it to run
		::ResumeThread(m_hThread);
	}
}


void CControlRefresher::Stop(void)
{
	if (m_hThread) 
	{
		// Let the thread know to nicely commit suicide.
		m_ack = false;
		m_exit = true;
		// Boost the thread’s priority so it can finish whatever it has to do before it dies.
		//::SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);
		// Wait until the thread has in fact died.
		while (!m_ack) Sleep(10);
		::CloseHandle(m_hThread);
		m_hThread = 0;
	}
}


bool CControlRefresher::IsRunning(void) const
{
	return m_hThread != 0;
}


void CControlRefresher::SetInterval(DWORD interval)
{
	m_interval = interval;
}


DWORD CControlRefresher::GetInterval(void) const
{
	return m_interval;
}


DWORD CControlRefresher::ThreadFunction(LPVOID pParam)
{
	CControlRefresher* refresher = (CControlRefresher*) pParam;
	ASSERT(refresher);
	while (!refresher->m_exit) {
		if (refresher->m_control->GetHwnd()) {
			refresher->m_control->RedrawWindow();
		}
		/*
			if (!refresher->m_exit) {
				::Sleep(refresher->m_interval);
			}
		}
		else {
			::Sleep(100);
		}
		*/
		if (!refresher->m_exit) {
			::Sleep(refresher->m_interval);
		}
	}
	refresher->m_ack = true;
	return 0;
}
