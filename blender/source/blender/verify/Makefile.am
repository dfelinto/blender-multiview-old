noinst_LTLIBRARIES = libblender_verify.la

libblender_verify_la_SOURCES = \
	BLO_sign_verify_Header.h \
	BLO_signer_info.h \
	BLO_verify.h \
	intern/BLO_verify.c

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue

EXTRA_DIST = \
	Makefile \
	intern/Makefile
