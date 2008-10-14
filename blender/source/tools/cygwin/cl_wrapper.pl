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
#
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


if ($ARGV[0] =~ /-M/) {
    # Dependency creation. Use gcc.
    $command = 'gcc';
    @args = @ARGV;
    $j = $#ARGV;
} else {
    my ($arg, $prevarg);
    # default behaviour
    $args[$j] = '/nologo';
    $j++;
    $command = 'cl';

    for ($i = 0 ; $i <= $#ARGV ; $i++) {
	$arg = $ARGV[$i];

	if (($i == 0) && ($arg eq "-")) {
		# cl_wrapper is used to call a different program
		# that needs commandline replacement as well
		# skipt this "-" argument
		$arg = undef();

		# drop the /nologo 
		$j--; 

		# next argument is the executable
		# don't rewrite it, it's a unix command
		$i++;
		$command = $ARGV[$i];
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

	if ($arg =~ /^\/Fd\/(.*)$/) {
	    # if argument is an output path for debug symbols 
	    # we'll make it dos compatible
	    if (-d "/$1") {
		$path = `cygpath -w '/$1'`;
		chomp $path;
		$debug && print "\n  cygpath -w '/$1'\n  $path\n";
		$arg = "/Fd$path";
	    }
	}

	if ($arg =~ /^\/fo\/(.*?)\/(.*?)\/(.*)$/) {
	    # if argument is an output path for
	    # rc, the recource compiler,
	    # we'll make it dos compatible
	    if (-d "/$1/$2") {
		$path = `cygpath -w '/$1/$2/$3'`;
		chomp $path;
		$debug && print "\n  cygpath -w '/$1/$2/$3'\n  $path\n";
		$arg = "/fo$path";
	    }
	}

	if ($arg =~ /^-I\/(.*)$/) {
	    # if argument is a path for includefiles
	    # we'll make it dos compatible
	    if (-d "/$1") {
		$path = `cygpath -w '/$1'`;
		chomp $path;
		$debug && print "\n  cygpath -w '/$1'\n  $path\n";
		$arg = "-I$path";
            }
        }

	if ($arg eq '-funsigned-char') {
	    # replace -funsigned-char with /J
	    $arg = '/J';
	}

	if ($arg eq '-E') {
	    # replace -E with /E
	    $arg = '/E';
	}

	if ($arg eq '-c') {
	    # replace -c with /c
	    $arg = '/c';
	}

	if ($arg eq '-Wall') {
	    $arg = '/W3';
	    # remove possible other /W0, yes this is a kludge :(
	    my $k;
	    for ($k = 0; $k <= $#ARGV; $k++ ) {
		if ($ARGV[$k] eq '/W0') {
		    $ARGV[$k] = '/W3';
		}
		if ($args[$k] eq '/W0') {
		    $args[$k] = '/W3';
		}
	    }
	}

	if ($arg eq '-g') {
	    # replace -g with /ZI
	    $arg = '/ZI';
	}

	if ($arg =~ /^-I(.*)$/) {
	    # argument starts with -I 
	    # translate it to /I
	    $arg = "/I$1";
	}
  
	if ($arg =~ /-D(.*)$/) {
	    # argument starts with -D: replace it with /D rest
	    $args[$j] = '/D';
	    $j++;
	    $args[$j] = $1;
	    $j++;
	    # we're done with $arg
	    $arg = undef();
	}

	if ($prevarg eq "-o") {
	    # if the previous argument was -o we'll have to
	    # figure out what we're going to '-output'. An
	    # object (/Fo) or an executable (/Fe)

	    if ($arg =~ /\.o$/) {
		$arg = "/Fo$arg";
	    } else {
		$arg = "/Fe$arg";
	    }

	    # we'll overwrite the -o option that's already in @args
	    $args[$j - 1] = $arg;
	    # we're done with $arg
	    $arg = undef();
	}

	if ($arg eq '-OPT:Olimit=0') {
	    # skip this argument
	    $arg = undef();
	}
  
	if (defined $arg && $arg ne '') {
	    $args[$j] = $arg;
	    $j++;
	}

	$prevarg = $arg;
    }
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
    if (/Command line warning D4024 : [^\.]*\.[o|a]', object file assumed/) {
	# cl is complaining about one of the .o or .a files: ignore
    } else {
	print STDERR $_;
    }
} 

wait;
exit ($? / 256);
