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

LIBNAME = blenpluginapi
DIR = $(OCGDIR)/blender/$(LIBNAME)

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
endif

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris"))
    CFLAGS += -shared
endif

CPPFLAGS += $(LEVEL_2_C_WARNINGS)

# path to our own external headerfiles. On win2k this needs to be
# longer, to avoid the util.h include problem
CPPFLAGS += -I..
ifeq ($(OS),windows)
    CPPFLAGS += -I../..
endif
# also do dna
CPPFLAGS += -I../../makesdna
# path to blenlib
CPPFLAGS += -I../../blenlib
# path to imbuf
CPPFLAGS += -I../../imbuf
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include

