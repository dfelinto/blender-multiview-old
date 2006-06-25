#
# $Id$
#
# ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version. The Blender
# Foundation also sells licenses for use in proprietary software under
# the Blender License.  See http://www.blender.org/BL/ for information
# about this.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): Peter Schlaile
#
# ***** END GPL/BL DUAL LICENSE BLOCK *****
#
#
# common bits used by all libraries
#

SRC_DIR = $(SRC_PATH)/lib$(NAME)
DIR = $(OCGDIR)/extern/ffmpeg/lib$(NAME)
VPATH = $(SRC_DIR)
EXTRADIRS = alpha armv4l i386 liba52 mlib ppc ps2 sh4 sparc

#FIXME: This should be in configure/config.mak
ifeq ($(TARGET_ARCH_SPARC64),yes)
CFLAGS+= -mcpu=ultrasparc -mtune=ultrasparc
endif

SRCS := $(OBJS:.o=.c) $(ASM_OBJS:.o=.S) $(CPPOBJS:.o=.cpp)
OBJS := 
# $(OBJS) $(ASM_OBJS) $(CPPOBJS)
STATIC_OBJS := $(OBJS) $(STATIC_OBJS)
CSRCS := $(SRCS)
CCSRCS :=

LIBNAME = $(NAME)

CFLAGS_BACKUP := $(CFLAGS)

include nan_compile.mk

# FIXME: hack!
CFLAGS := $(CFLAGS_BACKUP)
CFLAGS += -DHAVE_AV_CONFIG_H

ifdef TARGET_BUILTIN_VECTOR
$(DIR)/i386/fft_sse.o: CFLAGS+= -msse
depend: CFLAGS+= -msse
endif
ifdef TARGET_BUILTIN_3DNOW
$(DIR)/i386/fft_3dn.o: CFLAGS+= -m3dnow
ifeq ($(TARGET_ARCH_X86),yes)
$(DIR)/i386/fft_3dn2.o: CFLAGS+= -march=athlon
endif
ifeq ($(TARGET_ARCH_X86_64),yes)
$(DIR)/i386/fft_3dn2.o: CFLAGS+= -march=k8
endif
endif

makedirffmpeg::
	@for i in $(EXTRADIRS); do \
	    [ -d $(DIR)/$$i ] || mkdir $(DIR)/$$i ; \
	    [ -d $(DIR)/$$i/debug ] || mkdir $(DIR)/$$i/debug ; \
	done

install: makedir makedirffmpeg all debug
	@[ -d $(NAN_FFMPEG) ] || mkdir $(NAN_FFMPEG)
	@[ -d $(NAN_FFMPEG)/include ] || mkdir $(NAN_FFMPEG)/include
	@[ -d $(NAN_FFMPEG)/include/ffmpeg ] || mkdir $(NAN_FFMPEG)/include/ffmpeg
	@[ -d $(NAN_FFMPEG)/lib ] || mkdir $(NAN_FFMPEG)/lib
	@$(NANBLENDERHOME)/intern/tools/cpifdiff.sh $(DIR)/lib$(NAME).a $(NAN_FFMPEG)/lib/
ifeq ($(OS),darwin)
	ranlib $(NAN_FFMPEG)/lib/lib$(NAME).a
endif
	@$(NANBLENDERHOME)/intern/tools/cpifdiff.sh $(HEADERS) $(NAN_FFMPEG)/include/ffmpeg/
