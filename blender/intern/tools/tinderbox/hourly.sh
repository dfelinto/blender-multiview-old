#!/bin/sh
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
# This script is to be executed by cron every hour
# Note: install this script from intern/tools/tinderbox
#
# This script removes obsolete executables

MAXMINUTES=120
export PATH=$PATH:/usr/local/bin
BASEDIR='/export/develop/tinderbox'

cd $BASEDIR
if [ $? -ne 0 ]; then
	echo "cd $BASEDIR failed in `pwd`"
	exit 1
fi

find 20* -type f -mmin +$MAXMINUTES -exec rm {} \;

