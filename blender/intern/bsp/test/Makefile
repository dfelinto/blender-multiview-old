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
# bsp test makefile.
#

LIBNAME = bsp
SOURCEDIR = intern/$(LIBNAME)/test
DIR = $(OCGDIR)/$(SOURCEDIR)

include nan_compile.mk

DIRS = BSP_GhostTest

include nan_subdirs.mk

include nan_link.mk

LIBS = $(OCGDIR)/intern/$(LIBNAME)/test/BSP_GhostTest/$(DEBUG_DIR)libBSP_GhostTest.a
LIBS += $(OCGDIR)/intern/$(LIBNAME)/$(DEBUG_DIR)libbsp.a

SLIBS += $(NAN_MOTO)/lib/$(DEBUG_DIR)libmoto.a
SLIBS += $(NAN_GHOST)/lib/$(DEBUG_DIR)libghost.a
SLIBS += $(NAN_STRING)/lib/$(DEBUG_DIR)libstring.a

ifeq ($(OS),$(findstring $(OS), "beos darwin linux freebsd openbsd"))
    LLIBS = -L/usr/X11R6/lib -lglut -pthread -lXi -lXmu
endif

all debug:: $(LIBS) $(DIR)/$(DEBUG_DIR)BSPGhostTest

$(DIR)/$(DEBUG_DIR)BSPGhostTest:
	@echo "****> linking $@ in $(DIR)" 
	$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)BSPGhostTest $(LIBS) $(SLIBS) $(LLIBS) $(DADD)

clean::
	$(RM) $(DIR)/BSPGhostTest $(DIR)/debug/BSPGhostTest

test:: all
	$(DIR)/BSPGhostTest





