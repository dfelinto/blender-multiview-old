#!/usr/local/bin/perl
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
# generate html from tex documents on the fly from commit messages
# expects input like:
# U intranet/docs/develop/logicbricks.tex
use Cwd;

$debug=0;

$dir = cwd();

while (<>) {
    chomp;
    if ($debug) {
	printf("[$_]\n");
    }
    if (/^. (\.\/|)(.*)\/([^\/]*\.tex)$/) {
	if ($debug) {
	    printf("[$2][$3]\n");
	}
	chdir $2 or die "Cannot cd to $2 : $!\n";
	@args = ("latex", "$3");
	system(@args) == 0 or die "1st system @args failed: $?";
	system(@args) == 0 or die "2nd system @args failed: $?";
	@args = ("latex2html", "$3");
	system(@args) == 0 or die "3th system @args failed: $?";
	$texdir = $3;
	$texdir =~ s/\.tex$//;
	@args = ("rm", "-fr", "$texdir/*.tex");
	system(@args) == 0 or die "4th system @args failed: $?";
	chdir $dir or die "Cannot cd to $2 : $!\n";
    }
}
