if X11
BUILDPS = \
	intern/GHOST_DisplayManagerX11.cpp \
	intern/GHOST_DisplayManagerX11.h \
	intern/GHOST_SystemX11.cpp \
	intern/GHOST_SystemX11.h \
	intern/GHOST_WindowManager.cpp \
	intern/GHOST_WindowManager.h \
	intern/GHOST_WindowX11.cpp \
	intern/GHOST_WindowX11.h

DISTPS = \
	intern/GHOST_DisplayManagerWin32.cpp \
	intern/GHOST_DisplayManagerWin32.h \
	intern/GHOST_SystemWin32.cpp \
	intern/GHOST_SystemWin32.h \
	intern/GHOST_WindowWin32.cpp \
	intern/GHOST_WindowWin32.h \
	intern/GHOST_DisplayManagerCarbon.cpp \
	intern/GHOST_DisplayManagerCarbon.h \
	intern/GHOST_SystemCarbon.cpp \
	intern/GHOST_SystemCarbon.h \
	intern/GHOST_WindowCarbon.cpp \
	intern/GHOST_WindowCarbon.h
endif

if WIN32
BUILDPS = \
	intern/GHOST_DisplayManagerWin32.cpp \
	intern/GHOST_DisplayManagerWin32.h \
	intern/GHOST_SystemWin32.cpp \
	intern/GHOST_SystemWin32.h \
	intern/GHOST_WindowWin32.cpp \
	intern/GHOST_WindowWin32.h

DISTPS = \
	intern/GHOST_DisplayManagerX11.cpp \
	intern/GHOST_DisplayManagerX11.h \
	intern/GHOST_SystemX11.cpp \
	intern/GHOST_SystemX11.h \
	intern/GHOST_WindowManager.cpp \
	intern/GHOST_WindowManager.h \
	intern/GHOST_WindowX11.cpp \
	intern/GHOST_WindowX11.h \
	intern/GHOST_DisplayManagerCarbon.cpp \
	intern/GHOST_DisplayManagerCarbon.h \
	intern/GHOST_SystemCarbon.cpp \
	intern/GHOST_SystemCarbon.h \
	intern/GHOST_WindowCarbon.cpp \
	intern/GHOST_WindowCarbon.h
endif

if CARBON
BUILDPS = \
	intern/GHOST_DisplayManagerCarbon.cpp \
	intern/GHOST_DisplayManagerCarbon.h \
	intern/GHOST_SystemCarbon.cpp \
	intern/GHOST_SystemCarbon.h \
	intern/GHOST_WindowCarbon.cpp \
	intern/GHOST_WindowCarbon.h

DISTPS = \
	intern/GHOST_DisplayManagerX11.cpp \
	intern/GHOST_DisplayManagerX11.h \
	intern/GHOST_SystemX11.cpp \
	intern/GHOST_SystemX11.h \
	intern/GHOST_WindowManager.cpp \
	intern/GHOST_WindowManager.h \
	intern/GHOST_WindowX11.cpp \
	intern/GHOST_WindowX11.h \
	intern/GHOST_DisplayManagerWin32.cpp \
	intern/GHOST_DisplayManagerWin32.h \
	intern/GHOST_SystemWin32.cpp \
	intern/GHOST_SystemWin32.h \
	intern/GHOST_WindowWin32.cpp \
	intern/GHOST_WindowWin32.h
endif

noinst_LTLIBRARIES = libblender_GHOST.la  

INCLUDES = -I$(top_srcdir)/intern/string

libblender_GHOST_la_SOURCES = \
	GHOST_C-api.h \
	GHOST_IEvent.h \
	GHOST_IEventConsumer.h \
	GHOST_ISystem.h \
	GHOST_ITimerTask.h \
	GHOST_IWindow.h \
	GHOST_Rect.h \
	GHOST_Types.h \
	intern/GHOST_Buttons.cpp \
	intern/GHOST_Buttons.h \
	intern/GHOST_C-api.cpp \
	intern/GHOST_CallbackEventConsumer.cpp \
	intern/GHOST_CallbackEventConsumer.h \
	intern/GHOST_Debug.h \
	intern/GHOST_DisplayManager.cpp \
	intern/GHOST_DisplayManager.h \
	intern/GHOST_Event.h \
	intern/GHOST_EventButton.h \
	intern/GHOST_EventCursor.h \
	intern/GHOST_EventKey.h \
	intern/GHOST_EventManager.cpp \
	intern/GHOST_EventManager.h \
	intern/GHOST_EventPrinter.cpp \
	intern/GHOST_EventPrinter.h \
	intern/GHOST_ISystem.cpp \
	intern/GHOST_ModifierKeys.cpp \
	intern/GHOST_ModifierKeys.h \
	intern/GHOST_Rect.cpp \
	intern/GHOST_System.cpp \
	intern/GHOST_System.h \
	intern/GHOST_TimerManager.cpp \
	intern/GHOST_TimerManager.h \
	intern/GHOST_TimerTask.h \
	intern/GHOST_Window.cpp \
	intern/GHOST_Window.h \
	$(BUILDPS)

EXTRA_DIST = \
	Makefile \
	intern/Makefile \
	doc/ghost_all.cfg \
	doc/ghost_interface.cfg \
	make/msvc/ghost.dsp \
	make/msvc/ghost.dsw \
	make/pb/ghost.pbproj \
	$(DISTPS)
