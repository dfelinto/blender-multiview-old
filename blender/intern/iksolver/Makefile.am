noinst_LTLIBRARIES = libblender_IK.la

INCLUDES = -I$(top_srcdir)/intern/iksolver/intern/TNT \
	-I$(top_srcdir)/intern/iksolver/intern \
	-I$(top_srcdir)/intern/iksolver/extern \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/memutil

libblender_IK_la_SOURCES = \
	intern/IK_CGChainSolver.cpp \
	intern/IK_Chain.cpp \
	intern/IK_ConjugateGradientSolver.cpp \
	intern/IK_JacobianSolver.cpp \
	intern/IK_QChain.cpp \
	intern/IK_QJacobianSolver.cpp \
	intern/IK_QSegment.cpp \
	intern/IK_Segment.cpp \
	intern/IK_Solver.cpp \
	intern/MT_ExpMap.cpp
