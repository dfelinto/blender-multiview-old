noinst_LTLIBRARIES = libblender_api2_2x.la

libblender_api2_2x_la_SOURCES = \
	Blender.c \
	Object.c \
	gen_utils.c \
	gen_utils.h \
	interface.c \
	interface.h \
	modules.h

libblender_api2_2x_la_CFLAGS = -Wall

INCLUDES = \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/include
