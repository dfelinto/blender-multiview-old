noinst_LTLIBRARIES = libblender_blenpluginapi.la

LIBS =

libblender_blenpluginapi_la_SOURCES = \
	documentation.h \
	floatpatch.h \
	iff.h \
	plugin.h \
	util.h \
	intern/pluginapi.c

INCLUDES = -I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/source/blender/makesdna

EXTRA_DIST = \
	Makefile \
	intern/Makefile
