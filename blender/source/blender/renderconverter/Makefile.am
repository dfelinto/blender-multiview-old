if EXPPYTHON
  blenpythoninclude = -I$(top_srcdir)/source/blender/python
else
  blenpythoninclude = -I$(top_srcdir)/source/blender/bpython/include
endif

noinst_LTLIBRARIES = libblender_renderconverter.la

libblender_renderconverter_la_SOURCES = \
	RE_renderconverter.h \
	intern/convertBlenderScene.c

INCLUDES = -I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/source/blender/misc \
	$(blenpythoninclude)

EXTRA_DIST = \
	Makefile \
	intern/Makefile
