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
#

LIBNAME = readblenfile
DIR = $(OCGDIR)/blender/$(LIBNAME)
ALLTARGETS = $(OBJS) $(DIR)/$(DEBUG_DIR)test$(EXT)

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_2_C_WARNINGS)

# path to our own external headerfiles
CPPFLAGS += -I..
CPPFLAGS += -I../../readstreamglue

$(DIR)/$(DEBUG_DIR)test$(EXT): $(OBJS) ../intern/BLO_readblenfile.c
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(OCGDIR)/blender/readblenfile/$(DEBUG_DIR)libreadblenfile.a
	$(DIR)/$(DEBUG_DIR)test$(EXT)

