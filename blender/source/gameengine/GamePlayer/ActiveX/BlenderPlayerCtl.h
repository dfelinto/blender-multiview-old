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
 * Blender Player Active X control class declaration.
 */

#if !defined(AFX_BLENDERPLAYERCTL_H__9DA6C3C3_CF65_4267_AB50_DCCDE8341EBF__INCLUDED_)
#define AFX_BLENDERPLAYERCTL_H__9DA6C3C3_CF65_4267_AB50_DCCDE8341EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RawImageRsrc.h"
#include "BlenderDataPathProperty.h"
#include "CControlRefresher.h"
#include "STR_String.h"

struct BlendFileData;

class KX_KetsjiEngine;
class GPW_System;
class GPW_KeyboardDevice;
class GPC_MouseDevice;
class GPW_Canvas;
class RAS_IRenderTools;
class RAS_IRasterizer;
class KetsjiPortal;
class KX_ISceneConverter;
class NG_LoopBackNetworkDeviceInterface;
class SND_IAudioDevice;
class STR_String;


// BlenderPlayerCtl.h : Declaration of the CBlenderPlayerCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerCtrl : See BlenderPlayerCtl.cpp for implementation.

class CBlenderPlayerCtrl : public COleControl
{
	DECLARE_DYNCREATE(CBlenderPlayerCtrl)

// Constructor
public:
	CBlenderPlayerCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlenderPlayerCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual DWORD GetControlFlags();
	virtual void OnForeColorChanged();
	virtual void OnBackColorChanged();
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnDrawMetafile(CDC* pDC, const CRect& rcBounds);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CBlenderPlayerCtrl();

	DECLARE_OLECREATE_EX(CBlenderPlayerCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CBlenderPlayerCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CBlenderPlayerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CBlenderPlayerCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CBlenderPlayerCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CBlenderPlayerCtrl)
	BOOL m_showFrameCount;
	afx_msg void OnShowFrameCountChanged();
	BOOL m_showProfileInfo;
	afx_msg void OnShowProfileInfoChanged();
	CString m_blenderURL;
	afx_msg void OnBlenderURLChanged();
	BOOL m_showProperties;
	afx_msg void OnShowPropertiesChanged();
	CString m_loadingURL;
	afx_msg void OnLoadingURLChanged();
	short m_frameRate;
	afx_msg void OnFrameRateChanged();
	BOOL m_useFileBackColor;
	afx_msg void OnUseFileBackColorChanged();
	afx_msg void Rewind();
	afx_msg BOOL SendMessage(LPCTSTR to, LPCTSTR from, LPCTSTR subject, LPCTSTR body);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CBlenderPlayerCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CBlenderPlayerCtrl)
	dispidShowFrameCount = 1L,
	dispidShowProfileInfo = 2L,
	dispidBlenderURL = 3L,
	dispidShowProperties = 4L,
	dispidLoadingURL = 5L,
	dispidFrameRate = 6L,
	dispidUseFileBackColor = 7L,
	dispidRewind = 8L,
	dispidSendMessage = 9L,
	//}}AFX_DISP_ID
	};

protected:
	/**
	 * Creates a splash bitmap optimized for display.
	 */
	void createOffscreenSplash(CDC* pdc, int width, int height);

	/**
	 * Draws the control when it is idle.
	 */
	void drawIdle(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

	/**
	 * Draws the control when it is loading the custom loading animation.
	 */
	void drawLoadLoading(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

	/**
	 * Draws the control when it is loading the main Blender file.
	 */
	void drawLoadBlender(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

	/**
	 * Draws the control when the game engine is running.
	 */
	void drawEngine(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

	/**
	 * Draws the control when there was an error.
	 */
	void drawError(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

	/**
	 * Initializes the game engine.
	 * @return Indication of success.
	 */
	bool initEngine(void);

	/**
	 * Starts the game engine.
	 * @return Indication of success.
	 */
	bool startEngine(void);

	/**
	 * Stop the game engine.
	 */
	void stopEngine(void);

	/**
	 * Shuts the game engine down.
	 */
	void exitEngine(void);

	/**
	 * Starts downloading Blender and/or loading animation.
	 * @return Indication of success.
	 */
	bool startDownload(void);

	/**
	 * Starts downloading custom loading animation from the URL given.
	 * @return Indication of success.
	 */
	bool startLoadingDownload(void);

	/**
	 * Starts downloading Blender data from the URL given.
	 * @return Indication of success.
	 */
	bool startBlenderDownload(void);

	void SetGameData(struct BlendFileData *gamedata);
	bool LoadGameData(char *fromfile);
	bool LoadGameData(void *mem, int memsize);

	/**
	 * Loads the Blender data into the global structures.
	 * @param blenderData The data path property to load from.
	 * @param isPublisher The file data was generated by Blender Publisher
	 * @return Indication of success.
	 */
	bool activateBlenderData(CBlenderDataPathProperty& blenderData, bool& isPublisher);

	/**
	 * Check whether there is download progress.
	 */
	void checkDownload(void);

	/**
	 * Shows the error.
	 * @param msg	The error message to display.
	 */
	void showError(const CString& msg);

	/**
	 * Sends a message to the network device.
	 * @param to		Destination object name for the message.
	 * @param from		Sender object name for the message.
	 * @param subject	Subject field of the message.
	 * @param body		Body of the message.
	 */
	void sendMessage(
		const STR_String& to,
		const STR_String& from,
		const STR_String& subject,
		const STR_String& body);

	/**
	 * Updates the engine info display state from member state variables.
	 */
	void updateEngineInfoDisplay(void);

	/** 
	 * Determines progress from the cached data.
	 * @return Percentage (0-100%) of progress.
	 */
	float determineProgress(void) const;

	/**
	 * Updates loading animation percentage.
	 * @param delta New percentage.
	 */
	void updateLoadingAnimation(float progress);

	/**
	 * Updates the setting of the background color in the engine.
	 */
	void updateFrameColor(void);

	/**
	 * Converts a COLORREF to RGB values.
	 */
	void convertColorRefToRGB(COLORREF color, float& r, float& g, float& b);

	/**
	 * Converts a OLE_COLOR to RGB values.
	 */
	void convertOleColorToRGB(OLE_COLOR color, float& r, float& g, float& b);

	/**
	 * Processes messages from the game engine for the plugin.
	 */ 
	void processMessages(void);

	/**
	 * Loads a new HTML page.
	 */ 
	void loadURL(const STR_String& url);
	

	/**
	 * This typedef is used to record the current run state.
	 */
	typedef enum {
		stateIdle = 0,
		stateLoadLoading,
		stateLoadBlender,
		stateRun,
		stateError,
		stateLoadAsyncFailed
	} AXC_TRunState;

	/** The state of the control. */
	AXC_TRunState m_runState;

	/** The drawing state of the control. */
	bool m_drawingMetafile;

	/** URL of Blender data at start up. */
	CString m_initialBlenderURL;
	/** The data path of the main Blender file. */
	CBlenderDataPathProperty m_blenderData;
	/** The presence of a valid Blender data file. */
	bool m_blenderDataValid;
	/** Last file download progress measurement. */
	float m_lastProgress;
	/** URL of loading animation given at start up. */
	CString m_initialLoadingURL;
	/** The data path of the custom loading animation file. */
	CBlenderDataPathProperty m_loadingData;
	/** The presence of a valid custom loading animation. */
	bool m_loadingDataValid;

	/** Engine construction state. */
	bool m_engineInitialized;
	/** Engine state. */
	bool m_engineRunning;

	/** Some exit stuff */
	int m_exitRequested;
	STR_String	m_exitString;

	/** the gameengine itself */
	KX_KetsjiEngine* m_ketsjiengine;
	/** The game engine's system abstraction. */
	GPW_System* m_system;
	/** The game engine's keyboard abstraction. */
	GPW_KeyboardDevice* m_keyboard;
	/** The game engine's mouse abstraction. */
	GPC_MouseDevice* m_mouse;
	/** The game engine's canvas abstraction. */
	GPW_Canvas* m_canvas;
	/** The game engine's platform dependent render tools. */
	RAS_IRenderTools* m_rendertools;
	/** the rasterizer */
	RAS_IRasterizer* m_rasterizer;
	/** Converts Blender data files. */
	KX_ISceneConverter* m_sceneconverter;
	/** Network interface. */
	NG_LoopBackNetworkDeviceInterface* m_networkdevice;
	/** Sound device. */
	SND_IAudioDevice* m_audiodevice;

	/* Data for the active game file. */
	struct BlendFileData *m_gamedata;

	/** Blender logo image data. */
	RawImageRsrc m_blenderLogo;
	/** NaN logo image data. */
	RawImageRsrc m_nanLogo;
	/** Blender data used as loading animation. */
	MemoryResource m_loadingAnimation;

	/** Sort of mutex for locking. */
	static bool s_criticalSectionInitialized;
	static CRITICAL_SECTION s_cs;
	/* Used to advance frames */
	CControlRefresher m_refresher;

	/** Load-URL-message address. */
	static STR_String s_messageAddress;
	/** Load-URL-message subject. */
	static STR_String s_messageLoadURLSubject;

	/** Number of controls in this process. */
	static int s_numControls;
	/** Number of engines running in this process. */
	static int s_numEnginesRunning;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLENDERPLAYERCTL_H__9DA6C3C3_CF65_4267_AB50_DCCDE8341EBF__INCLUDED)

