if WIN32
BUILDTYPE=libgameblender_player_win.la
else
BUILDTYPE=libgameblender_player_Ghost.la
endif

lib_LTLIBRARIES = $(BUILDTYPE) libgameblender_player.la

libgameblender_player_la_SOURCES = \
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
	common/bmfont.cpp \
	ghost/GPG_Application.cpp \
	ghost/GPG_Canvas.cpp \
	ghost/GPG_KeyboardDevice.cpp \
	ghost/GPG_System.cpp \
	ghost/GPG_ghost.cpp

libgameblender_player_la_LIBADD = \
	$(BUILDTYPE)

libgameblender_player_Ghost_la_SOURCES = \
	common/unix/GPU_Canvas.cpp \
	common/unix/GPU_Engine.cpp \
	common/unix/GPU_KeyboardDevice.cpp \
	common/unix/GPU_System.cpp

libgameblender_player_win_la_SOURCES = \
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
	-I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/gameengine/Ketsji \
	-I$(top_srcdir)/source/gameengine/SceneGraph \
	-I$(top_srcdir)/source/gameengine/Network \
	-I$(top_srcdir)/source/gameengine/GamePlayer/common \
	-I$(top_srcdir)/source/gameengine/GamePlayer/netscape/src/ketsji \
	-I$(top_srcdir)/source/gameengine/Physics/Sumo/include \
	-I$(top_srcdir)/source/gameengine/Physics/Sumo/Fuzzics/include \
	-I$(top_srcdir)/source/gameengine/Rasterizer/RAS_OpenGLRasterizer \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/intern/ghost \
	-I$(top_srcdir)/intern/bmfont
