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
# This script must be run as user cvs, started from cron

export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin:/usr/X11R6/bin

#set -x

# ---------------- cvs part ----------------------------------
export CVSROOT=:ext:cvs.intra.blender.nl:/home/cvs
export CVS_RSH=ssh

# Initially do manually once :
#cd /usr/local/www/doxygen/ && cvs co source
# else
cd /usr/local/www/doxygen/ && cvs -Q update -P source

# ---------- now the doxygen stuff ---------------------------

export DOXHTML=/usr/local/www/intranet/docs/develop/doxygen
export DOXHOME=/usr/local/www/doxygen
cd $DOXHOME

rm -fr $DOXHTML/blender
mkdir $DOXHTML/blender
doxygen config_makedsna.dox
doxygen config_blenlib.dox
doxygen config_avi.dox
doxygen config_imbuf.dox

rm -fr $DOXHTML/gameengine
mkdir $DOXHTML/gameengine
doxygen config_snd_c_api.dox
