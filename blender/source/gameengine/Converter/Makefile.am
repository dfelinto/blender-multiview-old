noinst_LTLIBRARIES = libgameblender_converter.la

libgameblender_converter_la_SOURCES = \
	BL_ActionActuator.cpp \
	BL_ActionActuator.h \
	BL_ArmatureObject.cpp \
	BL_ArmatureObject.h \
	BL_BlenderDataConversion.cpp \
	BL_BlenderDataConversion.h \
	BL_DeformableGameObject.cpp \
	BL_DeformableGameObject.h \
	BL_MeshDeformer.cpp \
	BL_MeshDeformer.h \
	BL_SkinDeformer.cpp \
	BL_SkinDeformer.h \
	BL_SkinMeshObject.cpp \
	BL_SkinMeshObject.h \
	BlenderWorldInfo.cpp \
	BlenderWorldInfo.h \
	KX_BlenderScalarInterpolator.cpp \
	KX_BlenderScalarInterpolator.h \
	KX_BlenderSceneConverter.cpp \
	KX_BlenderSceneConverter.h \
	KX_ConvertActuators.cpp \
	KX_ConvertActuators.h \
	KX_ConvertControllers.cpp \
	KX_ConvertControllers.h \
	KX_ConvertProperties.cpp \
	KX_ConvertProperties.h \
	KX_ConvertSensors.cpp \
	KX_ConvertSensors.h \
	KX_IpoConvert.cpp \
	KX_IpoConvert.h

INCLUDES = -I$(top_srcdir)/source/gameengine/GameLogic \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/source/gameengine/Expressions \
	-I$(top_srcdir)/source/gameengine/GameLogic \
	-I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@ \
	-I$(top_srcdir)/source/gameengine/Ketsji \
	-I$(top_srcdir)/source/gameengine/SceneGraph \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/source/gameengine/Rasterizer \
	-I$(top_srcdir)/source/blender/makesdna \
	-I$(top_srcdir)/source/blender/blenkernel \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/gameengine/BlenderRoutines \
	-I$(top_srcdir)/source/gameengine/Physics/common \
	-I$(top_srcdir)/source/gameengine/SoundSystem \
	-I$(top_srcdir)/source/gameengine/BlenderRoutines \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/gameengine/Physics/Dummy \
	-I$(top_srcdir)/source/gameengine/Physics/BlOde \
	-I$(top_srcdir)/source/gameengine/Ketsji/KXNetwork \
	-I$(top_srcdir)/source/gameengine/Network

EXTRA_DIST = \
	Makefile
