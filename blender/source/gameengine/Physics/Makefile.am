noinst_LTLIBRARIES = libgameblender_physics.la

#there are some test prgrams under sumo/fuzzics/sample directory... if anyome wants to build them they should modify this file

libgameblender_physics_la_SOURCES = \
	common/PHY_DynamicTypes.h \
	common/PHY_IMotionState.cpp \
	common/PHY_IMotionState.h \
	common/PHY_IPhysicsController.cpp \
	common/PHY_IPhysicsController.h \
	common/PHY_IPhysicsEnvironment.cpp \
	common/PHY_IPhysicsEnvironment.h \
	common/PHY_Pro.h \
	BlOde/OdePhysicsController.cpp \
	BlOde/OdePhysicsController.h \
	BlOde/OdePhysicsEnvironment.cpp \
	BlOde/OdePhysicsEnvironment.h \
	Dummy/DummyPhysicsEnvironment.cpp \
	Dummy/DummyPhysicsEnvironment.h

INCLUDES = \
	-I$(top_srcdir)/source/gameengine/Physics/common \
	-I$(top_srcdir)/source/gameengine/Physics/Sumo/include \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/source/ode/include

EXTRA_DIST = \
	Makefile \
	common/Makefile \
	BlOde/Makefile \
	Dummy/Makefile
