if X11
BUILDPS = \
	intern/GHOST_DisplayManagerX11.cpp \
	intern/GHOST_SystemX11.cpp \
	intern/GHOST_WindowManager.cpp \
	intern/GHOST_WindowX11.cpp
endif

if WIN32
BUILDPS = \
	intern/GHOST_DisplayManagerWin32.cpp \
	intern/GHOST_SystemWin32.cpp \
	intern/GHOST_WindowWin32.cpp
endif

if CARBON
BUILDPS = \
	intern/GHOST_DisplayManagerCarbon.cpp \
	intern/GHOST_SystemCarbon.cpp \
	intern/GHOST_WindowCarbon.cpp
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
	intern/GHOST_C-api.cpp \
	intern/GHOST_CallbackEventConsumer.cpp \
	intern/GHOST_DisplayManager.cpp \
	intern/GHOST_EventManager.cpp \
	intern/GHOST_EventPrinter.cpp \
	intern/GHOST_ISystem.cpp \
	intern/GHOST_ModifierKeys.cpp \
	intern/GHOST_Rect.cpp \
	intern/GHOST_System.cpp \
	intern/GHOST_TimerManager.cpp \
	intern/GHOST_Window.cpp \
	$(BUILDPS)
