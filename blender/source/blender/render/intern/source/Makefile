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

LIBNAME = render
DIR = $(OCGDIR)/blender/$(LIBNAME)

include nan_compile.mk

ifeq ($(OS),$(findstring $(OS), "beos darwin freebsd linux openbsd solaris windows"))
    CFLAGS += -funsigned-char
    CCFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_2_C_WARNINGS)

# first /include is my own includes, second is the external interface. 
# The external modules follow after. There should be a nicer way to say this.
CPPFLAGS += -I../include 
CPPFLAGS += -I../../extern/include 
CPPFLAGS += -I../../../radiosity/extern/include 
CPPFLAGS += -I../../../misc
CPPFLAGS += -I../../../blenlib
CPPFLAGS += -I../../../imbuf
CPPFLAGS += -I../../../makesdna
CPPFLAGS += -I../../../blenkernel
CPPFLAGS += -I../../../../kernel/gen_messaging
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
# not very neat: the rest of blender..
CPPFLAGS += -I../../../include 

