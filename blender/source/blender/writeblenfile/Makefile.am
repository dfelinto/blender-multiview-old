noinst_LTLIBRARIES = libblender_writeblenfile.la

libblender_writeblenfile_la_SOURCES = \
	BLO_writeblenfile.h \
	intern/BLO_writeblenfile.c

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/readblenfile

EXTRA_DIST = \
	Makefile \
	intern/Makefile
