noinst_LTLIBRARIES = libblender_decrypt.la

LIBS =

libblender_decrypt_la_SOURCES = \
	BLO_decrypt.h \
	BLO_en_de_cryptHeader.h \
	intern/BLO_decrypt.c

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue

EXTRA_DIST = \
	Makefile \
	intern/Makefile
