#
# $Id$
#
# ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
#
# The contents of this file may be used under the terms of either the GNU
# General Public License Version 2 or later (the "GPL", see
# http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
# later (the "BL", see http://www.blender.org/BL/ ) which has to be
# bought from the Blender Foundation to become active, in which case the
# above mentioned GPL option does not apply.
#
# The Original Code is Copyright (C) 2002 by NaN Holding BV.
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
# ***** END GPL/BL DUAL LICENSE BLOCK *****
# ghost intern Makefile
#

LIBNAME = ghost
DIR = $(OCGDIR)/intern/$(LIBNAME)

CCSRCS = GHOST_Buttons.cpp GHOST_System.cpp GHOST_Window.cpp
CCSRCS += GHOST_EventManager.cpp GHOST_EventPrinter.cpp GHOST_WindowManager.cpp
CCSRCS += GHOST_ISystem.cpp GHOST_ModifierKeys.cpp GHOST_TimerManager.cpp
CCSRCS += GHOST_Rect.cpp GHOST_DisplayManager.cpp GHOST_C-api.cpp
CCSRCS += GHOST_CallbackEventConsumer.cpp

include nan_definitions.mk

ifeq ($(OS),$(findstring $(OS), "darwin"))
    CCSRCS += $(wildcard *Carbon.cpp)
endif

ifeq ($(OS),$(findstring $(OS), "windows"))
    CCSRCS += $(wildcard *Win32.cpp)
endif

ifeq ($(OS),$(findstring $(OS), "freebsd irix linux openbsd solaris"))
    CCSRCS += $(wildcard *X11.cpp)
endif

include nan_compile.mk

#CCFLAGS += $(LEVEL_2_CPP_WARNINGS)

CPPFLAGS += -I$(NAN_STRING)/include
CPPFLAGS += -I$(NAN_MEMUTIL)/include
CPPFLAGS += -I..
CPPFLAGS += -I$(OPENGL_HEADERS)

