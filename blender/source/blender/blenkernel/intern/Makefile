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

LIBNAME = blenkernel
DIR = $(OCGDIR)/blender/$(LIBNAME)

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd openbsd linux solaris windows"))
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_1_C_WARNINGS)

# OpenGL and Python
CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)

CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
# Reference to the types in makesdna and imbuf
CPPFLAGS += -I../../makesdna
CPPFLAGS += -I../../imbuf
# This mod uses the BLI and BLO module
CPPFLAGS += -I../../blenlib
CPPFLAGS += -I../../blenloader
CPPFLAGS += -I../../bpython/include
# also avi is used
CPPFLAGS += -I../../avi
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include

# we still refer to /include a bit...
CPPFLAGS += -I../../include

# to include the render stuff:
CPPFLAGS += -I../../render/extern/include

# for sound
#CPPFLAGS += -I../../../kernel/gen_system

CPPFLAGS += -I$(NAN_IKSOLVER)/include

# path to our own external headerfiles
CPPFLAGS += -I..

