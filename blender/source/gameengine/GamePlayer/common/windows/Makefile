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
# ATTENTION: this Makefile is only used for win builds!

LIBNAME = windows
DIR = $(OCGDIR)/gameengine/GamePlayer/common/$(LIBNAME)

include nan_compile.mk

CPPFLAGS += -I..
CPPFLAGS += -I$(NAN_PYTHON)/include/python$(NAN_PYTHON_VERSION)

CPPFLAGS += -I$(SRCHOME)/gameengine/GameLogic
CPPFLAGS += -I$(SRCHOME)/gameengine/Rasterizer
CPPFLAGS += -I$(SRCHOME)/gameengine/Ketsji
CPPFLAGS += -I$(SRCHOME)/gameengine/SoundSystem
CPPFLAGS += -I$(SRCHOME)/gameengine/Network
CPPFLAGS += -I$(SRCHOME)/gameengine/Network/LoopBackNetwork

CPPFLAGS += -I$(SRCHOME)/sumo/Fuzzics/include
CPPFLAGS += -I$(SRCHOME)/sumo/include

CPPFLAGS += -I$(NAN_MOTO)/include
CPPFLAGS += -I$(NAN_STRING)/include    
CPPFLAGS += -I$(NAN_BMFONT)/include

# Blender stuff
CPPFLAGS += -I$(SRCHOME)/blender/blenkernel
CPPFLAGS += -I$(SRCHOME)/blender/blenlib
CPPFLAGS += -I$(SRCHOME)/blender/blenloader
CPPFLAGS += -I$(SRCHOME)/blender/imbuf
CPPFLAGS += -I$(SRCHOME)/blender/makesdna
CPPFLAGS += -I$(SRCHOME)/kernel/gen_system

CPPFLAGS += -I../../kernel/gen_system

