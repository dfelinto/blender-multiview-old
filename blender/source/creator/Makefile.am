noinst_LTLIBRARIES = libblender_creator.la
INCLUDES = -I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/misc \
	-I$(top_srcdir)/source/blender/bpython/include \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/renderconverter \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/source/blender/include

libblender_creator_la_SOURCES= \
	buildinfo.c \
	creator.c
