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

LIBNAME = bloutines
DIR = $(OCGDIR)/gameengine/$(LIBNAME)

include nan_compile.mk

CCFLAGS += $(LEVEL_1_CPP_WARNINGS)

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_SUMO)/include
CPPFLAGS += -I$(NAN_STRING)/include    
CPPFLAGS += -I$(NAN_MOTO)/include
CPPFLAGS += -I$(NAN_BMFONT)/include
CPPFLAGS += -I$(NAN_FUZZICS)/include
# because of kernel dependency on makesdna
CPPFLAGS += -I../../blender/makesdna
CPPFLAGS += -I../../blender/include
# timer stuff
CPPFLAGS += -I../../blender/misc
# because of kernel dependency on imbuf
CPPFLAGS += -I../../blender/imbuf
CPPFLAGS += -I../../blender/blenlib
CPPFLAGS += -I../../blender/blenkernel
CPPFLAGS += -I../../blender/render/extern/include
CPPFLAGS += -I../../blender/blenloader
CPPFLAGS += -I../../blender/blenkernel
CPPFLAGS += -I../Converter
CPPFLAGS += -I../Expressions
CPPFLAGS += -I../GameLogic
CPPFLAGS += -I../Ketsji
CPPFLAGS += -I../Rasterizer
CPPFLAGS += -I../Rasterizer/RAS_OpenGLRasterizer
CPPFLAGS += -I../SceneGraph
CPPFLAGS += -I../SoundSystem
CPPFLAGS += -I../../kernel/gen_system
CPPFLAGS += -I../Network
CPPFLAGS += -I../Network/LoopBackNetwork
ifeq ($(OS),windows)
    CPPFLAGS += -I../../blender
endif

CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)

