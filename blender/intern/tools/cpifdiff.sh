#!/bin/sh
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
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The Original Code is Copyright (C) 2002 by Hans Lambermont
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
# ***** END GPL LICENSE BLOCK *****
#
# cp-if-diff copies src(s) to dst only if they are different, so trying
# not to unneccesarily break compiler dependencies

numargs=$#

if [ $numargs -lt 2 ]; then
	echo "Usage: cpifdiff source [source] target"
	exit 1
fi

cpargs=`expr $numargs - 1`
i=1
while [ $i -le $cpargs ]; do
    args="$args $1"
	#echo "arg $i = $args"
    i=`expr $i + 1`
    shift
done
dst=$1

for src in $args; do
	#echo "$src to $dst"
	if [ ! -f $src ]; then
		echo "ERROR $src is not a file"
		exit 1
	fi
	if [ -d $dst ]; then
		src2=`echo $src | sed 's/.*\///'`
		if [ -f "$dst/$src2" ]; then
			diff $src $dst 2>&1 >/dev/null
			if [ $? -eq 0 ]; then
				echo "    $src2 already installed"
			else
				echo "    install $src"
				cp -f $src $dst
			fi
		else
			echo "    Install $src"
			cp $src $dst
		fi
	elif [ -f $dst ]; then
		diff $src $dst 2>&1 >/dev/null
		if [ $? -eq 0 ]; then
			echo "    $src already installed"
		else
			echo "    install $src"
			cp -f $src $dst
		fi
	else
		echo "ERROR $dst is not a directory or a file"
		exit 1
	fi
done

