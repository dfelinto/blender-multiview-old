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

DIR = $(OCGDIR)/blender/makesdna
CSRCS = $(wildcard *.c)

ALLTARGETS = $(OBJS) $(DIR)/$(DEBUG_DIR)makesdna $(DIR)/$(SHARED_DIR)$(DEBUG_DIR)DNA.o

include nan_compile.mk

ifneq ($(OS),irix)
    CFLAGS += -funsigned-char
endif

CFLAGS += $(LEVEL_1_C_WARNINGS)

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_GUARDEDALLOC)/include
CPPFLAGS += -I../../blenlib
CPPFLAGS += -I..

ifeq ($(OS),windows)
    # Windows needs these extra libs because of winstuff... It is not 
    # _really_ needed, but it is the easiest fix for now. If you have 
    # some spare time, try to trace down the exact dep. Then again, you 
    # could also spend that time making the sdna system more robust.
    WINLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib
    WINLIBS +=	advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib
    WINLIBS +=	winmm.lib opengl32.lib glu32.lib largeint.lib
    WINLIBS +=	/link /nodefaultlib:libc
endif

clean::
	@$(RM) $(DIR)/makesdna* $(DIR)/DNA.c
	@$(RM) $(DIR)/debug/makesdna* $(DIR)/debug/DNA.c

# TODO include right .mk for ldflags

# A small note: we do not use the debug version of the alloc lib. That
# is done quite intentionally. If there is a bug in that lib, it needs
# to be fixed by the module maintainer.
$(DIR)/$(DEBUG_DIR)makesdna: $(OBJS) $(OCGDIR)/blender/blenlib/$(DEBUG_DIR)libblenlib.a 
	$(CC) $(LDFLAGS) -o $@ $(OBJS) \
	$(NAN_GUARDEDALLOC)/lib/libguardedalloc.a $(WINLIBS)

$(DIR)/$(DEBUG_DIR)DNA.c: $(DIR)/$(DEBUG_DIR)makesdna
    ifeq ($(OS),windows)
	$(SRCHOME)/tools/cygwin/cl_wrapper.pl - $(DIR)/$(DEBUG_DIR)makesdna \
	    $(DIR)/$(DEBUG_DIR)DNA.c
    else
	$(DIR)/$(DEBUG_DIR)makesdna $(DIR)/$(DEBUG_DIR)DNA.c
    endif

$(DIR)/$(SHARED_DIR)$(DEBUG_DIR)DNA.o: $(DIR)/$(DEBUG_DIR)DNA.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

