noinst_LTLIBRARIES = libblender_writestreamglue.la

libblender_writestreamglue_la_SOURCES = \
	BLO_getPubKey.h \
	BLO_writeStreamErrors.h \
	BLO_writeStreamGlue.h \
	intern/BLO_dumpFromMemory.c \
	intern/BLO_dumpFromMemory.h \
	intern/BLO_getPubKey.c \
	intern/BLO_streamGlueControl.c \
	intern/BLO_writeStreamGlue.c

INCLUDES = -I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/deflate \
	-I$(top_srcdir)/source/blender/encrypt \
	-I$(top_srcdir)/source/blender/sign \
	-I$(top_srcdir)/source/blender/writeblenfile \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/intern/keymaker

EXTRA_DIST = \
	Makefile \
	intern/Makefile
