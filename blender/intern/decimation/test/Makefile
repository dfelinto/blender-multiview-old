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
# decimation test makefile.
#

LIBNAME = decimation
SOURCEDIR = intern/$(LIBNAME)/test
DIR = $(OCGDIR)/$(SOURCEDIR)

include nan_compile.mk

DIRS = decimate_glut_test

include nan_subdirs.mk

include nan_link.mk

LIBS = $(DIR)/decimate_glut_test/intern/$(DEBUG_DIR)libdecimationtest.a
LIBS += $(DIR)/decimate_glut_test/intern/common/$(DEBUG_DIR)libcommon.a
LIBS += $(DIR)/decimate_glut_test/intern/ply/$(DEBUG_DIR)libply.a
LIBS += $(OCGDIR)/intern/$(LIBNAME)/$(DEBUG_DIR)libdecimation.a

SLIBS += $(NAN_MOTO)/lib/libmoto.a
SLIBS += $(NAN_GHOST)/lib/libghost.a
SLIBS += $(NAN_STRING)/lib/libstring.a


all debug:: $(LIBS) 
	@echo "****> linking $@ in $(DIR)"
	$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)decimationtest $(LIBS) $(SLIBS) $(LLIBS) $(DADD)

clean::
	$(RM) $(DIR)/decimationtest $(DIR)/debug/decimationtest

test:: all
	cd decimate_glut_test && $(DIR)/decimationtest

