noinst_LIBRARIES = libblender_render.a

libblender_render_a_SOURCES = \
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

INCLUDES = -I$(top_srcdir)/source/blender/render/intern/include -I$(top_srcdir)/intern/guardedalloc -I$(top_srcdir)/source/blender/blenlib -I$(top_srcdir)/source/blender/makesdna -I$(top_srcdir)/source/blender/render/extern/include -I$(top_srcdir)/source/blender/blenkernel -I$(top_srcdir)/source/blender/radiosity/extern/include -I$(top_srcdir)/source/blender/imbuf -I$(top_srcdir)/source/blender/include -I$(top_srcdir)/source/kernel/gen_messaging