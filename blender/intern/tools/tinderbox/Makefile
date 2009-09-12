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
# Installer. Allows install target on server.intra only.
#

all:
	@echo "Test tinderbox daemon perl script integrity."
	@perl -cw tinderbox-blender.pl

install: all
	@echo "Install tinderbox scripts."
	@[ `hostname` = server.intra.blender.nl ]
	@echo "Copy maintenance shell scripts to /export/develop/tinderbox/"
	@cp -f daily.sh /export/develop/tinderbox/
	@cp -f hourly.sh /export/develop/tinderbox/
	@echo "Copy tinderbox daemon perl script to /home/tinderbox/"
	@cp -f tinderbox-blender.pl /home/tinderbox/
