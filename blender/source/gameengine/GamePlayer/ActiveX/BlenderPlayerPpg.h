/**
 * $Id$
 *
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
#if !defined(AFX_BLENDERPLAYERPPG_H__23B95E44_7764_42EB_B5F1_35B9E713A17C__INCLUDED_)
#define AFX_BLENDERPLAYERPPG_H__23B95E44_7764_42EB_B5F1_35B9E713A17C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// BlenderPlayerPpg.h : Declaration of the CBlenderPlayerPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerPropPage : See BlenderPlayerPpg.cpp.cpp for implementation.

class CBlenderPlayerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CBlenderPlayerPropPage)
	DECLARE_OLECREATE_EX(CBlenderPlayerPropPage)

// Constructor
public:
	CBlenderPlayerPropPage();

// Dialog Data
	//{{AFX_DATA(CBlenderPlayerPropPage)
	enum { IDD = IDD_PROPPAGE_BLENDERPLAYER };
	CString	m_blenderURL;
	CString	m_loadingURL;
	UINT	m_frameRate;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CBlenderPlayerPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLENDERPLAYERPPG_H__23B95E44_7764_42EB_B5F1_35B9E713A17C__INCLUDED)

