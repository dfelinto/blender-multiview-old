noinst_LTLIBRARIES = libgameblender_routines.la

libgameblender_routines_la_SOURCES = \
	BL_KetsjiEmbedStart.cpp \
	KX_BlenderCanvas.cpp \
	KX_BlenderCanvas.h \
	KX_BlenderGL.cpp \
	KX_BlenderGL.h \
	KX_BlenderInputDevice.cpp \
	KX_BlenderInputDevice.h \
	KX_BlenderKeyboardDevice.cpp \
	KX_BlenderKeyboardDevice.h \
	KX_BlenderMouseDevice.cpp \
	KX_BlenderMouseDevice.h \
	KX_BlenderPolyMaterial.cpp \
	KX_BlenderPolyMaterial.h \
	KX_BlenderRenderTools.cpp \
	KX_BlenderRenderTools.h \
	KX_BlenderSystem.cpp \
	KX_BlenderSystem.h

INCLUDES = -I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/gameengine/Rasterizer \
	-I$(top_srcdir)/intern/bmfont \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/gameengine/Converter \
	-I$(top_srcdir)/source/gameengine/GameLogic \
	-I$(top_srcdir)/source/gameengine/Rasterizer/RAS_OpenGLRasterizer \
	-I$(top_srcdir)/source/gameengine/Network \
	-I$(top_srcdir)/source/gameengine/Network/LoopBackNetwork \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/source/gameengine/Ketsji \
	-I$(top_srcdir)/source/gameengine/Expressions \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/gameengine/SceneGraph \
	-I$(top_srcdir)/source/gameengine/SoundSystem \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/blenlib

EXTRA_DIST = \
	Makefile
