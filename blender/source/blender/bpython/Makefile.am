noinst_LTLIBRARIES = libblender_bpython.la

libblender_bpython_la_LDFLAGS = -static \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	-lpython@PYTHON_VERSION@ @X_EXTRA_LIBS@ -ldl -lm -lpthread

libblender_bpython_la_SOURCES = \
	intern/BPY_constobject.c \
	intern/opy_draw.c \
	intern/opy_matrix.c \
	intern/BPY_csg.c \
	intern/BPY_object.c \
	intern/opy_nmesh.c \
	intern/BPY_scene.c \
	intern/b_import.c \
	intern/BPY_image.c \
	intern/BPY_text.c \
	intern/opy_vector.c \
	intern/BPY_ipo.c \
	intern/BPY_tools.c \
	intern/b_interface.c \
	intern/BPY_links.c \
	intern/opy_window.c \
	intern/opy_blender.c \
	intern/opy_datablock.c \
	intern/BPY_main.c

#the python include is a hack... well atleast it works for me (don't understand why it doesn't have a direct link to their include files... or then it is my system that sucks)
INCLUDES = -I$(top_srcdir)/source/blender/blenkernel \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/intern/bmfont \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/blenlib/intern \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/bpython/include \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/intern/bsp/extern
