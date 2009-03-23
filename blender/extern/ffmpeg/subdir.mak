#
# $Id: common.mak 17912 2008-12-17 20:52:39Z ben2610 $
#
# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
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
# ***** END GPL LICENSE BLOCK *****
#
#
# common bits used by all libraries
#

SRC_DIR = $(SRC_PATH)/lib$(NAME)
DIR = $(OCGDIR)/extern/ffmpeg/lib$(NAME)
VPATH = $(SRC_DIR)
EXTRADIRS = alpha armv4l i386 liba52 mlib ppc ps2 sh4 sparc bfin x86 arm

CFLAGS   += $(CFLAGS-yes)
OBJS     += $(OBJS-yes)
ASM_OBJS += $(ASM_OBJS-yes)
CPP_OBJS += $(CPP_OBJS-yes)

SRCS := $(OBJS:.o=.c) $(ASM_OBJS:.o=.S) $(CPP_OBJS:.o=.cpp)
OBJS := 
# $(OBJS) $(ASM_OBJS) $(CPP_OBJS)
STATIC_OBJS := $(OBJS) $(STATIC_OBJS)
CSRCS := $(SRCS)
CCSRCS :=

LIBNAME = $(NAME)

CFLAGS_BACKUP := $(CFLAGS)

include nan_compile.mk

# FIXME: hack!
CFLAGS := $(CFLAGS_BACKUP)
CFLAGS += -DHAVE_AV_CONFIG_H
CFLAGS += -I.. -I../libavutil \
	-I../libavcodec -I../libavformat -I../libswscale -I../libavdevice \
	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_ISOC9X_SOURCE \
	$(OPTFLAGS)

makedirffmpeg::
	@for i in $(EXTRADIRS); do \
	    [ -d $(DIR)/$$i ] || mkdir $(DIR)/$$i ; \
	    [ -d $(DIR)/debug/$$i ] || mkdir $(DIR)/debug/$$i ; \
	done

clean::
	@[ ! -d $(DIR) ] || ( cd $(DIR) && $(RM) *.o )
	@[ ! -d $(DIR)/debug ] || ( cd $(DIR)/debug && $(RM) *.o )
	@for i in $(EXTRADIRS); do \
	    [ ! -d $(DIR)/$$i/*.o ] || \
	          ( cd $(DIR)/$$i && $(RM) *.o ) ; \
	    [ ! -d $(DIR)/debug/$$i/*.o ] || \
	          ( cd $(DIR)/debug/$$i && $(RM) *.o ) ; \
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
