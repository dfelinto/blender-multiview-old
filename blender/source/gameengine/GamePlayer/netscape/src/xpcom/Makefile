#
# $Id$
#
# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
# ***** END GPL LICENSE BLOCK *****
#
# Makefile for Netscape-api style plugin base.

LIBNAME = plugin_xpcom_hooks
DIR = $(OCGDIR)/gameengine/GamePlayer/netscape/src

include nan_compile.mk

CPPFLAGS += -c -DXP_UNIX
CPPFLAGS += $(LEVEL_1_CPP_WARNINGS) 

# only plugin related:
CPPFLAGS += -I.
CPPFLAGS += -I..
CPPFLAGS += -I../../netscape_api

# Mozilla headers:
CPPFLAGS += -I$(NAN_NSPR)/include
CPPFLAGS += -I$(NAN_NSPR)/include/nspr
CPPFLAGS += -I$(NAN_MOZILLA_INC)
CPPFLAGS += -I$(NAN_MOZILLA_INC)/nspr
CPPFLAGS += -I$(NAN_MOZILLA_INC)/xpcom

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
