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
# based on cvs:mozilla/webtools/tinderbox/examples/mozilla-unix.pl
# Note: install this script from intern/tools/tinderbox on server

require 5.000;

use Cwd;

# config section variables
use vars qw( $SEMA $VERSION $UNAME );

# globals
use vars qw( $Home @Pwd $Mymtime $SaveDate );

&gethome;

$SEMA = "$Home/.tinderpid";
$VERSION = "2.000";
$UNAME = `uname`;
chomp($UNAME);

$Mymtime = 0;

sub justme {
	if (open SEMA) {
		my $pid;
		chop($pid = <SEMA>);
		if ( $$ != $pid ) {
			print "PID $pid claims to be running and it is not us : $$\n";
			kill(0, $pid) and die "$0 already running (pid $pid)\n";
			print "PID $pid does not really exist\n";
		} else {
			print "PID $pid is us : $$\n";
		}
		close SEMA;
	}
} #EndSub-justme

sub gethome {
	@Pwd = getpwuid($<);
	$Home = $ENV{HOME} || $ENV{LOGDIR} || $Pwd[7] or
		die "no home directory for user $<";
} #EndSub-gethome

sub InitVars {
    $BuildAdministrator = "$ENV{'USER'}\@$ENV{'HOST'}";

    $BuildDepend = 1;	# depend or clobber
    $ReportStatus = 1;  # Send results to server or not
    $BuildOnce = 0;     # Build once, don't send results to server
    $BuildClassic = 0;  # Build classic source

    #relative path to binary
	if ($UNAME eq 'Darwin') {
		$BinaryName{'blenderplayer'} = 'blenderplayer';
		$BinaryName{'Dblenderplayer'} = 'debug/blenderplayer';
		$mail = '/usr/bin/mail';
	} elsif ($UNAME eq 'CYGWIN_NT-5.0') {
		$BinaryName{'blendercreator'} = 'blendercreator.exe';
		$BinaryName{'Dblendercreator'} = 'debug/blendercreator.exe';
		$BinaryName{'blenderpublisher'} = 'blenderpublisher.exe';
		$BinaryName{'Dblenderpublisher'} = 'debug/blenderpublisher.exe';
		$BinaryName{'blenderplugin'} = 'npB3DPlg.dll';
		$mail = '/usr/local/bin/blat.exe';
	} else {
		$BinaryName{'blendercreator'} = 'blendercreator';
		$BinaryName{'Dblendercreator'} = 'debug/blendercreator';
		$BinaryName{'blenderpublisher'} = 'blenderpublisher';
		$BinaryName{'Dblenderpublisher'} = 'debug/blenderpublisher';
		$BinaryName{'blenderplugin'} = 'npBlender3DPlugin.so';
		$BinaryName{'blenderpluginXPCOM'} = 'Blender3DPlugin.so';
		$mail = '/usr/bin/mail';
	}

    # Set these to what makes sense for your system
    $Make = 'make'; # Must be gnu make
    $CVS = 'cvs -Q';
    $CVSCO = 'co -P';

    # Set these proper values for your tinderbox server
    $Tinderbox_server = 'tinderbox\@cvs.intra.blender.nl';

    # These shouldn't really need to be changed
    $BuildSleep = 15; # Minimum wait period from start of build to start
                      # of next build in minutes (default 10)
    $BuildTree = '';
    $BuildTag = '';
    $BuildName = '';
    $Topsrcdir = 'source';
    $BuildObjDir = '';
    $BuildObjName = '';
    $ConfigGuess = './source/tools/guess/guessconfig';
    $GuessConfig = `cat /tmp/.nanguess`; #'NotSetYet';
    chomp($GuessConfig);
	$SaveDate = `date +%Y%m%d`;
	chomp($SaveDate);
} #EndSub-InitVars

sub ConditionalArgs {
    if ( $BuildClassic ) {
    } else {
		$BuildTree = 'source';
		if ($UNAME eq 'Darwin') {
			$FE = 'blenderplayer,Dblenderplayer'; 
		} elsif ($UNAME eq 'Linux') {
			$FE = 'blendercreator,Dblendercreator,blenderpublisher,Dblenderpublisher,blenderplugin,blenderpluginXPCOM'; 
		} else {
			$FE = 'blendercreator,Dblendercreator,blenderpublisher,Dblenderpublisher,blenderplugin'; 
		}
		$BuildModule = 'source release';
    }
    $CVSCO .= " -r $BuildTag" if ( $BuildTag ne '');
} #EndSub-ConditionalArgs

sub SetupEnv {
    umask(0);
    $ENV{"CVSROOT"} = ':pserver:tinderbox@cvs.intra.blender.nl:/home/cvs';
    $ENV{"SRCHOME"} = "$ENV{'HOME'}/develop/source";
    $ENV{"MAKEFLAGS"} = "-w -I $ENV{'SRCHOME'} --no-print-directory";
} #EndSub-SetupEnv

sub SetupPath {
    my($Path);
    $Path = $ENV{PATH};
    print "Path before: $Path\n";

    $ENV{'PATH'} = '/usr/local/bin:' . $ENV{'PATH'};
    if ( $OS eq 'SunOS' ) {
		$ENV{'PATH'} = $ENV{'PATH'} . ':/usr/ccs/bin';
    }

    $Path = $ENV{PATH};
    print "Path After:  $Path\n";
} #EndSub-SetupPath

sub GetSystemInfo {

    $OS = `uname -s`;
    $OSVer = `uname -r`;
    
    chop($OS, $OSVer);

    if ( $OS eq 'IRIX64' ) {
		$OS = 'IRIX';
    }
    
    $BuildName = $GuessConfig; #"HansBuildName";
    $DirName   = $GuessConfig; #"HansDirName";;

    $RealOSVer = $OSVer;
    
    if ( $OS eq 'Linux' ) {
		$RealOSVer = substr($OSVer,0,3);
    }

    $logfile = "${DirName}.log";
} #EndSub-GetSystemInfo

sub test_thesame {
	my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime,
		$mtime, $ctime, $blksize, $blocks)
		= stat($0) or die "cannot open $0: $!\n";
	if ( $Mymtime == 0 ) { $Mymtime = $mtime; }
	#print "$Mymtime $mtime\n";
	return 0 if ($Mymtime != $mtime);
	return 1;
}  #EndSub-test_thesame

sub BuildIt {
    my ($fe, @felist, $EarlyExit, $LastTime, $StartTimeStr);
	my ($StartDir);
	my ($DateNow, $hourmin);

    $StartDir = getcwd();
    $LastTime = 0;
    $EarlyExit = 0;

    while ( ! $EarlyExit ) {

	# Hans: start the logfile immediately
	chdir("$StartDir");
	unlink( "$logfile" );
	print "New BuildIt loop opening $logfile\n";
	open( LOG, ">$logfile" ) || print "can't open $?\n";

    print "Starting dir is : $StartDir\n";
    print LOG "Starting dir is : $StartDir\n";

	# Hans: clean once a day
	$DateNow = `date +%Y%m%d`;	
	chomp($DateNow);	
	if ($SaveDate ne $DateNow) {
		$hourmin = `date +%H%M`;
		if ($hourmin < 0600) {
			# Really clean stuff: rm -fr instead of make clean
			print LOG "Cleaning time ($SaveDate ne $DateNow), do a ";
			print LOG "rm -fr develop obj in $StartDir\n";
			open(MAKECLOBBER, "rm -fr develop obj 2>&1 |");	    
			while ( <MAKECLOBBER> ) {
				print $_;
				print LOG $_;
			}
			close( MAKECLOBBER );
		}
		$SaveDate = $DateNow;
	} else {
		print LOG "No cleaning time yet ($SaveDate eq $DateNow)\n";
	}

	if ( time - $LastTime < (60 * $BuildSleep) ) {
	    $SleepTime = (60 * $BuildSleep) - (time - $LastTime);
	    print "\n\nSleeping $SleepTime seconds ...\n";
	    sleep($SleepTime);
	}

	# done sleeping. check for script changes and if so restart script
	if ( test_thesame() == 0 ) {
		print "\n$0 changed, restart ourself. old pid = $$\n";
		exec ($0, "") or die "Couldn't replace myself: $!\n";
	} else {
		print LOG "No $0 changes, mtime is $Mymtime\n";
	}

	$LastTime = time;
	$StartTime = time - 60 * 10;
	$StartTimeStr = &CVSTime($StartTime);
	
	&StartBuild if ($ReportStatus);
 	$CurrentDir = getcwd();
	if ( $CurrentDir ne $StartDir ) {
	    print "startdir: $StartDir, curdir $CurrentDir\n";
	    die "curdir != startdir";
	}

	print LOG "Current dir: $CurrentDir\n";
	print LOG "Build Administrator is $BuildAdministrator\n";
	&PrintEnv;
	
	$BuildStatus = 0;

    mkdir("develop", 0777);
    mkdir("obj", 0777);
    chdir("develop") || die "Couldn't enter develop";

	if ( $BuildClassic ) {
	} else {
	    print"$CVS $CVSCO $BuildModule\n";
	    print LOG "$CVS $CVSCO $BuildModule\n";
	    open (PULL, "$CVS $CVSCO $BuildModule 2>&1 |") || die "open: $!\n";
	}
	while (<PULL>) {
	    print $_;
	    print LOG $_;
	}
	close(PULL);

	# Set GuessConfig and ConfigGuess
	print LOG "ConfigGuess = $ConfigGuess\n";
	$BuildObjDir = "$ENV{'HOME'}/obj/";
	open (GETOBJ, "$ConfigGuess 2>&1 |") || die "$ConfigGuess: $!\n";
	while (<GETOBJ>) {
	    $GuessConfig = $_;
	    chomp($GuessConfig);
	    print "ConfigGuess = [$GuessConfig]\n";
	    print LOG "ConfigGuess = [$GuessConfig]\n";
	    chomp($BuildObjName = $BuildObjDir . $GuessConfig); 
	}
	close (GETOBJ); 
	
	print "$CVS $CVSCO lib/$GuessConfig lib/java\n";
	print LOG "$CVS $CVSCO lib/$GuessConfig lib/java\n";
	open (PULL, "$CVS $CVSCO lib/$GuessConfig lib/java 2>&1 |\n") || die"open: $!\n";
	while (<PULL>) {
	    print $_;
	    print LOG $_;
	}
	close(PULL);

	# HANS: fix ranlib muck on osx
	if ($UNAME eq 'Darwin') {
		print "HANS: fix ranlib muck lib/$GuessConfig\n";
		print LOG "HANS: fix ranlib muck lib/$GuessConfig\n";
		open (PULL, "find lib/$GuessConfig -name '*.a' -exec ranlib {} \\; |\n") || die "open: $!\n";
		while (<PULL>) {
			print $_;
			print LOG $_;
		}
		close(PULL);
	}

	chdir($Topsrcdir) || die "chdir($Topsrcdir): $!\n";

	@felist = split(/,/, $FE);

	foreach $fe ( @felist ) {	    
		if (&BinaryExists($fe)) {
			#print LOG "deleting existing binary\n";
			&DeleteBinary($fe);
		}
	}

	if ($BuildClassic) {
	} else {
		my $MakeCmd = "$Make MAKE='$Make' all 2>&1 && $Make MAKE='$Make' debug 2>&1 && $Make MAKE='$Make' release 2>&1";
		print LOG "$MakeCmd |\n";
		open(BUILD, "$MakeCmd |\n");
		while (<BUILD>) {
		    print $_;
		    print LOG $_;
		}
		close(BUILD);
	}

	$AllBuildStatusStr = 'success';
	foreach $fe (@felist) {
		if (&BinaryExists($fe)) {
			print LOG "Yes, this build step is SUCCESSFUL\n";
			$BuildStatus = 0;
		} else {
			print LOG "No, this build step FAILED\n";
			$BuildStatus = 666;
			$AllBuildStatusStr = 'busted';
		}

		print LOG "Build Status = $BuildStatus\n";

	    $BuildStatusStr = ( $BuildStatus ? 'busted' : 'success' );

	    # Hans: scp action added
	    if ( $BuildStatusStr eq 'success' ) {
			#	print "fe=$fe\n";
		my ($pexe, $exe, $dates, $scpstr, $dy, $dm, $dd, $alldates);
		$pexe = $BuildObjName . '/' . $BinaryName{"$fe"};
			#	print "pexe=$pexe\n";
		if ($UNAME eq 'CYGWIN_NT-5.0') {
			$alldates = `date +%Y%m%d%H%M%S`;
			chomp($alldates);
			$myfe = $BinaryName{"$fe"};
			$myfe =~ s/\.exe//;
			$exe = $myfe . '-' . $alldates . '.exe';
		} else {
			$exe = $BinaryName{"$fe"} . '-' . `date +%Y%m%d%H%M%S`;
			chomp($exe);
		}
			#	print "exe=$exe\n";
		$dy = `date +%Y`; chomp($dy);
		$dm = `date +%m`; chomp($dm);
		$dd = `date +%d`; chomp($dd);
		$dates = $dy . '/' . $dm . '/' . $dd . '/' . $GuessConfig;
			#	print "dates=$dates\n";
		$scpstr = "scp $pexe tinderbox\@server:allbuilds/$dates/$exe";
		print "$scpstr\n";
		print LOG "$scpstr\n";
		open (SCP, "$scpstr 2>&1 |") || die "$scpstr: $!\n";
		while (<SCP>) {
	    		$scpresult = $_;
	    		chomp($scpresult);
	    		print "scp = [$scpresult]\n";
	    		print LOG "SCP = [$scpresult]\n";
		}
		close (SCP);
	    }

	    print LOG "tinderbox: tree: $BuildTree\n";
	    print LOG "tinderbox: builddate: $StartTime\n";
	    print LOG "tinderbox: status: $BuildStatusStr\n";
	    print LOG "tinderbox: build: $BuildName $fe\n";
	    print LOG "tinderbox: errorparser: unix\n";
	    print LOG "tinderbox: buildfamily: unix\n";
	    print LOG "tinderbox: END\n\n";	    
	}

	# only if all targets were successful will we scp the release
	if ( $AllBuildStatusStr eq 'success' ) {
		my ($dy, $dm, $dd, $alldates, $dates, $scpstr);
		$dy = `date +%Y`; chomp($dy);
		$dm = `date +%m`; chomp($dm);
		$dd = `date +%d`; chomp($dd);
		$dates = $dy . '/' . $dm . '/' . $dd;
		$scpstr = "scp ${BuildObjDir}2.*/*.tar.gz ${BuildObjDir}2.*/*.zip tinderbox\@server:allbuilds/$dates/";
		print "$scpstr\n";
		print LOG "$scpstr\n";
		open (SCP, "$scpstr 2>&1 |") || die "$scpstr: $!\n";
		while (<SCP>) {
	    		$scpresult = $_;
	    		chomp($scpresult);
	    		print "scp = [$scpresult]\n";
				# this will only confuse ppl as always one of gz or zip fails
				#print LOG "SCP = [$scpresult]\n";
		}
		close (SCP);
	}

	close(LOG);

	# ready LOGging, clean up log
	chdir("$StartDir");
	
# this fun line added on 2/5/98. do not remove. Translated to english,
# that's "take any line longer than 1000 characters, and split it into less
# than 1000 char lines.  If any of the resulting lines is
# a dot on a line by itself, replace that with a blank line."  
# This is to prevent cases where a <cr>.<cr> occurs in the log file.  Sendmail
# interprets that as the end of the mail, and truncates the log before
# it gets to Tinderbox.  (terry weismann, chris yeh)
#
# This was replaced by a perl 'port' of the above, writen by 
# preed@netscape.com; good things: no need for system() call, and now it's
# all in perl, so we don't have to do OS checking like before.

	open(LOG, "$logfile") || die "Couldn't open logfile: $!\n";
	open(OUTLOG, ">${logfile}.last") || die "Couldn't open logfile: $!\n";
	    
	while (<LOG>) {
	    $q = 0;
	    
	    for (;;) {
		$val = $q * 1000;
		$Output = substr($_, $val, 1000);
		
		last if $Output eq undef;
		
		$Output =~ s/^\.$//g;
		$Output =~ s/\n//g;
		print OUTLOG "$Output\n";
		$q++;
	    } #EndFor
		
	} #EndWhile
	    
	close(LOG);
	close(OUTLOG);

	if ($UNAME eq 'CYGWIN_NT-5.0') {
		system( "$mail ${logfile}.last -server cvs -f tinderbox\@win2k -t $Tinderbox_server" )
			if ($ReportStatus );
	} else {
		system( "$mail $Tinderbox_server < ${logfile}.last" )
			if ($ReportStatus );
	}
	unlink("$logfile");
	
	# if this is a test run, set early_exit to 0. 
	#This mean one loop of execution
	$EarlyExit++ if ($BuildOnce);
    }
    
} #EndSub-BuildIt

sub CVSTime {
    my($StartTimeArg) = @_;
    my($RetTime, $sec, $minute, $hour, $mday, $mon, $year);
    
    ($sec,$minute,$hour,$mday,$mon,$year) = localtime($StartTimeArg);
    $mon++; # month is 0 based.
    
    sprintf("%02d/%02d/%02d %02d:%02d:00", $mon,$mday,$year,$hour,$minute );
} #EndSub-CVSTime

sub StartBuild {
    my($fe, @felist);

    @felist = split(/,/, $FE);

	if ($UNAME eq 'CYGWIN_NT-5.0') {
		open( STARTBUILDLOG, ">startbuildlog" ) || print "can't open $?\n";
	} else {
		open( STARTBUILDLOG, "|$mail $Tinderbox_server" );
	}
    foreach $fe ( @felist ) {
		print STARTBUILDLOG "\n";
		print STARTBUILDLOG "tinderbox: tree: $BuildTree\n";
		print STARTBUILDLOG "tinderbox: builddate: $StartTime\n";
		print STARTBUILDLOG "tinderbox: status: building\n";
		print STARTBUILDLOG "tinderbox: build: $BuildName $fe\n";
		print STARTBUILDLOG "tinderbox: errorparser: unix\n";
		print STARTBUILDLOG "tinderbox: buildfamily: unix\n";
		print STARTBUILDLOG "tinderbox: END\n";
		print STARTBUILDLOG "\n";
    }
    close( STARTBUILDLOG );
	if ($UNAME eq 'CYGWIN_NT-5.0') {
		system ( "$mail startbuildlog -server cvs -f tinderbox\@win2k -t $Tinderbox_server" );
	}
	print LOG "StartBuildLog mailed to $Tinderbox_server\n";
} #EndSub-StartBuild

sub BinaryExists {
	my($fe) = @_;
	my($Binname);
	$fe = 'x' if (!defined($fe)); 
	$BinName = $BuildObjName . '/' . $BinaryName{"$fe"};
	print LOG "Testing existence of " . $BinName . "\n"; 
	if ((-e $BinName) && (-x $BinName) && (-s $BinName)) {
		1;
	} else {
		0;
	}
} #EndSub-BinaryExists

sub DeleteBinary {
    my($fe) = @_;
    my($BinName);
    $fe = 'x' if (!defined($fe)); 

    $BinName = $BuildObjName . '/' . $BinaryName{"$fe"};
    print LOG "Deleting             $BinName\n";
    unlink ($BinName) || print LOG "Unlinking             $BinName FAILED\n";

	if ($UNAME eq 'CYGWIN_NT-5.0') {
		my($winName) = $BinName;
		my($WinName);
		$winName =~ s/\.exe//;

		$WinName = $winName . ".exp";
		if (-e $WinName) {
			print LOG "Deleting             $WinName\n";
			unlink ($WinName) ||
				print LOG "Unlinking            $WinName FAILED\n";
		}

		$WinName = $winName . ".lib";
		if (-e $WinName) {
			print LOG "Deleting             $WinName\n";
			unlink ($WinName) ||
				print LOG "Unlinking            $WinName FAILED\n";
		}

		$WinName = $winName . ".ilk";
		if (-e $WinName) {
			print LOG "Deleting             $WinName\n";
			unlink ($WinName) ||
				print LOG "Unlinking            $WinName FAILED\n";
		}

		$WinName = $winName . ".pdb";
		if (-e $WinName) {
			print LOG "Deleting             $WinName\n";
			unlink ($WinName) ||
				print LOG "Unlinking            $WinName FAILED\n";
		}
	}
} #EndSub-DeleteBinary

sub PrintEnv {
	my ($key);
	print     "------------------- PrintEnv start\n";
	print LOG "------------------- PrintEnv start\n";
	foreach $key (keys %ENV) {
		print LOG "$key = $ENV{$key}\n";
		print "$key = $ENV{$key}\n";
	}
	print     "------------------- PrintEnv end\n";
	print LOG "------------------- PrintEnv end\n";
} #EndSub-PrintEnv

# Main function

justme();
open (SEMA, "> $SEMA") or die "can't write $SEMA: $!";
print SEMA "$$\n";
close (SEMA) or die "can't close $SEMA: $!";

print     "------------------- InitVars\n";
&InitVars;
$BuildDepend = 1;
print     "------------------- ConditionalArgs\n";
&ConditionalArgs;
print     "------------------- GetSystemInfo\n";
&GetSystemInfo;
print     "------------------- SetupEnv\n";
&SetupEnv;
print     "------------------- SetupPath\n";
&SetupPath;
print     "------------------- BuildIt\n";
&BuildIt;

1;

