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
# set some defaults when these are not overruled (?=) by environment variables
#

ifndef CONFIG_GUESS
  ifeq (debug, $(findstring debug, $(MAKECMDGOALS)))
    ifeq (all, $(findstring all, $(MAKECMDGOALS)))
all debug::
      ERRTXT = "ERROR: all and debug targets cannot be used together anymore"
      ERRTXT += "Use something like ..make all && make debug.. instead"
      $(error $(ERRTXT))
    endif
  endif
    export SRCHOME ?= $(HOME)/develop/source
    export NAN_LIBDIR ?= $(HOME)/develop/lib
    export NAN_OBJDIR ?= $(HOME)/obj
    export NAN_PYTHON ?= $(LCGDIR)/python
    export NAN_PYTHON_VERSION ?= 2.0
    export NAN_OPENAL ?= $(LCGDIR)/openal
    export NAN_FMOD ?= $(LCGDIR)/fmod
    export NAN_JPEG ?= $(LCGDIR)/jpeg
    export NAN_PNG ?= $(LCGDIR)/png
    export NAN_SDL ?= $(LCGDIR)/sdl
    export NAN_TERRAPLAY ?= $(LCGDIR)/terraplay
    export NAN_MESA ?= /usr/src/Mesa-3.1
    export NAN_MOTO ?= $(LCGDIR)/moto
    export NAN_SOLID ?= $(SRCHOME)/sumo/SOLID-3.0
    export NAN_SUMO ?= $(SRCHOME)/sumo
    export NAN_FUZZICS ?= $(SRCHOME)/sumo/Fuzzics
    export NAN_OPENSSL ?= $(LCGDIR)/openssl
    export NAN_BLENKEY ?= $(LCGDIR)/blenkey
    export NAN_DECIMATION ?= $(LCGDIR)/decimation
    export NAN_GUARDEDALLOC ?= $(LCGDIR)/guardedalloc
    export NAN_IKSOLVER ?= $(LCGDIR)/iksolver
    export NAN_BSP ?= $(LCGDIR)/bsp
    export NAN_STRING ?= $(LCGDIR)/string
    export NAN_MEMUTIL ?= $(LCGDIR)/memutil
    export NAN_CONTAINER ?= $(LCGDIR)/container
    export NAN_ACTION ?= $(LCGDIR)/action
    export NAN_IMG ?= $(LCGDIR)/img
    export NAN_GHOST ?= $(LCGDIR)/ghost
    export NAN_TEST_VERBOSITY ?= 1
    export NAN_ZLIB ?= $(LCGDIR)/zlib
    export NAN_BMFONT ?= $(LCGDIR)/bmfont
    export NAN_MOZILLA ?= $(LCGDIR)/mozilla
    export NAN_NSPR ?= $(LCGDIR)/nspr

    export NAN_BUILDINFO = true

    # Be paranoid regarding library creation (do not update archives)
    export NAN_PARANOID = true

    # Library Config_Guess DIRectory
    export LCGDIR = $(NAN_LIBDIR)/$(CONFIG_GUESS)

    # Object Config_Guess DIRectory
    export OCGDIR = $(NAN_OBJDIR)/$(CONFIG_GUESS)

    export CONFIG_GUESS := $(shell ${SRCHOME}/tools/guess/guessconfig)
    export OS := $(shell echo ${CONFIG_GUESS} | sed -e 's/-.*//')
    export OS_VERSION := $(shell echo ${CONFIG_GUESS} | sed -e 's/^[^-]*-//' -e 's/-[^-]*//')
    export CPU := $(shell echo ${CONFIG_GUESS} | sed -e 's/^[^-]*-[^-]*-//')
    export MAKE_START := $(shell date "+%H:%M:%S %d-%b-%Y")

  ifeq ($(OS),beos)
    ID = $(USER)
    HOST = $(HOSTNAME)
  endif
  ifeq ($(OS),darwin)
    ID = $(shell whoami)
    HOST = $(shell hostname -s)
  endif
  ifeq ($(OS),freebsd)
    ID = $(shell whoami)
    HOST = $(shell hostname -s)
  endif
  ifeq ($(OS),irix)
    ID = $(shell whoami)
    HOST = $(shell /usr/bsd/hostname -s)
  endif
  ifeq ($(OS),linux)
    ID = $(shell whoami)
    HOST = $(shell hostname -s)
  endif
  ifeq ($(OS),openbsd)
    ID = $(shell whoami)
    HOST = $(shell hostname -s)
  endif
  ifeq ($(OS),solaris)
    ID = $(shell /usr/ucb/whoami)
    HOST = $(shell hostname)
  endif
  ifeq ($(OS),windows)
    ID = $(LOGNAME)
  endif
    export ID HOST

endif



