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
 * Blender Player Active X control class implementation.
 */

#include "stdafx.h"
#include <mshtml.h> 

//#define AX_NO_SOUND

#include "BlenderPlayer.h"
#include "BlenderPlayerCtl.h"
#include "BlenderPlayerPpg.h"
#include "SafeControl.h"

#pragma warning (disable : 4786)

#include "GEN_messaging.h"
#include "SYS_System.h"
#include "NG_NetworkScene.h"
#include "NG_LoopBackNetworkDeviceInterface.h"
#include "KX_KetsjiEngine.h"
#include "KX_BlenderSceneConverter.h"
#include "RAS_OpenGLRasterizer.h"
#include "KX_PythonInit.h"
#include "KX_PyConstraintBinding.h"

#include "SND_DeviceManager.h"

#include "GPW_Canvas.h"
#include "GPW_KeyboardDevice.h"
#include "GPC_MouseDevice.h"
#include "GPW_System.h"
#include "GPC_RenderTools.h"


/**********************************
 * Begin Blender include block
 **********************************/
#ifdef __cplusplus
extern "C"
{
#endif  // __cplusplus

#include "BLI_blenlib.h"

#include "DNA_scene_types.h"

#include "BLO_readfile.h"

#include "BKE_report.h"

#ifdef __cplusplus
}
#endif // __cplusplus
/**********************************
 * End Blender include block
 **********************************/


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBlenderPlayerCtrl, COleControl)

CRITICAL_SECTION CBlenderPlayerCtrl::s_cs;
bool CBlenderPlayerCtrl::s_criticalSectionInitialized = false;
STR_String CBlenderPlayerCtrl::s_messageAddress;
STR_String CBlenderPlayerCtrl::s_messageLoadURLSubject;
int CBlenderPlayerCtrl::s_numControls = 0;
int CBlenderPlayerCtrl::s_numEnginesRunning = 0;


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CBlenderPlayerCtrl, COleControl)
	//{{AFX_MSG_MAP(CBlenderPlayerCtrl)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CBlenderPlayerCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CBlenderPlayerCtrl)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "showFrameCount", m_showFrameCount, OnShowFrameCountChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "showProfileInfo", m_showProfileInfo, OnShowProfileInfoChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "blenderURL", m_blenderURL, OnBlenderURLChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "showProperties", m_showProperties, OnShowPropertiesChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "loadingURL", m_loadingURL, OnLoadingURLChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "frameRate", m_frameRate, OnFrameRateChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CBlenderPlayerCtrl, "useFileBackColor", m_useFileBackColor, OnUseFileBackColorChanged, VT_BOOL)
	DISP_FUNCTION(CBlenderPlayerCtrl, "Rewind", Rewind, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBlenderPlayerCtrl, "SendMessage", SendMessage, VT_BOOL, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_FORECOLOR()
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CBlenderPlayerCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CBlenderPlayerCtrl, COleControl)
	//{{AFX_EVENT_MAP(CBlenderPlayerCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CBlenderPlayerCtrl, 2)
	PROPPAGEID(CBlenderPlayerPropPage::guid)
	PROPPAGEID(CLSID_CColorPropPage)
END_PROPPAGEIDS(CBlenderPlayerCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CBlenderPlayerCtrl, "BLENDERPLAYER.BlenderPlayerCtrl.1",
	0x5db05cb8, 0x7751, 0x469d, 0xa1, 0xdd, 0x45, 0xc8, 0xc2, 0x1, 0xc0, 0x13)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CBlenderPlayerCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DBlenderPlayer =
		{ 0x75be7171, 0x64c0, 0x4125, { 0x99, 0xa8, 0x1a, 0xe5, 0xe, 0xa4, 0x9d, 0x6c } };
const IID BASED_CODE IID_DBlenderPlayerEvents =
		{ 0x7990fce5, 0x9a6, 0x4e39, { 0x94, 0xb6, 0x3e, 0x47, 0xe, 0xd5, 0xc2, 0x2a } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwBlenderPlayerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CBlenderPlayerCtrl, IDS_BLENDERPLAYER, _dwBlenderPlayerOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerCtrl::CBlenderPlayerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CBlenderPlayerCtrl

BOOL CBlenderPlayerCtrl::CBlenderPlayerCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister) {
        HRESULT hr = S_OK ;
        // Register as safe for scripting
        hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls that are safely scriptable");
        if (FAILED(hr))
           return FALSE;
        hr = RegisterCLSIDInCategory(m_clsid, CATID_SafeForScripting);
        if (FAILED(hr))
            return FALSE;
        // register as safe for initializing
        hr = CreateComponentCategory(CATID_SafeForInitializing,
                L"Controls safely initializable from persistent data");
        if (FAILED(hr))
            return FALSE;
        hr = RegisterCLSIDInCategory(m_clsid, CATID_SafeForInitializing);
        if (FAILED(hr))
            return FALSE;

	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_BLENDERPLAYER,
			IDB_BLENDERPLAYER,
			afxRegInsertable | afxRegApartmentThreading,
			_dwBlenderPlayerOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	}
	else {
        HRESULT hr = S_OK ;
        hr = UnRegisterCLSIDInCategory(m_clsid, CATID_SafeForScripting);
        if (FAILED(hr))
            return FALSE;
        hr = UnRegisterCLSIDInCategory(m_clsid, CATID_SafeForInitializing);
        if (FAILED(hr))
            return FALSE;
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerCtrl::CBlenderPlayerCtrl - Constructor

CBlenderPlayerCtrl::CBlenderPlayerCtrl()
// C4355 warning on m_blenderData(this) and m_loadingData(this), but this is okay.
#pragma warning(disable: 4355)
: m_blenderData(this), m_loadingData(this), m_refresher(this, 1000)
{

	/* Initialize the critical section object used to lock game engine access */
	if (!s_criticalSectionInitialized) {
		InitializeCriticalSection(&s_cs);
		s_criticalSectionInitialized = true;
		s_messageAddress = "host_application";
		s_messageLoadURLSubject = "load_url";
	}
	s_numControls++;

	InitializeIIDs(&IID_DBlenderPlayer, &IID_DBlenderPlayerEvents);

	// Initialize member variables
	m_runState = stateIdle;
	m_showFrameCount = FALSE;
	m_showProfileInfo = FALSE;
	m_showProperties = FALSE;
	m_useFileBackColor = FALSE;
	m_drawingMetafile = false;
	m_blenderDataValid = false;
	m_loadingDataValid = false;
	m_lastProgress = 0;
	m_engineInitialized = false;
	m_engineRunning = false;
	m_frameRate = 60;
	m_ketsjiengine = 0;
	m_system = 0;
	m_keyboard = 0;
	m_mouse = 0;
	m_canvas = 0;
	m_rendertools = 0;
	m_rasterizer = 0;
	m_sceneconverter = 0;
	m_networkdevice = 0;
	m_audiodevice = 0;
	m_gamedata = 0;

	// Get loading animation Blender data from the resources
	HINSTANCE hInstApp = AfxGetResourceHandle();
	m_loadingAnimation.load(hInstApp, MAKEINTRESOURCE(IDR_LOAD), "BLEND");
	// Get logo pixmap data from the resources and convert to OpenGL compliant dimensions (128x128)
	m_blenderLogo.load(
		hInstApp, MAKEINTRESOURCE(IDR_LOGO_BLENDER), "RAW",
		115, 32,
		128, 128, RawImageRsrc::alignTopLeft, 8, 8);
	m_nanLogo.load(
		hInstApp, MAKEINTRESOURCE(IDR_LOGO_BLENDER3D), "RAW",
		136, 11,
		256, 256, RawImageRsrc::alignBottomRight, 8, 8);
	RecreateControlWindow();

	m_refresher.Start();
}


/////////////////////////////////////////////////////////////////////////////
// CBlenderPlayerCtrl::~CBlenderPlayerCtrl - Destructor

CBlenderPlayerCtrl::~CBlenderPlayerCtrl()
{
	exitEngine();
	m_refresher.Stop();
	s_numControls--;
}



void CBlenderPlayerCtrl::drawIdle(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	//CBrush bkBrush(TranslateColor(GetBackColor()));
	CBrush bkBrush;
	bkBrush.CreateHatchBrush(HS_BDIAGONAL, TranslateColor(GetForeColor()));
	pdc->FillRect(rcBounds, &bkBrush);
//	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
}



void CBlenderPlayerCtrl::drawLoadLoading(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	OLE_COLOR oleColor = GetForeColor();
	pdc->FillSolidRect(rcBounds, TranslateColor(oleColor));
}



void CBlenderPlayerCtrl::drawLoadBlender(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	drawEngine(pdc, rcBounds, rcInvalid);
}



void CBlenderPlayerCtrl::drawEngine(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (m_engineRunning) {
		if (m_canvas) {
			// Check if the device context is still the same as the one in the canvas.
			HDC hCurrentDC = pdc->GetSafeHdc();
			HDC hCanvasDC = m_canvas->GetHDC();
			if (hCurrentDC != hCanvasDC) {
				// Set the current device context so that we blit to the right device
				m_canvas->SetHDC(hCurrentDC);
			}

			/* Check if the width and height of the control are still the same as set
			 * in the canvas. This will make the control scale in Powerpoint.
			 * We don't want to scale when the control is scaled down.
			 */
			int ww = rcBounds.Width();
			int wh = rcBounds.Height();
			//this->GetControlSize(&ww, &wh);
			int cw = m_canvas->GetWidth();
			int ch = m_canvas->GetHeight();
			// Only size when the area is bigger
			if (((ww * wh) > (cw * ch))) {
				m_canvas->Resize(ww, wh);
			}
		}

		// Update the state of the game engine
		if (m_system) {			
			// Prevent access to the game engine data from other threads
			EnterCriticalSection(&s_cs);
			
			// kick the engine
			m_ketsjiengine->NextFrame();
			
			// render the frame
			m_ketsjiengine->Render();

			// first check if we want to exit
			m_exitRequested = m_ketsjiengine->GetExitCode();
			
			if (m_exitRequested == KX_EXIT_REQUEST_START_OTHER_GAME)
			{
				m_blenderURL = m_ketsjiengine->GetExitString();
			}
			
			// Release private access to the game engine data
			LeaveCriticalSection(&s_cs);
		}
	}
}



void CBlenderPlayerCtrl::drawError(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	drawEngine(pdc, rcBounds, rcInvalid);
}



bool CBlenderPlayerCtrl::initEngine(void)
{
	if (!m_engineInitialized) {

		SYS_SystemHandle syshandle = SYS_GetSystem();
		GEN_init_messaging_system();

		if (syshandle) {
			// Set game engine preferences
			SYS_WriteCommandLineInt(syshandle, "fixedtime", 0);
			SYS_WriteCommandLineInt(syshandle, "vertexarrays", 0);
			// Timing information display
			updateEngineInfoDisplay();
			
			m_rendertools = new GPC_RenderTools();
			if (m_rendertools)
			{
				// create the inputdevices
				m_keyboard = new GPW_KeyboardDevice();
				if (m_keyboard)
				{
					m_mouse = new GPC_MouseDevice();
					if (m_mouse)
					{
						// create a networkdevice
						m_networkdevice = new NG_LoopBackNetworkDeviceInterface();
						if (m_networkdevice)
						{
							// get an audiodevice
							SND_DeviceManager::Subscribe();
							m_audiodevice = SND_DeviceManager::Instance();
							if (m_audiodevice)
							{
								// create a ketsjisystem (only needed for timing and stuff)
								m_system = new GPW_System();
								if (m_system)
								{
									// create the ketsjiengine
									m_ketsjiengine = new KX_KetsjiEngine(m_system);
									
									// set the devices
									m_ketsjiengine->SetKeyboardDevice(m_keyboard);
									m_ketsjiengine->SetMouseDevice(m_mouse);
									m_ketsjiengine->SetNetworkDevice(m_networkdevice);
									m_ketsjiengine->SetRenderTools(m_rendertools);
									m_ketsjiengine->SetNetworkDevice(m_networkdevice);
									m_ketsjiengine->SetAudioDevice(m_audiodevice);
									
									m_ketsjiengine->SetUseFixedTime(false);
									//m_ketsjiengine->SetTimingDisplay(frameRate, profile, properties);
									
									m_engineInitialized = true;
								}
							}
						}
					} 
				}
			}
		}
	}

	return m_engineInitialized;
}



void CBlenderPlayerCtrl::SetGameData(struct BlendFileData *gamedata)
{
	if (m_gamedata) {
		BLO_blendfiledata_free(m_gamedata);
	}

	m_gamedata = gamedata;
}



bool CBlenderPlayerCtrl::LoadGameData(char *fromfile)
{
	ReportList reports;
	BlendFileData *bfd;
	
	BKE_reports_init(&reports, 0);
	bfd= BLO_read_from_file(fromfile, &reports);
	BKE_reports_clear(&reports);

	if (bfd) {
		SetGameData(bfd);
		return true;
	} else {
		return false;
	}
}



bool CBlenderPlayerCtrl::LoadGameData(void *mem, int memsize)
{
	ReportList reports;
	BlendFileData *bfd;
	
	BKE_reports_init(&reports, 0);
	bfd= BLO_read_from_memory(mem, memsize, &reports);
	BKE_reports_clear(&reports);

	if (bfd) {
		SetGameData(bfd);

		return true;
	} else {
		return false;
	}
}

bool CBlenderPlayerCtrl::startEngine(void)
{
	if (m_engineRunning) {
		return false;
	}
	if (!m_engineInitialized) {
		if (!initEngine()) {
			return false;
		}
	}
	if (m_hWnd == NULL) {
		// The device context is not valid unless we're activated and thus have a valid window handle.
		return false;
	}

	// Activate the global Blender database 
	bool isPublisher;
	bool logos = true;
	char* dir;
	CString path = m_blenderData.GetPath();
	switch (m_runState) {
	case stateLoadBlender:
		if (m_loadingDataValid) {
			if (activateBlenderData(m_loadingData, isPublisher)) {
				if (!isPublisher) {
					showError("Loading animation is not a Blender Publisher file.");
					return false;
				}
				logos = !isPublisher;
			}
			else {
				showError("Error loading file.");
				return false;
			}
		}
		else {
			if (!LoadGameData(m_loadingAnimation.getData(), m_loadingAnimation.getDataSize())) {
				return false;
			}
		}
		break;

	case stateRun:
		if (!m_blenderDataValid) {
			return false;
		}
		if (activateBlenderData(m_blenderData, isPublisher)) {
			if (m_loadingDataValid && !isPublisher) {
				showError("Loading animations only allowed with Blender Publisher files.");
				return false;
			}
			logos = !isPublisher;
		}
		else {
			showError("Error loading file.");
			return false;
		}
		break;

	case stateLoadAsyncFailed:
		// Director does not load files asynchronously, try loading it from disk.
		dir = (char*)((LPCTSTR)path);
		if (!LoadGameData(dir)) {
			showError("Error loading file.");
			return false;
		}
		isPublisher = m_gamedata->type == BLENFILETYPE_PUB;
		logos = !isPublisher;
		break;

	case stateError:
		// Until we have an error animation
		if (!LoadGameData(m_loadingAnimation.getData(), m_loadingAnimation.getDataSize())) {
			showError("Error loading file.");
			return false;
		}
		break;

	default:
		return false;
	}

	// Create the canvas.
	int width, height;
	this->GetControlSize(&width, &height);
	CDC* dc = this->GetDC();
	if (!dc) {
		return false;
	}

	HDC hDC = dc->GetSafeHdc();
	m_canvas = new GPW_Canvas(m_hWnd, hDC, width, height);
	if (!m_canvas) {
		return false;
	}

	// Initialize the canvas
	m_canvas->Init();

	if (logos) {
		m_canvas->SetBannerDisplayEnabled(true);
		width = m_blenderLogo.getWidth();
		height = m_blenderLogo.getHeight();
		m_canvas->AddBanner(width, height, width, height,
			(unsigned char*)m_blenderLogo.getData(), GPC_Canvas::alignTopLeft);
		width = m_nanLogo.getWidth();
		height = m_nanLogo.getHeight();
		m_canvas->AddBanner(width, height, width, height,
			(unsigned char*)m_nanLogo.getData(), GPC_Canvas::alignBottomRight);
	}

	// Check if there is a camera, do not start without it.
	if (!m_gamedata->curscene->camera) {
		showError(CString("No camera in the Blender file."));
		return false;
	}

	STR_String startscenename = m_gamedata->curscene->id.name + 2;

	if (m_ketsjiengine)
	{
		// create the rasterizer
		m_rasterizer = new RAS_OpenGLRasterizer(m_canvas);
		m_ketsjiengine->SetCanvas(m_canvas);
		m_ketsjiengine->SetRasterizer(m_rasterizer);
		updateEngineInfoDisplay();
		
		// create a scene converter, create and convert the starting scene
		m_sceneconverter = new KX_BlenderSceneConverter(m_gamedata->main, m_ketsjiengine);
		if (m_sceneconverter)
		{
			m_ketsjiengine->SetSceneConverter(m_sceneconverter);
			
			KX_Scene* startscene = new KX_Scene(m_keyboard,
				m_mouse,
				m_networkdevice,
				m_audiodevice,
				startscenename,
				m_gamedata->curscene);
			
			PyObject* m_dictionaryobject = initGamePlayerPythonScripting("Ketsji", psl_Highest, m_gamedata->main, 0, NULL);
			m_ketsjiengine->SetPyNamespace(m_dictionaryobject);

			initRasterizer(m_rasterizer, m_canvas);			
			PyDict_SetItemString(m_dictionaryobject, "GameLogic", initGameLogic(m_ketsjiengine, startscene)); // Same as importing the module
			initGameKeys();			
			initPythonConstraintBinding();
			initMathutils();
			initGeometry();
			initBGL();
			
			m_sceneconverter->ConvertScene(
				startscenename,
				startscene,
				m_dictionaryobject,
				m_rendertools,
				m_canvas);
			m_ketsjiengine->AddScene(startscene);
			
			if (m_frameRate > 0) {
				UINT nElapse = 1000 / m_frameRate;
				m_refresher.SetInterval(nElapse);
			}
			
			m_audiodevice->StopAllObjects();
			m_rasterizer->Init();
			m_ketsjiengine->StartEngine(true);
			
			m_engineRunning = true;
			updateFrameColor();
			// Timing information display
			updateEngineInfoDisplay();
		}
	}
	if (!m_engineRunning) {
		stopEngine();
	}
	else {
		s_numEnginesRunning++;
	}
	return m_engineRunning;
}


void CBlenderPlayerCtrl::stopEngine()
{
	/* 
	 * Disable Python exit until we are the last engine being stopped.
	 * This is because there is only one Python interpreter running 
	 * for all engines at the moment.
	 * Until this is changed, we can stop the interpreter only when
	 * the last engine is destroyed. If we would shut it down every time
	 * we shut an engine down, remaining engines will crash on Python use.
	 */
	if (m_engineRunning) {
		s_numEnginesRunning--;
		if (s_numEnginesRunning < 1) {
			exitGamePythonScripting();
		}
	}

	if (m_ketsjiengine)
		m_ketsjiengine->StopEngine();

	if (m_networkdevice)
		m_networkdevice->Disconnect();

	if (m_sceneconverter) {
		delete m_sceneconverter;
		m_sceneconverter = 0;
	}
	if (m_engineRunning) {
		// Free game data
		SetGameData(NULL);
	}
	m_refresher.SetInterval(1000);
	m_engineRunning = false;
}


void CBlenderPlayerCtrl::exitEngine()
{
	stopEngine();

	if (m_ketsjiengine)
	{
		delete m_ketsjiengine;
		m_ketsjiengine = 0;
	}
	if (m_system)
	{
		delete m_system;
		m_system = 0;
	}
	if (m_audiodevice)
	{
		SND_DeviceManager::Unsubscribe();
		m_audiodevice = 0;
	}
	if (m_networkdevice)
	{
		delete m_networkdevice;
		m_networkdevice = 0;
	}
	if (m_mouse)
	{
		delete m_mouse;
		m_mouse = 0;
	}
	if (m_keyboard)
	{
		delete m_keyboard;
		m_keyboard = 0;
	}
	if (m_rasterizer)
	{
		delete m_rasterizer;
		m_rasterizer = 0;
	}
	if (m_rendertools)
	{
		delete m_rendertools;
		m_rendertools = 0;
	}
	if (m_canvas)
	{
		delete m_canvas;
		m_canvas = 0;
	}

	m_engineInitialized = false;
}


bool CBlenderPlayerCtrl::startDownload(void)
{
	bool success;
	if (!m_loadingDataValid && !m_loadingURL.IsEmpty()) {
		success = startLoadingDownload();
	}
	else {
		success = startBlenderDownload();
	}
	return success;
}


bool CBlenderPlayerCtrl::startBlenderDownload(void)
{
	bool success = true;
	InternalSetReadyState(READYSTATE_INTERACTIVE);
	m_blenderData.SetPath(m_blenderURL);
	CFileException error;
	if (m_blenderData.Open(&error)) {
		m_blenderDataValid = false;
		m_runState = stateLoadBlender;
		InvalidateControl();
	}
	else {
		// Director does not load files asynchronously, try loading it from disk.
		m_runState = stateLoadAsyncFailed;
		InvalidateControl();
		/*
		CString str;
		str.Format("Could not open file: %s.", m_blenderURL);
		showError(str);
		success = false;
		*/
	}
	return success;
}


bool CBlenderPlayerCtrl::startLoadingDownload(void)
{
	bool success = true;
	InternalSetReadyState(READYSTATE_INTERACTIVE);
	m_loadingData.SetPath(m_loadingURL);
	CFileException error;
	if (m_loadingData.Open(&error)) {
		m_loadingDataValid = false;
		m_runState = stateLoadLoading;
		// Force a redraw
		InvalidateControl();
	}
	else {
		m_runState = stateError;
		CString str;
		str.Format("Could not open file: %s.", m_loadingURL);
		success = false;
	}
	return success;
}


bool CBlenderPlayerCtrl::activateBlenderData(CBlenderDataPathProperty& blenderData, bool& isPublisher)
{
	bool success = false;

	// Download of Blender data file complete
	int filelen = blenderData.GetLength();
	if (filelen > 0) {
		// Did receive data, try to read it (should test here)
		CWaitCursor waitCursor;
		blenderData.SeekToBegin();
		unsigned char* buf = (unsigned char*)::malloc(filelen);
		blenderData.Read(buf, filelen);

		// Load the Blender file from memory
		success = LoadGameData(buf, filelen);
		isPublisher = m_gamedata->type == BLENFILETYPE_PUB;
		::free(buf);

		waitCursor.Restore();
	}
	else {
		// Something wrong with the download
		CString msg = "Invalid Blender data.";
		showError(msg);
	}

	return success;
}


void CBlenderPlayerCtrl::checkDownload(void)
{
	switch (m_runState) {
	case stateLoadLoading:
		if (GetReadyState() == READYSTATE_COMPLETE) {
			stopEngine();
			m_loadingDataValid = true;
			startBlenderDownload();
			//m_runState = stateLoadBlender;
			//InternalSetReadyState(READYSTATE_INTERACTIVE);
		}
		break;
	case stateLoadBlender:
		if (GetReadyState() == READYSTATE_COMPLETE) {
			stopEngine();
			m_blenderDataValid = true;
			m_runState = stateRun;
		}
		break;
	}
}


void CBlenderPlayerCtrl::showError(const CString& msg)
{
	// First: show a dialog
	CString caption = "Blender Player Error";
	// Temporary disable for Director 8.5 which chokes on the Dialog
	// MessageBox(msg, caption, MB_ICONERROR);

	if (m_engineRunning) {
		stopEngine();
	}
	m_runState = stateError;
}


void CBlenderPlayerCtrl::sendMessage(
		const STR_String& to,
		const STR_String& from,
		const STR_String& subject,
		const STR_String& body)
{
	if (m_networkdevice) {
		// Store a progress message in the network device.
		NG_NetworkMessage* msg = new NG_NetworkMessage(to, from, subject, body);
		m_networkdevice->SendNetworkMessage(msg);
		msg->Release();
	}
}


void CBlenderPlayerCtrl::updateEngineInfoDisplay(void)
{

	SYS_SystemHandle syshandle = SYS_GetSystem();
	if (syshandle) {
		SYS_WriteCommandLineInt(syshandle, "show_framerate", m_showFrameCount);
		SYS_WriteCommandLineInt(syshandle, "show_profile", m_showProfileInfo);
		SYS_WriteCommandLineInt(syshandle, "show_properties", m_showProperties);
	}
	if (m_ketsjiengine)
	{
		// Timing information display
		m_ketsjiengine->SetTimingDisplay(
			m_showFrameCount==TRUE, m_showProfileInfo==TRUE, m_showProperties==TRUE);
	}
}


float CBlenderPlayerCtrl::determineProgress(void) const
{
	float progress;
	if ((m_blenderData.m_ulProgress > 0) && 
		(m_blenderData.m_ulProgressMax != m_blenderData.m_ulProgress)) {
		progress = (float)m_blenderData.m_ulProgress;
		progress /= (float)m_blenderData.m_ulProgressMax;
	}
	else {
		progress = 0.f;
	}
	return progress ;
}


void CBlenderPlayerCtrl::updateLoadingAnimation(float progress)
{
	// Send a message with the current progress
	STR_String to = "";
	STR_String from = "";
	STR_String subject = "progress";
	STR_String body;
	body.Format("%f", progress);
	sendMessage(to, from, subject, body);
}


void CBlenderPlayerCtrl::updateFrameColor(void)
{
	if (m_engineRunning) {
		m_ketsjiengine->SetUseOverrideFrameColor(!m_useFileBackColor);
		float r, g, b;
		convertOleColorToRGB(GetBackColor(), r, g, b);
		m_ketsjiengine->SetOverrideFrameColor(r, g, b);
	}
}

	
void CBlenderPlayerCtrl::convertColorRefToRGB(COLORREF color, float& r, float& g, float& b)
{
	unsigned char* ptr = (unsigned char*) &color;
	r = ((float)ptr[0]) / ((float)0xFF);
	g = ((float)ptr[1]) / ((float)0xFF);
	b = ((float)ptr[2]) / ((float)0xFF);
}


void CBlenderPlayerCtrl::convertOleColorToRGB(OLE_COLOR color, float& r, float& g, float& b)
{
	COLORREF ref = TranslateColor(color);
	convertColorRefToRGB(ref, r, g, b);
}


void CBlenderPlayerCtrl::processMessages(void)
{
	// Only process messages if the engine is running
	if (m_engineRunning && m_networkdevice) {
		vector<NG_NetworkMessage*> msgs;
		msgs = m_networkdevice->RetrieveNetworkMessages();
		if (msgs.size() > 0) {
			vector<NG_NetworkMessage*>::iterator it;
			/* At the moment, we look for messages for the "host_application"
			 * with subject "load_url" only.
			 */
			for (it = msgs.begin(); it != msgs.end(); it++) {
				NG_NetworkMessage* msg = *it;
				if (msg->GetDestinationName() == s_messageAddress) {
					if (msg->GetSubject() == s_messageLoadURLSubject) {
						loadURL(msg->GetMessageText());
					}
				}
			}
		}
	}
}


void CBlenderPlayerCtrl::loadURL(const STR_String& url)
{
	bool success = false;
	IOleClientSite* pClientSite = GetClientSite();
	if (pClientSite) {
		IOleContainer* pContainer;
		pClientSite->GetContainer(&pContainer);
		if (pContainer) {
			IHTMLDocument2* pHTMLDocument2;
			HRESULT hr;
			hr = pContainer->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDocument2);
			if (hr == S_OK) {
				STR_String genURL = url;
				CString strURL = genURL.Ptr();
				pHTMLDocument2->put_URL(strURL.AllocSysString());
				pHTMLDocument2->Release();
			}
			pContainer->Release();
		}
		pClientSite->Release();
	}
}


void CBlenderPlayerCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// Check whether there was file downloading progress
	checkDownload();

	if (!m_hWnd) {
		/* Can't draw with OpenGL without valid window.
		 * This is really en error situation since OnDrawMetaFile should be called
		 * when the window handle is invalid.
		 */
		return;
	}

	// Save the current time to establish frame drawing time below
	DWORD ticksStart = ::GetTickCount();

	if (m_drawingMetafile) {
		// The control is being activated
		if (m_runState != stateIdle) {
			if (m_hWnd) {
				HDC hDC = this->GetDC()->GetSafeHdc();
				exitEngine();
				startEngine();
			}
		}
		m_drawingMetafile = false;
	}

	if (m_engineRunning) {
		// See if the engine had messages for us
		processMessages();
	}

	float progress;
	switch (m_runState) {
	case stateIdle:
		drawIdle(pdc, rcBounds, rcInvalid);
		break;

	case stateLoadLoading:
		drawLoadLoading(pdc, rcBounds, rcInvalid);
		break;

	case stateLoadBlender:
		if (!m_engineRunning) {
			m_lastProgress = 0;
			if (!startEngine()) {
				InvalidateControl();
			}
		}
		// Downloading data, show download progress
		progress = determineProgress();
		if (progress != m_lastProgress) {
			updateLoadingAnimation(progress);
			m_lastProgress = progress;
		}
		drawLoadBlender(pdc, rcBounds, rcInvalid);
		break;

	case stateRun:
		if (!m_engineRunning) {
			if (!startEngine()) {
				InvalidateControl();
			}
		}
		if (m_exitRequested == KX_EXIT_REQUEST_START_OTHER_GAME)
		{
			// kill the current engine, clean up etc
			exitEngine();
			// reset the exitrequest
			m_exitRequested = KX_EXIT_REQUEST_NO_REQUEST;
			// load/rewind the blendfile
			Rewind();
		}
		else
		{
			drawEngine(pdc, rcBounds, rcInvalid);
		}
		break;

	case stateError:
		if (!m_engineRunning) {
			m_lastProgress = 0;
			if (!startEngine()) {
				InvalidateControl();
			}
		}
		else {
			// Until there is an error animation we show the loading animation
			if (!m_lastProgress) {
				updateLoadingAnimation(100.f);
				m_lastProgress = 100.f;
			}
			drawError(pdc, rcBounds, rcInvalid);
		}
		break;

	case stateLoadAsyncFailed:
		if (!m_engineRunning) {
			if (!startEngine()) {
				InvalidateControl();
			}
			else {
				m_runState = stateRun;
			}
		}
		break;
	}
	/*
	if (m_engineRunning && (m_frameRate > 0)) {
		DWORD ticksEnd = ::GetTickCount();
		DWORD ticks = ticksEnd - ticksStart;
		if (ticks < m_frameRate) {
			UINT nElapse = 1000 / m_frameRate;
			m_refresher.SetInterval(nElapse);
		}
		else {
			m_refresher.SetInterval(ticks);
		}
	}
	*/
}


/*
 * This routine is called when a control is needed to be drawn 
 * to a metafile device context often used when the control is in inactive
 * state.
 * The default implementation (in COleControl) calls OnDraw.
 * We can't use OpenGL when inactive but only routines offered by the device
 * context pDC.
 */
void CBlenderPlayerCtrl::OnDrawMetafile(CDC* pDC, const CRect& rcBounds) 
{
	if (!m_drawingMetafile) {
		// Switched to inactive state
		if (m_engineRunning) {
			stopEngine();
		}
		m_drawingMetafile = true;
	}
	checkDownload();
	drawIdle(pDC, rcBounds, rcBounds);
}


void CBlenderPlayerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	stopEngine();
	m_runState = stateIdle;
	m_frameRate = 60;
	m_showFrameCount = FALSE;
	m_showProfileInfo = FALSE;
	m_showProperties = FALSE;
	m_useFileBackColor = FALSE;
	m_drawingMetafile = false;
	m_blenderDataValid = false;
	m_loadingDataValid = false;
	m_lastProgress = 0;

	InternalSetReadyState(READYSTATE_INTERACTIVE);
	// 1.0.0.3 parameter name:
	// PX_String(pPX, _T("data"), m_blenderURL);
	PX_String(pPX, _T("blenderURL"), m_blenderURL);
	m_initialBlenderURL = m_blenderURL;
	PX_String(pPX, _T("loadingURL"), m_loadingURL);
	m_initialLoadingURL = m_loadingURL;
	PX_Short(pPX, _T("frameRate"), m_frameRate);
	/**
	 * There is a bug in the PX_Bool implementation.
	 * BOOLs with True value are returned as -1!
	 */
	if (PX_Bool(pPX, _T("showFrameCount"), m_showFrameCount)) {
		m_showFrameCount = m_showFrameCount != 0;
	}
	if (PX_Bool(pPX, _T("showProfileInfo"), m_showProfileInfo)) {
		m_showProfileInfo = m_showProfileInfo != 0;
	}
	if (PX_Bool(pPX, _T("showProperties"), m_showProperties)) {
		m_showProperties = m_showProperties != 0;
	}
	if (PX_Bool(pPX, _T("useFileBackColor"), m_useFileBackColor)) {
		m_useFileBackColor = m_useFileBackColor != 0;
	}

	// TEMP 
	//m_blenderURL = "c:\\loadtest.blend";
	//m_initialBlenderURL = m_blenderURL;

	/*
	if (!m_blenderURL.IsEmpty()) {
		startDownload();
	}
	else {
		m_runState = stateIdle;
	}
	*/
	startDownload();
	InvalidateControl();
}


void CBlenderPlayerCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}


void CBlenderPlayerCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_BLENDERPLAYER);
	dlgAbout.DoModal();
}



void CBlenderPlayerCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_keyboard /* && !(nFlags & KF_REPEAT) */) {
		m_keyboard->ConvertWinEvent(nChar, true);
	}
	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CBlenderPlayerCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_keyboard /* && !(nFlags & KF_REPEAT) */) {
		m_keyboard->ConvertWinEvent(nChar, false);
	}
	COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CBlenderPlayerCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertButtonEvent(GPC_MouseDevice::buttonLeft, true, point.x, point.y);
	}
	COleControl::OnLButtonDown(nFlags, point);
}


void CBlenderPlayerCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertButtonEvent(GPC_MouseDevice::buttonLeft, false, point.x, point.y);
	}
	COleControl::OnLButtonUp(nFlags, point);
}


void CBlenderPlayerCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertButtonEvent(GPC_MouseDevice::buttonMiddle, true, point.x, point.y);
	}
	COleControl::OnMButtonDown(nFlags, point);
}


void CBlenderPlayerCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertButtonEvent(GPC_MouseDevice::buttonMiddle, false, point.x, point.y);
	}
	COleControl::OnMButtonUp(nFlags, point);
}


void CBlenderPlayerCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertButtonEvent(GPC_MouseDevice::buttonRight, true, point.x, point.y);
	}
	COleControl::OnRButtonDown(nFlags, point);
}


void CBlenderPlayerCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertButtonEvent(GPC_MouseDevice::buttonRight, false, point.x, point.y);
	}
	COleControl::OnRButtonUp(nFlags, point);
}


void CBlenderPlayerCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_mouse) {
		m_mouse->ConvertMoveEvent(point.x, point.y);
	}
	COleControl::OnMouseMove(nFlags, point);
}


LRESULT CBlenderPlayerCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
#ifdef DO_YOU_WANT_CRASHES_UNDER_WIN98_AND_WIN_ME
	// If the cursor is inside our window, grab the focus so that we get keyboard events
	POINT point;
	if (::GetCursorPos(&point)) {
		if (WindowFromPoint(point) == this) {
			if (this->GetFocus() != this) {
				this->SetFocus();
			}
		}
	}
#endif //DO_YOU_WANT_CRASHES_UNDER_WIN98_AND_WIN_ME
	return COleControl::WindowProc(message, wParam, lParam);
}


BOOL CBlenderPlayerCtrl::PreTranslateMessage(MSG* pMsg) 
{
    bool handleNow = false;
	bool down = false;

    switch (pMsg->message)
	{
//   case WM_ACTIVATE:
//		Beep(1000, 1000);
//		break;
    case WM_KEYDOWN:
		down = true;
		// Fall through
    case WM_KEYUP:
		/*
		 * Grab these keyboard events before IE handles them.
		 * We could also grab VK_FXX keys but currently we don't.
		 * In IE, these keys have special meanings.
		 */
		switch (pMsg->wParam)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_END:
		case VK_HOME:
		case VK_PRIOR:
		case VK_NEXT:
			handleNow = true;
			break;
		}
		if (handleNow) {
			if (down) {
				OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			}
			else {
				OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			}
		}
		break;
    }
    return handleNow ? 1 : COleControl::PreTranslateMessage(pMsg);
}


void CBlenderPlayerCtrl::OnLoadingURLChanged() 
{
	m_loadingDataValid = false;
	startDownload();
	SetModifiedFlag();
}

void CBlenderPlayerCtrl::OnBlenderURLChanged() 
{
	m_blenderDataValid = false;
	startDownload();
	SetModifiedFlag();
}


void CBlenderPlayerCtrl::Rewind() 
{
	if (m_runState == stateRun) {
		stopEngine();
		m_runState = stateIdle;
		m_showFrameCount = FALSE;
		m_showProfileInfo = FALSE;
		m_showProperties = FALSE;
		m_drawingMetafile = false;
		m_blenderDataValid = false;
		m_loadingDataValid = false;
		m_lastProgress = 0;
		InternalSetReadyState(READYSTATE_INTERACTIVE);
		startDownload();
	}
}


void CBlenderPlayerCtrl::OnShowFrameCountChanged() 
{
	updateEngineInfoDisplay();
	SetModifiedFlag();
}


void CBlenderPlayerCtrl::OnShowProfileInfoChanged() 
{
	updateEngineInfoDisplay();
	SetModifiedFlag();
}


void CBlenderPlayerCtrl::OnShowPropertiesChanged() 
{
	updateEngineInfoDisplay();
	SetModifiedFlag();
}


BOOL CBlenderPlayerCtrl::SendMessage(LPCTSTR to, LPCTSTR from, LPCTSTR subject, LPCTSTR body) 
{
	STR_String lto = to;
	STR_String lfrom = from;
	STR_String lsubject = subject;
	STR_String lbody = body;
	sendMessage(lto, lfrom, lsubject, lbody);
	return TRUE;
}


DWORD CBlenderPlayerCtrl::GetControlFlags() 
{
	//return clipPaintDC;
	return COleControl::GetControlFlags();
}

void CBlenderPlayerCtrl::OnForeColorChanged() 
{
	COleControl::OnForeColorChanged();
}

void CBlenderPlayerCtrl::OnBackColorChanged() 
{
	updateFrameColor();	
	COleControl::OnBackColorChanged();
}

void CBlenderPlayerCtrl::OnFrameRateChanged() 
{
	if (m_engineRunning && (m_frameRate > 0)) {
		UINT nElapse = 1000 / m_frameRate;
		m_refresher.SetInterval(nElapse);
	}
	SetModifiedFlag();
}

void CBlenderPlayerCtrl::OnUseFileBackColorChanged() 
{
	updateFrameColor();
	SetModifiedFlag();
}
