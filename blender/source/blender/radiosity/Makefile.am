noinst_LTLIBRARIES = libblender_radiosity.la

LIBS =

libblender_radiosity_la_SOURCES = \
	extern/include/radio.h \
	extern/include/radio_types.h \
	intern/source/raddisplay.c \
	intern/source/radfactors.c \
	intern/source/radio.c \
	intern/source/radnode.c \
	intern/source/radpostprocess.c \
	intern/source/radpreprocess.c

INCLUDES = -I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/radiosity/extern/include \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/render/extern/include

EXTRA_DIST = \
	Makefile \
	intern/Makefile \
	intern/source/Makefile
