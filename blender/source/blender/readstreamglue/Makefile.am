noinst_LIBRARIES = libblender_readstreamglue.la

libblender_readstreamglue_la_SOURCES = \
	intern/BLO_keyStore.c \
	intern/BLO_readStreamGlue.c \
	intern/BLO_readStreamGlueLoopBack.c

INCLUDES = -I$(top_srcdir)/intern/keymaker \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/inflate \
	-I$(top_srcdir)/source/blender/decrypt \
	-I$(top_srcdir)/source/blender/verify
