noinst_LTLIBRARIES = libblender_blenloader.la

LIBS =

libblender_blenloader_la_SOURCES = \
	BLO_genfile.h \
	BLO_readfile.h \
	BLO_soundfile.h \
	BLO_writefile.h \
	intern/genfile.c \
	intern/genfile.h \
	intern/readblenentry.c \
	intern/readfile.c \
	intern/readfile.h \
	intern/writefile.c

INCLUDES = -I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/readblenfile \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/python \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue

EXTRA_DIST = \
	Makefile \
	intern/Makefile
