noinst_LTLIBRARIES = libblender_api2_2x.la

LIBS =

libblender_api2_2x_la_SOURCES = \
  Blender.c \
  Blender.h \
  Types.c \
  Types.h \
  Object.c \
  Object.h \
  NMesh.c \
  NMesh.h \
  Material.c \
  Material.h \
  Camera.c \
  Camera.h \
  Lamp.c \
  Lamp.h \
  Curve.c \
  Curve.h \
  Image.c \
  Image.h \
  Window.c \
  Window.h \
  Draw.c \
  Draw.h \
  BGL.c \
  BGL.h \
  Text.c \
  Text.h \
  vector.c \
  vector.h \
  constant.c \
  constant.h \
  rgbTuple.c \
  rgbTuple.h \
  gen_utils.c \
  gen_utils.h \
  EXPP_interface.c \
  EXPP_interface.h \
  modules.h

libblender_api2_2x_la_CFLAGS = -Wall

INCLUDES = \
  -I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
  -I$(top_srcdir)/source/blender/blenkernel \
  -I$(top_srcdir)/source/blender/blenlib \
  -I$(top_srcdir)/source/blender/makesdna \
  -I$(top_srcdir)/intern/guardedalloc \
  -I$(top_srcdir)/intern/bmfont/ \
  -I$(top_srcdir)/source/blender/include
