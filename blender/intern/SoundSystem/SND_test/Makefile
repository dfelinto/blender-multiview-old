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

LIBNAME = SoundSystem
DIR = $(OCGDIR)/gameengine/$(LIBNAME)
ALLTARGETS = $(OBJS) $(DIR)/$(DEBUG_DIR)SoundSystem

include nan_compile.mk

CPPFLAGS += $(NAN_LEVEL_1_WARNINGS)

CPPFLAGS += -I$(NAN_MOTO)/include
CPPFLAGS += -I../../../kernel/gen_system
CPPFLAGS += -I.. -I../SND_BlenderWaveCache -I../SND_OpenAL

TESTLIB = $(OCGDIR)/gameengine/OpenALSoundSystem/$(DEBUG_DIR)libOpenALSoundSystem.a
TESTLIB += $(OCGDIR)/gameengine/BlenderWaveCache/$(DEBUG_DIR)libBlenderWaveCache.a
TESTLIB += $(OCGDIR)/kernel/gen_system/$(DEBUG_DIR)libgen_system.a
TESTLIB += $(OCGDIR)/gameengine/SoundSystem/$(DEBUG_DIR)libSoundSystem.a
TESTLIB += $(NAN_OPENAL)/lib/libopenal.a

$(DIR)/$(DEBUG_DIR)SoundSystem: $(OBJS) $(TESTLIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(TESTLIB) -lm -pthread -ldl -lstdc++
