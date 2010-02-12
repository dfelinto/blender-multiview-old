#!/usr/bin/env perl
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
