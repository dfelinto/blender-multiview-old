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
# iksolver test makefile.
#

LIBNAME = iksolver
SOURCEDIR = intern/$(LIBNAME)/test
DIR = $(OCGDIR)/$(SOURCEDIR)

include nan_compile.mk

DIRS = ik_glut_test

include nan_subdirs.mk

include nan_link.mk

LIBS = $(OCGDIR)/intern/$(LIBNAME)/test/ik_glut_test/intern/$(DEBUG_DIR)libintern.a
LIBS +=  $(OCGDIR)/intern/$(LIBNAME)/test/ik_glut_test/common/$(DEBUG_DIR)libcommon.a
LIBS += $(OCGDIR)/intern/$(LIBNAME)/$(DEBUG_DIR)libiksolver.a

SLIBS += $(NAN_MOTO)/lib/$(DEBUG_DIR)libmoto.a

ifeq ($(OS),$(findstring $(OS), "beos darwin linux freebsd openbsd"))
    LLIBS = -L/usr/X11R6/lib -lglut -pthread
endif

all debug:: $(LIBS) $(DIR)/$(DEBUG_DIR)iksolvertest

$(DIR)/$(DEBUG_DIR)iksolvertest:
	@echo "****> linking $@ in $(DIR)"
	$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)iksolvertest $(LIBS) $(SLIBS) $(LLIBS) $(DADD)

clean::
	$(RM) $(DIR)/iksolvertest $(DIR)/debug/iksolvertest

test:: all
	$(DIR)/iksolvertest

