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

LIBNAME = unix
DIR = $(OCGDIR)/gameengine/GamePlayer/common/$(LIBNAME)

include nan_compile.mk

CCFLAGS += $(LEVEL_1_CPP_WARNINGS)

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_STRING)/include    

CPPFLAGS += -I..
CPPFLAGS += -I../../netscape/src/ketsji
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)

# Game Engine
CPPFLAGS += -I../../../../gameengine/Converter
CPPFLAGS += -I../../../../gameengine/Expressions
CPPFLAGS += -I../../../../gameengine/GameLogic
CPPFLAGS += -I../../../../gameengine/Ketsji
CPPFLAGS += -I../../../../gameengine/Ketsji/KXNetwork
CPPFLAGS += -I../../../../gameengine/Network
CPPFLAGS += -I../../../../gameengine/Network/LoopBackNetwork
CPPFLAGS += -I../../../../gameengine/Rasterizer
CPPFLAGS += -I../../../../gameengine/Rasterizer/RAS_OpenGLRasterizer
CPPFLAGS += -I../../../../gameengine/SceneGraph
ifeq ($(NAN_SND), OPENAL)
    CPPFLAGS += -I../../../../gameengine/SoundSystem
    CPPFLAGS += -I../../../../gameengine/SoundSystem/SND_OpenAL
else
    CPPFLAGS += -I../../../../gameengine/SoundSystem
    CPPFLAGS += -I../../../../gameengine/SoundSystem/SND_Dummy
endif

CPPFLAGS += -I../../../../sumo/Fuzzics/include
CPPFLAGS += -I../../../../sumo/include
CPPFLAGS += -I$(NAN_MOTO)/include
CPPFLAGS += -I$(NAN_BMFONT)/include

# Blender stuff
CPPFLAGS += -I../../../../blender/blenkernel
CPPFLAGS += -I../../../../blender/blenlib
CPPFLAGS += -I../../../../blender/blenloader
CPPFLAGS += -I../../../../blender/imbuf
CPPFLAGS += -I../../../../blender/makesdna
CPPFLAGS += -I../../../../kernel/gen_system

CPPFLAGS += -I../../kernel/gen_system

