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

LIBNAME = ns_api_test_colorcycle
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/test

include nan_compile.mk

CPPFLAGS += -DXP_UNIX
CFLAGS += $(LEVEL_1_C_WARNINGS) $(LEVEL_2_C_WARNINGS) $(FIX_STUBS_WARNINGS)

CPPFLAGS += -I../../src
CPPFLAGS += -I../../netscape_api
CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_NSPR)/include
CPPFLAGS += -I$(NAN_NSPR)/include/nspr
CPPFLAGS += -I$(NAN_MOZILLA_INC)/nspr

