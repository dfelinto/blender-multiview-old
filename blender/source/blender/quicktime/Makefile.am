noinst_LTLIBRARIES = libblender_quicktime.la

LIBS =

libblender_quicktime_la_SOURCES = \
	apple/quicktime_import.c \
	apple/quicktime_export.c

INCLUDES = -I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/avi \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/source/blender/imbuf/intern \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/render/extern/include

EXTRA_DIST = \
	Makefile
