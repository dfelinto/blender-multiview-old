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

ORGLIBNAME = src
LIBNAME ?= $(ORGLIBNAME)
DIR ?= $(OCGDIR)/blender/$(ORGLIBNAME)

CSRCS ?= $(wildcard *.c)
PUB_CSRCS ?= $(CSRCS) $(wildcard pub/*.c)
CRE_CSRCS ?= $(CSRCS) $(wildcard cre/*.c)

OBJS = $(OCGDIR)/blender/makesdna/$(DEBUG_DIR)DNA.o

ALLTARGETS = creator publisher

include nan_compile.mk

# Cflags stuff ------------------------------------------

CFLAGS += $(LEVEL_1_C_WARNINGS) 
#CFLAGS += $(LEVEL_1_C_WARNINGS) -diag_error 1196

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
endif

# PreProcessor stuff ------------------------------------------

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)
CPPFLAGS += -I$(NAN_GHOST)/include
CPPFLAGS += -I$(NAN_BMFONT)/include

# External interfaces of modules:
CPPFLAGS += -I../render/extern/include
CPPFLAGS += -I../renderconverter
CPPFLAGS += -I../blenkernel
CPPFLAGS += -I../blenlib
CPPFLAGS += -I../bpython/include
CPPFLAGS += -I../makesdna
CPPFLAGS += -I../imbuf
CPPFLAGS += -I../blenloader
CPPFLAGS += -I../verify
CPPFLAGS += -I..
CPPFLAGS += -I../misc
CPPFLAGS += -I../../kernel/gen_system
CPPFLAGS += -I../../gameengine/SoundSystem

# Only used by py_demo.c !!!
CPPFLAGS += -I../radiosity/extern/include

CPPFLAGS += -I$(SRCHOME)/sumo/include
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include

CPPFLAGS += -I$(NAN_DECIMATION)/include
CPPFLAGS += -I$(NAN_BSP)/include

CPPFLAGS += -I$(NAN_BLENKEY)/include
CPPFLAGS += -I../readstreamglue
CPPFLAGS += -I../include

ifdef NAN_BUILDINFO
    CPPFLAGS += -DNAN_BUILDINFO
endif

ifeq ($(OS),linux)
    ifeq ($(CPU),alpha)
	CPPFLAGS += -I$(NAN_MESA)/include
    endif
    ifeq ($(CPU),i386)
	CPPFLAGS += -I$(NAN_MESA)/include
    endif
    ifeq ($(CPU),powerpc)
	CPPFLAGS += -I/usr/src/MesaCVS/include
    endif
endif
