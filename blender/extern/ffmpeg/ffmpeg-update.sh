#!/bin/sh

echo "*** FFMPEG-SVN Update utility"
echo "*** This gets a new ffmpeg-svn tree and adapts it to blenders build structure"
echo "*** Warning! This script will wipe all of ffmpeg/lib*..."
echo "*** Please run again with --i-really-know-what-im-doing ..."

if [ "x$1" = "x--i-really-know-what-im-doing" ] ; then
   echo proceeding...
else
   exit -1
fi

rm -rf libavcodec libavformat libavutil libpostproc

cd ..
svn checkout svn://svn.mplayerhq.hu/ffmpeg/trunk ffmpeg
cd ffmpeg

rm -rf `find . -type d -name ".svn"`

rm -f pktdumper.c output_example.c ffmpeg.c ffplay.c ffserver.* \
   cmdutils.c cmdutils.h \
   qt-faststart.c cws2fws.c INSTALL README MAINTAINERS ffinstall.nsi \
   clean-diff build_avopt unwrap-diff
   
rm -rf doc Doxyfile tests vhook

cp common_blender.mak common.mak
cp Makefile_blender Makefile

