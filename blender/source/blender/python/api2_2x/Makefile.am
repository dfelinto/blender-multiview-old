noinst_LTLIBRARIES = libblender_api2_2x.la

libblender_api2_2x_la_SOURCES = \
	Blender.c \
	Blender_wrap.c \
	Object.c \
	Object_wrap.c \
	interface.c \
	interface.h \
	modules.h

INCLUDES = \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/blender/makesdna

EXTRA_DIST = \
	Blender.i \
	Object.i
