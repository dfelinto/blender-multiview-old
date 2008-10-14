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
# Note: dotex.pl needs /usr/local/bin/perl

export CVSROOT=:ext:cvs.intra.blender.nl:/home/cvs
export CVS_RSH=ssh

debug=0
#debug=1

# Use locking
# todo

# Check for new commits.
# implemented by looking at the md5 checksum of the last line in
# /home/cvs/CVSROOT/history
TMPFILE="/tmp/.webupdate"
newmd5=`ssh repo 'tail -1 /home/cvs/CVSROOT/history | md5'`
if [ -f $TMPFILE ]; then
    oldmd5=`cat /tmp/.webupdate`
else
    oldmd5=''
fi
# Create new TMPFILE
rm -f $TMPFILE
echo $newmd5 > $TMPFILE

# Update work to do ?
if [ "x$newmd5" = "x$oldmd5" ]; then
    # No work to do
    if [ $debug -eq 1 ]; then
	echo "$newmd5 = $oldmd5"
    fi
else
    # Yes, there was a new commit, update cvs
    if [ $debug -eq 1 ]; then
	cd /usr/local/www/ && \
	cvs -Q co -P intranet | ./dotex.pl
    else
	cd /usr/local/www/ && \
	cvs -Q co -P intranet | ./dotex.pl > /dev/null 2>&1
    fi
fi
