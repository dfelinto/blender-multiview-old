SUBDIRS = api2_2x

noinst_LTLIBRARIES = libblender_python.la

libblender_python_la_SOURCES = BPY_interface.c

libblender_python_la_CFLAGS = -Wall

libblender_python_la_LIBADD = \
	api2_2x/libblender_api2_2x.la

INCLUDES = \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna
