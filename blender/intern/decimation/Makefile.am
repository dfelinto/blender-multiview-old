noinst_LTLIBRARIES = libblender_LOD.la

LIBS =

INCLUDES = -I$(top_srcdir)/intern/decimation/intern \
	-I$(top_srcdir)/intern/decimation/extern \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/container \
	-I$(top_srcdir)/intern/memutil

libblender_LOD_la_SOURCES = \
	extern/LOD_decimation.h \
	intern/LOD_DecimationClass.h \
	intern/LOD_EdgeCollapser.cpp \
	intern/LOD_EdgeCollapser.h \
	intern/LOD_ExternBufferEditor.h \
	intern/LOD_ExternNormalEditor.cpp \
	intern/LOD_ExternNormalEditor.h \
	intern/LOD_FaceNormalEditor.cpp \
	intern/LOD_FaceNormalEditor.h \
	intern/LOD_ManMesh2.cpp \
	intern/LOD_ManMesh2.h \
	intern/LOD_MeshBounds.h \
	intern/LOD_MeshException.h \
	intern/LOD_MeshPrimitives.cpp \
	intern/LOD_MeshPrimitives.h \
	intern/LOD_QSDecimator.cpp \
	intern/LOD_QSDecimator.h \
	intern/LOD_Quadric.h \
	intern/LOD_QuadricEditor.cpp \
	intern/LOD_QuadricEditor.h \
	intern/LOD_decimation.cpp
