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
# decimation intern Makefile
#

LIBNAME = decimationtest
SOURCEDIR = intern/decimation/test/decimate_glut_test/intern
DIR = $(OCGDIR)/$(SOURCEDIR)

include nan_compile.mk

CPPFLAGS += -I$(OPENGL_HEADERS)
CPPFLAGS += -I$(NAN_MOTO)/include
CPPFLAGS += -I$(NAN_MEMUTIL)/include
CPPFLAGS += -I$(NAN_GHOST)/include
CPPFLAGS += -I$(NAN_STRING)/include
CPPFLAGS += -I../../../extern
CPPFLAGS += -Icommon

DIRS = common ply

include nan_subdirs.mk
