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
# radiosity uses the render lib
#

LIBNAME = radiosity
DIR = $(OCGDIR)/blender/$(LIBNAME)

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_1_C_WARNINGS)

CPPFLAGS += -I$(OPENGL_HEADERS)

# not very neat....
CPPFLAGS += -I../../../blenkernel
CPPFLAGS += -I../../../blenlib
CPPFLAGS += -I../../../makesdna
CPPFLAGS += -I../../../imbuf
CPPFLAGS += -I../../../
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include

# first /include is my own includes, second are the external includes
# third is the external interface. there should be a nicer way to say this 
CPPFLAGS += -I../include -I../../../include -I../../extern/include
CPPFLAGS += -I../../../render/extern/include -I../../../misc

