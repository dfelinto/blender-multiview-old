noinst_LTLIBRARIES = libblender_api2_2x.la

LIBS =

libblender_api2_2x_la_SOURCES = \
	Blender.h \
	Blender.c \
	Object.c \
	Object.h \
	Camera.c \
	Camera.h \
	Lamp.c \
	Lamp.h \
	Image.c \
	Image.h \
	constant.c \
	constant.h \
	gen_utils.c \
	gen_utils.h \
	interface.c \
	interface.h \
	modules.h

libblender_api2_2x_la_CFLAGS = -Wall

INCLUDES = \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/include
