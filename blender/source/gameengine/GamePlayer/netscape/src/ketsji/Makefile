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
#
# Makefile for Netscape-api style plugin base.

LIBNAME = plugin_ketsji_hooks
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/src

include nan_compile.mk

CPPFLAGS += -c 

ifeq ($(OS),$(findstring $(OS), "freebsd irix solaris linux"))
CPPFLAGS +=-DXP_UNIX
endif

CPPFLAGS += $(LEVEL_1_C_WARNINGS) 

# only plugin related:
CPPFLAGS += -I.
CPPFLAGS += -I..
CPPFLAGS += -I../../netscape_api
CPPFLAGS += -I../../common
CPPFLAGS += -I$(NAN_NSPR)/include

# game engine: main chunks:
CPPFLAGS += -I../../../../Expressions
CPPFLAGS += -I../../../../Converter
CPPFLAGS += -I../../../../Ketsji
CPPFLAGS += -I../../../../Ketsji/KXNetwork
CPPFLAGS += -I../../../../GameLogic
CPPFLAGS += -I../../../../Rasterizer
CPPFLAGS += -I../../../../Rasterizer/RAS_OpenGLRasterizer
CPPFLAGS += -I../../../../SceneGraph
CPPFLAGS += -I../../../../SoundSystem
CPPFLAGS += -I../../../../Network
CPPFLAGS += -I../../../../Network/LoopBackNetwork

# physics
CPPFLAGS += -I../../../../../sumo/Fuzzics/include

# moto
CPPFLAGS += -I../../../../../sumo/Fuzzics/include
CPPFLAGS += -I../../../../../sumo/include
CPPFLAGS += -I$(NAN_MOTO)/include

# player dirs:
CPPFLAGS += -I../../../common
CPPFLAGS += -I../../../common/unix
CPPFLAGS += -I../../../common/windows

# blenderish stuff
CPPFLAGS += -I../../../../../blender/blenlib
CPPFLAGS += -I../../../../../blender/blenkernel
CPPFLAGS += -I../../../../../blender/blenloader
CPPFLAGS += -I../../../../../blender/makesdna

# general system and other libs:
CPPFLAGS += -I../../../../../kernel/gen_system
CPPFLAGS += -I../../../../../kernel/gen_messaging
CPPFLAGS += -I$(NAN_STRING)/include
CPPFLAGS += -I$(NAN_BMFONT)/include
CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)
