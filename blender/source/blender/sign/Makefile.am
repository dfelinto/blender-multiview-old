noinst_LTLIBRARIES = libblender_sign.la

libblender_sign_la_SOURCES = \
	BLO_sign.h \
	intern/BLO_sign.c

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/verify \
	-I$(top_srcdir)/intern/keymaker

EXTRA_DIST = \
	Makefile \
	intern/Makefile
