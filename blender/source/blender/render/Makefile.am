noinst_LTLIBRARIES = libblender_render.la

LIBS =

libblender_render_la_SOURCES = \
	extern/include/render.h \
	extern/include/render_types.h \
	intern/include/RE_DummyShadowBuffer.h \
	intern/include/RE_ShadowBuffer.h \
	intern/include/RE_basicShadowBuffer.h \
	intern/include/RE_callbacks.h \
	intern/include/edgeRender.h \
	intern/include/envmap.h \
	intern/include/errorHandler.h \
	intern/include/gammaCorrectionTables.h \
	intern/include/initrender.h \
	intern/include/jitter.h \
	intern/include/old_zbuffer_types.h \
	intern/include/outerRenderLoop.h \
	intern/include/pixelblending.h \
	intern/include/pixelblending_types.h \
	intern/include/pixelshading.h \
	intern/include/renderHelp.h \
	intern/include/renderPreAndPost.h \
	intern/include/render_intern.h \
	intern/include/rendercore.h \
	intern/include/rendercore_int.h \
	intern/include/shadbuf.h \
	intern/include/shadowBuffer.h \
	intern/include/texture.h \
	intern/include/vanillaRenderPipe.h \
	intern/include/vanillaRenderPipe_int.h \
	intern/include/vanillaRenderPipe_types.h \
	intern/include/zbuf.h \
	intern/include/zbuf_int.h \
	intern/include/zbuf_types.h \
	intern/include/zbufferdatastruct.h \
	intern/include/zbufferdatastruct_types.h \
	intern/source/RE_DummyShadowBuffer.cpp \
	intern/source/RE_basicShadowBuffer.cpp \
	intern/source/RE_callbacks.c \
	intern/source/edgeRender.c \
	intern/source/envmap.c \
	intern/source/errorHandler.c \
	intern/source/gammaCorrectionTables.c \
	intern/source/initrender.c \
	intern/source/jitter.c \
	intern/source/outerRenderLoop.c \
	intern/source/pixelblending.c \
	intern/source/pixelshading.c \
	intern/source/renderHelp.c \
	intern/source/renderPreAndPost.c \
	intern/source/rendercore.c \
	intern/source/renderdatabase.c \
	intern/source/shadbuf.c \
	intern/source/shadowBuffer.cpp \
	intern/source/vanillaRenderPipe.c \
	intern/source/zbuf.c \
	intern/source/zbufferdatastruct.c

INCLUDES = -I$(top_srcdir)/source/blender/render/intern/include \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/radiosity/extern/include \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/kernel/gen_messaging

EXTRA_DIST = \
	Makefile \
	intern/Makefile \
	intern/source/Makefile
