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
# Make a little test for the sys_types replacement.
#

DIR = $(OCGDIR)/blender/deflate/test
ALLTARGETS = $(DIR)/stubbed_test

include nan_compile.mk

CFLAGS += -funsigned-char

# this module's header
CPPFLAGS += -I../..

# external headers
CPPFLAGS += -I../../../readstreamglue
CPPFLAGS += -I../../../writestreamglue

#################################################################
# libs
TESTLIBS  = $(OCGDIR)/blender/deflate/$(DEBUG_DIR)libdeflate.a
TESTLIBS += $(OCGDIR)/blender/writestreamglueSTUB/$(DEBUG_DIR)libwritestreamglueSTUB.a
TESTLIBS += $(OCGDIR)/kernel/gen_messaging/$(DEBUG_DIR)libgen_messaging.a

ifeq ($(OS),$(findstring $(OS), "freebsd linux"))
    TESTLIBS += -lz
endif

ifeq ($(OS),windows)
    TESTLIBS += ws2_32.lib 
    TESTLIBS += $(NAN_ZLIB)/lib/libz.a
endif

# There's hardly anything to debug here, so forget about it.
$(DIR)/stubbed_test: $(OBJS)
	$(CC) $(OBJS) $(TESTLIBS) -o $(DIR)/stubbed_test

clean::
	$(RM) $(DIR)/stubbed_test

test:: $(DIR)/stubbed_test
	$(DIR)/stubbed_test $(NAN_TEST_VERBOSITY)
