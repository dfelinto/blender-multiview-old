#assumption X11 flag means that it is a unix system... maybe it should be chaned later
if X11
BUILDX11=libgameplayer_unix.a
else
BUILDX11=
endif

if WIN32
BUILDWIN=libgameplayer_win.a
else
BUILDWIN=
endif

noinst_LIBRARIES = libgameplayer.a $(BUILDX11) $(BUILDWIN)

#I'm unsure if I should build the ghost/nestcape or activeX sources... 
#so essentially this file is not complete by any means
libgameplayer_a_SOURCES = \
	common/GPC_Canvas.cpp \
	common/GPC_Engine.cpp \
	common/GPC_KeyboardDevice.cpp \
	common/GPC_MouseDevice.cpp \
	common/GPC_PolygonMaterial.cpp \
	common/GPC_RawImage.cpp \
	common/GPC_RawLoadDotBlendArray.cpp \
	common/GPC_RawLogoArrays.cpp \
	common/GPC_RenderTools.cpp \
	common/GPC_System.cpp \
	common/bmfont.cpp

libgameplayer_unix_a_SOURCES = \
	common/unix/GPU_Canvas.cpp \
	common/unix/GPU_Engine.cpp \
	common/unix/GPU_KeyboardDevice.cpp \
	common/unix/GPU_System.cpp

libgameplayer_win_a_SOURCES = \
	common/windows/GPW_Canvas.cpp \
	common/windows/GPW_Engine.cpp \
	common/windows/GPW_KeyboardDevice.cpp \
	common/windows/GPW_System.cpp

INCLUDES = -I$(top_srcdir)/source/gameengine/Rasterizer \
	-I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/blenloader \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/gameengine/GameLogic \
	-I$(top_srcdir)/source/gameengine/Converter \
	-I$(top_srcdir)/source/gameengine/Network/LoopBackNetwork \
	-I$(top_srcdir)/source/gameengine/SoundSystem \
	-I$(top_srcdir)/source/gameengine/Expressions \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/source/gameengine/Ketsji \
	-I$(top_srcdir)/source/gameengine/SceneGraph \
	-I$(top_srcdir)/source/gameengine/Network \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/intern/bmfont
