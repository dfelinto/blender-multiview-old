noinst_LTLIBRARIES = libblender_deflate.la

libblender_deflate_la_SOURCES = \
	BLO_deflate.h \
	intern/BLO_deflate.c

#note: a blatant copy from decrypt
INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/intern/keymaker \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/inflate

EXTRA_DIST = \
	Makefile \
	intern/Makefile
