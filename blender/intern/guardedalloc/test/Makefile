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
# Test the guarded memory module
#

LIBNAME = guardedalloc
SOURCEDIR = intern/$(LIBNAME)/test
DIR = $(OCGDIR)/$(SOURCEDIR)
DIRS = simpletest

include nan_subdirs.mk

include nan_compile.mk
include nan_link.mk

TESTLIBS  = $(OCGDIR)/intern/$(LIBNAME)/$(DEBUG_DIR)lib$(LIBNAME).a

all debug::
	@echo "****> linking $@ in $(SOURCEDIR)"
	$(CC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)memtest $(DIR)/memtest.o $(TESTLIBS)

clean::
	$(RM) $(DIR)/memtest $(DIR)/debug/memtest

test:: $(DIR)/memtest
	$(DIR)/memtest $(NAN_TEST_VERBOSITY)

