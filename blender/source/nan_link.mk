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
# linking only

include nan_definitions.mk

ifdef NAN_DEBUG
    LDFLAGS += $(NAN_DEBUG)
endif

DBG_LDFLAGS	+= -g

ifneq (x$(DEBUG_DIR), x)
    LDFLAGS+=$(DBG_LDFLAGS)
else
    LDFLAGS+=$(REL_LDFLAGS)
endif

######################## OS dependencies (alphabetic!) ################

	# default (overriden by windows)
SOEXT = .so

ifeq ($(OS),beos)
    LLIBS = -L/boot/develop/lib/x86/ -lGL -lbe -L/boot/home/config/lib/
    LLIBS += -lpython1.5
endif

ifeq ($(OS),darwin)
    LLIBS    += -lGLU -lGL
    LLIBS    += -lz -framework Carbon -framework AGL
    LDFLAGS += -L/System/Library/Frameworks/OpenGL.framework/Libraries
    DBG_LDFLAGS += -L/System/Library/Frameworks/OpenGL.framework/Libraries
endif

ifeq ($(OS),freebsd)
    LLIBS = -L/usr/X11R6/lib -lX11 -lXmu -lm -lutil -lz -pthread -lc_r
    DADD = -lGL -lGLU
    DYNLDFLAGS = -shared $(LDFLAGS)
  ifeq ($(OS_VERSION),$(findstring $(OS_VERSION), "3.4 4.0"))
    COMMENT = "MESA 3.0"
    SADD = /usr/X11R6/lib/libGL.a /usr/X11R6/lib/libGLU.a
    LLIBS += -lc
  else
  endif
endif

ifeq ($(OS),irix)
    LLIBS = -lmovieGL -lGLU -lGL -lXmu -lXext -lX11 -lc -lm -ldmedia
    LLIBS += -lcl -laudio -ldb -lCio -lz -woff 84,171
    DYNLDFLAGS = -shared $(LDFLAGS)
endif

ifeq ($(OS),linux)
  ifeq ($(CPU),alpha)
    COMMENT = "MESA 3.1"
    LLIBS = -lGL -lGLU -L/usr/X11R6/lib/ -lXmu -lXext -lX11
    LLIBS += -lc -lm -ldl -lutil
    LOPTS = -export-dynamic
  endif
  ifeq ($(CPU),i386)
    COMMENT = "MESA 3.1"
    LLIBS = -L$(NAN_MESA)/lib -L/usr/X11R6/lib -lXmu -lXext -lX11 -lXi
    LLIBS += -lutil -lc -lm -ldl -lpthread
    LOPTS = -export-dynamic
    DADD = -lGL -lGLU
    SADD = $(NAN_MESA)/lib/libGL.a $(NAN_MESA)/lib/libGLU.a
    DYNLDFLAGS = -shared $(LDFLAGS)
  endif
  ifeq ($(CPU),powerpc)
    COMMENT = "MESA 3.1"
    LLIBS = -L/usr/X11R6/lib/ -lXmu -lXext -lX11 -lc -ldl -lm -lutil
    DADD = -lGL -lGLU
    SADD = /usr/lib/libGL.a /usr/lib/libGLU.a
    LOPTS = -export-dynamic
  endif
    LLIBS += -lz
endif

ifeq ($(OS),openbsd)
    SADD = /usr/local/lib/libGL.a /usr/local/lib/libGLU.a
    SADD += /usr/X11R6/lib/libXmu.a /usr/X11R6/lib/libXext.a
    SADD += /usr/X11R6/lib/libX11.a /usr/lib/libm.a -pthread
endif

ifeq ($(OS),solaris)
    LLIBS = -lGLU -lGL -lXmu -lXext -lX11 -lc -lm -ldl -lsocket -lnsl
    DYNLDFLAGS = -shared $(LDFLAGS)
endif

ifeq ($(OS),windows)
    EXT = .exe
	SOEXT = .dll
    DADD = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib
    DADD += advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib
    DADD += vfw32.lib winmm.lib opengl32.lib glu32.lib largeint.lib dxguid.lib
    DADD += libcmt.lib
    LOPTS = /link
    LOPTS += /NODEFAULTLIB:"libc" 
    LOPTS += /NODEFAULTLIB:"libcd" 
    LOPTS += /NODEFAULTLIB:"libcp" 
    LOPTS += /NODEFAULTLIB:"libcpd" 
    LOPTS += /NODEFAULTLIB:"python20" 
    LOPTS += /NODEFAULTLIB:"msvcrt" 
    LOPTS += /SUBSYSTEM:CONSOLE
    LDFLAGS += /MT
    DYNLDFLAGS = /LD
endif

