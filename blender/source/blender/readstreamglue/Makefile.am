noinst_LTLIBRARIES = libblender_readstreamglue.la

LIBS =

libblender_readstreamglue_la_SOURCES = \
	BLO_keyStore.h \
	BLO_readStreamErrors.h \
	BLO_readStreamGlue.h \
	BLO_sys_types.h \
	intern/BLO_keyStore.c \
	intern/BLO_keyStorePrivate.h \
	intern/BLO_readStreamGlue.c \
	intern/BLO_readStreamGlueLoopBack.c \
	intern/BLO_readStreamGlueLoopBack.h

INCLUDES = \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/inflate

EXTRA_DIST = \
	Makefile \
	intern/Makefile
