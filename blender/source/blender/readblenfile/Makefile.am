noinst_LIBRARIES = libblender_readblenfile.la

libblender_readblenfile_la_SOURCES = \
	intern/BLO_readblenfile.c \
	stub/BLO_readblenfileSTUB.c

#note I'm unsure if the stub file should be included... better safe than sorry

INCLUDES = -I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/kernel/gen_messaging
