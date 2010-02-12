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
# The Original Code is http://lxr.mozilla.org/mozilla/source/webtools/tinderbox/examples/mozilla-unix.pl
#
# Contributor(s): Hans Lambermont
#
# ***** END GPL LICENSE BLOCK *****
#
# Initial Installation Instructions :
# 1) make a useraccount named 'tinderbox'
# 2) copy this script to ~tinderbox/
# 3) su - tinderbox
# 4) touch .cvspass
# 5) setenv CVSROOT :pserver:anonymous@cvs.blender.org:/cvsroot/bf-blender
# 6) cvs login (password is anonymous)
# And from then on just :
# 7) ./tinderbox-blender.pl
#
# Optionally use a .tinderboxrc configuration file, located in the directory
# you start tinderbox from (usually the homedir :)

require 5.000;

# globals. most of 'em are evil ugly globals
use vars qw( $VERSION $UNAME $Home @Pwd $Mymtime $SaveDate
			 $Hostname $Mailer $Tinderbox_server $BuildAdministrator
			 $BuildDepend $ReportStatus $BuildOnce $BuildAutotools
			 $BuildType @BuildType %BinaryName $Make $CVS $CVSCO $BuildSleep
			 $BuildTree $BuildTag $BuildName $Topsrcdir $BuildObjDir
			 $BuildObjName $ConfigGuess $GuessConfig $FE $BuildModule
			 $OS $OSVer $logfile $StartTime $StartDir $rcfile);

# check which mail system we're going to use
eval "use Mail::Send";
if ($@) {
	eval "use Net::SMTP";
	if ($@) {
		die "install Mail::Send or Net::SMTP\n";
	} else {
		$Mailer = 'Net::SMTP';
	}
} else {
	$Mailer = 'Mail::Send';
}

use Cwd;
use Sys::Hostname;
use File::Compare;
use File::Copy;
use strict;

sub SendMail{
	my ($subject, $body) = @_;
	my $msg;

	if ($Mailer eq 'Net::SMTP')
	{
		if (defined $ENV{"SMTP_SERVER"})
		{
			$msg = Net::SMTP->new($ENV{'SMTP_SERVER'}, Hello => 'localhost');
		} else {
			$msg = Net::SMTP->new('localhost', Hello => 'localhost');
		}

		if (defined $msg) {
			$msg->mail($ENV{USER});
			$msg->to($Tinderbox_server);

			$msg->data();
			$msg->datasend("Subject: $subject\n");
			$msg->datasend("From: $ENV{USER}\n");
			$msg->datasend("To: $Tinderbox_server\n");
			$msg->datasend("\n");

			$msg->datasend("$body\n");
			$msg->dataend();

			$msg->quit;
		} else {
			die "Set the environment variable SMTP_SERVER to your outgoing mail server\n";
		}
	} else {
		$msg = new Mail::Send;
		$msg->subject($subject); 
		$msg->to($Tinderbox_server);

		# use the command below if you want Mail::Send to use Net::SMTP
		# instead of talking to mail or sendmail directly
		# $fh = $msg->open('smtp', Server => $ENV{"SMTP_SERVER"},
		#				    Hello => 'localhost');
		my $fh = $msg->open;

		print $fh "$body\n";
		$fh->close;
	}
} #EndSub-SendMail

sub GetHome {
	@Pwd = getpwuid($<);
	$Home = $ENV{HOME} || $ENV{LOGDIR} || $Pwd[7] or
		die "no home directory for user $<";
} #EndSub-gethome

sub JustMe {
	my $SEMA = "$Home/.tinderpid";
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
	# If we're still here, write our PID
	open (SEMA, "> $SEMA") or die "can't write $SEMA: $!";
	print SEMA "$$\n";
	close (SEMA) or die "can't close $SEMA: $!";
} #EndSub-JustMe

sub InitVars {
	$Hostname = hostname();
	$Hostname =~ s/\..*$//;

	$UNAME = `uname`;
	chomp($UNAME);

	$Mymtime = 0;
	$StartDir = "";

    $BuildAdministrator = "$ENV{'USER'}\@$ENV{'HOST'}";

    $BuildDepend = 1;	# depend or clobber
    $ReportStatus = 1;  # Send results to server or not
    $BuildOnce = 0;     # Build once, don't send results to server
    $BuildAutotools = 0;# Build with the autotools
    @BuildType = ('trad', 'auto'); # BuildType descriptions

	$FE = 'blenderdynamic'; 
    #relative path to binary
	if ($UNAME eq 'Darwin') {
		$FE = 'blenderplayer,blenderdynamic'; 
		$BinaryName{'blenderplayer'} = 'bin/blenderplayer';
		$BinaryName{'blenderdynamic'} = 'bin/blender';
#		$BinaryName{'Dblenderplayer'} = 'debug/bin/blenderplayer';
	} elsif ($UNAME eq 'CYGWIN_NT-5.0') {
		$BinaryName{'blenderdynamic'} = 'bin/blender.exe';
#		$BinaryName{'Dblenderdynamic'} = 'debug/blender.exe';
#		$BinaryName{'blenderplugin'} = 'npB3DPlg.dll';
	} else {
		$BinaryName{'blenderdynamic'} = 'bin/blender';
#		$BinaryName{'Dblenderdynamic'} = 'debug/blender';
#		$BinaryName{'blenderplugin'} = 'npBlender3DPlugin.so';
#		$BinaryName{'blenderpluginXPCOM'} = 'Blender3DPlugin.so';
	}
	$VERSION = '$Revision$';

    # Set these to what makes sense for your system
	if ($UNAME eq 'FreeBSD') {
		$Make = 'gmake'; # Must be gnu make
	} else {
		$Make = 'make'; # Must be gnu make
	}
    $CVS = 'cvs -Q';

	$rcfile = '.tinderboxrc'; # Optional configuration file

	# Defaults in case there is no configuration file supplied, these
	# shouldn't really need to be changed

	$Tinderbox_server = 'tinderbox@xserve.blender.org';
	$BuildSleep = 60; # Minimum wait period from start of build to start
                      # of next build in minutes (default 10)
    $BuildTree = 'source';
    $BuildTag = '';
    $BuildName = '';
    $BuildObjDir = '';
    $BuildObjName = '';
	$BuildModule = 'blender lib';
    $ConfigGuess = './blender/source/tools/guess/guessconfig';
	print "ConfigGuess set to $ConfigGuess\n";
    $GuessConfig = "nanguess"; # actually the hint and cache filename for now
	print "GuessConfig set to $GuessConfig\n";
	$SaveDate = `date +%Y%m%d`;
	chomp($SaveDate);
} #EndSub-InitVars

sub ConditionalArgs {
	if ($BuildAutotools) {
		$BinaryName{'blenderdynamic'} = 'blender';
		$BuildObjDir = "$ENV{'HOME'}/develop/";
	} else {
		$BinaryName{'blenderdynamic'} = 'bin/blender';
		$Topsrcdir = 'blender';
		$BuildObjDir = "$ENV{'HOME'}/develop/blender/obj/";
	}
	# append version to BuildType
	my $BTVersion = $VERSION;
	$BTVersion =~ s/^.*\d+\.(\d+).*$/$1/;
	$BuildType = $BuildType[$BuildAutotools] . ".$BTVersion";

	# Checkout the specified revision if there is one, the trunk if not
	$CVSCO = 'co -P';
	$CVSCO .= " -r $BuildTag" if ( $BuildTag ne '');
} #EndSub-ConditionalArgs

sub GetSystemInfo {

	# test for GuessConfig hint
	if ( ! -f $GuessConfig ) {
		print "Manually create the '$GuessConfig' hint and cache by running\n";
		print "'blender/source/tools/guess/guessconfig > $GuessConfig' once\n";
		exit 1;
	}

	# read GuessConfig hint
	open (FH, "< $GuessConfig") || die "$GuessConfig: $!\n";
	$GuessConfig = <FH>;
	close(FH);
	chomp($GuessConfig);
	print "GuessConfig set to $GuessConfig\n";

    $BuildName = $GuessConfig; #"HansBuildName";
    my $DirName   = $GuessConfig; #"HansDirName";;

	# then we let tinderbox do it all over in their style
    $OS = `uname -s`;
    $OSVer = `uname -r`;
    
    chop($OS, $OSVer);

    if ( $OS eq 'IRIX64' ) {
		$OS = 'IRIX';
    }

    $logfile = "${DirName}.log";
} #EndSub-GetSystemInfo

sub SetupEnv {
    umask(0);
    $ENV{"CVSROOT"} = ':pserver:anonymous@cvs.blender.org:/cvsroot/bf-blender';
    $ENV{"NANBLENDERHOME"} = "$ENV{'HOME'}/develop/blender";
    $ENV{"SRCHOME"} = "$ENV{'HOME'}/develop/blender/source";
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

sub test_thesame {
	my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime,
		$mtime, $ctime, $blksize, $blocks) = stat("$StartDir/$0") or
		die "test_thesame cannot open $StartDir/$0: $!\n";
	if ( $Mymtime == 0 ) { $Mymtime = $mtime; }
	#print "$Mymtime $mtime\n";
	return 0 if ($Mymtime != $mtime);
	return 1;
}  #EndSub-test_thesame

sub BuildIt {
    my ($fe, @felist, $EarlyExit, $LastTime, $StartTimeStr, $Message);
	my ($DateNow, $hourmin);

    $StartDir = getcwd();
    $LastTime = 0;
    $EarlyExit = 0;

	# Set Mymtime
	test_thesame();

	while ( ! $EarlyExit ) {
	
	# Reload the configuration file each run, if it exists
	if (-f $rcfile) {
		print "Including config file $rcfile\n";
		do $rcfile;
		&ConditionalArgs;
	} else {
		print "Config file $rcfile not found\n";
	}

	# Hans: start the logfile immediately
	chdir("$StartDir");
	unlink( "$logfile" );
	print "New BuildIt loop. Opening [$logfile]\n";
	open( LOG, ">$logfile" ) || die "Can't open [$logfile]: $?\n";

    print "Using tinderbox-blender version : $VERSION\n";
    print LOG "Using tinderbox-blender version : $VERSION\n";
	
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
	    my $SleepTime = (60 * $BuildSleep) - (time - $LastTime);
	    print "\n\nSleeping $SleepTime seconds ...\n";
	    sleep($SleepTime);
	}

	# done sleeping.
	$LastTime = time;
	$StartTime = time - 60 * 10;
	$StartTimeStr = &CVSTime($StartTime);
	
	&StartBuild if ($ReportStatus);
 	my $CurrentDir = getcwd();
	if ( $CurrentDir ne $StartDir ) {
	    print "startdir: $StartDir, curdir $CurrentDir\n";
	    die "curdir != startdir";
	}

	print LOG "Current dir: $CurrentDir\n";
	print LOG "Build Administrator is $BuildAdministrator\n";
	&PrintEnv;
	
	my $BuildStatus = 0;

    mkdir("develop", 0777);
    chdir("develop") || die "Couldn't enter develop";

	print"$CVS $CVSCO $BuildModule\n";
	print LOG "$CVS $CVSCO $BuildModule\n";
	open (PULL, "$CVS $CVSCO $BuildModule 2>&1 |") || die "open: $!\n";
	while (<PULL>) {
	    print $_;
	    print LOG $_;
		# TODO actually check for updates/patches before we rebuild
	}
	close(PULL);
	# TODO cvs error code checking, or check for things like :
	#  fork failed
	#  Resource temporarily unavailable

    # Update ourselves from CVS for the next run, if we changed that is.
	if ( -f "$StartDir/enableTinderboxAutoUpdate") {
		my $CVS_Script = "blender/intern/tools/tinderbox/tinderbox-blender.pl";
		my $diff = compare($CVS_Script, "$StartDir/$0");
		if ($diff == 1) {
			print "\n$CVS_Script changed in CVS, copy it over $StartDir/$0\n";
			copy($CVS_Script, "$StartDir/$0");
			# Let test_thesame act on this action lateron
		} elsif ($diff == 0) {
			my $line = "\n$0 did not change in CVS\n";
			print $line;
			print LOG $line;
		} else {
			my $line = "\n$0 compare error :-( auto-update offline\n";
			print $line;
			print LOG $line;
		}
    	# check for script changes and if so restart script
        # This still works for multi-OS NFS mounted builds.
    	if (test_thesame() == 0) {
			chdir("$StartDir");
    		print "\n$0 changed, restart ourself. old pid = $$\n";
    		exec ("$0", "") or die "Couldn't replace myself: $!\n";
    	} else {
    		print LOG "No $StartDir/$0 changes, mtime is $Mymtime\n";
    	}
	} else {
		my $line = "No $StartDir/enableTinderboxAutoUpdate found\n";
		print $line;
		print LOG $line;
	}

	# Set GuessConfig and ConfigGuess
	print LOG "ConfigGuess = $ConfigGuess\n";
	if (! -f $ConfigGuess) {
		die "$ConfigGuess not found, aborting\n";
	}
	open (GETOBJ, "$ConfigGuess 2>&1 |") || die "$ConfigGuess: $!\n";
	while (<GETOBJ>) {
		$GuessConfig = $_;
		chomp($GuessConfig);
		print "ConfigGuess = [$GuessConfig]\n";
		print LOG "ConfigGuess = [$GuessConfig]\n";
		chomp($BuildObjName = $BuildObjDir . $GuessConfig); 
	}
	close (GETOBJ); 

	# overwrite nanguess hint and cache
	open (PUTOBJ, "> $StartDir/nanguess") || die "Cannot write nanguess: $!\n";
	print PUTOBJ $GuessConfig;
	close(PUTOBJ);

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

	if ($BuildAutotools) {
		chdir("blender") || die "chdir(blender): $!\n";
		print `pwd` . " -> chdir(blender)\n";
		system("./bootstrap");
		$Topsrcdir = $BuildObjName;
		mkdir($Topsrcdir, 0777);
		chdir($Topsrcdir) || die "chdir($Topsrcdir): $!\n";
		print `pwd` . " -> chdir($Topsrcdir)\n";
		system("../blender/configure");
	}
	
	chdir($Topsrcdir) || die "chdir($Topsrcdir): $!\n";
	print `pwd` . " -> chdir($Topsrcdir)\n";
	
	@felist = split(/,/, $FE);

	foreach $fe ( @felist ) {	    
		if (&BinaryExists($fe)) {
			#print LOG "deleting existing binary\n";
			&DeleteBinary($fe);
		}
	}

	#my $MakeCmd = "$Make MAKE='$Make' all 2>&1 && $Make MAKE='$Make' debug 2>&1 && $Make MAKE='$Make' release 2>&1";
	my $MakeCmd = "$Make MAKE='$Make' all 2>&1";
	print "$MakeCmd\n";
	print LOG "$MakeCmd |\n";
	open(BUILD, "$MakeCmd |\n");
	while (<BUILD>) {
		print $_;
		print LOG $_;
	}
	close(BUILD);

	my $AllBuildStatusStr = 'success';
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

	    my $BuildStatusStr = ( $BuildStatus ? 'busted' : 'success' );

#	    # Hans: scp action added
#	    if ( $BuildStatusStr eq 'success' ) {
#			#	print "fe=$fe\n";
#		my ($pexe, $exe, $dates, $scpstr, $dy, $dm, $dd, $alldates);
#		$pexe = $BuildObjName . '/' . $BinaryName{"$fe"};
#			#	print "pexe=$pexe\n";
#		if ($UNAME eq 'CYGWIN_NT-5.0') {
#			$alldates = `date +%Y%m%d%H%M%S`;
#			chomp($alldates);
#			$myfe = $BinaryName{"$fe"};
#			$myfe =~ s/\.exe//;
#			$exe = $myfe . '-' . $alldates . '.exe';
#		} else {
#			$exe = $BinaryName{"$fe"} . '-' . `date +%Y%m%d%H%M%S`;
#			chomp($exe);
#		}
#			#	print "exe=$exe\n";
#		$dy = `date +%Y`; chomp($dy);
#		$dm = `date +%m`; chomp($dm);
#		$dd = `date +%d`; chomp($dd);
#		$dates = $dy . '/' . $dm . '/' . $dd . '/' . $GuessConfig;
#			#	print "dates=$dates\n";
#		$scpstr = "scp $pexe tinderbox\@server:allbuilds/$dates/$exe";
#		print "$scpstr\n";
#		print LOG "$scpstr\n";
#		open (SCP, "$scpstr 2>&1 |") || die "$scpstr: $!\n";
#		while (<SCP>) {
#	    		$scpresult = $_;
#	    		chomp($scpresult);
#	    		print "scp = [$scpresult]\n";
#	    		print LOG "SCP = [$scpresult]\n";
#		}
#		close (SCP);
#	    }

	    print LOG "tinderbox: tree: $BuildTree\n";
	    print LOG "tinderbox: builddate: $StartTime\n";
	    print LOG "tinderbox: status: $BuildStatusStr\n";
	    print LOG "tinderbox: build: $BuildName ($Hostname) $BuildType $fe\n";
	    print LOG "tinderbox: errorparser: unix\n";
	    print LOG "tinderbox: buildfamily: unix\n";
	    print LOG "tinderbox: END\n\n";	    
	}

#	# only if all targets were successful will we scp the release
#	if ( $AllBuildStatusStr eq 'success' ) {
#		my ($dy, $dm, $dd, $alldates, $dates, $scpstr);
#		$dy = `date +%Y`; chomp($dy);
#		$dm = `date +%m`; chomp($dm);
#		$dd = `date +%d`; chomp($dd);
#		$dates = $dy . '/' . $dm . '/' . $dd;
#		$scpstr = "scp ${BuildObjDir}2.*/*.tar.gz ${BuildObjDir}2.*/*.zip tinderbox\@server:allbuilds/$dates/";
#		print "$scpstr\n";
#		print LOG "$scpstr\n";
#		open (SCP, "$scpstr 2>&1 |") || die "$scpstr: $!\n";
#		while (<SCP>) {
#	    		$scpresult = $_;
#	    		chomp($scpresult);
#	    		print "scp = [$scpresult]\n";
#				# this will only confuse ppl as always one of gz or zip fails
#				#print LOG "SCP = [$scpresult]\n";
#		}
#		close (SCP);
#	}

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

	$Message = '';
	while (<LOG>) {
		for (my $q=0;;$q++) {
			my $val = $q * 1000;
			my $Output = substr($_, $val, 1000);

			last if $Output eq undef;

			$Output =~ s/^\.$//g;
			$Output =~ s/\n//g;
			$Message .= "$Output\n";
		} #EndFor
	} #EndWhile

	close(LOG);
	unlink("$logfile");

	&SendMail("Tinderbox Report", $Message);

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
    my($fe, @felist, $message);

    @felist = split(/,/, $FE);

    foreach $fe ( @felist ) {
		$message .= "\n";
		$message .= "tinderbox: tree: $BuildTree\n";
		$message .= "tinderbox: builddate: $StartTime\n";
		$message .= "tinderbox: status: building\n";
		$message .= "tinderbox: build: $BuildName ($Hostname) $BuildType $fe\n";
		$message .= "tinderbox: errorparser: unix\n";
		$message .= "tinderbox: buildfamily: unix\n";
		$message .= "tinderbox: END\n";
		$message .= "\n";
    }

    &SendMail("Tinderbox Report: Startbuildlog", $message);
	print LOG "StartBuildLog mailed to $Tinderbox_server\n";
} #EndSub-StartBuild

sub BinaryExists {
	my($fe) = @_;
	my($Binname);
	$fe = 'x' if (!defined($fe)); 
	my $BinName = $BuildObjName . '/' . $BinaryName{"$fe"};
	print LOG "Testing existence of " . $BinName . "\n"; 
	if ((-e $BinName) && (-x $BinName) && (-s $BinName)) {
		1;
	} else {
		0;
	}
} #EndSub-BinaryExists

sub DeleteBinary {
    my($fe) = @_;
    $fe = 'x' if (!defined($fe)); 

    my $BinName = $BuildObjName . '/' . $BinaryName{"$fe"};
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

# 'Main' function
print     "------------------- GetHome\n";
&GetHome;
print     "------------------- JustMe\n";
&JustMe();
print     "------------------- InitVars\n";
&InitVars;
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

