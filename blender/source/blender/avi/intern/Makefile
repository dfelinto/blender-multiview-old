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

LIBNAME = avi
DIR = $(OCGDIR)/blender/$(LIBNAME)

include nan_compile.mk

CFLAGS += $(LEVEL1_C_WARNINGS)

ifeq ($(CPU),$(findstring $(CPU), "powerpc mips sparc"))
    CPPFLAGS += -DWORDS_BIGENDIAN
else
    # alpha i386
    CPPFLAGS += -DWORDS_LITTLEENDIAN
endif

# the JPEG library
CPPFLAGS += -I$(NAN_JPEG)/include
# path to the guarded memory allocator
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
# our own include
CPPFLAGS += -I..

