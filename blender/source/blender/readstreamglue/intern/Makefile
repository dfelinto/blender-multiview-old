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

LIBNAME = readstreamglue
DIR = $(OCGDIR)/blender/$(LIBNAME)

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_2_C_WARNINGS)

# path to our own private header files
CPPFLAGS += -I.

# path to our own external header files
CPPFLAGS += -I..

# external modules
CPPFLAGS += -I../../../kernel/gen_messaging
CPPFLAGS += -I../../blenloader
CPPFLAGS += -I../../inflate
CPPFLAGS += -I../../decrypt
CPPFLAGS += -I../../verify

CPPFLAGS += -I$(NAN_OPENSSL)/include

#TODO make keystore a seperate lib
CPPFLAGS += -I$(NAN_BLENKEY)/include

ifeq ($(OS),$(findstring $(OS), "solaris windows"))
    CPPFLAGS += -I$(NAN_ZLIB)/include
endif

