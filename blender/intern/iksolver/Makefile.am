noinst_LTLIBRARIES = libblender_IK.la

LIBS =

INCLUDES = -I$(top_srcdir)/intern/iksolver/intern/TNT \
	-I$(top_srcdir)/intern/iksolver/intern \
	-I$(top_srcdir)/intern/iksolver/extern \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/memutil

libblender_IK_la_SOURCES = \
	extern/IK_solver.h \
	intern/IK_QChain.cpp \
	intern/IK_QChain.h \
	intern/IK_QJacobianSolver.cpp \
	intern/IK_QJacobianSolver.h \
	intern/IK_QSegment.cpp \
	intern/IK_QSegment.h \
	intern/IK_QSolver_Class.h \
	intern/IK_Solver.cpp \
	intern/MT_ExpMap.cpp \
	intern/MT_ExpMap.h \
	intern/TNT/cmat.h \
	intern/TNT/subscript.h \
	intern/TNT/svd.h \
	intern/TNT/tntmath.h \
	intern/TNT/vec.h

EXTRA_DIST = \
	Makefile \
	intern/Makefile \
	make/msvc_6_0/iksolver.dsp \
	make/msvc_6_0/iksolver.dsw
