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
