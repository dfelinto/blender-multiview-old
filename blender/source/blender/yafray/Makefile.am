noinst_LTLIBRARIES = libblender_yafrayexport.la

LIBS =

libblender_yafrayexport_la_CFLAGS = -Wall

libblender_yafrayexport_la_SOURCES = intern/yafray_Render.cpp \
intern/yafray_Render.h \
intern/export_File.cpp intern/export_File.h \
intern/api.cpp

INCLUDES = -I$(top_srcdir)/intern/guardedalloc \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/intern/bmfont \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/intern/bsp/extern \
	-I$(top_srcdir)/source/blender/renderconverter \
	-I$(top_srcdir)/source/blender/radiosity/extern/include \
	-I$(top_srcdir)/intern/decimation/extern \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/python \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/intern/SoundSystem \
	-I$(top_srcdir)/source/blender/readstreamglue \
	-I$(top_srcdir)/source/blender/img \
	-I$(top_srcdir)/source/blender/quicktime \
	-I$(top_srcdir)/intern/ghost

EXTRA_DIST = \
	Makefile
