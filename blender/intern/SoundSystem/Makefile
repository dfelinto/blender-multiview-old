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

include nan_definitions.mk

SOURCEDIR = source/gameengine/SoundSystem
DIR = $(OCGDIR)/SoundSystem
DIRS = intern
DIRS += dummy

ifeq ($(OS),windows)
    DIRS += fmod 
    DIRS += openal
endif
ifeq ($(OS),freebsd)
    DIRS += openal
endif
ifeq ($(OS),$(findstring $(OS), "linux"))
  ifeq ($(CPU),i386)
    DIRS += openal 
  endif
endif

include nan_subdirs.mk
