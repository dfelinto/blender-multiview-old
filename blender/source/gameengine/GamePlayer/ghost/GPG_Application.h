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
 * GHOST Blender Player application declaration file.
 */

#include "GHOST_IEventConsumer.h"

#include "STR_String.h"

class KX_KetsjiEngine;
class KX_ISceneConverter;
class NG_LoopBackNetworkDeviceInterface;
class SND_IAudioDevice;
class RAS_IRasterizer;
class GHOST_IEvent;
class GHOST_ISystem;
class GHOST_ITimerTask;
class GHOST_IWindow;
class GPC_MouseDevice;
class GPC_RenderTools;
class GPG_Canvas;
class GPG_KeyboardDevice;
class GPG_System;
struct Main;

class GPG_Application : public GHOST_IEventConsumer
{
public:
	GPG_Application(GHOST_ISystem* system, struct Main *maggie, STR_String startSceneName);
	~GPG_Application(void);

			bool SetGameEngineData(struct Main *maggie, STR_String startSceneName);
			bool startWindow(STR_String& title, int windowLeft, int windowTop, int windowWidth, int windowHeight,
			const bool stereoVisual, const int stereoMode);
			bool startFullScreen(int width, int height, int bpp, int frequency, const bool stereoVisual, const int stereoMode);
	virtual	bool processEvent(GHOST_IEvent* event);
			int getExitRequested(void);
			const STR_String& getExitString(void);
			bool StartGameEngine(int stereoMode);
			void StopGameEngine();

protected:
	bool	handleButton(GHOST_IEvent* event, bool isDown);
	bool	handleCursorMove(GHOST_IEvent* event);
	bool	handleKey(GHOST_IEvent* event, bool isDown);

	/**
	 * Initializes the game engine.
	 */
	bool initEngine(GHOST_IWindow* window, int stereoMode);

	/**
	 * Starts the game engine.
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

	/* The game data */
	STR_String				m_startSceneName;
	struct Main*			m_maggie;

	/* Exit state. */
	int						m_exitRequested;
	STR_String				m_exitString;
	/* GHOST system abstraction. */
	GHOST_ISystem*			m_system;
	/* Main window. */
	GHOST_IWindow*			m_mainWindow;
	/* Timer to advance frames. */
	GHOST_ITimerTask*		m_frameTimer;
	/* The cursor shape displayed. */
	GHOST_TStandardCursor	m_cursor;

	/** Engine construction state. */
	bool m_engineInitialized;
	/** Engine state. */
	bool m_engineRunning;

	/** the gameengine itself */
	KX_KetsjiEngine* m_ketsjiengine;
	/** The game engine's system abstraction. */
	GPG_System* m_kxsystem;
	/** The game engine's keyboard abstraction. */
	GPG_KeyboardDevice* m_keyboard;
	/** The game engine's mouse abstraction. */
	GPC_MouseDevice* m_mouse;
	/** The game engine's canvas abstraction. */
	GPG_Canvas* m_canvas;
	/** The game engine's platform dependent render tools. */
	GPC_RenderTools* m_rendertools;
	/** the rasterizer */
	RAS_IRasterizer* m_rasterizer;
	/** Converts Blender data files. */
	KX_ISceneConverter* m_sceneconverter;
	/** Network interface. */
	NG_LoopBackNetworkDeviceInterface* m_networkdevice;
	/** Sound device. */
	SND_IAudioDevice* m_audiodevice;
};


