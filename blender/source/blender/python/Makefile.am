SUBDIRS = api2_2x

noinst_LTLIBRARIES = libblender_python.la

LIBS =

libblender_python_la_SOURCES = \
  BPY_interface.c \
  BPY_menus.c \
  BPY_menus.h

libblender_python_la_CFLAGS = -Wall

libblender_python_la_LIBADD = \
	api2_2x/libblender_api2_2x.la

INCLUDES = \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/include/
