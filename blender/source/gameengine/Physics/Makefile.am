noinst_LTLIBRARIES = libgameblender_physics.la

#there are some test prgrams under sumo/fuzzics/sample directory... if anyome wants to build them they should modify this file

libgameblender_physics_la_SOURCES = \
	common/PHY_IMotionState.cpp \
	common/PHY_IPhysicsController.cpp \
	common/PHY_IPhysicsEnvironment.cpp \
	BlOde/OdePhysicsController.cpp \
	BlOde/OdePhysicsEnvironment.cpp \
	Dummy/DummyPhysicsEnvironment.cpp

INCLUDES = \
	-I$(top_srcdir)/source/gameengine/Physics/common \
	-I$(top_srcdir)/source/gameengine/Physics/Sumo/include \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/source/ode/include
