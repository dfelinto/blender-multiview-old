noinst_LTLIBRARIES = libblender_encrypt.la

LIBS =

libblender_encrypt_la_SOURCES = \
	BLO_encrypt.h \
	intern/BLO_encrypt.c

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/decrypt

EXTRA_DIST = \
	Makefile \
	intern/Makefile
