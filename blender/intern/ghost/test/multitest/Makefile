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
# GHOST gears test application Makefile
#

DIR = $(OCGDIR)/intern/ghost/test

# we don't want a library here, only object files:
ALLTARGETS = $(OBJS)

include nan_compile.mk
include nan_link.mk

CFLAGS += $(LEVEL_1_C_WARNINGS)
CCFLAGS += $(LEVEL_1_CPP_WARNINGS)

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_STRING)/include
CPPFLAGS += -I$(NAN_BMFONT)/include
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
CPPFLAGS += -I../..

OCGGHOST = $(OCGDIR)/intern/ghost

LIBS = $(OCGGHOST)/$(DEBUG_DIR)libghost.a
SLIBS += $(LCGDIR)/string/lib/libstring.a
SLIBS += $(LCGDIR)/bmfont/lib/libbmfont.a
SLIBS += $(LCGDIR)/guardedalloc/lib/libguardedalloc.a

all::
	@echo "- link $(DIR)/$(DEBUG_DIR)multitest -"
	@$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)multitest $(OBJS) $(LIBS) $(SLIBS) $(LLIBS) $(DADD) $(LOPTS)
