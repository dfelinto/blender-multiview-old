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
# iksolver main makefile.
#

include nan_definitions.mk

LIBNAME = iksolver
SOURCEDIR = intern/$(LIBNAME)
DIR = $(OCGDIR)/$(SOURCEDIR)
DIRS = intern
TESTDIRS = test

include nan_subdirs.mk

install: all debug
	@[ -d $(NAN_IKSOLVER) ] || mkdir $(NAN_IKSOLVER)
	@[ -d $(NAN_IKSOLVER)/include ] || mkdir $(NAN_IKSOLVER)/include
	@[ -d $(NAN_IKSOLVER)/lib ] || mkdir $(NAN_IKSOLVER)/lib
	@[ -d $(NAN_IKSOLVER)/lib/debug ] || mkdir $(NAN_IKSOLVER)/lib/debug
	cp -f $(DIR)/libiksolver.a $(NAN_IKSOLVER)/lib/
	cp -f $(DIR)/debug/libiksolver.a $(NAN_IKSOLVER)/lib/debug/
	cp -f extern/*.h $(NAN_IKSOLVER)/include/

