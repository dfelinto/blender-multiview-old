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

LIBNAME = imbuf
DIR = $(OCGDIR)/blender/imbuf

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_1_C_WARNINGS)

CPPFLAGS += -I$(NAN_JPEG)/include
CPPFLAGS += -I$(NAN_PNG)/include
CPPFLAGS += -I$(NAN_ZLIB)/include
CPPFLAGS += -I../../include
CPPFLAGS += -I../../blenkernel
CPPFLAGS += -I../../blenlib
CPPFLAGS += -I../../avi
# path to the guarded memory allocator
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
# This is not really needed, but until /include is cleaned, it must be
# there for proper compilation.
# - No, it is also needed in antialias, for listbase (nzc)
CPPFLAGS += -I../../makesdna
# external interface of this module
CPPFLAGS += -I..

