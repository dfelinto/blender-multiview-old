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
# Makefile for Netscape-api style plugin base.

LIBNAME = plugin_xpcom_hooks
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/src

include nan_compile.mk

CPPFLAGS += -c -DXP_UNIX
CPPFLAGS += $(LEVEL_1_C_WARNINGS) 

# only plugin related:
CPPFLAGS += -I.
CPPFLAGS += -I..
CPPFLAGS += -I../../netscape_api

# Mozilla headers:
CPPFLAGS += -I$(NAN_MOZILLA)/include
CPPFLAGS += -I$(NAN_MOZILLA)/include/nspr

# quick hack for using some intermediates:
# :OBJPATH = /home/nzc/obj/linux-glibc2.1.2-i386/gameengine/GamePlayer/netscape/src

# To generate the proper code, use the xpidl compiler that comes with
# Mozilla. This is the standard XPCOM compiler. These tools are not in
# the NaN repo at this moment, sorry... Get a Mozilla tarball and
# build one. You need to generate a header and a .xpt file. This
# generation is only needed when the interface changes, so in general
# you should use the existing files.

# Compile the Blender3DPlugin interface
IDLSRC = Blender3DPlugin.idl

# This is the place where you built the Moz stuff.
MOZ_BUILD_PLACE = /home/nzc/dist

# The Mozilla idl compiler
IDL = $(MOZ_BUILD_PLACE)/bin/xpidl

# warnings and verbose:
IDLFLAGS = -w -v

# Repo with Moz interfaces.
INTERFACE_REPO = -I$(MOZ_BUILD_PLACE)/idl

# Output modes: specify what to generate
GENHEADER = -m header
GENTYPELIB = -m typelib
GENDOC = -m doc
GENJAVA = -m java

idl:
	$(IDL) $(IDLFLAGS) $(GENHEADER) $(INTERFACE_REPO) $(IDLSRC)
	$(IDL) $(IDLFLAGS) $(GENTYPELIB) $(INTERFACE_REPO) $(IDLSRC)
