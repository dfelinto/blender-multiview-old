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

LIBNAME = plugin_ketsji_hooks
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/src

include nan_compile.mk

CPPFLAGS += -c 

ifeq ($(OS),$(findstring $(OS), "freebsd irix solaris linux"))
CPPFLAGS +=-DXP_UNIX
endif

CPPFLAGS += $(LEVEL_1_CPP_WARNINGS) 

# only plugin related:
CPPFLAGS += -I.
CPPFLAGS += -I..
CPPFLAGS += -I../../netscape_api
CPPFLAGS += -I../../common
CPPFLAGS += -I$(NAN_NSPR)/include
CPPFLAGS += -I$(NAN_NSPR)/include/nspr
CPPFLAGS += -I$(NAN_MOZILLA_INC)/nspr

# game engine: main chunks:
CPPFLAGS += -I../../../../Expressions
CPPFLAGS += -I../../../../Converter
CPPFLAGS += -I../../../../Ketsji
CPPFLAGS += -I../../../../Ketsji/KXNetwork
CPPFLAGS += -I../../../../GameLogic
CPPFLAGS += -I../../../../Rasterizer
CPPFLAGS += -I../../../../Rasterizer/RAS_OpenGLRasterizer
CPPFLAGS += -I../../../../SceneGraph
CPPFLAGS += -I../../../../Network
CPPFLAGS += -I../../../../Network/LoopBackNetwork

# moto
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
CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)

