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

LIBNAME = ns_api_test_stub
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/test

include nan_compile.mk

CPPFLAGS += -DXP_UNIX
CPPFLAGS += $(LEVEL_1_C_WARNINGS) $(LEVEL_2_C_WARNINGS) $(FIX_STUBS_WARNINGS)

CPPFLAGS += -I../../src
CPPFLAGS += -I../../netscape_api
CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_NSPR)/include

