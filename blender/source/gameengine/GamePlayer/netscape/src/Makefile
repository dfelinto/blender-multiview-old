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

LIBNAME = ns_api_plugin
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/src

include nan_compile.mk

# The XP_UNIX is for making the ns library switch to unix hooks.
ifeq ($(OS),$(findstring $(OS), "solaris linux freebsd irix"))
CPPFLAGS += -c -DXP_UNIX 
endif

CPPFLAGS += $(LEVEL_2_C_WARNINGS) 

# string library
CPPFLAGS += -I$(NAN_STRING)/include    

# only plugin related:
CPPFLAGS += -I$(NAN_NSPR)/include
CPPFLAGS += -I../netscape_api
CPPFLAGS += -I../common
CPPFLAGS += -I../test
CPPFLAGS += -Ixpcom
CPPFLAGS += -Iketsji
CPPFLAGS += -I.

# game engine: main chunks:
CPPFLAGS += -I../../../GameLogic
CPPFLAGS += -I../../../Rasterizer
#CPPFLAGS += -I../../../Network/LoopBackNetwork

# physics
CPPFLAGS += -I../../../../sumo/Fuzzics/include

# moto
CPPFLAGS += -I$(NAN_MOTO)/include

# player dirs:
CPPFLAGS += -I../../common
CPPFLAGS += -I../../common/unix

# general system:
CPPFLAGS += -I../../../../kernel/gen_system
CPPFLAGS += -I../../../../kernel/gen_messaging

# blenderish stuff
CPPFLAGS += -I../../../../blender/blenlib
CPPFLAGS += -I../../../../blender/blenkernel
CPPFLAGS += -I../../../../blender/blenloader
CPPFLAGS += -I../../../../blender/makesdna


# needed on linux? why? for blender_plugin_types
CPPFLAGS += -I$(OPENGL_HEADERS)

###########################
# Also do the ketsji version of the hooks. Tests are separate.
SOURCEDIR = source/gameengine/GamePlayer/netscape/src

ifeq ($(OS),$(findstring $(OS), "freebsd irix solaris windows"))
DIRS = ketsji
endif

ifeq ($(OS),$(findstring $(OS), "linux"))
  ifeq ($(CPU),i386)
    DIRS = ketsji xpcom
  endif
endif


include nan_subdirs.mk
