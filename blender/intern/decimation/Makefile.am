noinst_LTLIBRARIES = libblender_LOD.la
INCLUDES = -I$(top_srcdir)/intern/decimation/intern \
	-I$(top_srcdir)/intern/decimation/extern \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/container \
	-I$(top_srcdir)/intern/memutil

libblender_LOD_la_SOURCES = \
	extern/LOD_decimation.h \
	intern/LOD_EdgeCollapser.cpp \
	intern/LOD_ExternNormalEditor.cpp \
	intern/LOD_FaceNormalEditor.cpp \
	intern/LOD_ManMesh2.cpp \
	intern/LOD_MeshPrimitives.cpp \
	intern/LOD_QSDecimator.cpp \
	intern/LOD_QuadricEditor.cpp \
	intern/LOD_decimation.cpp
