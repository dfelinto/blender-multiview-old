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
# Makefile for GHOST game player

LIBNAME = ghost
DIR = $(OCGDIR)/gameengine/GamePlayer/ghost

include nan_compile.mk

CCFLAGS += $(LEVEL_1_CPP_WARNINGS)

# OpenGL header files
CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_STRING)/include    
CPPFLAGS += -I$(NAN_BMFONT)/include

CPPFLAGS += -I../../GamePlayer/common

# Game Engine includes
CPPFLAGS += -I../../Converter
CPPFLAGS += -I../../Expressions
CPPFLAGS += -I../../GameLogic
CPPFLAGS += -I../../Ketsji
CPPFLAGS += -I../../Ketsji/KXNetwork
CPPFLAGS += -I../../Network
CPPFLAGS += -I../../Network/LoopBackNetwork
CPPFLAGS += -I../../Rasterizer
CPPFLAGS += -I../../Rasterizer/RAS_OpenGLRasterizer
CPPFLAGS += -I../../SceneGraph
CPPFLAGS += -I../../SoundSystem
CPPFLAGS += -I../../SoundSystem/SND_OpenAL

# Sumo
CPPFLAGS += -I../../../sumo/Fuzzics/include
CPPFLAGS += -I../../../sumo/include
CPPFLAGS += -I$(NAN_MOTO)/include

# Blender includes
CPPFLAGS += -I../../../blender/blenkernel
CPPFLAGS += -I../../../blender/blenlib
CPPFLAGS += -I../../../blender/blenloader
CPPFLAGS += -I../../../blender/imbuf
CPPFLAGS += -I../../../blender/makesdna
CPPFLAGS += -I../../../blender/readblenfile

# kernel? GEN? stuff
CPPFLAGS += -I../../../kernel/gen_system
CPPFLAGS += -I../../../kernel/gen_messaging

CPPFLAGS += -I$(NAN_STRING)/include
CPPFLAGS += -I$(NAN_GHOST)/include
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)

