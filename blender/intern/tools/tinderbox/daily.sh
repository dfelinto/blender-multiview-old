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
# This script is to be executed by cron daily at 00:00
# Note: install this script from intern/tools/tinderbox on server
#
# Create new directory for today

export PATH=$PATH:/usr/local/bin
BASEDIR='/export/develop/tinderbox'
#DATE=`date +%Y%m%d`
DATEY=`date +%Y`
DATEM=`date +%m`
DATED=`date +%d`
DATES="$DATEY/$DATEM/$DATED"
PLATFORMS="darwin-1.4-powerpc freebsd-4.2-i386 irix-6.5-mips linux-glibc2.1.2-i386 linux-glibc2.1.3-alpha linux-glibc2.1.3-powerpc solaris-2.8-sparc solaris-2.8-i386 windows"

doplatformdirs() {
	for DIR in $PLATFORMS; do
		if [ ! -d $DIR ]; then
			mkdir $DIR
			if [ $? -ne 0 ]; then
				echo "mkdir $DIR failed in `pwd`"
				exit 1
			fi
		fi
		if [ ! -d $DIR/debug ]; then
			mkdir $DIR/debug
			if [ $? -ne 0 ]; then
				echo "mkdir $DIR/debug failed in `pwd`"
				exit 1
			fi
		fi
	done
}

dodir() {
	DIR=$1
	if [ ! -d $DIR ]; then
		mkdir $DIR
		if [ $? -ne 0 ]; then
			echo "mkdir $DIR failed in `pwd`"
			exit 1
		fi
	fi
	cd $DIR
	if [ $? -ne 0 ]; then
		echo "cd $DIR failed in `pwd`"
		exit 1
	fi
}

cd $BASEDIR
if [ $? -ne 0 ]; then
	echo "cd $BASEDIR failed in `pwd`"
	exit 1
fi

# clean up old dirs, the ugly way
find -d . -type d | xargs rmdir 2>/dev/null

dodir $DATEY
dodir $DATEM
dodir $DATED
doplatformdirs
cd $BASEDIR
if [ $? -ne 0 ]; then
	echo "cd $BASEDIR failed in `pwd`"
	exit 1
fi
rm -f today
ln -s $DATES today

