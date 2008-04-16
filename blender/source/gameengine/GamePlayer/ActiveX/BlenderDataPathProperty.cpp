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

// BlenderDataPathProperty.cpp : implementation file

//

#include "stdafx.h"
#include "BlenderDataPathProperty.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CBlenderDataPathProperty

CBlenderDataPathProperty::CBlenderDataPathProperty(COleControl* pControl)
    : CCachedDataPathProperty(pControl), m_ulProgress(0), m_ulProgressMax(0)
{
}

CBlenderDataPathProperty::~CBlenderDataPathProperty()
{
}


// Do not edit the following lines, which are needed by ClassWizard.

#ifdef DSADSA
BEGIN_MESSAGE_MAP(CBlenderDataPathProperty, CCachedDataPathProperty)
	//{{AFX_MSG_MAP(CBlenderDataPathProperty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


/////////////////////////////////////////////////////////////////////////////
// CBlenderDataPathProperty member functions

void CBlenderDataPathProperty::OnDataAvailable(DWORD dwSize, DWORD bscfFlag) 
{
	CCachedDataPathProperty::OnDataAvailable(dwSize, bscfFlag);

	if (bscfFlag & BSCF_FIRSTDATANOTIFICATION) {
		// First data has arrived
		m_ulProgress = m_ulProgressMax = 0;
		ResetData();
	}

	if (bscfFlag & BSCF_LASTDATANOTIFICATION) {
		// All data is in
		GetControl()->InternalSetReadyState(READYSTATE_COMPLETE);
	}	
	
	// Force a redraw
	GetControl()->InvalidateControl();
}

void CBlenderDataPathProperty::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR szStatusText) 
{	
	if ((ulStatusCode == BINDSTATUS_BEGINDOWNLOADDATA) ||
	    (ulStatusCode == BINDSTATUS_DOWNLOADINGDATA) ||
		(ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA)) {
		m_ulProgress = ulProgress; 
		m_ulProgressMax = ulProgressMax;
		/*
		if (ulProgressMax) {
			float hz = 1000;
			hz += ((float)ulProgress) / ((float)ulProgressMax) * 2000.f;
			Beep((DWORD)hz, 100);
		}
		*/

		// Force a redraw
		GetControl()->Refresh();
	}

	CCachedDataPathProperty::OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
}
