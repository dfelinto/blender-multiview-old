noinst_LTLIBRARIES = libblender_bpython.la

libblender_bpython_la_LDFLAGS = -static \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	-lpython@PYTHON_VERSION@ @X_EXTRA_LIBS@ -lm

libblender_bpython_la_SOURCES = \
	include/BPY_extern.h \
	include/BPY_objtypes.h \
	intern/BPY_constobject.c \
	intern/BPY_constobject.h \
	intern/BPY_csg.c \
	intern/BPY_csg.h \
	intern/BPY_image.c \
	intern/BPY_ipo.c \
	intern/BPY_links.c \
	intern/BPY_listbase_macro.h \
	intern/BPY_macros.h \
	intern/BPY_main.c \
	intern/BPY_main.h \
	intern/BPY_modules.h \
	intern/BPY_object.c \
	intern/BPY_scene.c \
	intern/BPY_text.c \
	intern/BPY_tools.c \
	intern/BPY_tools.h \
	intern/BPY_types.h \
	intern/BPY_window.h \
	intern/api.h \
	intern/b_import.c \
	intern/b_import.h \
	intern/b_interface.c \
	intern/b_interface.h \
	intern/opy_blender.c \
	intern/opy_datablock.c \
	intern/opy_datablock.h \
	intern/opy_draw.c \
	intern/opy_matrix.c \
	intern/opy_nmesh.c \
	intern/opy_nmesh.h \
	intern/opy_vector.c \
	intern/opy_vector.h \
	intern/opy_window.c

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

EXTRA_DIST = \
	Makefile \
	intern/Makefile \
	intern/README
