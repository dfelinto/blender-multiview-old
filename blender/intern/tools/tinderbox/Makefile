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
