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
