noinst_LTLIBRARIES = libblender_writestreamglue.la

libblender_writestreamglue_la_SOURCES = \
	intern/BLO_dumpFromMemory.c \
	intern/BLO_getPubKey.c \
	intern/BLO_streamGlueControl.c \
	intern/BLO_writeStreamGlue.c \
	stub/BLO_getPubKeySTUB.c \
	stub/BLO_keyStoreSTUB.c \
	stub/BLO_streamGlueControlSTUB.c \
	stub/BLO_writeStreamGlueSTUB.c


#note note sure if the stub directory should be included... better safe than sorry

INCLUDES = -I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/intern/keymaker
