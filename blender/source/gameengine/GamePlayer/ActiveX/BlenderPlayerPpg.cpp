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
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

// BlenderPlayerPpg.cpp : Implementation of the CBlenderPlayerPropPage property page class.


#include "stdafx.h"
#include "BlenderPlayer.h"
#include "BlenderPlayerPpg.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

IMPLEMENT_DYNCREATE(CBlenderPlayerPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CBlenderPlayerPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CBlenderPlayerPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CBlenderPlayerPropPage, "BLENDERPLAYER.BlenderPlayerPropPage.1",
	0xe899de88, 0xbf3f, 0x4d2c, 0xb5, 0x74, 0x10, 0x9d, 0x1f, 0x99, 0x9e, 0xdc)


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerPropPage::CBlenderPlayerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CBlenderPlayerPropPage

BOOL CBlenderPlayerPropPage::CBlenderPlayerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_BLENDERPLAYER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerPropPage::CBlenderPlayerPropPage - Constructor

CBlenderPlayerPropPage::CBlenderPlayerPropPage() :
	COlePropertyPage(IDD, IDS_BLENDERPLAYER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CBlenderPlayerPropPage)
	m_blenderURL = _T("");
	m_loadingURL = _T("");
	m_frameRate = 0;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerPropPage::DoDataExchange - Moves data between page and properties

void CBlenderPlayerPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CBlenderPlayerPropPage)
	DDP_Text(pDX, IDC_EDIT_BLENDER_URL, m_blenderURL, _T("blenderURL") );
	DDX_Text(pDX, IDC_EDIT_BLENDER_URL, m_blenderURL);
	DDP_Text(pDX, IDC_EDIT_LOADING_URL, m_loadingURL, _T("loadingURL") );
	DDX_Text(pDX, IDC_EDIT_LOADING_URL, m_loadingURL);
	DDP_Text(pDX, IDC_EDIT_FRAME_RATE, m_frameRate, _T("frameRate") );
	DDX_Text(pDX, IDC_EDIT_FRAME_RATE, m_frameRate);
	DDV_MinMaxUInt(pDX, m_frameRate, 1, 100);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerPropPage message handlers
