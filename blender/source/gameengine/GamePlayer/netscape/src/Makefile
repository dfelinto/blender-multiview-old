#
# $Id$
#
# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
# ***** END GPL LICENSE BLOCK *****
#
# Makefile for Netscape-api style plugin base.

LIBNAME = ns_api_plugin
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/src

include nan_compile.mk

# The XP_UNIX is for making the ns library switch to unix hooks.
ifeq ($(OS),$(findstring $(OS), "solaris linux freebsd irix"))
CPPFLAGS += -c -DXP_UNIX 
endif

CPPFLAGS += $(LEVEL_2_CPP_WARNINGS) 

# string library
CPPFLAGS += -I$(NAN_STRING)/include    

# only plugin related:
CPPFLAGS += -I$(NAN_NSPR)/include
CPPFLAGS += -I$(NAN_NSPR)/include/nspr
CPPFLAGS += -I$(NAN_MOZILLA_INC)/nspr
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
