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
# FYI: on windows this might be a symlink to /contrib/bin/perl
# This script translates some common gcc options to Microsoft Visual C++
# cl options. Type 'cl -help' for a list or look at the microsoft page:
# http://msdn.microsoft.com/library/devprods/vs6/visualc/vccore/_core_compiler_reference.htm
# When dependencies are created it rewrites some aruments for gcc.
# It also catches some warning messages about unknown extensions. Visual
# C++ uses .obj as an extension for objects and .lib as an extension for
# libraries. We're going to allow .o and .a as well.

require 5.0;
use IPC::Open3;
use strict;

my $debug = 0; # 0 : debugging output disabled, 1 : enabled
my $command;
my @args;
my $i = 0;
my $j = 0;
my $path;

if ($ARGV[0] =~ /-c/) {
	# Run as javac
	$command = 'javac';
} else {
	# Run as javah
	$command = 'javah';
}

my ($arg, $prevarg);

for ($i = 1 ; $i <= $#ARGV ; $i++) {
	$arg = $ARGV[$i];

	if ($prevarg eq "-classpath") {
		my $k = 0;
		my @paths;

		# replace classpath ':' separator with ';'
		@paths = split(":", $arg);
		for ($k = 0; $k <= $#paths ; $k++) {
			my $arg = $paths[$k];

			if ($arg =~ /(.*)\/cygdrive\/(.*)$/) {
				# argument contains a path (/cygdrive/c/rest)
				# we'll make it dos compatible
				$path = `cygpath -w '/cygdrive/$2'`;
				chomp $path;
				$debug && print "\n  cygpath -w '/cygdrive/$2'\n  $path\n";
				$arg = "$1$path";
			}

			if ($arg =~ /^\/(.*?)\/(.*?)\/(.*)$/) {
				# if argument starts with a unix path
				# we'll make it dos compatible
				if (-d "/$1/$2") {
					$path = `cygpath -w '$arg'`;
					chomp $path;
					$debug && print "\n  cygpath -w '$arg'\n  $path \n";
					$arg = "$path";
				}
			}

			$paths[$k] = $arg;			
		}
		$arg = join ";", @paths;
	}

	if ($arg =~ /(.*)\/cygdrive\/(.*)$/) {
		# argument contains a path (/cygdrive/c/rest)
		# we'll make it dos compatible
		$path = `cygpath -w '/cygdrive/$2'`;
		chomp $path;
		$debug && print "\n  cygpath -w '/cygdrive/$2'\n  $path\n";
		$arg = "$1$path";
	}

	if ($arg =~ /^\/(.*?)\/(.*?)\/(.*)$/) {
		# if argument starts with a unix path
		# we'll make it dos compatible
		if (-d "/$1/$2") {
			$path = `cygpath -w '$arg'`;
			chomp $path;
			$debug && print "\n  cygpath -w '$arg'\n  $path \n";
			$arg = "$path";
		}
	}

	if (defined $arg && $arg ne '') {
		$args[$j] = $arg;
		$j++;
	}

	$prevarg = $arg;
}

# replace the // by \\ because $command does not see // as a network path
for ($i = 0; $i < $j; $i++) {
	$args[$i] =~ s/\/\//\\\\/g;
}
  
# join all the arguments and put ' around each argument
my $arguments = join "' '", @args;
# add the command and the outermost '-s
my $commandline = "$command '$arguments'";
$debug && print STDERR "\nDEBUG: $commandline\n\n"; 

#print STDERR "\nDEBUG: $commandline\n\n"; 

my $pid = &open3(\*IN, \*OUT, \*ERR, $commandline);
close(IN);

while (<OUT>) {
	if (/^\w+\.c(|pp)\W\W$/) {
	# skip, we already show which file we're compiling
	# the \W\W is there to catch the ^M^M at the end
	# of the line
	} else {
	print $_;
	}
}

# scan STDERR for warning messages about .o and .a files
while (<ERR>) {
	print STDERR $_;
} 

wait;
exit ($? / 256);
