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
#

LIBNAME = creator
DIR = $(OCGDIR)/creator

# this HAS to go
CSRCS = creator.c

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd
solaris windows"))
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_1_C_WARNINGS)

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I../blender/render/extern/include
CPPFLAGS += -I../blender/radiosity/extern/include
CPPFLAGS += -I../blender/misc
# two needed for the kernel
CPPFLAGS += -I../blender/imbuf
CPPFLAGS += -I../blender/makesdna
CPPFLAGS += -I../blender/blenlib
CPPFLAGS += -I../blender/include
CPPFLAGS += -I../blender/renderconverter
CPPFLAGS += -I../blender/blenkernel
CPPFLAGS += -I../blender/bpython/include
CPPFLAGS += -I../blender/blenloader
CPPFLAGS += -I../kernel/gen_system
CPPFLAGS += -I../kernel/gen_messaging
CPPFLAGS += -I$(NAN_BLENKEY)/include
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
