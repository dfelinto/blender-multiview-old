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
