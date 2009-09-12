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
# Bounces make to subdirectories. Also installs after succesful all target.

SOURCEDIR = extern/ffmpeg
DIR = $(OCGDIR)/extern/ffmpeg

include nan_subdirs.mk

ALLDIRS = libavcodec libavformat libavutil libswscale libavdevice

config::
	./configure --disable-shared --enable-gpl --enable-zlib --disable-vhook --disable-ffserver --disable-ffplay --enable-swscale --enable-pthreads --enable-yasm --disable-bzlib --disable-outdevs --disable-demuxer=alsa
all::
	@for i in $(ALLDIRS); do \
	    echo "====> $(MAKE) $@ in $(SOURCEDIR)/$$i" ;\
	    $(MAKE) -C $$i install || exit 1; \
	done
clean test debug:: config
	@for i in $(ALLDIRS); do \
	    echo "====> $(MAKE) $@ in $(SOURCEDIR)/$$i" ;\
	    $(MAKE) -C $$i $@ || exit 1; \
	done

install: config all
