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
# bmfont main makefile.
#

include nan_definitions.mk

LIBNAME = bmfont
SOURCEDIR = intern/$(LIBNAME)
DIR = $(OCGDIR)/$(SOURCEDIR)
DIRS = intern
#not ready yet TESTDIRS = test

include nan_subdirs.mk

install: all debug
	@[ -d $(NAN_BMFONT) ] || mkdir $(NAN_BMFONT)
	@[ -d $(NAN_BMFONT)/include ] || mkdir $(NAN_BMFONT)/include
	@[ -d $(NAN_BMFONT)/lib ] || mkdir $(NAN_BMFONT)/lib
	@[ -d $(NAN_BMFONT)/lib/debug ] || mkdir $(NAN_BMFONT)/lib/debug
	cp -f $(DIR)/libbmfont.a $(NAN_BMFONT)/lib/
	cp -f $(DIR)/debug/libbmfont.a $(NAN_BMFONT)/lib/debug/
	cp -f *.h $(NAN_BMFONT)/include/

