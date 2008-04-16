/**
 * $Id$
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
 * ***** END GPL LICENSE BLOCK *****
 */

#include "CControlRefresher.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
