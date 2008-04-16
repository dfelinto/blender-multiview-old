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

