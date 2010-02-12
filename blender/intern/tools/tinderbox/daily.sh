#!/bin/sh
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
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

