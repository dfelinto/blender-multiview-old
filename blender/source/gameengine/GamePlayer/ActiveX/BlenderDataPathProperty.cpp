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

// BlenderDataPathProperty.cpp : implementation file

//

#include "stdafx.h"
#include "BlenderDataPathProperty.h"

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
