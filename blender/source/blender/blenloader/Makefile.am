noinst_LIBRARIES = libblender_blenloader.la

libblender_blenloader_la_SOURCES = \
	intern/genfile.c \
	intern/readblenentry.c \
	intern/readfile.c \
	intern/writefile.c

INCLUDES = -I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/readblenfile \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/bpython/include \
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/blender/bpython/include \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/source/blender/writestreamglue \
	-I$(top_srcdir)/source/blender/readstreamglue
