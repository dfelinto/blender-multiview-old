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
# container main makefile.
#

include nan_definitions.mk

LIBNAME = container
SOURCEDIR = intern/$(LIBNAME)
DIR = $(OCGDIR)/$(SOURCEDIR)
DIRS = intern
#not yet TESTDIRS = test
 
include nan_subdirs.mk

install: all debug
	@[ -d $(NAN_CONTAINER) ] || mkdir $(NAN_CONTAINER)
	@[ -d $(NAN_CONTAINER)/include ] || mkdir $(NAN_CONTAINER)/include
	@[ -d $(NAN_CONTAINER)/lib ] || mkdir $(NAN_CONTAINER)/lib
	@[ -d $(NAN_CONTAINER)/lib/debug ] || mkdir $(NAN_CONTAINER)/lib/debug
	cp -f $(DIR)/libcontainer.a $(NAN_CONTAINER)/lib/
	cp -f $(DIR)/debug/libcontainer.a $(NAN_CONTAINER)/lib/debug
	cp -f *.h $(NAN_CONTAINER)/include/

