noinst_LTLIBRARIES = libblender_readblenfile.la

libblender_readblenfile_la_SOURCES = \
	BLO_readblenfile.h \
	intern/BLO_readblenfile.c 

INCLUDES = -I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/kernel/gen_messaging

EXTRA_DIST = \
	Makefile \
	intern/Makefile
