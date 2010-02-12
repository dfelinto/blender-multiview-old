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
#if !defined(AFX_BLENDERDATAPATHPROPERTY_H__C3C26CAC_072A_431C_B985_20836EE58F76__INCLUDED_)
#define AFX_BLENDERDATAPATHPROPERTY_H__C3C26CAC_072A_431C_B985_20836EE58F76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlenderDataPathProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlenderDataPathProperty command target

class CBlenderDataPathProperty : public CCachedDataPathProperty
{
// Attributes
public:

// Operations
public:
	CBlenderDataPathProperty(COleControl* pControl);
	virtual ~CBlenderDataPathProperty();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlenderDataPathProperty)
	protected:
	virtual void OnDataAvailable(DWORD dwSize, DWORD bscfFlag);
	virtual void OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR szStatusText);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CBlenderDataPathProperty)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
public:
	/** Indicates the current progress during download relative to the expected maximum indicated in ulProgressMax. */
	ULONG m_ulProgress;
	/** Indicates the expected maximum value of ulProgress for the download. */
	ULONG m_ulProgressMax;
	/** Timer used to retrieve data. */
	//static UINT s_dataTimerEventID;
	/** The id of the timer used to retrieve data. */
	UINT m_dataTimerID;
	/** Records data download state. */
	BOOL m_bDownloadComplete;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLENDERDATAPATHPROPERTY_H__C3C26CAC_072A_431C_B985_20836EE58F76__INCLUDED_)

